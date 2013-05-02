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
gboolean data_delete_manga(gint);
GList *data_get_incomplete_manga(void);
GList *data_get_manga(void);
Manga *data_get_manga_by_id(gint);
GList *data_get_unread_manga(void);
void data_get_volumes_for_manga(Manga*);
gboolean data_mark_volume_read(gint, gint, gint);
gboolean data_remove_volume_from_manga(gint, gint);
gboolean data_update_manga(gint, const gchar*, gint);

#endif /* __DATA_H__ */
