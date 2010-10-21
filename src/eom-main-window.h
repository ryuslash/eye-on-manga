#ifndef __EOM_MAIN_WINDOW_H__
#define __EOM_MAIN_WINDOW_H__

#include <hildon/hildon-stackable-window.h>

G_BEGIN_DECLS

#define EOM_TYPE_MAIN_WINDOW                    \
  (eom_main_window_get_type())

#define EOM_MAIN_WINDOW(obj)                            \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                   \
                               EOM_TYPE_MAIN_WINDOW,    \
                               EomMainWindow))

#define EOM_MAIN_WINDOW_CLASS(klass)                    \
  (G_TYPE_CHECK_CLASS_CAST ((klass),                    \
                            EOM_TYPE_MAIN_WINDOW,       \
                            EomMainWindowClass))

#define EOM_IS_MAIN_WINDOW(obj)                         \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               EOM_TYPE_MAIN_WINDOW))

#define EOM_IS_MAIN_WINDOW_CLASS(klass)                 \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                    \
                            EOM_TYPE_MAIN_WINDOW))

#define EOM_MAIN_WINDOW_GET_CLASS(obj)                  \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              EOM_TYPE_MAIN_WINDOW,     \
                              EomMainWindowClass))

typedef struct _EomMainWindow      EomMainWindow;
typedef struct _EomMainWindowClass EomMainWindowClass;

struct _EomMainWindowClass
{
  HildonStackableWindowClass parent_class;
};

struct _EomMainWindow
{
  HildonStackableWindow parent;

  GtkTreeIter           iter;
  GtkListStore         *store;
  GtkTreeSelection     *selection;

  GtkWidget *add_button;
  GtkWidget *remove_button;
  GtkWidget *edit_button;
};

GType      eom_main_window_get_type(void);

GtkWidget *eom_main_window_new(void);

void       eom_main_window_load(EomMainWindow *self);
void       eom_main_window_add_line(EomMainWindow *window,
                                    gint id,
                                    const gchar *name,
                                    gint current_qty,
                                    gint total_qty);
void       eom_main_window_set_no_select(EomMainWindow *self);
void       eom_main_window_set_has_select(EomMainWindow *self);

G_END_DECLS

#endif /* __EOM_MAIN_WINDOW_H__ */
