#include <gtk/gtk.h>

struct collection
{
  const gchar *name;
        gint   current_qty;
        gint   total_qty;
};

GList    *data_get_series(void);
gboolean  data_add_series(gchar *name, int total_qty);
