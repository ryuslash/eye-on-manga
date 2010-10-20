#ifndef __C_EDIT_WINDOW_H__
#define __C_EDIT_WINDOW_H__

#include <hildon/hildon-stackable-window.h>

G_BEGIN_DECLS

#define C_TYPE_EDIT_WINDOW                      \
  (c_edit_window_get_type())

#define C_EDIT_WINDOW(obj)                              \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                   \
                               C_TYPE_EDIT_WINDOW,      \
                               CEditWindow))

#define C_EDIT_WINDOW_CLASS(klass)              \
  (G_TYPE_CHECK_CLASS_CAST ((klass),            \
                            C_TYPE_EDIT_WINDOW, \
                            CEditWindowClass))

#define C_IS_EDIT_WINDOW(obj)                           \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               C_TYPE_EDIT_WINDOW))

#define C_IS_EDIT_WINDOW_CLASS(klass)                   \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                    \
                            C_TYPE_EDIT_WINDOW))

#define C_EDIT_WINDOW_GET_CLASS(obj)                    \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              C_TYPE_EDIT_WINDOW,       \
                              CEditWindowClass))

typedef struct _CEditWindow      CEditWindow;
typedef struct _CEditWindowClass CEditWindowClass;

struct _CEditWindowClass
{
  HildonStackableWindowClass parent_class;
};

struct _CEditWindow
{
  HildonStackableWindow parent;
};

GType c_edit_window_get_type(void);

GtkWidget *c_edit_window_new(void);

G_END_DECLS

#endif /* __C_EDIT_WINDOW_H__ */
