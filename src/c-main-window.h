#ifndef __C_MAIN_WINDOW_H__
#define __C_MAIN_WINDOW_H__

#include <hildon/hildon-stackable-window.h>

G_BEGIN_DECLS

#define C_TYPE_MAIN_WINDOW                      \
  (c_main_window_get_type())

#define C_MAIN_WINDOW(obj)                         \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),              \
                               C_TYPE_MAIN_WINDOW, \
                               CMainWindow))

#define C_MAIN_WINDOW_CLASS(klass)              \
  (G_TYPE_CHECK_CLASS_CAST ((klass),            \
                            C_TYPE_MAIN_WINDOW, \
                            CMainWindowClass))

#define C_IS_MAIN_WINDOW(obj)                           \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               C_TYPE_MAIN_WINDOW))

#define C_IS_MAIN_WINDOW_CLASS(klass)                   \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                    \
                            C_TYPE_MAIN_WINDOW))

#define C_MAIN_WINDOW_GET_CLASS(obj)                    \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              C_TYPE_MAIN_WINDOW,       \
                              CMainWindowClass))

typedef struct _CMainWindow      CMainWindow;
typedef struct _CMainWindowClass CMainWindowClass;

struct _CMainWindowClass
{
  HildonStackableWindowClass parent_class;
};

struct _CMainWindow
{
  HildonStackableWindow parent;

  GtkTreeIter           iter;
  GtkListStore         *store;
};

GType      c_main_window_get_type(void);

GtkWidget *c_main_window_new(void);

void       c_main_window_add_line(CMainWindow *window,
                                  const gchar *name,
                                  gint current_qty,
                                  gint total_qty);

G_END_DECLS

#endif /* __C_MAIN_WINDOW_H__ */
