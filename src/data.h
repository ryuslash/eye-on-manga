#include <gtk/gtk.h>

struct collection
{
  int   id;
  int   current_qty;
  int   total_qty;
  char *name;
};

GList             *data_get_series(void);
struct collection *data_get_series_by_id(gint collection_id);
gint              *data_get_items_by_collection_id(gint collection_id);
gboolean           data_add_series(gchar *name, gint total_qty);
gboolean           data_add_to_series(gint id, gint count);
