#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Estructuras
typedef struct {
    int simbolo;  // 0= Picas, 1= Corazones, 2= Diamantes, 3= Tréboles
    int valor;    // 0= As, 1= 2, ..., 10= J, 11= Q, 12= K
    char imagen[20];  // Nombre de la imagen (por ejemplo, "0-2.png")
} Carta;

// Estructura para almacenar los datos del jugador
typedef struct {
    char nombre[50];         // Nombre del jugador
    double dineroTotal;      // Dinero total del jugador
    double apuestaActual;    // Apuesta actual del jugador
    int retirado;           // ¿El jugador se retiró?
    //int rol;                 // 0 - Ninguno, 1 - Small Blind, 2 - Big Blind
    Carta cartas[2];         //cartas en la mano del jugador
    int numCartas;           // numero de cartas del jugador
} Jugador;

Jugador jugadores[3];  // Array de jugadores

int turno_actual = 0;  // Índice del jugador cuyo turno está activo
int turno_anterior=0;
int turno_siguiente=0;

typedef struct {
    Jugador jugadores[3];        // Lista de jugadores
    Carta cartas_comunitarias[5]; // Cartas comunitarias
    int cartas_comunitarias_reveladas;       // Número de cartas comunitarias visibles
    Carta baraja[52];                          // baraja de cartas
    int turno_actual;                        // Índice del jugador cuyo turno es
    int num_jugadores_activos;               // Número de jugadores activos en la ronda
} Juego;

// Definir la baraja como un arreglo de cartas
#define NUM_CARTAS 52
Carta baraja[NUM_CARTAS];

//variables globales
int cartasComunitarias = 1; // Inicialmente no hay cartas

double pozo=0.0; //Apuesta actual de la ronda 

// Declarar la función actualizar_labels antes de usarla
void actualizar_labels(GtkWidget *label_pozo, GtkWidget *label_turno);




void inicializarBaraja() {
    int i = 0;

    // Llenamos las 52 cartas estándar
    for (int simbolo = 0; simbolo <= 3; simbolo++) { // Picas, Corazones, Diamantes, Tréboles
        for (int valor = 0; valor <= 12; valor++) { // As, 2, 3,..., 10, J, Q, K
            baraja[i].simbolo = simbolo;
            baraja[i].valor = valor;

            // Crear el nombre de la imagen basado en el símbolo y valor
            sprintf(baraja[i].imagen, "%d-%d.png", simbolo, valor);

            i++;
        }
    }
}

// Función para mezclar la baraja de cartas
// Función para mezclar la baraja de cartas
void mezclarBaraja(Carta *baraja, int numCartas) {
    // Inicializar la semilla del generador de números aleatorios
    srand(time(NULL));

    // Algoritmo de Fisher-Yates para mezclar la baraja
    for (int i = numCartas - 1; i > 0; i--) {
        // Elegir un índice aleatorio entre 0 y i
        int j = rand() % (i + 1);

        // Intercambiar las cartas en los índices i y j
        Carta temp = baraja[i];
        baraja[i] = baraja[j];
        baraja[j] = temp;
    }
}

// Variable global para el índice del baraja
int indice_carta_actual = 0;

// Reparte una carta del baraja
Carta repartir_carta(Carta baraja[]) {
    if (indice_carta_actual >= 52) {
        Carta baraja[52];
        
        mezclarBaraja(baraja, NUM_CARTAS);
        indice_carta_actual=0;
    }
    return baraja[indice_carta_actual++];
}

// Función global para cargar la imagen de una carta
GdkPixbuf* cargarImagenCarta(Carta carta) {
    char archivo[30];
    snprintf(archivo, sizeof(archivo), "images/cartas/%d-%d.png", carta.simbolo, carta.valor);
    return gdk_pixbuf_new_from_file(archivo, NULL);
}

// Función global para actualizar una carta en un GtkImage
void actualizar_carta(Carta carta, GtkImage *imagen) {
    GdkPixbuf *pixbuf = cargarImagenCarta(carta);
    if (pixbuf != NULL) {
        gtk_image_set_from_pixbuf(imagen, pixbuf);
        g_object_unref(pixbuf);
    } else {
        printf("Error al cargar la imagen de la carta %d-%d\n", carta.simbolo, carta.valor);
    }
}


