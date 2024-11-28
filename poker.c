#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

// Estructuras
typedef struct {
    int valor;        // Valor de 1 (As) a 13 (Rey)
    const char *palo; // Palo de la carta
} Carta;

// Rutas de las imágenes de los palos
const char *suits[] = {
    "imagenes/corazones.png",
    "imagenes/diamantes.png",
    "imagenes/tréboles.png",
    "imagenes/picas.png"
};

// Inicializa el mazo con 52 cartas
void inicializar_mazo(Carta mazo[]) {
    int indice = 0;
    const char *palos[] = { "Corazones", "Diamantes", "Tréboles", "Picas" };
    
    for (int p = 0; p < 4; p++) {
        for (int v = 1; v <= 13; v++) {
            mazo[indice].valor = v;
            mazo[indice].palo = palos[p];
            indice++;
        }
    }
}

// Baraja el mazo de cartas
void barajar_mazo(Carta mazo[], int tam) {
    srand(time(NULL));
    for (int i = tam - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carta temp = mazo[i];
        mazo[i] = mazo[j];
        mazo[j] = temp;
    }
}

// Variable global para el índice del mazo
int indice_carta_actual = 0;

// Reparte una carta del mazo
Carta repartir_carta(Carta mazo[]) {
    if (indice_carta_actual >= 52) {
        printf("No quedan más cartas en el mazo.\n");
        exit(1);
    }
    return mazo[indice_carta_actual++];
}

// Callback para generar una carta aleatoria
void on_generate_button_clicked(GtkButton *button, gpointer user_data) {
    // Recupera el mazo desde el puntero
    Carta *mazo = (Carta *)g_object_get_data(G_OBJECT(button), "mazo");

    // Obtiene la siguiente carta
    Carta carta = repartir_carta(mazo);

    GtkImage *card_suit = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "card_suit"));
    GtkLabel *card_value = GTK_LABEL(g_object_get_data(G_OBJECT(button), "card_value"));

    // Busca el índice del palo para cargar la imagen correspondiente
    int suit_index = 0;
    if (strcmp(carta.palo, "Corazones") == 0) suit_index = 0;
    else if (strcmp(carta.palo, "Diamantes") == 0) suit_index = 1;
    else if (strcmp(carta.palo, "Tréboles") == 0) suit_index = 2;
    else if (strcmp(carta.palo, "Picas") == 0) suit_index = 3;

    // Carga la imagen del palo y ajusta el tamaño
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(suits[suit_index], NULL);
    if (pixbuf) {
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 50, 50, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(card_suit, scaled_pixbuf);
        g_object_unref(pixbuf);  // Libera la imagen original
        g_object_unref(scaled_pixbuf);  // Libera la imagen escalada
    }

    // Actualiza el valor de la carta
    char value_text[20];
    if (carta.valor == 1)
        snprintf(value_text, sizeof(value_text), "A de %s", carta.palo);
    else if (carta.valor == 11)
        snprintf(value_text, sizeof(value_text), "J de %s", carta.palo);
    else if (carta.valor == 12)
        snprintf(value_text, sizeof(value_text), "Q de %s", carta.palo);
    else if (carta.valor == 13)
        snprintf(value_text, sizeof(value_text), "K de %s", carta.palo);
    else
        snprintf(value_text, sizeof(value_text), "%d de %s", carta.valor, carta.palo);

    gtk_label_set_text(card_value, value_text);
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkImage *card_suit;
    GtkLabel *card_value;
    GtkButton *generate_button;

    // Inicializa GTK
    gtk_init(&argc, &argv);

    // Inicializa el mazo y lo baraja
    Carta mazo[52];
    inicializar_mazo(mazo);
    barajar_mazo(mazo, 52);

    // Carga la interfaz de Glade
    builder = gtk_builder_new_from_file("interface.glade");

    // Obtén los widgets
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    card_suit = GTK_IMAGE(gtk_builder_get_object(builder, "card_suit"));
    card_value = GTK_LABEL(gtk_builder_get_object(builder, "card_value"));
    generate_button = GTK_BUTTON(gtk_builder_get_object(builder, "generate_button"));

    // Conecta los widgets al callback
    g_object_set_data(G_OBJECT(generate_button), "card_suit", card_suit);
    g_object_set_data(G_OBJECT(generate_button), "card_value", card_value);
    g_object_set_data(G_OBJECT(generate_button), "mazo", mazo);
    g_signal_connect(generate_button, "clicked", G_CALLBACK(on_generate_button_clicked), NULL);

    // Conecta el cierre de la ventana
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Muestra la ventana
    gtk_widget_show_all(window);

    // Inicia el loop de GTK
    gtk_main();

    // Libera recursos
    g_object_unref(builder);

    return 0;
}
