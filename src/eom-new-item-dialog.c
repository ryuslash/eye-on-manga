#include "eom-new-item-dialog.h"

#include <gtk/gtk.h>
#include <hildon/hildon-entry.h>
#include <stdlib.h>

static void eom_new_item_dialog_class_init(EomNewItemDialogClass*);
static void eom_new_item_dialog_init(EomNewItemDialog*);

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
eom_new_item_dialog_class_init(EomNewItemDialogClass *class)
{}

static void
eom_new_item_dialog_init(EomNewItemDialog *dialog)
{
    GtkWidget *content_area;
    GtkWidget *hbox;

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

    gtk_window_set_title(GTK_WINDOW(dialog), "New item");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                           GTK_STOCK_OK,
                           GTK_RESPONSE_OK,
                           NULL);
}
