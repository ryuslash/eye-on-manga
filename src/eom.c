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
