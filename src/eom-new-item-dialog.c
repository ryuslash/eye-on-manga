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
#include "eom-new-item-dialog.h"

#include <gtk/gtk.h>
#include <hildon/hildon-entry.h>
#include <stdlib.h>

#include "data.h"

enum {
    EOM_NEW_PROP_MID = 1
};

static void change(GtkButton*,GtkEntry*);
static void eom_new_item_dialog_class_init(EomNewItemDialogClass*);
static void eom_new_item_dialog_init(EomNewItemDialog*);
static void finalize(GObject*);
static void get_property(GObject*, guint, GValue*, GParamSpec*);
static void set_manga_id(EomNewItemDialog*, gint);
static void set_property(GObject*, guint, const GValue*, GParamSpec*);

G_DEFINE_TYPE(EomNewItemDialog, eom_new_item_dialog, GTK_TYPE_DIALOG)

const gchar *
eom_new_item_dialog_get_name(EomNewItemDialog *dialog)
{
    return hildon_entry_get_text(HILDON_ENTRY(dialog->name_entry));
}

gint
eom_new_item_dialog_get_total_qty(EomNewItemDialog *dialog)
{
    return atoi(hildon_entry_get_text(HILDON_ENTRY(dialog->qty_entry)));
}

GtkWidget *
eom_new_item_dialog_new(void)
{
    return g_object_new(EOM_TYPE_NEW_ITEM_DIALOG, NULL);
}

static void
change(GtkButton *button, GtkEntry *entry)
{
    gchar action, *txt;
    int current_value;
    int delta;

    action = gtk_button_get_label(button)[0];
    current_value = atoi(gtk_entry_get_text(entry));

    switch (action) {
    case '-':
        delta = -1;
        break;
    case '+':
        delta = +1;
        break;
    default:
        delta = 0;
        break;
    }

    txt = g_strdup_printf("%d", current_value + delta);
    gtk_entry_set_text(entry, txt);
    g_free(txt);
}

static void
eom_new_item_dialog_class_init(EomNewItemDialogClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GParamSpec *pspec;

    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;
    gobject_class->finalize = finalize;
    pspec = g_param_spec_int("manga-id", "ID of the manga",
                             "Set the manga-id", 0, INT_MAX, 0,
                             G_PARAM_WRITABLE);

    g_object_class_install_property(gobject_class, EOM_NEW_PROP_MID,
                                    pspec);
}

static void
eom_new_item_dialog_init(EomNewItemDialog *dialog)
{
    GtkWidget *content_area;
    GtkWidget *hbox;
    GtkWidget *pbutton, *mbutton;

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(content_area), hbox);

    dialog->name_entry = hildon_entry_new(HILDON_SIZE_AUTO);
    hildon_entry_set_placeholder(HILDON_ENTRY(dialog->name_entry),
                                 "Name...");
    gtk_box_pack_start(GTK_BOX(hbox), dialog->name_entry, TRUE, TRUE, 0);

    dialog->qty_entry = hildon_entry_new(HILDON_SIZE_AUTO);
    g_object_set(G_OBJECT(dialog->qty_entry), "width-chars", 5, NULL);
    hildon_entry_set_text(HILDON_ENTRY(dialog->qty_entry), "0");
    gtk_box_pack_start(GTK_BOX(hbox), dialog->qty_entry, TRUE, TRUE, 0);

    mbutton = hildon_gtk_button_new(HILDON_SIZE_AUTO);
    gtk_button_set_label(GTK_BUTTON(mbutton), "-");
    g_signal_connect_after(mbutton, "clicked", G_CALLBACK(change),
                           dialog->qty_entry);
    gtk_box_pack_start(GTK_BOX(hbox), mbutton, FALSE, FALSE, 0);

    pbutton = hildon_gtk_button_new(HILDON_SIZE_AUTO);
    gtk_button_set_label(GTK_BUTTON(pbutton), "+");
    g_signal_connect_after(pbutton, "clicked", G_CALLBACK(change),
                           dialog->qty_entry);
    gtk_box_pack_start(GTK_BOX(hbox), pbutton, FALSE, FALSE, 0);

    gtk_window_set_title(GTK_WINDOW(dialog), "New item");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                           GTK_STOCK_OK,
                           GTK_RESPONSE_OK,
                           NULL);
}

static void
finalize(GObject *object)
{
    EomNewItemDialog *self = EOM_NEW_ITEM_DIALOG(object);

    g_free(self->manga);

    G_OBJECT_CLASS(eom_new_item_dialog_parent_class)->finalize(object);
}

static void
get_property(GObject *object, guint property_id, GValue *value,
             GParamSpec *pspec)
{
    EomNewItemDialog *self = EOM_NEW_ITEM_DIALOG(object);

    switch(property_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void
set_manga_id(EomNewItemDialog *self, gint manga_id)
{
    Manga *manga = data_get_manga_by_id(manga_id);
    gchar *txt;
    self->manga = manga;
    txt = g_strdup_printf("%d", manga->total_qty);

    gtk_entry_set_text(GTK_ENTRY(self->name_entry), manga->name);
    gtk_entry_set_text(GTK_ENTRY(self->qty_entry), txt);

    g_free(txt);
}

static void
set_property(GObject *object, guint property_id, const GValue *value,
             GParamSpec *pspec)
{
    EomNewItemDialog *self = EOM_NEW_ITEM_DIALOG(object);
    gint manga_id = g_value_get_int(value);

    switch (property_id) {
    case EOM_NEW_PROP_MID:
        set_manga_id(self, manga_id);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}
