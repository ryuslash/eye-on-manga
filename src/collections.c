#include <hildon/hildon.h>
#include "interface.h"

int main(int argc, char *argv[])
{
  hildon_gtk_init(&argc, &argv);

  g_set_application_name("Collections");

  interface_show_main_window();

  gtk_main();

  return 0;
}
