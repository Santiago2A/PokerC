#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

// Estructuras
typedef struct {
    int valor;        // Valor de 1 (As) a 13 (Rey)
    const char *palo; // Palo de la carta
} Carta;

typedef struct {
    Carta cartas[2];          // Cartas del jugador
               // Valor calculado de la mano
    int apuesta_actual;       // Cantidad apostada en la ronda actual
    int total_fichas;         // Fichas totales del jugador
    //bool retirado;            // Indica si el jugador se retiró
} Jugador;

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
    if (indice_carta_actual >= 40) {
        //Carta mazo[52];
        inicializar_mazo(mazo);
        barajar_mazo(mazo, 52);
        indice_carta_actual=0;
    }
    return mazo[indice_carta_actual++];
}


// Callback para generar una carta aleatoria
void on_generate_button_clicked(GtkButton *button, gpointer user_data) {
    // Recupera el mazo desde el puntero
    Carta *mazo = (Carta *)g_object_get_data(G_OBJECT(button), "mazo");

    // Obtiene las cartas para los jugadores
    Carta carta_jugador1_1 = repartir_carta(mazo);
    Carta carta_jugador1_2 = repartir_carta(mazo);
    Carta carta_jugador2_1 = repartir_carta(mazo);
    Carta carta_jugador2_2 = repartir_carta(mazo);

    Carta carta_jugador3_1 = repartir_carta(mazo);
    Carta carta_jugador3_2 = repartir_carta(mazo);


    // Recupera las imágenes y etiquetas para ambos jugadores
    GtkImage *imagen_carta_jugador1_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador1_1"));
    GtkImage *imagen_carta_jugador1_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador1_2"));
    GtkImage *imagen_carta_jugador2_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador2_1"));
    GtkImage *imagen_carta_jugador2_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador2_2"));

    GtkImage *imagen_carta_jugador3_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador3_1"));
    GtkImage *imagen_carta_jugador3_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador3_2"));


    GtkLabel *label_cartajugador_11 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "label_cartajugador_11"));
    GtkLabel *label_cartajugador_12 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "label_cartajugador_12"));
    GtkLabel *label_cartajugador_21 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "label_cartajugador_21"));
    GtkLabel *label_cartajugador_22 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "label_cartajugador_22"));


    GtkLabel *label_cartajugador_31 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "label_cartajugador_31"));
    GtkLabel *label_cartajugador_32 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "label_cartajugador_32"));

    // Función auxiliar para actualizar una carta
    void actualizar_carta(Carta carta, GtkImage *imagen, GtkLabel *label) {
        // Determina el índice del palo para cargar la imagen
        int suit_index = 0;
        if (strcmp(carta.palo, "Corazones") == 0) suit_index = 0;
        else if (strcmp(carta.palo, "Diamantes") == 0) suit_index = 1;
        else if (strcmp(carta.palo, "Tréboles") == 0) suit_index = 2;
        else if (strcmp(carta.palo, "Picas") == 0) suit_index = 3;

        // Carga la imagen del palo y ajusta el tamaño
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(suits[suit_index], NULL);
        if (pixbuf) {
            GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 50, 50, GDK_INTERP_BILINEAR);
            gtk_image_set_from_pixbuf(imagen, scaled_pixbuf);
            g_object_unref(pixbuf);  // Libera la imagen original
            g_object_unref(scaled_pixbuf);  // Libera la imagen escalada
        }

        // Genera el texto de la carta
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

        // Actualiza el label con el texto de la carta
        gtk_label_set_text(label, value_text);
    }

    // Actualiza las cartas del jugador 1
    actualizar_carta(carta_jugador1_1, imagen_carta_jugador1_1, label_cartajugador_11);
    actualizar_carta(carta_jugador1_2, imagen_carta_jugador1_2, label_cartajugador_12);
    
    // Actualiza las cartas del jugador 2
    actualizar_carta(carta_jugador2_1, imagen_carta_jugador2_1, label_cartajugador_21);
    actualizar_carta(carta_jugador2_2, imagen_carta_jugador2_2, label_cartajugador_22);
    // Actualiza las cartas del jugador 2
    actualizar_carta(carta_jugador3_1, imagen_carta_jugador3_1, label_cartajugador_31);
    actualizar_carta(carta_jugador3_2, imagen_carta_jugador3_2, label_cartajugador_32);

    
}


