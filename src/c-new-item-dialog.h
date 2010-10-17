#ifndef __C_NEW_ITEM_DIALOG_H__
#define __C_NEW_ITEM_DIALOG_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS

#define C_TYPE_NEW_ITEM_DIALOG                  \
  (c_new_item_dialog_get_type())

#define C_NEW_ITEM_DIALOG(obj)                          \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                   \
                               C_TYPE_NEW_ITEM_DIALOG,  \
                               CNewItemDialog))

#define C_NEW_ITEM_DIALOG_CLASS(klass)                  \
  (G_TYPE_CHECK_CLASS_CAST ((klass),                    \
                            C_TYPE_NEW_ITEM_DIALOG,     \
                            CNewItemDialogClass))

#define C_IS_NEW_ITEM_DIALOG(obj)                       \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                   \
                               C_TYPE_NEW_ITEM_DIALOG))

#define C_IS_NEW_ITEM_DIALOG_CLASS(klass)               \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                    \
                            C_TYPE_NEW_ITEM_DIALOG))

#define C_NEW_ITEM_DIALOG_GET_CLASS(obj)                \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              C_TYPE_NEW_ITEM_DIALOG,   \
                              CNewItemDialogClass))

typedef struct _CNewItemDialog      CNewItemDialog;
typedef struct _CNewItemDialogClass CNewItemDialogClass;

struct _CNewItemDialogClass
{
  GtkDialogClass parent_class;
};

struct _CNewItemDialog
{
  GtkDialog  parent;

  GtkWidget *name_entry;
  GtkWidget *qty_entry;
};

GType        c_new_item_dialog_get_type(void);

GtkWidget   *c_new_item_dialog_new(void);

const gchar *c_new_item_dialog_get_name(CNewItemDialog *dialog);

gint         c_new_item_dialog_get_total_qty(CNewItemDialog *dialog);

G_END_DECLS

#endif /* __C_NEW_ITEM_DIALOG_H__ */
