#include "c-main-window.h"
#include <hildon/hildon.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "c-new-item-dialog.h"
#include "data.h"
#include "interface.h"

G_DEFINE_TYPE(CMainWindow, c_main_window, HILDON_TYPE_STACKABLE_WINDOW)

enum {
  COL_ID = 0,
  COL_NAME,
  COL_CURRENT,
  COL_TOTAL,
  NUM_COLS
};

static void c_main_window_add_menu(CMainWindow *window);
static void c_main_window_on_new(GtkWidget *widget, GtkWindow *window);
static void c_main_window_on_selection_changed(GtkTreeSelection *selection,
                                               gpointer user_data);
static void c_main_window_on_add_clicked(GtkWidget *widget, gpointer user_data);
static void c_main_window_on_remove_clicked(GtkWidget *widget,
                                            gpointer user_data);

GtkWidget *c_main_window_new(void)
{
  return g_object_new(C_TYPE_MAIN_WINDOW, NULL);
}

void c_main_window_load(CMainWindow *self)
{
  GList *list;

  gtk_list_store_clear(self->store);

  list = data_get_series();

  while (list) {
    struct collection *col = list->data;
    c_main_window_add_line(self, col->id, col->name,
                           col->current_qty, col->total_qty);
    list = g_list_next(list);
  }

  g_list_free_1(list);
}

void c_main_window_add_line(CMainWindow *window,
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

void c_main_window_set_no_select(CMainWindow *self)
{
  if (GTK_IS_WIDGET(self->add_button))
    gtk_widget_set_sensitive(GTK_WIDGET(self->add_button), FALSE);
  if (GTK_IS_WIDGET(self->remove_button))
    gtk_widget_set_sensitive(GTK_WIDGET(self->remove_button), FALSE);
}

void c_main_window_set_has_select(CMainWindow *self)
{
  if (GTK_IS_WIDGET(self->add_button))
    gtk_widget_set_sensitive(GTK_WIDGET(self->add_button), TRUE);
  if (GTK_IS_WIDGET(self->remove_button))
    gtk_widget_set_sensitive(GTK_WIDGET(self->remove_button), TRUE);
}

static void c_main_window_class_init(CMainWindowClass *class)
{}

static void c_main_window_init(CMainWindow *window)
{
  GtkCellRenderer   *renderer;
  GtkWidget         *view;
  GtkWidget         *vbox;
  GtkWidget         *hbuttonbox;
  GtkWidget         *pannablearea;
  GtkTreeViewColumn *current_column;
  int                index;

  index = -1;

  c_main_window_add_menu(window);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  window->store = gtk_list_store_new(NUM_COLS,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_INT,
                                     G_TYPE_INT);

  vbox = gtk_vbox_new(FALSE, 0);

  pannablearea = hildon_pannable_area_new();
  g_object_set(G_OBJECT(pannablearea),
               "mov-mode", HILDON_MOVEMENT_MODE_VERT,
               NULL);
  gtk_box_pack_start(GTK_BOX(vbox), pannablearea, TRUE, TRUE, 0);

  view = gtk_tree_view_new();
  hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA(pannablearea),
                                         view);

  window->selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
  g_signal_connect(window->selection, "changed",
                   G_CALLBACK(c_main_window_on_selection_changed),
                   (gpointer)window);

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

  hbuttonbox = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_END);
  gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, TRUE, 0);

  window->add_button =
    hildon_button_new_with_text(HILDON_SIZE_AUTO_WIDTH |
                                HILDON_SIZE_FINGER_HEIGHT,
                                HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
                                "Add",
                                NULL);
  g_signal_connect(window->add_button, "clicked",
                   G_CALLBACK(c_main_window_on_add_clicked),
                   (gpointer)window);
  gtk_box_pack_start(GTK_BOX(hbuttonbox), window->add_button, FALSE, FALSE, 0);

  window->remove_button =
    hildon_button_new_with_text(HILDON_SIZE_AUTO_WIDTH |
                                HILDON_SIZE_FINGER_HEIGHT,
                                HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
                                "Remove",
                                NULL);
  g_signal_connect(window->remove_button, "clicked",
                   G_CALLBACK(c_main_window_on_remove_clicked),
                   (gpointer)window);
  gtk_box_pack_start(GTK_BOX(hbuttonbox),
                     window->remove_button, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(window), vbox);

  c_main_window_set_no_select(window);
}

