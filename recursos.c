#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

//#################--- VARIABLES ---########################




// Variable que guarda cuántas cartas hay en la mesa
int cartasEnMesa = 0; // Inicialmente no hay cartas
int bet = 0;
int ronda = 0;

//#################--- STRUCTS ---##########################




// Definición de la estructura para una carta
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
    int seRetiro;           // ¿El jugador se retiró?
    int rol;                 // 0 - Ninguno, 1 - Small Blind, 2 - Big Blind
    Carta cartas[2];         //cartas en la mano del jugador
    int numCartas;           // numero de cartas del jugador
} Jugador;



//#################--- ARRAYS ---###########################


// Definir la baraja como un arreglo de cartas
#define NUM_CARTAS 52
Carta baraja[NUM_CARTAS];

// Definir un arreglo global para las cartas sobre la mesa
#define MAX_CARTAS_EN_MESA 5  // Definimos un límite máximo de cartas en la mesa
Carta cartasMesa[MAX_CARTAS_EN_MESA];  // Arreglo para almacenar las cartas en la mesa

#define NUM_JUGADORES 3



//#################--- FUNCIONES ---########################


// Declaración de funciones
void on_window_destroy(GtkWidget *widget, gpointer data);
GtkWidget* cargar_imagen_fondo(const gchar *ruta_imagen);

// Función para cargar la imagen de la mesa
GtkWidget* cargar_imagen_fondo(const gchar *ruta_imagen) {
    GtkWidget *imagen = gtk_image_new_from_file(ruta_imagen);
    return imagen;
}


// Función para inicializar la baraja
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

// Función para cargar la imagen de una carta
GdkPixbuf* cargarImagenCarta(Carta carta) {
    char archivo[30];
    // Crear el nombre del archivo basado en el simbolo y valor
    snprintf(archivo, sizeof(archivo), "images/cartas/%d-%d.png", carta.simbolo, carta.valor);
    // Cargar la imagen del archivo
    return gdk_pixbuf_new_from_file(archivo, NULL);
}


// Función para repartir cartas a los jugadores
void repartirCartas(Jugador *jugadores, int numJugadores, Carta *baraja) {
    // Repartimos 2 cartas a cada jugador
    int i, j;
    for (i = 0; i < numJugadores; i++) {
        // Asignamos dos cartas a cada jugador
        for (j = 0; j < 2; j++) {
            jugadores[i].cartas[j] = baraja[0];  // Asignamos la carta en la posición 0 de la baraja
            for (int k = 0; k < NUM_CARTAS - 1; k++) {
                baraja[k] = baraja[k + 1];  // Desplazamos todas las cartas hacia la izquierda
            }
            baraja[NUM_CARTAS - 1] = (Carta){-1, -1}; // "Eliminamos" la carta al final de la baraja
        }
        jugadores[i].numCartas = 2;  // Cada jugador tiene 2 cartas
    }
}


// Función para poner una carta sobre la mesa y almacenarla
void ponerCartaSobreLaMesa(GtkWidget *window, Carta *baraja, int *cartasEnMesa) {
    // Asegurarse de que no se exceda el límite de cartas en la mesa
    if (*cartasEnMesa >= MAX_CARTAS_EN_MESA) {
        printf("Ya hay suficientes cartas en la mesa.\n");
        return;
    }

    // Toma la carta del primer lugar de la baraja
    Carta carta = baraja[0]; // Tomamos la primera carta
    for (int i = 0; i < NUM_CARTAS - 1; i++) {
        baraja[i] = baraja[i + 1];  // Desplazamos todas las cartas hacia la izquierda
    }
    baraja[NUM_CARTAS - 1] = (Carta){-1, -1}; // "Eliminamos" la carta al final de la baraja (la ponemos como inválida)

    // Almacenamos la carta sobre la mesa en el arreglo
    cartasMesa[*cartasEnMesa] = carta;

    // Cargar la imagen de la carta
    GdkPixbuf *imagen = cargarImagenCarta(carta);

    // Determinar la posición de la carta sobre la mesa
    int x = 436 + *cartasEnMesa * 84;  // Las cartas se colocan con un desplazamiento de 84 píxeles en X
    int y = 304;  // Las cartas siempre se colocan en la misma coordenada Y (304)

    // Crear un contenedor para la imagen
    GtkWidget *imagenCarta = gtk_image_new_from_pixbuf(imagen);

    // Crear un contenedor de la imagen en la ventana (puedes usar un contenedor como un 'fixed' o 'box' para posicionarlo)
    gtk_fixed_put(GTK_FIXED(window), imagenCarta, x, y);

    // Actualizamos el número de cartas en la mesa
    (*cartasEnMesa)++;
}

