#include "c-edit-window.h"
#include <hildon/hildon.h>
#include <limits.h>
#include "data.h"

enum {
  C_EDIT_PROP_0,
  C_EDIT_PROP_CID
};

static void c_edit_window_set_collection_id(CEditWindow *self,
                                            gint collection_id);

G_DEFINE_TYPE(CEditWindow, c_edit_window, HILDON_TYPE_STACKABLE_WINDOW)

GtkWidget *c_edit_window_new(gint collection_id)
{
  g_print("1: %d\n", collection_id);
  return g_object_new(C_TYPE_EDIT_WINDOW, "collection-id", collection_id, NULL);
}

static void c_edit_window_set_property(GObject      *object,
                                       guint         property_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
  CEditWindow *self = C_EDIT_WINDOW(object);

  switch (property_id) {
  case C_EDIT_PROP_CID:
    g_print("2: %d\n", g_value_get_int(value));
    self->collection_id = g_value_get_int(value);
    break;
  default:
    /* We don't have any other properties */
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

static void c_edit_window_get_property(GObject    *object,
                                       guint       property_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  CEditWindow *self = C_EDIT_WINDOW(object);

  switch (property_id) {
  case C_EDIT_PROP_CID:
    g_value_set_int(value, self->collection_id);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

static void c_edit_window_class_init(CEditWindowClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec   *pspec;

  gobject_class->set_property = c_edit_window_set_property;
  gobject_class->get_property = c_edit_window_get_property;

  pspec = g_param_spec_int("collection-id",
                           "ID for the collection",
                           "Set the collection-id",
                           0,
                           INT_MAX,
                           0,
                           G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject_class,
                                  C_EDIT_PROP_CID,
                                  pspec);
}

static void c_edit_window_init(CEditWindow *self)
{
  GtkWidget *pannablearea;
  GtkWidget *table;
  GtkWidget *nameclabel;
  GtkWidget *haveclabel;
  GtkWidget *totalclabel;
  GtkWidget *vbox;

  g_print("3: %d\n", self->collection_id);

  pannablearea = hildon_pannable_area_new();
  g_object_set(G_OBJECT(pannablearea),
               "mov-mode", HILDON_MOVEMENT_MODE_VERT,
               NULL);
  gtk_container_add(GTK_CONTAINER(self), pannablearea);

  table = gtk_table_new(4, 2, FALSE);
  hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA(pannablearea),
                                         table);

  nameclabel = gtk_label_new("Name:");
  gtk_misc_set_alignment(GTK_MISC(nameclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), nameclabel, 0, 1, 0, 1,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  self->name_label = gtk_label_new("");
  gtk_misc_set_alignment(GTK_MISC(self->name_label), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), self->name_label, 1, 2, 0, 1,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  haveclabel = gtk_label_new("You have:");
  gtk_misc_set_alignment(GTK_MISC(haveclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), haveclabel, 0, 1, 1, 2,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  self->have_label = gtk_label_new("");
  gtk_misc_set_alignment(GTK_MISC(self->have_label), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), self->have_label, 1, 2, 1, 2,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  totalclabel = gtk_label_new("There are:");
  gtk_misc_set_alignment(GTK_MISC(totalclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), totalclabel, 0, 1, 2, 3,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  self->total_label = gtk_label_new("");
  gtk_misc_set_alignment(GTK_MISC(self->total_label), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), self->total_label, 1, 2, 2, 3,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  vbox = gtk_vbox_new(TRUE, 0);
  gtk_table_attach(GTK_TABLE(table), vbox, 0, 2, 3, 4,
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
}

static void c_edit_window_set_collection_id(CEditWindow *self,
                                            gint collection_id)
{
  struct collection *col;
  gint *items;

  col = data_get_series_by_id(collection_id);
  items = data_get_items_by_collection_id(collection_id);

  gtk_label_set_text(GTK_LABEL(self->name_label), col->name);
  gtk_label_set_text(GTK_LABEL(self->have_label),
                     g_strdup_printf("%d", col->current_qty));
  gtk_label_set_text(GTK_LABEL(self->total_label),
                     g_strdup_printf("%d", col->total_qty));

  g_free(col);
}
