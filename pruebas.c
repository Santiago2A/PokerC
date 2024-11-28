#include <gtk/gtk.h>

// Función para manejar el evento del botón
void on_button_clicked(GtkButton *button, gpointer user_data) {
    GtkLabel *label = GTK_LABEL(user_data);
    gtk_label_set_text(label, "¡Hola, mundo!");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Cargar el archivo Glade
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "interface.glade", NULL);

    // Obtener referencias a los widgets
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    GtkLabel *label_message = GTK_LABEL(gtk_builder_get_object(builder, "label_message"));
    GtkButton *btn_click = GTK_BUTTON(gtk_builder_get_object(builder, "btn_click"));

    // Conectar la señal del botón
    g_signal_connect(btn_click, "clicked", G_CALLBACK(on_button_clicked), label_message);

    // Mostrar la ventana principal
    gtk_widget_show_all(main_window);

    // Conectar el cierre de la ventana al final del programa
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Iniciar el bucle principal de GTK
    gtk_main();

    return 0;
}
