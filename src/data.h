#include <gtk/gtk.h>

struct collection
{
  int   id;
  int   current_qty;
  int   total_qty;
  char *name;
};

GList    *data_get_series(void);
gboolean  data_add_series(gchar *name, gint total_qty);
gboolean  data_add_to_series(gint id, gint count);
