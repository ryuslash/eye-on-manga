#include "eom-detail-window.h"

#include <hildon/hildon.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "data.h"
#include "eom-main-window.h"
#include "eom-new-item-dialog.h"
#include "interface.h"

#define COLUMNS 4

enum {
  EOM_EDIT_PROP_0,
  EOM_EDIT_PROP_CID
};

static void add_menu(EomDetailWindow*);
static void cb_destroy(GtkWidget*, gpointer);
static GtkWidget *create_volume_button(gchar*, GtkTable*, int, int);
static void eom_detail_window_class_init(EomDetailWindowClass*);
static void eom_detail_window_init(EomDetailWindow *self);
static void finalize(GObject*);
static void get_property(GObject*, guint, GValue*, GParamSpec*);
static void on_delete(GtkWidget*, gpointer);
static void on_edit(GtkWidget*, EomDetailWindow*);
static void on_volume_read_toggled(GtkToggleButton*, gpointer);
static void on_volume_toggled(GtkToggleButton*, gpointer);
static void set_manga_id(EomDetailWindow*, gint);
static void set_property(GObject*, guint, const GValue*, GParamSpec*);

G_DEFINE_TYPE(EomDetailWindow, eom_detail_window,
              HILDON_TYPE_STACKABLE_WINDOW);

void
eom_detail_window_load(EomDetailWindow *self)
{
    gint i, j = 0, row = 0, col = 0;
    int total_qty = self->manga->total_qty;
    int rows;

    if (!total_qty)
        total_qty = self->manga->current_qty + 1;

    rows = (int)ceil(total_qty / COLUMNS);

    gtk_window_set_title(GTK_WINDOW(self), self->manga->name);
    gtk_container_foreach(GTK_CONTAINER(self->ctable), cb_destroy, NULL);
    gtk_table_resize(GTK_TABLE(self->ctable), rows, COLUMNS);
    gtk_container_foreach(GTK_CONTAINER(self->rtable), cb_destroy, NULL);
    gtk_table_resize(GTK_TABLE(self->rtable), rows, COLUMNS);

    for (i = 0; i < total_qty; i++) {
        GtkWidget *cbtn, *rbtn;
        gchar *txt;

        if (i > 0 && i % COLUMNS == 0) {
            row++;
            col = 0;
        }

        txt = g_strdup_printf("%d", i + 1);
        cbtn = create_volume_button(txt, GTK_TABLE(self->ctable), col,
                                    row);
        rbtn = create_volume_button(txt, GTK_TABLE(self->rtable), col,
                                    row);

        if (j < self->manga->vol_count
            && self->manga->volumes[j].number == i + 1) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtn), TRUE);

            if (self->manga->volumes[j].read)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtn),
                                             TRUE);

            j++;
        }

        g_signal_connect(cbtn, "toggled", G_CALLBACK(on_volume_toggled),
                         (gpointer)self);
        g_signal_connect(rbtn, "toggled",
                         G_CALLBACK(on_volume_read_toggled),
                         (gpointer)self);

        g_free(txt);
        col++;
    }

    gtk_widget_show_all(self->volsbox);
}

GtkWidget *
eom_detail_window_new(gint manga_id)
{
    return g_object_new(EOM_TYPE_EDIT_WINDOW,
                        "manga-id", manga_id,
                        NULL);
}

static void
add_menu(EomDetailWindow *window)
{
    HildonAppMenu *appmenu;
    GtkWidget *button;

    appmenu = HILDON_APP_MENU(hildon_app_menu_new());

    button = hildon_gtk_button_new(HILDON_SIZE_AUTO);
    gtk_button_set_label(GTK_BUTTON(button), "Edit...");
    g_signal_connect_after(button, "clicked", G_CALLBACK(on_edit),
                           window);
    hildon_app_menu_append(appmenu, GTK_BUTTON(button));

    button = hildon_gtk_button_new(HILDON_SIZE_AUTO);
    gtk_button_set_label(GTK_BUTTON(button), "Delete...");
    g_signal_connect_after(button, "clicked", G_CALLBACK(on_delete),
                           (gpointer)window);
    hildon_app_menu_append(appmenu, GTK_BUTTON(button));

    gtk_widget_show_all(GTK_WIDGET(appmenu));
    hildon_stackable_window_set_main_menu(
        HILDON_STACKABLE_WINDOW(window), appmenu
    );
}

static void
cb_destroy(GtkWidget *widget, gpointer user_data)
{
    gtk_widget_destroy(widget);
}

static GtkWidget *
create_volume_button(gchar* text, GtkTable* table, int column, int row)
{
    GtkWidget *btn;

    btn = hildon_gtk_toggle_button_new(HILDON_SIZE_THUMB_HEIGHT
                                       | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label(GTK_BUTTON(btn), text);
    gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(btn), FALSE);
    gtk_table_attach(GTK_TABLE(table), btn,
                     column, column + 1,
                     row, row + 1,
                     GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL,
                     0, 0);

    return btn;
}

