#ifndef __DATA_H__
#define __DATA_H__

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
void      data_get_volumes_by_manga_id(gint manga_id, gint *size, gint **vols);
gboolean  data_add_manga(gchar *name, gint total_qty);
gboolean  data_add_to_manga(gint id, gint count);
gboolean  data_add_volume_to_manga(gint manga_id, gint volume);
gboolean  data_remove_volume_from_manga(gint manga_id, gint volume);

#endif /* __DATA_H__ */
