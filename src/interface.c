#include <hildon/hildon.h>
#include "interface.h"
#include "c-main-window.h"
#include "c-edit-window.h"
#include "c-new-item-dialog.h"
#include "data.h"

void interface_show_main_window(void)
{
  GtkWidget *window;

  window = c_main_window_new();
  gtk_widget_show_all(window);

  c_main_window_load(C_MAIN_WINDOW(window));
}

void interface_show_edit_window(void)
{
  HildonWindowStack *stack;
  GtkWidget *window;

  stack = hildon_window_stack_get_default();

  window = c_edit_window_new();
  hildon_window_stack_push(stack, HILDON_STACKABLE_WINDOW(window), NULL);
  gtk_widget_show_all(window);
}

GtkWidget *interface_show_new_item_dialog(GtkWindow *window)
{
  GtkWidget *dialog;

  dialog = c_new_item_dialog_new();
  gtk_widget_show_all(dialog);

  gtk_window_set_transient_for(GTK_WINDOW(dialog), window);

  return dialog;
}