int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkImage *imagen_carta_jugador1_1, *imagen_carta_jugador1_2;
    GtkImage *imagen_carta_jugador2_1, *imagen_carta_jugador2_2;
    GtkLabel *label_cartajugador_11, *label_cartajugador_12;
    GtkLabel *label_cartajugador_21, *label_cartajugador_22;

    GtkImage *imagen_carta_jugador3_1, *imagen_carta_jugador3_2;
    GtkLabel *label_cartajugador_31, *label_cartajugador_32;

    GtkButton *generate_button;

    // Inicializa GTK
    gtk_init(&argc, &argv);

    // Inicializa el mazo y lo baraja
    Carta mazo[52];
    inicializar_mazo(mazo);
    barajar_mazo(mazo, 52);

    // Carga la interfaz de Glade
    builder = gtk_builder_new_from_file("interface.glade");

    // Obtén los widgets desde el archivo Glade
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    generate_button = GTK_BUTTON(gtk_builder_get_object(builder, "generate_button"));

    imagen_carta_jugador1_1 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador1_1"));
    imagen_carta_jugador1_2 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador1_2"));
    imagen_carta_jugador2_1 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador2_1"));
    imagen_carta_jugador2_2 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador2_2"));

    imagen_carta_jugador3_1 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador3_1"));
    imagen_carta_jugador3_2 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador3_2"));

    label_cartajugador_11 = GTK_LABEL(gtk_builder_get_object(builder, "label_cartajugador_11"));
    label_cartajugador_12 = GTK_LABEL(gtk_builder_get_object(builder, "label_cartajugador_12"));
    label_cartajugador_21 = GTK_LABEL(gtk_builder_get_object(builder, "label_cartajugador_21"));
    label_cartajugador_22 = GTK_LABEL(gtk_builder_get_object(builder, "label_cartajugador_22"));

    label_cartajugador_31 = GTK_LABEL(gtk_builder_get_object(builder, "label_cartajugador_31"));
    label_cartajugador_32 = GTK_LABEL(gtk_builder_get_object(builder, "label_cartajugador_32"));

    // Conecta los widgets al callback
    g_object_set_data(G_OBJECT(generate_button), "mazo", mazo);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador1_1", imagen_carta_jugador1_1);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador1_2", imagen_carta_jugador1_2);
    g_object_set_data(G_OBJECT(generate_button), "label_cartajugador_11", label_cartajugador_11);
    g_object_set_data(G_OBJECT(generate_button), "label_cartajugador_12", label_cartajugador_12);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador2_1", imagen_carta_jugador2_1);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador2_2", imagen_carta_jugador2_2);
    g_object_set_data(G_OBJECT(generate_button), "label_cartajugador_21", label_cartajugador_21);
    g_object_set_data(G_OBJECT(generate_button), "label_cartajugador_22", label_cartajugador_22);

    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador3_1", imagen_carta_jugador3_1);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador3_2", imagen_carta_jugador3_2);
    g_object_set_data(G_OBJECT(generate_button), "label_cartajugador_31", label_cartajugador_31);
    g_object_set_data(G_OBJECT(generate_button), "label_cartajugador_32", label_cartajugador_32);

    // Conecta la señal del botón
    g_signal_connect(generate_button, "clicked", G_CALLBACK(on_generate_button_clicked), NULL);

    // Muestra la ventana principal
    gtk_widget_show_all(window);

    // Inicia el bucle principal de GTK
    gtk_main();

    return 0;
}