// Callback para generar una carta aleatoria
void on_generate_button_clicked(GtkButton *button, gpointer user_data) {
    // Recupera el baraja desde el puntero
    Carta *baraja = (Carta *)g_object_get_data(G_OBJECT(button), "baraja");

    // Obtiene las cartas para los jugadores
    Carta carta_jugador1_1 = repartir_carta(baraja);
    Carta carta_jugador1_2 = repartir_carta(baraja);
    Carta carta_jugador2_1 = repartir_carta(baraja);
    Carta carta_jugador2_2 = repartir_carta(baraja);

    Carta carta_jugador3_1 = repartir_carta(baraja);
    Carta carta_jugador3_2 = repartir_carta(baraja);

    //Carta comunitaria1 = repartir_carta(baraja);
    

    // Recupera las imágenes y etiquetas para ambos jugadores
    GtkImage *imagen_carta_jugador1_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador1_1"));
    GtkImage *imagen_carta_jugador1_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador1_2"));
    GtkImage *imagen_carta_jugador2_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador2_1"));
    GtkImage *imagen_carta_jugador2_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador2_2"));

    GtkImage *imagen_carta_jugador3_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador3_1"));
    GtkImage *imagen_carta_jugador3_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador3_2"));
    
    //GtkImage *carta_comunitaria1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "carta_comunitaria1"));

    


    // Actualiza las cartas del jugador 1
    actualizar_carta(carta_jugador1_1, imagen_carta_jugador1_1);
    actualizar_carta(carta_jugador1_2, imagen_carta_jugador1_2);
    
    // Actualiza las cartas del jugador 2
    actualizar_carta(carta_jugador2_1, imagen_carta_jugador2_1);
    actualizar_carta(carta_jugador2_2, imagen_carta_jugador2_2);
    // Actualiza las cartas del jugador 2
    actualizar_carta(carta_jugador3_1, imagen_carta_jugador3_1);
    actualizar_carta(carta_jugador3_2, imagen_carta_jugador3_2);
    
    

    
}

void on_comunitaria_button_clicked(GtkButton *button, gpointer user_data) {
    Carta *baraja = (Carta *)g_object_get_data(G_OBJECT(button), "baraja");
    GtkImage *carta_comunitaria1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "carta_comunitaria1"));
    GtkImage *carta_comunitaria2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "carta_comunitaria2"));
    GtkImage *carta_comunitaria3 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "carta_comunitaria3"));
    GtkImage *carta_comunitaria4 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "carta_comunitaria4"));
    GtkImage *carta_comunitaria5 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "carta_comunitaria5"));
    // Obtiene las cartas para los jugadores
    // Verificar si los datos están correctamente inicializados
    
    switch (cartasComunitarias)
    {
    case 1:
        Carta comunitaria1 = repartir_carta(baraja);
        actualizar_carta(comunitaria1, carta_comunitaria1 );
        cartasComunitarias++;
        break;
    case 2:
        Carta comunitaria2 = repartir_carta(baraja);
        actualizar_carta(comunitaria2, carta_comunitaria2 );
        cartasComunitarias++;
        break;
    case 3:
        Carta comunitaria3 = repartir_carta(baraja);
        actualizar_carta(comunitaria3, carta_comunitaria3 );
        cartasComunitarias++;
        break;
    case 4:
        Carta comunitaria4 = repartir_carta(baraja);
        actualizar_carta(comunitaria4, carta_comunitaria4 );
        cartasComunitarias++;
        break;
    case 5:
        Carta comunitaria5 = repartir_carta(baraja);
        actualizar_carta(comunitaria5, carta_comunitaria5 );
        cartasComunitarias++;
        break;
    
    
    default:
        cartasComunitarias=1;
        break;
    }

    
    
    

    

    // Recupera las imágenes y etiquetas para ambos jugadores
    
    //GtkImage *carta_comunitaria1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "carta_comunitaria1"));

    

    // Actualiza las cartas del jugador 1
    
    
    
    
    
    
   
}



void inicializar_jugadores() {
    // Asignar valores iniciales a cada jugador
    for (int i = 0; i < 3; i++) {
        snprintf(jugadores[i].nombre, sizeof(jugadores[i].nombre), "Jugador %d", i + 1); // Nombres predeterminados
        jugadores[i].dineroTotal = 1000;  // Valor fijo inicial
        jugadores[i].apuestaActual = 0;
        jugadores[i].retirado = 0;
        //jugadores[i].numCartas = 0;
        // Nota: las cartas se inicializarían más tarde según las reglas del juego
    }
}

//void cambiar_turno(GtkWidget *label_pozo, GtkWidget *label_turno) {
void cambiar_turno() {
    // Cambiar al siguiente jugador
    turno_actual = (turno_actual + 1) % 3;

    if (jugadores[turno_actual].retirado==1){
        printf("%s retirado",jugadores[turno_actual].nombre);
        cambiar_turno();
    }

    // Llamar a la función para actualizar los labels
    //actualizar_labels(label_pozo, label_turno);
    
}

