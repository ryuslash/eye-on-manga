#include "eom-detail-window.h"

#include <hildon/hildon.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "data.h"
#include "eom-main-window.h"

#define COLUMNS 8

enum {
  EOM_EDIT_PROP_0,
  EOM_EDIT_PROP_CID
};

static GtkWidget *create_volume_button(gchar*, GtkTable*, int, int);
static void eom_detail_window_class_init(EomDetailWindowClass*);
static void eom_detail_window_init(EomDetailWindow *self);
static void finalize(GObject*);
static void get_property(GObject*, guint, GValue*, GParamSpec*);
static void on_volume_read_toggled(GtkToggleButton*, gpointer);
static void on_volume_toggled(GtkToggleButton*, gpointer);
static void set_manga_id(EomDetailWindow*, gint);
static void set_property(GObject*, guint, const GValue*, GParamSpec*);

G_DEFINE_TYPE(EomDetailWindow, eom_detail_window,
              HILDON_TYPE_STACKABLE_WINDOW)

GtkWidget *
eom_detail_window_new(gint manga_id)
{
    return g_object_new(EOM_TYPE_EDIT_WINDOW,
                        "manga-id", manga_id,
                        NULL);
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
    GParamSpec   *pspec;

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

    panarea = hildon_pannable_area_new();
    g_object_set(G_OBJECT(panarea),
                 "mov-mode", HILDON_MOVEMENT_MODE_VERT,
                 NULL);
    gtk_container_add(GTK_CONTAINER(self), panarea);

    self->volsbox = gtk_vbox_new(FALSE, 0);
    hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA(panarea),
                                           self->volsbox);

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
            data_add_to_manga(self->manga->id, 1);
            return;
        }
        self->manga->current_qty++;
    }
    else {
        /* Remove 1 from mangas collected */
        if (!data_add_to_manga(self->manga->id, -1)) {
            return;
        }
        if (!data_remove_volume_from_manga(self->manga->id,
                                           volume)) {
            data_add_to_manga(self->manga->id, 1); /* Undo */
            return;
        }
        self->manga->current_qty--;
    }
}

static void
set_manga_id(EomDetailWindow *self, gint manga_id)
{
    GtkWidget *clabel, *rlabel;
    GtkWidget *ctable, *rtable;
    Manga *manga;
    gint i, j = 0, row = 0, col = 0;

    manga = data_get_manga_by_id(manga_id);
    data_get_volumes_for_manga(manga);
    self->manga = manga;

    gtk_window_set_title(GTK_WINDOW(self), manga->name);

    clabel = gtk_label_new("Collected:");
    gtk_misc_set_alignment(GTK_MISC(clabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(self->volsbox), clabel, FALSE, FALSE, 0);

    ctable = gtk_table_new((int)floor(manga->total_qty / COLUMNS),
                           COLUMNS, TRUE);
    gtk_box_pack_start(GTK_BOX(self->volsbox), ctable, FALSE, FALSE, 0);

    rlabel = gtk_label_new("Read:");
    gtk_misc_set_alignment(GTK_MISC(rlabel), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(self->volsbox), rlabel, FALSE, FALSE, 0);

    rtable = gtk_table_new((int)floor(manga->total_qty / COLUMNS),
                           COLUMNS, TRUE);
    gtk_box_pack_start(GTK_BOX(self->volsbox), rtable, FALSE, FALSE, 0);

    for (i = 0; i < manga->total_qty; i++) {
        GtkWidget *cbtn, *rbtn;
        gchar *txt;

        if (i > 0 && i % COLUMNS == 0) {
            row++;
            col = 0;
        }

        txt = g_strdup_printf("%d", i + 1);

        cbtn = create_volume_button(txt, GTK_TABLE(ctable), col, row);
        rbtn = create_volume_button(txt, GTK_TABLE(rtable), col, row);

        if (j < manga->vol_count && manga->volumes[j].number == i + 1) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbtn), TRUE);

            if (manga->volumes[j].read)
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
