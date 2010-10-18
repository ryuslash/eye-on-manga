#include <hildon/hildon.h>
#include <stdlib.h>
#include "interface.h"
#include "collections.h"

gchar *collections_get_data_file(void)
{
  static gchar *filedir = NULL;

  if (filedir == NULL) {
    filedir = g_strdup_printf("%s/collections.db",
                              collections_get_config_dir());
  }

  return filedir;
}

gchar *collections_get_config_dir(void)
{
  static gchar *filedir = NULL;

  if (filedir == NULL) {
    filedir = g_strdup_printf("%s/.collections", getenv("HOME"));
  }

  return filedir;
}

int main(int argc, char *argv[])
{
  hildon_gtk_init(&argc, &argv);

  g_set_application_name("Collections");

  interface_show_main_window();

  gtk_main();

  return 0;
}
