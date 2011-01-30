#include "eom-edit-window.h"
#include <hildon/hildon.h>
#include <limits.h>
#include <stdlib.h>
#include "data.h"
#include "eom-main-window.h"

enum {
  EOM_EDIT_PROP_0,
  EOM_EDIT_PROP_CID
};

static void eom_edit_window_set_manga_id(EomEditWindow *self,
                                         gint manga_id);
static void eom_edit_window_on_volume_toggled(GtkToggleButton *togglebutton,
                                              gpointer user_data);
static void
eom_edit_window_on_volume_read_toggled(GtkToggleButton *togglebutton,
                                       gpointer user_data);

G_DEFINE_TYPE(EomEditWindow, eom_edit_window, HILDON_TYPE_STACKABLE_WINDOW)

GtkWidget *eom_edit_window_new(gint manga_id)
{
  return g_object_new(EOM_TYPE_EDIT_WINDOW, "manga-id", manga_id, NULL);
}

static void eom_edit_window_set_property(GObject      *object,
                                         guint         property_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  EomEditWindow *self = EOM_EDIT_WINDOW(object);
  gint manga_id = g_value_get_int(value);

  switch (property_id) {
  case EOM_EDIT_PROP_CID:
    eom_edit_window_set_manga_id(self, manga_id);
    break;
  default:
    /* We don't have any other properties */
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

static void eom_edit_window_get_property(GObject    *object,
                                         guint       property_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
  EomEditWindow *self = EOM_EDIT_WINDOW(object);

  switch (property_id) {
  case EOM_EDIT_PROP_CID:
    g_value_set_int(value, self->current_manga->id);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}

static void eom_edit_window_finalize(GObject *object)
{
  EomEditWindow *self = EOM_EDIT_WINDOW(object);

  g_free(self->current_manga);

  G_OBJECT_CLASS(eom_edit_window_parent_class)->finalize(object);
}

static void eom_edit_window_class_init(EomEditWindowClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec   *pspec;

  gobject_class->set_property = eom_edit_window_set_property;
  gobject_class->get_property = eom_edit_window_get_property;
  gobject_class->finalize = eom_edit_window_finalize;

  pspec = g_param_spec_int("manga-id",
                           "ID of the manga",
                           "Set the manga-id",
                           0,
                           INT_MAX,
                           0,
                           G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject_class,
                                  EOM_EDIT_PROP_CID,
                                  pspec);
}

static void eom_edit_window_init(EomEditWindow *self)
{
  GtkWidget *pannablearea;
  GtkWidget *table;
  GtkWidget *nameclabel;
  GtkWidget *haveclabel;
  GtkWidget *totalclabel;

  pannablearea = hildon_pannable_area_new();
  g_object_set(G_OBJECT(pannablearea),
               "mov-mode", HILDON_MOVEMENT_MODE_VERT,
               NULL);
  gtk_container_add(GTK_CONTAINER(self), pannablearea);

  table = gtk_table_new(4, 2, FALSE);
  hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA(pannablearea),
                                         table);

  /* Label for the name field */
  nameclabel = gtk_label_new("Name:");
  gtk_misc_set_alignment(GTK_MISC(nameclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), nameclabel, 0, 1, 0, 1,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  /* The name field */
  self->name_entry = hildon_entry_new(HILDON_SIZE_AUTO);
  gtk_entry_set_alignment(GTK_ENTRY(self->name_entry), 1.0);
  gtk_table_attach(GTK_TABLE(table), self->name_entry, 1, 2, 0, 1,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  /* Label for the collected field */
  haveclabel = gtk_label_new("You have:");
  gtk_misc_set_alignment(GTK_MISC(haveclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), haveclabel, 0, 1, 1, 2,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  /* The collected field */
  self->have_label = gtk_label_new("");
  gtk_misc_set_alignment(GTK_MISC(self->have_label), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), self->have_label, 1, 2, 1, 2,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  /* Label for the total field */
  totalclabel = gtk_label_new("There are:");
  gtk_misc_set_alignment(GTK_MISC(totalclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), totalclabel, 0, 1, 2, 3,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  /* The total field */
  self->total_entry = hildon_entry_new(HILDON_SIZE_AUTO);
  gtk_entry_set_alignment(GTK_ENTRY(self->total_entry), 1.0);
  gtk_table_attach(GTK_TABLE(table), self->total_entry, 1, 2, 2, 3,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  self->volumes_box = gtk_vbox_new(TRUE, 0);
  gtk_table_attach(GTK_TABLE(table), self->volumes_box, 0, 2, 3, 4,
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
}

static void eom_edit_window_set_manga_id(EomEditWindow *self, gint manga_id)
{
  Manga *manga;
  gint i;
  gint j = 0;
  GtkWidget *clabel;
  GtkWidget *tlabel;
  GtkWidget *buttonbox;

  manga = data_get_manga_by_id(manga_id);
  data_get_volumes_for_manga(manga);

  self->current_manga = manga;

  gtk_entry_set_text(GTK_ENTRY(self->name_entry), manga->name);
  gtk_label_set_text(GTK_LABEL(self->have_label),
                     g_strdup_printf("%d", manga->current_qty));
  gtk_entry_set_text(GTK_ENTRY(self->total_entry),
                     g_strdup_printf("%d", manga->total_qty));

  /* TODO: Create labels for collected and read lists */

  for (i = 0; i < manga->total_qty; i++) {
    GtkWidget *button;
    GtkWidget *read_button;

    buttonbox = gtk_hbox_new(TRUE, 2);
    gtk_box_pack_start(GTK_BOX(self->volumes_box), buttonbox, TRUE, TRUE, 0);

    /* Button indicating collected state */
    button = gtk_toggle_button_new_with_label(g_strdup_printf("%d", i + 1));
    gtk_box_pack_start(GTK_BOX(buttonbox), button, TRUE, TRUE, 0);

    /* Button indicating read state */
    read_button =
      gtk_toggle_button_new_with_label(g_strdup_printf("%d", i + 1));
    gtk_box_pack_start(GTK_BOX(buttonbox), read_button, TRUE, TRUE, 0);

    if (j < manga->vol_count && manga->volumes[j].number == i+1) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
      if (manga->volumes[j].read)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(read_button), TRUE);
      j++;
    }
    g_signal_connect(button, "toggled",
                     G_CALLBACK(eom_edit_window_on_volume_toggled),
                     (gpointer)self);
    g_signal_connect(read_button, "toggled",
                     G_CALLBACK(eom_edit_window_on_volume_read_toggled),
                     (gpointer)self);
  }
}

static void eom_edit_window_on_volume_toggled(GtkToggleButton *togglebutton,
                                              gpointer user_data)
{
  EomEditWindow *self;
  gboolean       active;
  gint           volume;


  self = (EomEditWindow *)user_data;
  active = gtk_toggle_button_get_active(togglebutton);
  volume = atoi(gtk_button_get_label(GTK_BUTTON(togglebutton)));

  if (active) {
    /* Add 1 to mangas collected */
    if (!data_add_to_manga(self->current_manga->id, 1)) {
      return;
    }
    if (!data_add_volume_to_manga(self->current_manga->id, volume)) {
      data_add_to_manga(self->current_manga->id, 1);
      return;
    }
    self->current_manga->current_qty++;
  }
  else {
    /* Remove 1 from mangas collected */
    if (!data_add_to_manga(self->current_manga->id, -1)) {
      return;
    }
    if (!data_remove_volume_from_manga(self->current_manga->id, volume)) {
      data_add_to_manga(self->current_manga->id, 1); /* Undo */
      return;
    }
    self->current_manga->current_qty--;
  }

  gtk_label_set_text(GTK_LABEL(self->have_label),
                     g_strdup_printf("%d", self->current_manga->current_qty));
}

static void
eom_edit_window_on_volume_read_toggled(GtkToggleButton *togglebutton,
                                       gpointer user_data)
{
  EomEditWindow *self;
  gboolean       active;
  gint           volume;

  self = (EomEditWindow *)user_data;
  active = gtk_toggle_button_get_active(togglebutton);
  volume = atoi(gtk_button_get_label(GTK_BUTTON(togglebutton)));

  if (!data_mark_volume_read(active, self->current_manga->id, volume))
    g_print("coulnd't mark volume as read\n");
}
