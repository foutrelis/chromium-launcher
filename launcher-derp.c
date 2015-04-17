#include <gtk/gtk.h>

int main( int argc, char *argv[]) {
	GtkWidget *dialog;
	GdkGeometry min_size = {
		.min_width = 300,
		.min_height = -1,
	};

	gtk_init(&argc, &argv);

	dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
		GTK_BUTTONS_OK, "%s", argc < 2 ? "Unknown error occurred" : argv[1]);
	if (argc == 3)
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG (dialog),
			"%s", argv[2]);

	gtk_window_set_geometry_hints(
		GTK_WINDOW(dialog), NULL, &min_size, GDK_HINT_MIN_SIZE);
	gtk_window_set_title(GTK_WINDOW(dialog), "Chromium launcher error");

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return 0;
}
