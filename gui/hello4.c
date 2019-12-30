#include <stdio.h>
#include <stdlib.h>
#include "gtk/gtk.h"

static gboolean
delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
  gtk_main_quit ();
  return FALSE;
}

static void
t_button_signal_handler(GtkWidget *widget, gpointer  data)
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    gtk_button_set_label((GtkButton*) widget, "ON" );
  else
    gtk_button_set_label((GtkButton*) widget, "OFF" );
}

static void
check_signal_handler(GtkWidget *widget, gpointer  data)
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    g_print( "check activated\n" );
  else
    g_print( "check deactivated\n" );
}

static void
radio_signal_handler(GtkWidget *widget, gpointer  data)
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    g_print( "%s activated\n", gtk_button_get_label((GtkButton*)widget));
  else
    g_print( "%s deactivated\n", gtk_button_get_label((GtkButton*)widget));
}

static void
hscale_signal_handler(GtkAdjustment *widget, gpointer  data)
{
  g_print("value changed %d\n", (gint)widget->value );
}

int
main(int argc, char* argv[])
{
  GtkWidget *window;
  GSList *group;
  GtkObject *adj;
  GtkWidget *button, *hscale;
  GtkWidget *vbox, *hbox;
  GtkWidget *separator;
  GtkWidget *label;
  GtkWidget *quitbox;

  gtk_init( &argc, &argv );

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (delete_event), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  vbox = gtk_vbox_new (FALSE, 0);

  // label
  label = gtk_label_new ("horizontal box in vertical box");
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 0);

  // toggle button
  button = gtk_toggle_button_new_with_label ("OFF");
  g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (t_button_signal_handler), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  // check button
  button = gtk_check_button_new_with_label ("check");
  g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (check_signal_handler), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  // radio button
  button = gtk_radio_button_new_with_label( NULL, "radio 0" );
  g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (radio_signal_handler), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);
  group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
  button = gtk_radio_button_new_with_label( group, "radio 1");
  g_signal_connect (G_OBJECT (button), "toggled", G_CALLBACK (radio_signal_handler), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 5);
  gtk_widget_show (separator);

  // hscale
  adj = gtk_adjustment_new (0.0, 0.0, 50.0, 0.1, 0, 0);
  hscale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
  g_signal_connect(G_OBJECT (adj), "value_changed", G_CALLBACK (hscale_signal_handler), NULL );
  gtk_box_pack_start (GTK_BOX (vbox), hscale, FALSE, FALSE, 0);
  gtk_widget_show (hscale);

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 5);
  gtk_widget_show (separator);

  quitbox = gtk_hbox_new (FALSE, 0);
  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (G_OBJECT (button), "clicked",
  G_CALLBACK (gtk_main_quit),
  G_OBJECT (window));
  gtk_box_pack_start (GTK_BOX (quitbox), button, TRUE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), quitbox, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show (button);
  gtk_widget_show (quitbox);

  gtk_widget_show (vbox);
  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
