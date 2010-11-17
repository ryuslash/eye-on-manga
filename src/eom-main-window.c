#include "eom-main-window.h"
#include <hildon/hildon.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "eom-new-item-dialog.h"
#include "data.h"
#include "interface.h"

G_DEFINE_TYPE(EomMainWindow, eom_main_window, HILDON_TYPE_STACKABLE_WINDOW)

enum {
  COL_ID = 0,
  COL_NAME,
  COL_CURRENT,
  COL_TOTAL,
  NUM_COLS
};

static void eom_main_window_add_menu(EomMainWindow *window);
static void eom_main_window_on_new(GtkWidget *widget, GtkWindow *window);
static void eom_main_window_on_add_clicked(GtkWidget *widget,
                                           gpointer user_data);
static void eom_main_window_on_remove_clicked(GtkWidget *widget,
                                              gpointer user_data);
static gboolean eom_main_window_on_edit_closed(GtkWidget *widget,
                                               GdkEvent *event,
                                               gpointer user_data);
static void eom_main_window_on_row_activated(GtkWidget *treeview,
                                             GtkTreePath *path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data);

GtkWidget *eom_main_window_new(void)
{
  return g_object_new(EOM_TYPE_MAIN_WINDOW, NULL);
}

void eom_main_window_load(EomMainWindow *self)
{
  GList *list;

  gtk_list_store_clear(self->store);

  list = data_get_manga();

  while (list) {
    Manga *manga = list->data;
    eom_main_window_add_line(self, manga->id, manga->name,
                             manga->current_qty, manga->total_qty);

    free(manga->name);
    free(manga);
    list = g_list_next(list);
  }

  g_list_free_1(list);
}

void eom_main_window_add_line(EomMainWindow *window,
                              gint id,
                              const gchar *name,
                              gint current_qty,
                              gint total_qty)
{
  gtk_list_store_append(window->store, &window->iter);
  gtk_list_store_set(window ->store, &window->iter,
                     COL_ID, id,
                     COL_NAME, name,
                     COL_CURRENT, current_qty,
                     COL_TOTAL, total_qty,
                     -1);
}

static void eom_main_window_class_init(EomMainWindowClass *class)
{}

static void eom_main_window_init(EomMainWindow *window)
{
  GtkCellRenderer   *renderer;
  GtkWidget         *view;
  GtkWidget         *pannablearea;
  GtkTreeViewColumn *current_column;
  GtkTreeSelection  *selection;
  int                index;

  index = -1;

  eom_main_window_add_menu(window);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  window->store = gtk_list_store_new(NUM_COLS,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_INT,
                                     G_TYPE_INT);

  pannablearea = hildon_pannable_area_new();
  g_object_set(G_OBJECT(pannablearea),
               "mov-mode", HILDON_MOVEMENT_MODE_VERT,
               NULL);
  gtk_container_add(GTK_CONTAINER(window), pannablearea);

  view = hildon_gtk_tree_view_new(HILDON_UI_MODE_NORMAL);
  g_signal_connect(view, "row-activated",
                   G_CALLBACK(eom_main_window_on_row_activated), window);
  gtk_container_add(GTK_CONTAINER(pannablearea), view);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              ++index,
                                              "Naam",
                                              renderer,
                                              "text", COL_NAME,
                                              NULL);
  current_column = gtk_tree_view_get_column(GTK_TREE_VIEW(view), index);
  gtk_tree_view_column_set_expand(current_column, TRUE);

  renderer = gtk_cell_renderer_text_new();
  gtk_object_set(GTK_OBJECT(renderer),
                 "xalign", 1.0,
                 "width", 100,
                 NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              ++index,
                                              "",
                                              renderer,
                                              "text", COL_CURRENT,
                                              NULL);

  renderer = gtk_cell_renderer_text_new();
  gtk_object_set(GTK_OBJECT(renderer),
                 "xalign", 1.0,
                 "width", 100,
                 NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              ++index,
                                              "",
                                              renderer,
                                              "text", COL_TOTAL,
                                              NULL);

  gtk_tree_view_set_model(GTK_TREE_VIEW(view),
                          GTK_TREE_MODEL(window->store));
  g_object_unref(window->store);
}

static void eom_main_window_add_menu(EomMainWindow *window)
{
  GtkWidget *appmenu;
  GtkWidget *new_button;

  appmenu = hildon_app_menu_new();

  new_button = hildon_gtk_button_new(HILDON_SIZE_AUTO);
  gtk_button_set_label(GTK_BUTTON(new_button), "New Manga");
  g_signal_connect_after(new_button,
                         "clicked",
                         G_CALLBACK(eom_main_window_on_new),
                         GTK_WINDOW(window));

  hildon_app_menu_append(HILDON_APP_MENU(appmenu), GTK_BUTTON(new_button));
  gtk_widget_show_all(appmenu);

  hildon_stackable_window_set_main_menu(HILDON_STACKABLE_WINDOW(window),
                                        HILDON_APP_MENU(appmenu));
}

static void eom_main_window_on_new(GtkWidget *widget, GtkWindow *window)
{
  GtkWidget *dialog;
  gint result;
  gchar *name = NULL;
  gint total_qty;

  dialog = interface_show_new_item_dialog(window);

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  if (result == GTK_RESPONSE_OK) {
    const gchar *tmp;
    tmp = eom_new_item_dialog_get_name(EOM_NEW_ITEM_DIALOG(dialog));
    name = (gchar *)malloc(strlen(tmp) + 1);

    strcpy(name, tmp);
    strcat(name, "\0");

    total_qty = eom_new_item_dialog_get_total_qty(EOM_NEW_ITEM_DIALOG(dialog));

    gtk_widget_destroy(dialog);
  }

  if (name != NULL) {
    if (data_add_manga(name, total_qty))
      eom_main_window_load(EOM_MAIN_WINDOW(window));
  }
}

static gboolean eom_main_window_on_edit_closed(GtkWidget *widget,
                                           GdkEvent *event,
                                           gpointer user_data)
{
  EomMainWindow *self = user_data;

  eom_main_window_load(self);

  return FALSE;
}

static void eom_main_window_on_row_activated(GtkWidget *treeview,
                                             GtkTreePath *path,
                                             GtkTreeViewColumn *column,
                                             gpointer user_data)
{
  EomMainWindow *self = user_data;
  GtkTreeIter iter;
  GtkTreeModel *model;
  gint id;
  GtkWidget *window;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
  if (!gtk_tree_model_get_iter(model, &iter, path))
    return;

  gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
  window = interface_show_edit_window(id);
  g_signal_connect(window, "delete-event",
                   G_CALLBACK(eom_main_window_on_edit_closed), self);
}