void on_button_actualizar_clicked(GtkButton *button, gpointer user_data) {
    // Los punteros a los widgets a actualizar
    GtkWidget *label_pozo = (GtkWidget*) user_data;
    GtkWidget *label_turno = (GtkWidget*) g_object_get_data(G_OBJECT(button), "label_turno");
    GtkWidget *creditos_jugador1 = (GtkWidget*) g_object_get_data(G_OBJECT(button), "creditos_jugador1");
    GtkWidget *creditos_jugador2 = (GtkWidget*) g_object_get_data(G_OBJECT(button), "creditos_jugador2");
    GtkWidget *creditos_jugador3 = (GtkWidget*) g_object_get_data(G_OBJECT(button), "creditos_jugador3");

    // Actualizamos los labels con los valores correspondientes
    char texto_pozo[50];
    snprintf(texto_pozo, sizeof(texto_pozo), "Pozo: %.2f", pozo);
    gtk_label_set_text(GTK_LABEL(label_pozo), texto_pozo);

    char texto_turno[90];
    snprintf(texto_turno, sizeof(texto_turno), "Turno de: %s", jugadores[turno_actual].nombre);
    gtk_label_set_text(GTK_LABEL(label_turno), texto_turno);

    char texto_creditos1[50];
    snprintf(texto_creditos1, sizeof(texto_creditos1), "Creditos: %.2f", jugadores[0].dineroTotal);
    gtk_label_set_text(GTK_LABEL(creditos_jugador1), texto_creditos1);

    char texto_creditos2[50];
    snprintf(texto_creditos2, sizeof(texto_creditos2), "Creditos: %.2f", jugadores[1].dineroTotal);
    gtk_label_set_text(GTK_LABEL(creditos_jugador2), texto_creditos2);

    char texto_creditos3[50];
    snprintf(texto_creditos3, sizeof(texto_creditos3), "Creditos: %.2f", jugadores[2].dineroTotal);
    gtk_label_set_text(GTK_LABEL(creditos_jugador3), texto_creditos3);
}

