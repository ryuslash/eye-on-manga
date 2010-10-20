#include "c-edit-window.h"
#include <hildon/hildon.h>
#include "data.h"

G_DEFINE_TYPE(CEditWindow, c_edit_window, HILDON_TYPE_STACKABLE_WINDOW)

GtkWidget *c_edit_window_new(void)
{
  return g_object_new(C_TYPE_EDIT_WINDOW, NULL);
}

static void c_edit_window_class_init(CEditWindowClass *class)
{}

static void c_edit_window_init(CEditWindow *self)
{
  GtkWidget *pannablearea;
  GtkWidget *table;
  GtkWidget *nameclabel;
  GtkWidget *namelabel;
  GtkWidget *haveclabel;
  GtkWidget *havelabel;
  GtkWidget *totalclabel;
  GtkWidget *totallabel;
  GtkWidget *layout;
  int i;
  struct collection test = { 0, 3, 10, "Biomega" };

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

  namelabel = gtk_label_new(test.name);
  gtk_misc_set_alignment(GTK_MISC(namelabel), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), namelabel, 1, 2, 0, 1,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  haveclabel = gtk_label_new("You have:");
  gtk_misc_set_alignment(GTK_MISC(haveclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), haveclabel, 0, 1, 1, 2,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  havelabel = gtk_label_new(g_strdup_printf("%d", test.current_qty));
  gtk_misc_set_alignment(GTK_MISC(havelabel), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), havelabel, 1, 2, 1, 2,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  totalclabel = gtk_label_new("There are:");
  gtk_misc_set_alignment(GTK_MISC(totalclabel), 0.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), totalclabel, 0, 1, 2, 3,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  totallabel = gtk_label_new((test.total_qty == 0)
                             ? "?"
                             : g_strdup_printf("%d", test.total_qty));
  gtk_misc_set_alignment(GTK_MISC(totallabel), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), totallabel, 1, 2, 2, 3,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  /*layout = gtk_layout_new(NULL, NULL);
  gtk_table_attach(GTK_TABLE(table), layout, 0, 2, 3, 4,
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  for (i = 0; i < test.total_qty; i++) {
    GtkWidget *button;

    button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT |
                                     HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label(GTK_BUTTON(button), g_strdup_printf("%d", i));

    }*/
}
