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
#include <hildon/hildon.h>
#include "interface.h"
#include "eom-main-window.h"
#include "eom-detail-window.h"
#include "eom-new-item-dialog.h"
#include "data.h"

void interface_show_main_window(void)
{
  GtkWidget *window;

  window = eom_main_window_new();
  gtk_widget_show_all(window);

  eom_main_window_load(EOM_MAIN_WINDOW(window));
}

GtkWidget *interface_show_detail_window(gint manga_id)
{
  GtkWidget *window;

  window = eom_detail_window_new(manga_id);
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
