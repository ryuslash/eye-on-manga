#include "c-main-window.h"
#include <hildon/hildon.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "c-new-item-dialog.h"
#include "interface.h"

G_DEFINE_TYPE(CMainWindow, c_main_window, HILDON_TYPE_STACKABLE_WINDOW)

enum {
  COL_NAME = 0,
  COL_CURRENT,
  COL_TOTAL,
  NUM_COLS
};

static void c_main_window_add_menu(CMainWindow *window);
static void c_main_window_on_new(GtkWidget *widget, GtkWindow *window);

GtkWidget *c_main_window_new(void)
{
  return g_object_new(C_TYPE_MAIN_WINDOW, NULL);
}

void c_main_window_add_line(CMainWindow *window,
                            const gchar *name,
                            gint current_qty,
                            gint total_qty)
{
  gtk_list_store_append(window->store, &window->iter);
  gtk_list_store_set(window ->store, &window->iter,
                     COL_NAME, name,
                     COL_CURRENT, current_qty,
                     COL_TOTAL, total_qty,
                     -1);
}

static void c_main_window_class_init(CMainWindowClass *class)
{}

static void c_main_window_init(CMainWindow *window)
{
  GtkCellRenderer   *renderer;
  GtkWidget         *view;
  GtkTreeViewColumn *current_column;
  int                index;

  index = -1;

  c_main_window_add_menu(window);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  window->store = gtk_list_store_new(NUM_COLS,
                                     G_TYPE_STRING,
                                     G_TYPE_INT,
                                     G_TYPE_INT);

  view = gtk_tree_view_new();

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
  gtk_object_set(GTK_OBJECT(renderer), "xalign", 1.0, NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              ++index,
                                              "",
                                              renderer,
                                              "text", COL_CURRENT,
                                              NULL);

  renderer = gtk_cell_renderer_text_new();
  gtk_object_set(GTK_OBJECT(renderer), "xalign", 1.0, NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                              ++index,
                                              "",
                                              renderer,
                                              "text", COL_TOTAL,
                                              NULL);

  gtk_tree_view_set_model(GTK_TREE_VIEW(view),
                          GTK_TREE_MODEL(window->store));
  g_object_unref(window->store);

  gtk_container_add(GTK_CONTAINER(window), view);
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
    c_main_window_add_line(C_MAIN_WINDOW(window), name, 0, total_qty);
  }
}
