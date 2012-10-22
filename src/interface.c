#include <hildon/hildon.h>
#include "interface.h"
#include "eom-main-window.h"
#include "eom-edit-window.h"
#include "eom-new-item-dialog.h"
#include "data.h"

void interface_show_main_window(void)
{
  GtkWidget *window;

  window = eom_main_window_new();
  gtk_widget_show_all(window);

  eom_main_window_load(EOM_MAIN_WINDOW(window));
}

GtkWidget *interface_show_edit_window(gint manga_id)
{
  GtkWidget *window;

  window = eom_edit_window_new(manga_id);
  gtk_widget_show_all(window);

  return window;
}

GtkWidget *interface_show_new_item_dialog(GtkWindow *window)
{
  GtkWidget *dialog;

  dialog = eom_new_item_dialog_new();
  gtk_widget_show_all(dialog);

  gtk_window_set_transient_for(GTK_WINDOW(dialog), window);

  return dialog;
}