static void c_main_window_add_menu(CMainWindow *window)
{
  GtkWidget *appmenu;
  GtkWidget *new_button;

  appmenu = hildon_app_menu_new();

  new_button = hildon_gtk_button_new(HILDON_SIZE_AUTO);
  gtk_button_set_label(GTK_BUTTON(new_button), "New Item");
  g_signal_connect_after(new_button,
                         "clicked",
                         G_CALLBACK(c_main_window_on_new),
                         GTK_WINDOW(window));

  hildon_app_menu_append(HILDON_APP_MENU(appmenu), GTK_BUTTON(new_button));
  gtk_widget_show_all(appmenu);

  hildon_stackable_window_set_main_menu(HILDON_STACKABLE_WINDOW(window),
                                        HILDON_APP_MENU(appmenu));
}

static void c_main_window_on_new(GtkWidget *widget, GtkWindow *window)
{
  GtkWidget *dialog;
  gint result;
  gchar *name = NULL;
  gint total_qty;

  dialog = interface_show_new_item_dialog(window);

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  if (result == GTK_RESPONSE_OK) {
    const gchar *tmp;
    tmp = c_new_item_dialog_get_name(C_NEW_ITEM_DIALOG(dialog));
    name = (gchar *)malloc(strlen(tmp) + 1);

    strcpy(name, tmp);
    strcat(name, "\0");

    total_qty = c_new_item_dialog_get_total_qty(C_NEW_ITEM_DIALOG(dialog));

    gtk_widget_destroy(dialog);
  }

  if (name != NULL) {
    if (data_add_series(name, total_qty))
      c_main_window_load(C_MAIN_WINDOW(window));
  }
}

static void c_main_window_on_selection_changed(GtkTreeSelection *selection,
                                               gpointer user_data)
{
  gint count;
  CMainWindow *self = (CMainWindow *)user_data;

  count = gtk_tree_selection_count_selected_rows(selection);
  if (count == 0)
    c_main_window_set_no_select(self);
  else
    c_main_window_set_has_select(self);
}

static void c_main_window_on_add_clicked(GtkWidget *widget, gpointer user_data)
{
  CMainWindow *self;
  gint count;

  self = (CMainWindow *)user_data;
  count = gtk_tree_selection_count_selected_rows(self->selection);

  if (count > 0) {
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(self->selection, &model, &self->iter)) {
      gint id;
      gint current_count;

      gtk_tree_model_get(model, &self->iter, COL_ID, &id, -1);
      gtk_tree_model_get(model, &self->iter, COL_CURRENT, &current_count, -1);
      if (data_add_to_series(id, 1))
        gtk_list_store_set(GTK_LIST_STORE(self->store), &self->iter,
                           COL_CURRENT, current_count + 1, -1);

    }
  }
}

static void c_main_window_on_remove_clicked(GtkWidget *widget,
                                            gpointer user_data)
{
  CMainWindow *self;
  gint count;

  self = (CMainWindow *)user_data;
  count = gtk_tree_selection_count_selected_rows(self->selection);

  if (count > 0) {
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(self->selection, &model, &self->iter)) {
      gint id;
      gint current_count;

      gtk_tree_model_get(model, &self->iter, COL_ID, &id, -1);
      gtk_tree_model_get(model, &self->iter, COL_CURRENT, &current_count, -1);

      if (current_count > 0 && data_add_to_series(id, -1))
        gtk_list_store_set(GTK_LIST_STORE(self->store), &self->iter,
                           COL_CURRENT, current_count - 1, -1);
    }
  }
}