static void
eom_detail_window_class_init(EomDetailWindowClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GParamSpec *pspec;

    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    gobject_class->finalize = finalize;
    pspec = g_param_spec_int("manga-id", "ID of the manga",
                             "Set the manga-id", 0, INT_MAX, 0,
                             G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    g_object_class_install_property(gobject_class, EOM_EDIT_PROP_CID,
                                    pspec);
}

static void
eom_detail_window_init(EomDetailWindow *self)
{
    GtkWidget *panarea;
    GtkWidget *clabel, *rlabel;
    GtkWidget *vbox;

    add_menu(self);

    panarea = hildon_pannable_area_new();
    g_object_set(G_OBJECT(panarea),
                 "mov-mode", HILDON_MOVEMENT_MODE_VERT,
                 NULL);
    gtk_container_add(GTK_CONTAINER(self), panarea);

    self->volsbox = gtk_hbox_new(TRUE, 5);
    hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA(panarea),
                                           self->volsbox);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(self->volsbox), vbox, TRUE, TRUE, 0);

    clabel = gtk_label_new("Collected:");
    gtk_misc_set_alignment(GTK_MISC(clabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), clabel, FALSE, FALSE, 0);

    self->ctable = gtk_table_new(0, 0, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), self->ctable, FALSE, FALSE, 0);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(self->volsbox), vbox, TRUE, TRUE, 0);

    rlabel = gtk_label_new("Read:");
    gtk_misc_set_alignment(GTK_MISC(rlabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), rlabel, FALSE, FALSE, 0);

    self->rtable = gtk_table_new(0, 0, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), self->rtable, FALSE, FALSE, 0);
}

static void
finalize(GObject *object)
{
    EomDetailWindow *self = EOM_DETAIL_WINDOW(object);

    g_free(self->manga);

    G_OBJECT_CLASS(eom_detail_window_parent_class)->finalize(object);
}

static void
get_property(GObject *object, guint property_id, GValue *value,
             GParamSpec *pspec)
{
    EomDetailWindow *self = EOM_DETAIL_WINDOW(object);

    switch (property_id) {
    case EOM_EDIT_PROP_CID:
        g_value_set_int(value, self->manga->id);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
on_delete(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog, *label, *content_area;
    EomDetailWindow *self = EOM_DETAIL_WINDOW(user_data);
    gint result;
    gchar *ttxt, *ctxt;

    ttxt = g_strdup_printf("Delete %s", self->manga->name);
    ctxt = g_strdup_printf("Really delete \"%s\"?", self->manga->name);
    dialog = gtk_dialog_new_with_buttons(
        ttxt, GTK_WINDOW(self), GTK_DIALOG_MODAL,
        GTK_STOCK_OK, GTK_RESPONSE_OK,
        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
        NULL
    );
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(ctxt);

    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show(label);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);
    g_free(ttxt);
    g_free(ctxt);

    if (result == GTK_RESPONSE_OK) {
        if (data_delete_manga(self->manga->id)) {
            gtk_widget_hide(GTK_WIDGET(self));
            gtk_widget_destroy(GTK_WIDGET(self));
        }
    }
}

static void
on_edit(GtkWidget *widget, EomDetailWindow *self)
{
    GtkWidget *dialog;
    gint result;
    const gchar *name = NULL;
    gint total_qty;

    dialog = interface_show_new_item_dialog(GTK_WINDOW(self));
    g_object_set(G_OBJECT(dialog), "manga-id", self->manga->id, NULL);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK) {
        name = eom_new_item_dialog_get_name(EOM_NEW_ITEM_DIALOG(dialog));
        total_qty = eom_new_item_dialog_get_total_qty(EOM_NEW_ITEM_DIALOG(dialog));
    }

    if (name != NULL) {
        if (data_update_manga(self->manga->id, name, total_qty))
            set_manga_id(self, self->manga->id);
    }

    gtk_widget_destroy(dialog);
}

static void
on_volume_read_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    EomDetailWindow *self = (EomDetailWindow *)user_data;
    gboolean active = gtk_toggle_button_get_active(togglebutton);
    gint volume = atoi(gtk_button_get_label(GTK_BUTTON(togglebutton)));

    if (!data_mark_volume_read(active, self->manga->id, volume))
        g_print("coulnd't mark volume as read\n");
}

static void
on_volume_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    EomDetailWindow *self = (EomDetailWindow *)user_data;
    gboolean active = gtk_toggle_button_get_active(togglebutton);
    gint volume = atoi(gtk_button_get_label(GTK_BUTTON(togglebutton)));

    if (active) {
        /* Add 1 to mangas collected */
        if (!data_add_to_manga(self->manga->id, 1)) {
            return;
        }
        if (!data_add_volume_to_manga(self->manga->id, volume)) {
            data_add_to_manga(self->manga->id, -1);
            return;
        }
    }
    else {
        /* Remove 1 from mangas collected */
        if (!data_add_to_manga(self->manga->id, -1)) {
            return;
        }
        if (!data_remove_volume_from_manga(self->manga->id, volume)) {
            data_add_to_manga(self->manga->id, 1); /* Undo */
            return;
        }
    }

    set_manga_id(self, self->manga->id);
}

static void
set_manga_id(EomDetailWindow *self, gint manga_id)
{
    Manga *manga;

    manga = data_get_manga_by_id(manga_id);
    data_get_volumes_for_manga(manga);
    self->manga = manga;

    eom_detail_window_load(self);
}

static void
set_property(GObject *object, guint property_id, const GValue *value,
             GParamSpec *pspec)
{
    EomDetailWindow *self = EOM_DETAIL_WINDOW(object);
    gint manga_id = g_value_get_int(value);

    switch (property_id) {
    case EOM_EDIT_PROP_CID:
        set_manga_id(self, manga_id);
        break;
    default:
        /* We don't have any other properties */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
