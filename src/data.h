#ifndef __DATA_H__
#define __DATA_H__

#include <gtk/gtk.h>

typedef struct
{
    int number;
    int read;
} Volume;

typedef struct
{
    int id;
    int current_qty;
    int total_qty;
    char *name;
    int vol_count;
    Volume *volumes;
} Manga;

gboolean data_add_manga(const gchar*, gint);
gboolean data_add_to_manga(gint, gint);
gboolean data_add_volume_to_manga(gint, gint);
GList *data_get_incomplete_manga(void);
GList *data_get_manga(void);
Manga *data_get_manga_by_id(gint);
GList *data_get_unread_manga(void);
void data_get_volumes_for_manga(Manga*);
gboolean data_mark_volume_read(gint, gint, gint);
gboolean data_remove_volume_from_manga(gint, gint);
gboolean data_update_manga(gint, const gchar*, gint);

#endif /* __DATA_H__ */
