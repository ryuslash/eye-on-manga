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

    GtkWidget *name_entry;
    GtkWidget *have_label;
    GtkWidget *total_entry;
    GtkWidget *volsbox;

    Manga *manga;
} EomDetailWindow;

GType eom_detail_window_get_type(void);
GtkWidget *eom_detail_window_new(gint);

G_END_DECLS

#endif /* __EOM_DETAIL_WINDOW_H__ */
