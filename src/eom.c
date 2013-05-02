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
#include <stdlib.h>

#include "eom.h"
#include "interface.h"

gchar *
eom_get_data_file(void)
{
  static gchar *filedir = NULL;

  if (filedir == NULL) {
    filedir = g_strdup_printf("%s/eye-on-manga.db",
                              eom_get_config_dir());
  }

  return filedir;
}

gchar *
eom_get_config_dir(void)
{
  static gchar *filedir = NULL;

  if (filedir == NULL) {
    filedir = g_strdup_printf("%s/.eom", getenv("HOME"));
  }

  return filedir;
}

int main(int argc, char *argv[])
{
  hildon_gtk_init(&argc, &argv);
  g_set_application_name("Eye on Manga");
  interface_show_main_window();
  gtk_main();

  return 0;
}