void subir_apuesta(GtkWidget *button, gpointer user_data) {
    GtkWidget *label_turno, *label_pozo;
    gpointer *data = (gpointer *)user_data;
    GtkWidget *spin_apostar = GTK_WIDGET(user_data);

    // Obtener el valor de la apuesta desde el spin button
    double apuesta = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_apostar));

    if (apuesta==0){
        jugadores[turno_actual].retirado=1;
        printf("%s se retiro\n", jugadores[turno_actual].nombre);
        cambiar_turno();
    } else {
        // Verificar que el jugador tiene suficiente dinero
        if (jugadores[turno_actual].dineroTotal >= apuesta) {
            // Restar la apuesta del dinero total del jugador
            jugadores[turno_actual].dineroTotal -= apuesta;

            // Añadir la apuesta al pozo
            pozo += apuesta;

            // Imprimir la acción en la consola (opcional, para depuración)
            printf("%s apostó %.2f. Dinero restante: %.2f\n", 
                jugadores[turno_actual].nombre, apuesta, jugadores[turno_actual].dineroTotal);
            printf("El pozo ahora es: %.2f\n", pozo);

            // Cambiar el turno al siguiente jugador
            //cambiar_turno(label_pozo, label_turno);
            cambiar_turno();
        } else {
            // Si no tiene suficiente dinero, mostrar un mensaje de error
            printf("allin");
                apuesta= jugadores[turno_actual].dineroTotal;
                pozo += apuesta;
                jugadores[turno_actual].dineroTotal=0;
                printf("%s apostó %.2f. Dinero restante: %.2f\n", 
                jugadores[turno_actual].nombre, apuesta, jugadores[turno_actual].dineroTotal);
                printf("El pozo ahora es: %.2f\n", pozo);
        }
        
    }

    
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

    GtkButton *generate_button, *button_rcomunitaria;
    GtkWidget *spin_apostar, *button_apostar, *button_retirarse,*button_actualizar;
    GtkWidget *label_turno, *label_pozo;

    GtkImage *carta_comunitaria1, *carta_comunitaria2, *carta_comunitaria3, *carta_comunitaria4, *carta_comunitaria5;
    GtkWidget *creditos_jugador1, *creditos_jugador2, *creditos_jugador3;
                       
    inicializar_jugadores();
    
    //nombre para los jugadores
    strcpy(jugadores[0].nombre, "Jugador 1");
    strcpy(jugadores[1].nombre, "Jugador 2");
    strcpy(jugadores[2].nombre, "Jugador 3");

    // Inicializa GTK
    gtk_init(&argc, &argv);

    // Inicializa  baraja
    
    // Inicializamos la baraja
    inicializarBaraja();

    //mezclar la baraja
    mezclarBaraja(baraja, NUM_CARTAS);

    

    // Carga la interfaz de Glade
    builder = gtk_builder_new_from_file("interface.glade");

    // Obtén los widgets desde el archivo Glade
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    generate_button = GTK_BUTTON(gtk_builder_get_object(builder, "generate_button"));

    button_rcomunitaria = GTK_BUTTON(gtk_builder_get_object(builder, "button_rcomunitaria"));

    imagen_carta_jugador1_1 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador1_1"));
    imagen_carta_jugador1_2 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador1_2"));
    imagen_carta_jugador2_1 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador2_1"));
    imagen_carta_jugador2_2 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador2_2"));

    imagen_carta_jugador3_1 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador3_1"));
    imagen_carta_jugador3_2 = GTK_IMAGE(gtk_builder_get_object(builder, "imagen_carta_jugador3_2"));

    carta_comunitaria1 = GTK_IMAGE(gtk_builder_get_object(builder, "carta_comunitaria1"));
    carta_comunitaria2 = GTK_IMAGE(gtk_builder_get_object(builder, "carta_comunitaria2"));
    carta_comunitaria3 = GTK_IMAGE(gtk_builder_get_object(builder, "carta_comunitaria3"));
    carta_comunitaria4 = GTK_IMAGE(gtk_builder_get_object(builder, "carta_comunitaria4"));
    carta_comunitaria5 = GTK_IMAGE(gtk_builder_get_object(builder, "carta_comunitaria5"));
    // Carga los widgets
    spin_apostar = GTK_WIDGET(gtk_builder_get_object(builder, "spin_apostar"));
    button_apostar = GTK_WIDGET(gtk_builder_get_object(builder, "button_apostar"));
    button_actualizar = GTK_WIDGET(gtk_builder_get_object(builder, "button_actualizar"));
    
    

    // Inicializa labels
    label_turno = GTK_WIDGET(gtk_builder_get_object(builder, "label_turno"));
    label_pozo = GTK_WIDGET(gtk_builder_get_object(builder, "label_pozo"));
    
    creditos_jugador1 = GTK_WIDGET(gtk_builder_get_object(builder, "creditos_jugador1"));
    creditos_jugador2 = GTK_WIDGET(gtk_builder_get_object(builder, "creditos_jugador2"));
    creditos_jugador3 = GTK_WIDGET(gtk_builder_get_object(builder, "creditos_jugador3"));

  
    // Conecta los widgets al callback
    g_object_set_data(G_OBJECT(generate_button), "baraja", baraja);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador1_1", imagen_carta_jugador1_1);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador1_2", imagen_carta_jugador1_2);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador2_1", imagen_carta_jugador2_1);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador2_2", imagen_carta_jugador2_2);
    
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador3_1", imagen_carta_jugador3_1);
    g_object_set_data(G_OBJECT(generate_button), "imagen_carta_jugador3_2", imagen_carta_jugador3_2);
    
    g_object_set_data(G_OBJECT(button_rcomunitaria), "baraja", baraja);
    g_object_set_data(G_OBJECT(button_rcomunitaria), "carta_comunitaria1", carta_comunitaria1);// Conecta la señal del botón
    g_object_set_data(G_OBJECT(button_rcomunitaria), "carta_comunitaria2", carta_comunitaria2);// Conecta la señal del botón
    g_object_set_data(G_OBJECT(button_rcomunitaria), "carta_comunitaria3", carta_comunitaria3);
    g_object_set_data(G_OBJECT(button_rcomunitaria), "carta_comunitaria4", carta_comunitaria4);
    g_object_set_data(G_OBJECT(button_rcomunitaria), "carta_comunitaria5", carta_comunitaria5);

    g_object_set_data(G_OBJECT(button_actualizar), "label_turno", label_turno);
    g_object_set_data(G_OBJECT(button_actualizar), "creditos_jugador1", creditos_jugador1);
    g_object_set_data(G_OBJECT(button_actualizar), "creditos_jugador2", creditos_jugador2);
    g_object_set_data(G_OBJECT(button_actualizar), "creditos_jugador3", creditos_jugador3);

// Conectar el botón con la función de callback
g_signal_connect(button_actualizar, "clicked", G_CALLBACK(on_button_actualizar_clicked), label_pozo);


    g_signal_connect(generate_button, "clicked", G_CALLBACK(on_generate_button_clicked), NULL);

    g_signal_connect(button_rcomunitaria, "clicked", G_CALLBACK(on_comunitaria_button_clicked), NULL);
    // Conectar el botón con la función
    g_signal_connect(button_apostar, "clicked", G_CALLBACK(subir_apuesta), spin_apostar);

    g_signal_connect(button_actualizar, "clicked", G_CALLBACK(on_button_actualizar_clicked), label_pozo);
   
    

    // Crear un array de punteros para pasar a user_data
    gpointer user_data[] = {spin_apostar, label_turno, label_pozo};

    // Muestra la ventana principal
    gtk_widget_show_all(window);

    // Inicia el bucle principal de GTK
    gtk_main();

    return 0;
}

