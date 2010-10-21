#ifndef __EOM_EDIT_WINDOW_H__
#define __EOM_EDIT_WINDOW_H__

#include <hildon/hildon-stackable-window.h>

G_BEGIN_DECLS

#define EOM_TYPE_EDIT_WINDOW                    \
  (eom_edit_window_get_type())

#define EOM_EDIT_WINDOW(obj)                            \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                   \
                               EOM_TYPE_EDIT_WINDOW,    \
                               EomEditWindow))

#define EOM_EDIT_WINDOW_CLASS(klass)                    \
  (G_TYPE_CHECK_CLASS_CAST ((klass),                    \
                            EOM_TYPE_EDIT_WINDOW,       \
                            EomEditWindowClass))

#define EOM_IS_EDIT_WINDOW(obj)                         \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               EOM_TYPE_EDIT_WINDOW))

#define EOM_IS_EDIT_WINDOW_CLASS(klass)                 \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                    \
                            EOM_TYPE_EDIT_WINDOW))

#define EOM_EDIT_WINDOW_GET_CLASS(obj)                  \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              EOM_TYPE_EDIT_WINDOW,     \
                              EomEditWindowClass))

typedef struct _EomEditWindow      EomEditWindow;
typedef struct _EomEditWindowClass EomEditWindowClass;

struct _EomEditWindowClass
{
  HildonStackableWindowClass parent_class;
};

struct _EomEditWindow
{
  HildonStackableWindow parent;

  GtkWidget *name_label;
  GtkWidget *have_label;
  GtkWidget *total_label;

  gint manga_id;
};

GType      eom_edit_window_get_type(void);

GtkWidget *eom_edit_window_new(gint manga_id);

G_END_DECLS

#endif /* __EOM_EDIT_WINDOW_H__ */