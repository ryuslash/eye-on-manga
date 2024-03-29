/* Eye on Manga - Manga collection software for Maemo 5
   Copyright (C) 2010-2013  Tom Willemse

   Eye on Manga is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Eye on Manga is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Eye on Manga.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "eom-main-window.h"

#include <string.h>
#include <stdlib.h>

#include <hildon/hildon.h>
#include <gtk/gtk.h>

#include "data.h"
#include "eom-new-item-dialog.h"
#include "interface.h"

G_DEFINE_TYPE(EomMainWindow, eom_main_window,
              HILDON_TYPE_STACKABLE_WINDOW)

enum {
    COL_ID = 0,
    COL_NAME,
    COL_CURRENT,
    COL_TOTAL,
    NUM_COLS
};

enum {
    STATE_NORMAL = 0,
    STATE_COLLECT,
    STATE_READ
};

struct filter_args {
    EomMainWindow *window;
    gint state;
};

static void add_menu(EomMainWindow*);
static void eom_main_window_class_init(EomMainWindowClass*);
static void eom_main_window_init(EomMainWindow*);
static void on_detail_hidden(GtkWidget*, gpointer);
static void on_filter(GtkWidget*, struct filter_args*);
static void on_new(GtkWidget*, GtkWindow*);
static void on_row_activated(GtkWidget*, GtkTreePath*, GtkTreeViewColumn*, gpointer);

void
eom_main_window_add_line(EomMainWindow *window, gint id,
                         const gchar *name, gint current_qty,
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

void
eom_main_window_load(EomMainWindow *self)
{
    GList *list;
    int i;

    gtk_list_store_clear(self->store);

    switch (self->state) {
    case STATE_COLLECT:
        list = data_get_incomplete_manga();
        break;
    case STATE_READ:
        list = data_get_unread_manga();
        break;
    default:
        list = data_get_manga();
        break;
    }

    while (list) {
        Manga *manga = list->data;

        eom_main_window_add_line(self, manga->id, manga->name,
                                 manga->current_qty, manga->total_qty);
        free(manga->name);

        if (manga->volumes != NULL) {
            for (i = 0; i < manga->vol_count; i++)
                free(manga->volumes + i);
        }

        free(manga);
        list = g_list_next(list);
    }

    g_list_free_1(list);
}

GtkWidget *
eom_main_window_new(void)
{
    return g_object_new(EOM_TYPE_MAIN_WINDOW, NULL);
}

static void
add_menu(EomMainWindow *window)
{
    HildonAppMenu *appmenu;
    GtkWidget *new_button;
    GtkWidget *all_filter, *collect_filter, *read_filter;
    struct filter_args *args0 = malloc(sizeof(struct filter_args));
    struct filter_args *args1 = malloc(sizeof(struct filter_args));
    struct filter_args *args2 = malloc(sizeof(struct filter_args));

    args0->window = window;
    args0->state = STATE_NORMAL;
    args1->window = window;
    args1->state = STATE_COLLECT;
    args2->window = window;
    args2->state = STATE_READ;
    appmenu = HILDON_APP_MENU(hildon_app_menu_new());

    new_button = hildon_gtk_button_new(HILDON_SIZE_AUTO);
    gtk_button_set_label(GTK_BUTTON(new_button), "New...");
    g_signal_connect_after(new_button, "clicked", G_CALLBACK(on_new),
                           GTK_WINDOW(window));
    hildon_app_menu_append(appmenu, GTK_BUTTON(new_button));

    all_filter = hildon_gtk_radio_button_new(HILDON_SIZE_AUTO, NULL);
    gtk_button_set_label(GTK_BUTTON(all_filter), "All");
    g_signal_connect_after(all_filter, "clicked", G_CALLBACK(on_filter),
                           args0);
    hildon_app_menu_add_filter(appmenu, GTK_BUTTON(all_filter));
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(all_filter), FALSE);

    collect_filter =
        hildon_gtk_radio_button_new_from_widget(HILDON_SIZE_AUTO,
                                                GTK_RADIO_BUTTON(all_filter));
    gtk_button_set_label(GTK_BUTTON(collect_filter), "Collect");
    g_signal_connect_after(collect_filter, "clicked",
                           G_CALLBACK(on_filter), args1);
    hildon_app_menu_add_filter(appmenu, GTK_BUTTON(collect_filter));
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(collect_filter), FALSE);

    read_filter =
        hildon_gtk_radio_button_new_from_widget(HILDON_SIZE_AUTO,
                                                GTK_RADIO_BUTTON(all_filter));
    gtk_button_set_label(GTK_BUTTON(read_filter), "Read");
    g_signal_connect_after(read_filter, "clicked",
                           G_CALLBACK(on_filter), args2);
    hildon_app_menu_add_filter(appmenu, GTK_BUTTON(read_filter));
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(read_filter), FALSE);

    gtk_widget_show_all(GTK_WIDGET(appmenu));

    hildon_stackable_window_set_main_menu(HILDON_STACKABLE_WINDOW(window),
                                          HILDON_APP_MENU(appmenu));
}

static void
eom_main_window_class_init(EomMainWindowClass *class)
{}

static void
eom_main_window_init(EomMainWindow *window)
{
    GtkCellRenderer   *renderer;
    GtkWidget         *view;
    GtkWidget         *pannablearea;
    GtkTreeViewColumn *current_column;
    GtkTreeSelection  *selection;
    int                index;

    index = -1;

    add_menu(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit),
                     NULL);

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
    g_signal_connect(view, "row-activated", G_CALLBACK(on_row_activated),
                     window);
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
    current_column =
        gtk_tree_view_get_column(GTK_TREE_VIEW(view), index);
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

static void
on_detail_hidden(GtkWidget *widget, gpointer user_data)
{
    EomMainWindow *self = EOM_MAIN_WINDOW(user_data);
    eom_main_window_load(self);
}

static void
on_filter(GtkWidget *widget, struct filter_args *arg)
{
    arg->window->state = arg->state;
    eom_main_window_load(arg->window);
}

static void
on_new(GtkWidget *widget, GtkWindow *window)
{
    GtkWidget *dialog;
    gint result;
    const gchar *name = NULL;
    gint total_qty;

    dialog = interface_show_new_item_dialog(window);
    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK) {
        name = eom_new_item_dialog_get_name(EOM_NEW_ITEM_DIALOG(dialog));
        total_qty = eom_new_item_dialog_get_total_qty(EOM_NEW_ITEM_DIALOG(dialog));
    }

    if (name != NULL) {
        if (data_add_manga(name, total_qty))
            eom_main_window_load(EOM_MAIN_WINDOW(window));
        else
            hildon_banner_show_information(window, NULL,
                                           "Could not add manga");
    }

    gtk_widget_destroy(dialog);
}

static void
on_row_activated(GtkWidget *treeview, GtkTreePath *path,
                 GtkTreeViewColumn *column, gpointer user_data)
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
    window = interface_show_detail_window(id);
    g_signal_connect(window, "hide", G_CALLBACK(on_detail_hidden), self);
}
