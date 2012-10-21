#ifndef __EOM_NEW_ITEM_DIALOG_H__
#define __EOM_NEW_ITEM_DIALOG_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS

#define EOM_TYPE_NEW_ITEM_DIALOG                \
  (eom_new_item_dialog_get_type())

#define EOM_NEW_ITEM_DIALOG(obj)                                \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                           \
                               EOM_TYPE_NEW_ITEM_DIALOG,        \
                               EomNewItemDialog))

#define EOM_NEW_ITEM_DIALOG_CLASS(klass)                \
  (G_TYPE_CHECK_CLASS_CAST ((klass),                    \
                            EOM_TYPE_NEW_ITEM_DIALOG,   \
                            EomNewItemDialogClass))

#define EOM_IS_NEW_ITEM_DIALOG(obj)                             \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                           \
                               EOM_TYPE_NEW_ITEM_DIALOG))

#define EOM_IS_NEW_ITEM_DIALOG_CLASS(klass)             \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                    \
                            EOM_TYPE_NEW_ITEM_DIALOG))

#define EOM_NEW_ITEM_DIALOG_GET_CLASS(obj)              \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                    \
                              EOM_TYPE_NEW_ITEM_DIALOG, \
                              EomNewItemDialogClass))

typedef struct
{
  GtkDialogClass parent_class;
} EomNewItemDialogClass;

typedef struct
{
  GtkDialog  parent;

  GtkWidget *name_entry;
  GtkWidget *qty_entry;
} EomNewItemDialog;

const gchar *eom_new_item_dialog_get_name(EomNewItemDialog*);
gint eom_new_item_dialog_get_total_qty(EomNewItemDialog*);
GType eom_new_item_dialog_get_type(void);
GtkWidget *eom_new_item_dialog_new(void);

G_END_DECLS

#endif /* __EOM_NEW_ITEM_DIALOG_H__ */
