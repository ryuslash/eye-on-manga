#include "c-new-item-dialog.h"
#include <hildon/hildon-entry.h>
#include <gtk/gtk.h>
#include <stdlib.h>

G_DEFINE_TYPE(CNewItemDialog, c_new_item_dialog, GTK_TYPE_DIALOG)

GtkWidget *c_new_item_dialog_new(void)
{
  return g_object_new(C_TYPE_NEW_ITEM_DIALOG, NULL);
}

const gchar *c_new_item_dialog_get_name(CNewItemDialog *dialog)
{
  return hildon_entry_get_text(HILDON_ENTRY(dialog->name_entry));
}

gint c_new_item_dialog_get_total_qty(CNewItemDialog *dialog)
{
  return atoi(hildon_entry_get_text(HILDON_ENTRY(dialog->qty_entry)));
}

static void c_new_item_dialog_class_init(CNewItemDialogClass *class)
{}

static void c_new_item_dialog_init(CNewItemDialog *dialog)
{
  GtkWidget *content_area;
  GtkWidget *hbox;

  content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(content_area), hbox);

  dialog->name_entry = hildon_entry_new(HILDON_SIZE_AUTO);
  hildon_entry_set_placeholder(HILDON_ENTRY(dialog->name_entry), "Name...");
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