// Función para poner las cartas de los jugadores sobre la mesa
void ponerCartasJugadores(GtkWidget *window, Jugador *jugadores, int numJugadores) {
    // Coordenadas predefinidas para cada jugador
    int posiciones[3][2][2] = {
        {{69, 304}, {148, 304}},  // Jugador 1
        {{563, 512}, {643, 512}}, // Jugador 2
        {{1062, 304}, {1141, 304}} // Jugador 3
    };

    // Iteramos sobre los jugadores y colocamos sus cartas
    for (int i = 0; i < numJugadores; i++) {
        // Para cada jugador, colocamos sus dos cartas
        for (int j = 0; j < 2; j++) {
            // Obtener la carta del jugador
            Carta carta = jugadores[i].cartas[j];

            // Cargar la imagen de la carta del jugador
            GdkPixbuf *imagen = cargarImagenCarta(carta);

            // Crear un contenedor para la imagen de la carta
            GtkWidget *imagenCarta = gtk_image_new_from_pixbuf(imagen);

            // Obtener las posiciones de la carta para este jugador
            int x = posiciones[i][j][0];
            int y = posiciones[i][j][1];

            // Colocar la carta en la mesa
            gtk_fixed_put(GTK_FIXED(window), imagenCarta, x, y);
        }
    }
}

// Función para rotar los roles de los jugadores
void rotarRoles(Jugador jugadores[], int numJugadores) {
    Jugador temp = jugadores[0];
    for (int i = 0; i < numJugadores - 1; i++) {
        jugadores[i] = jugadores[i + 1];
    }
    jugadores[numJugadores - 1] = temp;
}


// Función para revisar si todas las apuestas son iguales
int evenBet(Jugador jugadores[], int numJugadores) {
    double primeraApuesta = jugadores[0].apuestaActual;
    for (int i = 1; i < numJugadores; i++) {
        if (jugadores[i].apuestaActual != primeraApuesta) {
            return 0;  // Las apuestas no son iguales
        }
    }
    return 1;  // Todas las apuestas son iguales
}


// Función para manejar la ronda inicial
void rondaInicial(GtkWidget *fixed, Jugador *jugadores, int numJugadores, Carta *baraja, int *bet, int *ronda) {
    inicializarBaraja();
    mezclarBaraja(baraja, NUM_CARTAS);
    rotarRoles(jugadores, NUM_JUGADORES);
    repartirCartas(jugadores,numJugadores, baraja);

    while (&bet == 0) {
        for (int i = 0; i < NUM_JUGADORES; i++) {
            // Muestra las opciones de apuesta de los jugadores
            // Aquí podrías incluir la lógica para recibir las apuestas gráficamente
        }
        if (evenBet(jugadores, NUM_JUGADORES) == 1) {
            *bet = 1;
            *ronda = 1;
            // Poner 3 cartas sobre la mesa
            ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
            ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
            ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
        }
    }
}


// Función para manejar las rondas siguientes
void rondasSiguientes(int *cartasEnMesa, Jugador *jugadores) {
    while (&cartasEnMesa < 5) {
        for (int i = 0; i < NUM_JUGADORES; i++) {
            // Muestra las opciones de apuesta a los jugadores
            // Aquí podrías incluir la lógica para recibir las apuestas gráficamente
        }
        if (evenBet(jugadores, NUM_JUGADORES) == 1 && cartasEnMesa < 5) {
            ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
        }
    }
}










//#########################--- MAIN ---#################################


int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *image_table;
    GtkWidget *fixed;


    // Inicializar GTK+
    gtk_init(&argc, &argv);

    //time
    srand(time(NULL));

    // Inicializamos la baraja
    inicializarBaraja();

    //mezclar la baraja
    mezclarBaraja(baraja, NUM_CARTAS);

    //Definir jugadores
    Jugador jugadores[3];

    //nombre para los jugadores
    strcpy(jugadores[0].nombre, "Jugador 1");
    strcpy(jugadores[1].nombre, "Jugador 2");
    strcpy(jugadores[2].nombre, "Jugador 3");

    //repartir cartas a los jugadores
    repartirCartas(jugadores, 3, baraja);

    // Crear la ventana principal
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Juego de Póker");
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720); // Tamaño de la ventana
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    //Contenedor fixed
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);



    // Cargar la imagen de la mesa como fondo
    image_table = cargar_imagen_fondo("images/env/table.png"); // Ruta de la imagen
    gtk_fixed_put(GTK_FIXED(fixed), image_table, 0, 0); // Agregar imagen de mesa al contenedor

    // Llamar a la función para poner cartas sobre la mesa
    ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
    ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
    ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
    ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);
    ponerCartaSobreLaMesa(fixed, baraja, &cartasEnMesa);

    // Suponiendo que los jugadores han sido asignados sus cartas
    ponerCartasJugadores(fixed, jugadores, 3);  // Coloca las cartas de los jugadores en la mesa


    // Mostrar todos los widgets
    gtk_widget_show_all(window);

    // Iniciar el bucle principal de GTK+
    gtk_main();

    return 0;
}

// Callback para destruir la ventana
void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

