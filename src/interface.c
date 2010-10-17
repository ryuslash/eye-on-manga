#include <hildon/hildon.h>
#include "interface.h"
#include "c-main-window.h"
#include "c-new-item-dialog.h"

void interface_show_main_window(void)
{
  GtkWidget *window;

  window = c_main_window_new();
  gtk_widget_show_all(window);

  c_main_window_add_line(C_MAIN_WINDOW(window), "Biomega",  2, 0);
  c_main_window_add_line(C_MAIN_WINDOW(window), "Blame!",   7, 10);
  c_main_window_add_line(C_MAIN_WINDOW(window), "Hellsing", 2, 14);
}

GtkWidget *interface_show_new_item_dialog(GtkWindow *window)
{
  GtkWidget *dialog;

  dialog = c_new_item_dialog_new();
  gtk_widget_show_all(dialog);

  gtk_window_set_transient_for(GTK_WINDOW(dialog), window);

  return dialog;
}
