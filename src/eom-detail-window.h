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
#ifndef __EOM_DETAIL_WINDOW_H__
#define __EOM_DETAIL_WINDOW_H__

#include <hildon/hildon-stackable-window.h>

#include "data.h"

G_BEGIN_DECLS

#define EOM_TYPE_EDIT_WINDOW                    \
    (eom_detail_window_get_type())

#define EOM_DETAIL_WINDOW(obj)                          \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj),                 \
                                 EOM_TYPE_EDIT_WINDOW,  \
                                 EomDetailWindow))

#define EOM_DETAIL_WINDOW_CLASS(klass)              \
    (G_TYPE_CHECK_CLASS_CAST ((klass),              \
                              EOM_TYPE_EDIT_WINDOW, \
                              EomDetailWindowClass))

#define EOM_IS_EDIT_WINDOW(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                 \
                                 EOM_TYPE_EDIT_WINDOW))

#define EOM_IS_EDIT_WINDOW_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE ((klass),                  \
                              EOM_TYPE_EDIT_WINDOW))

#define EOM_DETAIL_WINDOW_GET_CLASS(obj)                \
    (G_TYPE_INSTANCE_GET_CLASS ((obj),                  \
                                EOM_TYPE_EDIT_WINDOW,   \
                                EomDetailWindowClass))

typedef struct
{
    HildonStackableWindowClass parent_class;
} EomDetailWindowClass;

typedef struct
{
    HildonStackableWindow parent;
    GtkWidget *volsbox;
    GtkWidget *ctable;
    GtkWidget *rtable;
    Manga *manga;
} EomDetailWindow;

GType eom_detail_window_get_type(void);
void eom_detail_window_load(EomDetailWindow*);
GtkWidget *eom_detail_window_new(gint);

G_END_DECLS

#endif /* __EOM_DETAIL_WINDOW_H__ */
