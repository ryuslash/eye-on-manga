#include <gtk/gtk.h>

typedef struct _manga Manga;

struct _manga
{
  int   id;
  int   current_qty;
  int   total_qty;
  char *name;
};

GList    *data_get_manga(void);
Manga    *data_get_manga_by_id(gint manga_id);
gint     *data_get_volumes_by_manga_id(gint manga_id);
gboolean  data_add_manga(gchar *name, gint total_qty);
gboolean  data_add_to_manga(gint id, gint count);
