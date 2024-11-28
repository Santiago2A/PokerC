#include <gtk/gtk.h>
#include "poker.h"

// Widgets GTK
GtkWidget *imagen_palo_jugador1_1;
GtkWidget *valor_label_jugador1_1;

// Muestra la carta (palo + valor) en los widgets correspondientes
void mostrar_carta(GtkWidget *imagen_widget, GtkWidget *label_widget, Carta carta) {
    // Ruta de la imagen basada en el palo
    char ruta_imagen[100];
    snprintf(ruta_imagen, sizeof(ruta_imagen), "imagenes/%s.png", carta.palo);

    // Establecer la imagen del palo
    gtk_image_set_from_file(GTK_IMAGE(imagen_widget), ruta_imagen);

    // Establecer el texto del valor
    char texto_valor[10];
    if (carta.valor == 1)
        snprintf(texto_valor, sizeof(texto_valor), "As");
    else if (carta.valor == 11)
        snprintf(texto_valor, sizeof(texto_valor), "J");
    else if (carta.valor == 12)
        snprintf(texto_valor, sizeof(texto_valor), "Q");
    else if (carta.valor == 13)
        snprintf(texto_valor, sizeof(texto_valor), "K");
    else
        snprintf(texto_valor, sizeof(texto_valor), "%d", carta.valor);

    gtk_label_set_text(GTK_LABEL(label_widget), texto_valor);
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *ventana;
    Carta mazo[TAM_MAZO];

    // Inicializar lógica
    inicializar_mazo(mazo);
    barajar_mazo(mazo, TAM_MAZO);

    // Inicializar GTK
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("interface.glade");
    gtk_builder_connect_signals(builder, NULL);

    // Obtener referencias a los widgets
    ventana = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    imagen_palo_jugador1_1 = GTK_WIDGET(gtk_builder_get_object(builder, "imagen_palo_jugador1_1"));
    valor_label_jugador1_1 = GTK_WIDGET(gtk_builder_get_object(builder, "valor_label_jugador1_1"));

    // Repartir y mostrar una carta
    Carta carta1 = repartir_carta(mazo);
    mostrar_carta(imagen_palo_jugador1_1, valor_label_jugador1_1, carta1);

    // Mostrar ventana principal
    gtk_widget_show_all(ventana);
    gtk_main();

    return 0;
}
