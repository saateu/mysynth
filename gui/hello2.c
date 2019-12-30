#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
        GtkWidget *dialog;

        gtk_init(&argc, &argv);
        dialog = gtk_message_dialog_new(NULL, 
                                GTK_DIALOG_DESTROY_WITH_PARENT, 
                                GTK_MESSAGE_OTHER,
                                GTK_BUTTONS_OK,
                                "Hello!"
                                );
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 0;
}


