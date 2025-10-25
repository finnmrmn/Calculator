#include <gtk/gtk.h>

typedef struct {
    GtkWidget *input;
    GtkWidget *output;
} InputOutput;

static GtkWidget *new_result_text_view(const char *text) {
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, text, -1);
    return text_view;
}

static void on_input_entered(GtkEntry *entry, gpointer user_data) {
    InputOutput *input_output = (InputOutput *)user_data;

    const char *input = gtk_editable_get_text(GTK_EDITABLE(input_output->input));

    const char *output = "To be implemented...";
    // Add text views to output.
    GtkWidget *input_result_text_view = new_result_text_view(input);
    GtkWidget *output_result_result_text_view = new_result_text_view(output);
    gtk_list_box_prepend(GTK_LIST_BOX(input_output->output), output_result_result_text_view);
    gtk_list_box_prepend(GTK_LIST_BOX(input_output->output), input_result_text_view);
    gtk_list_box_prepend(GTK_LIST_BOX(input_output->output), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    g_print("Input: %s\n", input);
    g_print("Output: %s\n", output);
}

static void on_reset_button_clicked(GtkButton *button, gpointer user_data) {
    InputOutput *input_output = (InputOutput *)user_data;
    gtk_editable_set_text(GTK_EDITABLE(input_output->input), "");
    gtk_list_box_remove_all(GTK_LIST_BOX(input_output->output));
    g_print("Reset button clicked.\n");
}

static void activate(GtkApplication *app, gpointer user_data) {

    // Obtain monitor dimensions.
    GdkDisplay *display = gdk_display_get_default();
    if (!display) {
        g_printerr("No display found.\n");
        return;
    }
    GListModel *monitors = gdk_display_get_monitors(display);
    if (!monitors || g_list_model_get_n_items(monitors) == 0) {
        g_printerr("No monitors found.\n");
        return;
    }
    GdkMonitor *monitor = g_list_model_get_item(monitors, 0);
    if (!monitor) {
        g_printerr("Could not get monitor.\n");
        return;
    }
    GdkRectangle monitor_geometry = { 0 };
    gdk_monitor_get_geometry(monitor, &monitor_geometry);

    // Desired window size.
    int window_height = monitor_geometry.height / 2;
    int window_width = monitor_geometry.width / 4;
    int vertical_margin = window_height / 8;
    int horizontal_margin = window_width / 8;
    int line_spacing = window_height / 64;

    // Using these widgets.
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, line_spacing);
    GtkWidget *input_entry = gtk_entry_new();
    GtkWidget *reset_button = gtk_button_new_with_label("reset");
    GtkWidget *output_scrolled_window = gtk_scrolled_window_new();
    GtkWidget *output_vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, line_spacing);
    GtkWidget *output_list_box = gtk_list_box_new();

    // Input-output data to be fed to signals.
    InputOutput *input_output = g_new(InputOutput, 1);
    input_output->input = input_entry;
    input_output->output = output_list_box;

    // The structure of the widgets.
    gtk_window_set_child(GTK_WINDOW(window), vertical_box);
    gtk_box_append(GTK_BOX(vertical_box), input_entry);
    gtk_box_append(GTK_BOX(vertical_box), reset_button);
    gtk_box_append(GTK_BOX(vertical_box), output_scrolled_window);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(output_scrolled_window), output_list_box);

    // Output scrolled window initialisation.
    gtk_widget_set_vexpand(output_scrolled_window, TRUE);

    // Reset button initialisation.
    g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_button_clicked), input_output);
    gtk_widget_set_halign(reset_button, GTK_ALIGN_END);
    gtk_widget_set_hexpand(reset_button, FALSE);
    
    // Input entry initalisation.
    g_signal_connect(input_entry, "activate", G_CALLBACK(on_input_entered), input_output);
    gtk_entry_set_placeholder_text(GTK_ENTRY(input_entry), "Awaiting input...");

    // Vertical box initialisation.
    gtk_widget_set_margin_top(vertical_box, vertical_margin);
    gtk_widget_set_margin_bottom(vertical_box, vertical_margin);
    gtk_widget_set_margin_start(vertical_box, horizontal_margin);
    gtk_widget_set_margin_end(vertical_box, horizontal_margin);

    // Window initialisation.
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("finnmrmn.com.github", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
