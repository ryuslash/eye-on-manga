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
#ifndef __EOM_MAIN_WINDOW_H__
#define __EOM_MAIN_WINDOW_H__

#include <hildon/hildon-stackable-window.h>

G_BEGIN_DECLS

#define EOM_TYPE_MAIN_WINDOW                    \
    (eom_main_window_get_type())

#define EOM_MAIN_WINDOW(obj)                            \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj),                 \
                                 EOM_TYPE_MAIN_WINDOW,  \
                                 EomMainWindow))

#define EOM_MAIN_WINDOW_CLASS(klass)                \
    (G_TYPE_CHECK_CLASS_CAST ((klass),              \
                              EOM_TYPE_MAIN_WINDOW, \
                              EomMainWindowClass))

#define EOM_IS_MAIN_WINDOW(obj)                         \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                 \
                                 EOM_TYPE_MAIN_WINDOW))

#define EOM_IS_MAIN_WINDOW_CLASS(klass)                 \
    (G_TYPE_CHECK_CLASS_TYPE ((klass),                  \
                              EOM_TYPE_MAIN_WINDOW))

#define EOM_MAIN_WINDOW_GET_CLASS(obj)                  \
    (G_TYPE_INSTANCE_GET_CLASS ((obj),                  \
                                EOM_TYPE_MAIN_WINDOW,   \
                                EomMainWindowClass))

typedef struct
{
    HildonStackableWindowClass parent_class;
} EomMainWindowClass;

typedef struct
{
    HildonStackableWindow parent;

    GtkTreeIter           iter;
    GtkListStore         *store;

    int state;
} EomMainWindow;

void eom_main_window_add_line(EomMainWindow*, gint, const gchar*, gint,
                              gint);
GType eom_main_window_get_type(void);
void eom_main_window_load(EomMainWindow*);
GtkWidget *eom_main_window_new(void);

G_END_DECLS

#endif /* __EOM_MAIN_WINDOW_H__ */
