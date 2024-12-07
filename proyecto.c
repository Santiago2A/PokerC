#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
////Proyecto Poker en C con GTK Glade //Santiago Arias Abarca C20718 //Jose Pablo Alcazar C10165

/*
Este programa es un juego de poker realizado con la interfaz de glade
Se inicia con un boton repartir, que reparte 2 cartas a cada jugador
Las apuestas se manejan con un un boton spin, para que el usuario pueda manejar la cantidad que va a apostar
Si apuesta 0 se toma el jugador como retirado
Si el jugador intenta ingresar más creditos de los que tiene se toma como un allin
Una vez se tiene la ronda inicial de apuestas se tiene un boton de carta comunitaria que genera 1 a 1 las cartas comunitarias
Se debe generar una carta comunitaria cada vez que termina una ronda
Existe un boton en el esquina superior izquierda que actualiza los labels en cualquier momento
El ganador se decide si solo queda un jugador activo o si se encuentran las 5 cartas comunitarias se proceden a analizar los mazos

*/

// Estructuras
typedef struct {
    int simbolo;  // 0= Picas, 1= Corazones, 2= Diamantes, 3= Tréboles
    int valor;    // 0= As, 1= 2, ..., 10= J, 11= Q, 12= K
    char imagen[20];  // Nombre de la ruta de la imagen (por ejemplo, "0-2.png")
} Carta;

// Estructura para almacenar los datos del jugador
typedef struct {
    char nombre[50];         // Nombre del jugador
    double dineroTotal;      // Dinero total del jugador
    double apuestaActual;    // Apuesta actual del jugador
    int retirado;
    int ordenmano;    // orden del par o trio           // ¿El jugador se retiró?
            
    
    Carta cartas[7];  // Cartas del jugador (máximo 7)
    int mano;         // Tipo de mano que tiene el jugador (0-9)
    int cartaAlta;    // Carta más alta del jugador
} Jugador;

Jugador jugadores[3];  // Array de jugadores





// Definir la baraja como un arreglo de cartas
#define NUM_CARTAS 52
Carta baraja[NUM_CARTAS];

//variables globales
int cartasComunitarias = 1; // Inicialmente no hay cartas

double pozo=0.0; //Apuesta actual de la ronda 

int turno_actual = 0;  // Índice del jugador cuyo turno está activo
//Declaracion de funciones
int ganador=1;

// Prototipo de la función reconocer_mano
void reconocer_mano(Jugador *jugador);

// Prototipo de la función comparar_manos
int comparar_manos(Jugador *jugador1, Jugador *jugador2, Jugador *jugador3);

void imprimir_cartas(Jugador *jugador) ;




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

    jugadores[0].cartas[0] = carta_jugador1_1;
    jugadores[0].cartas[1] = carta_jugador1_2;

    jugadores[1].cartas[0] = carta_jugador2_1;
    jugadores[1].cartas[1] = carta_jugador2_2;

    jugadores[2].cartas[0] = carta_jugador3_1;
    jugadores[2].cartas[1] = carta_jugador3_2;

    
    

    // Recupera las imágenes y etiquetas para ambos jugadores
    GtkImage *imagen_carta_jugador1_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador1_1"));
    GtkImage *imagen_carta_jugador1_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador1_2"));
    GtkImage *imagen_carta_jugador2_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador2_1"));
    GtkImage *imagen_carta_jugador2_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador2_2"));

    GtkImage *imagen_carta_jugador3_1 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador3_1"));
    GtkImage *imagen_carta_jugador3_2 = GTK_IMAGE(g_object_get_data(G_OBJECT(button), "imagen_carta_jugador3_2"));
    
    

    


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
    
    
    switch (cartasComunitarias)
    {
    case 1:
        Carta comunitaria1 = repartir_carta(baraja);
        for (int i=0; i<3; i++){
            jugadores[i].cartas[2] = comunitaria1;   
        }
        actualizar_carta(comunitaria1, carta_comunitaria1 );
        
        cartasComunitarias++;
        break;
    case 2:
        Carta comunitaria2 = repartir_carta(baraja);

        for (int i=0; i<3; i++){
            jugadores[i].cartas[3] = comunitaria2;   
        }
        actualizar_carta(comunitaria2, carta_comunitaria2 );
        cartasComunitarias++;
        break;
    case 3:
        Carta comunitaria3 = repartir_carta(baraja);
        for (int i=0; i<3; i++){
            jugadores[i].cartas[4] = comunitaria3;   
        }

        
        actualizar_carta(comunitaria3, carta_comunitaria3 );
        cartasComunitarias++;
        break;
    case 4:
        Carta comunitaria4 = repartir_carta(baraja);
        for (int i=0; i<3; i++){
            jugadores[i].cartas[5] = comunitaria4;
              
        }
        actualizar_carta(comunitaria4, carta_comunitaria4 );
        cartasComunitarias++;
        break;
    case 5:
        Carta comunitaria5 = repartir_carta(baraja);

        for (int i=0; i<3; i++){
            
            jugadores[i].cartas[6] = comunitaria5;   
        }
        actualizar_carta(comunitaria5, carta_comunitaria5 );
        cartasComunitarias++;
        break;
    
    
    default:

        
        
        
        for (int i = 0; i < 3; i++)
        {
            imprimir_cartas(&jugadores[i]);
        }
        
        for (int i = 0; i < 3; i++)
        {
            reconocer_mano(&jugadores[i]);
        }
        
// ######################################Ganador 
        
        ganador = comparar_manos(&jugadores[0], &jugadores[1], &jugadores[2]);
        printf("ganador %d", ganador);
        if (ganador == 1) {
            
            printf("Jugador 1 gana.\n");
        } else if (ganador == 2) {
            printf("Jugador 2 gana.\n");
        } else if (ganador == 3) {
            printf("Jugador 3 gana.\n");
        } else {
            printf("Es un empate entre los jugadores.\n");
            for (int i = 0; i < 3; i++)
            {
                jugadores[i].dineroTotal +=pozo/3;
            }
            
            

        }

        
        
        jugadores[ganador-1].dineroTotal +=pozo;
        pozo=0;

        for (int i = 0; i < 3; i++)
        {
            jugadores[i].retirado=0 ; //reinicia el estado de los jugadores
        }

        
        

        


        cartasComunitarias=1;
        break;
    }


  
   
}

// Función para convertir el valor de la carta a un texto legible
const char* obtener_valor(int valor) {
    static const char* valores[] = {"As", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    return valores[valor];
}

// Función para convertir el símbolo de la carta a un texto legible
const char* obtener_simbolo(int simbolo) {
    static const char* simbolos[] = {"Picas", "Corazones", "Diamantes", "Tréboles"};
    return simbolos[simbolo];
}

// Función para imprimir las cartas de un jugador
void imprimir_cartas(Jugador *jugador) {
    printf("Cartas de %s:\n", jugador->nombre);
    for (int i = 0; i < 7; i++) { // Asume que el jugador siempre tiene 7 cartas
        printf("  %s de %s (%s)\n", 
               obtener_valor(jugador->cartas[i].valor), 
               obtener_simbolo(jugador->cartas[i].simbolo), 
               jugador->cartas[i].imagen);
    }
}



void inicializar_jugadores() {
    // Asignar valores iniciales a cada jugador
    for (int i = 0; i < 3; i++) {
        
        jugadores[i].dineroTotal = 1000;  // Valor fijo inicial
        jugadores[i].apuestaActual = 0;
        jugadores[i].retirado = 0;
        
        
    }

    //nombre para los jugadores
    strcpy(jugadores[0].nombre, "Jugador 1");
    strcpy(jugadores[1].nombre, "Jugador 2");
    strcpy(jugadores[2].nombre, "Jugador 3");
}


void cambiar_turno() {
    // Cambiar al siguiente jugador
    turno_actual = (turno_actual + 1) % 3;

    if (jugadores[turno_actual].retirado==1){
        printf("%s retirado \n",jugadores[turno_actual].nombre);
        cambiar_turno();
    }

    
    
}

void on_button_actualizar_clicked(GtkButton *button, gpointer user_data) {
    // Los punteros a los widgets a actualizar
    GtkWidget *label_pozo = (GtkWidget*) user_data;
    GtkWidget *label_turno = (GtkWidget*) g_object_get_data(G_OBJECT(button), "label_turno");
    GtkWidget *creditos_jugador1 = (GtkWidget*) g_object_get_data(G_OBJECT(button), "creditos_jugador1");
    GtkWidget *creditos_jugador2 = (GtkWidget*) g_object_get_data(G_OBJECT(button), "creditos_jugador2");
    GtkWidget *creditos_jugador3 = (GtkWidget*) g_object_get_data(G_OBJECT(button), "creditos_jugador3");

    GtkWidget *label_ganador = (GtkWidget*) g_object_get_data(G_OBJECT(button), "label_ganador");


    // Actualizamos los labels con los valores correspondientes
    char texto_pozo[50];
    snprintf(texto_pozo, sizeof(texto_pozo), "Pozo: %.2f", pozo);
    gtk_label_set_text(GTK_LABEL(label_pozo), texto_pozo);

    char texto_turno[90];
    snprintf(texto_turno, sizeof(texto_turno), "Turno de: %s", jugadores[turno_actual].nombre);
    gtk_label_set_text(GTK_LABEL(label_turno), texto_turno);

    char texto_creditos1[50];
    snprintf(texto_creditos1, sizeof(texto_creditos1), "Creditos J1: %.2f", jugadores[0].dineroTotal);
    gtk_label_set_text(GTK_LABEL(creditos_jugador1), texto_creditos1);

    char texto_creditos2[50];
    snprintf(texto_creditos2, sizeof(texto_creditos2), "Creditos J2: %.2f", jugadores[1].dineroTotal);
    gtk_label_set_text(GTK_LABEL(creditos_jugador2), texto_creditos2);

    char texto_creditos3[50];
    snprintf(texto_creditos3, sizeof(texto_creditos3), "Creditos J3: %.2f", jugadores[2].dineroTotal);
    gtk_label_set_text(GTK_LABEL(creditos_jugador3), texto_creditos3);

    char texto_ganador[99];
    if (ganador==0)
    {
        snprintf(texto_ganador, sizeof(texto_ganador), "Ultimo Ganador: Empate" );
    gtk_label_set_text(GTK_LABEL(label_ganador), texto_ganador);
    } else{
    
    snprintf(texto_ganador, sizeof(texto_ganador), "Ultimo Ganador: %s", jugadores[ganador-1].nombre);
    gtk_label_set_text(GTK_LABEL(label_ganador), texto_ganador);
    }

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


// Función para ordenar las cartas por valor
void ordenar_cartas(Carta cartas[], int numCartas) {
    for (int i = 0; i < numCartas ; i++) {
        for (int j = i + 1; j < numCartas; j++) {
            if (cartas[i].valor > cartas[j].valor) {
                Carta temp = cartas[i];
                cartas[i] = cartas[j];
                cartas[j] = temp;
            }
        }
    }
}

// Función para reconocer la mejor mano del jugador
void reconocer_mano(Jugador *jugador) {


    // Definir carta alta
    ordenar_cartas(jugador->cartas, 2);
    jugador->cartaAlta = jugador->cartas[1].valor; // Última carta tras ordenar es la más alta


    ordenar_cartas(jugador->cartas, 7); // Ordenar las cartas del jugador por valor
    
    // Variables de análisis
    int valores[13] = {0};  // Contador de valores (0=As, ..., 12=K)
    int simbolos[4] = {0};  // Contador de símbolos (palos)

    // Contamos valores y símbolos
    for (int i = 0; i < 7; i++) {
        valores[jugador->cartas[i].valor]++;
        simbolos[jugador->cartas[i].simbolo]++;
        
    }
   

    // Revisamos combinaciones
    int par = 0, trio = 0, cuatro = 0;
    for (int i = 0; i < 13; i++) {
        if (valores[i] == 2) {
            par++;
            jugador->ordenmano = i;
        }
        if (valores[i] == 3) {
            trio++;
            jugador->ordenmano = i;
        }
        if (valores[i] == 4) {
            cuatro++;
            jugador->ordenmano = i;
        }
    }
    
    int escalera = 0;
    for (int i = 0; i <= 8; i++) {
        if (valores[i] && valores[i + 1] && valores[i + 2] && valores[i + 3] && valores[i + 4]) {
            escalera = 1;
            break;
        }
    }
    

    int color = 0;
    for (int i = 0; i < 4; i++) {
        if (simbolos[i] >= 5) {
            color = 1;
            break;
        }
    }
    

    // Evaluar combinaciones
    if (escalera && color) {
        jugador->mano = 8; // Escalera de color
    } else if (cuatro == 1) {
        jugador->mano = 7; // Cuatro iguales
    } else if (trio == 1 && par == 1) {
        jugador->mano = 6; // Full house
    } else if (color) {
        jugador->mano = 5; // Color
    } else if (escalera) {
        jugador->mano = 4; // Escalera
    } else if (trio == 1) {
        jugador->mano = 3; // Trío
    } else if (par == 2) {
        jugador->mano = 2; // Doble pareja
    } else if (par == 1) {
        jugador->mano = 1; // Pareja
    } else {
        jugador->mano = 0; // Carta alta
    }

}

// Función para comparar las manos de tres jugadores
int comparar_manos(Jugador *jugador1, Jugador *jugador2, Jugador *jugador3) {

    int ganador12;

    //caso en el que ninguno se retira
    if (jugador1->retirado == 0 && jugador2->retirado == 0 && jugador3->retirado == 0){

        // Primero comparamos las manos de jugador1 y jugador2
        if (jugador1->mano > jugador2->mano) {
            ganador12 = 1; // Jugador 1 gana sobre jugador 2
        } else if (jugador1->mano < jugador2->mano) {
            ganador12 = 2; // Jugador 2 gana sobre jugador 1
        } else if (jugador1->mano == 1 || jugador1->mano == 2 || jugador1->mano == 3 || jugador1->mano == 7){
            if (jugador1->ordenmano > jugador2->ordenmano){
                ganador12 = 1;
            }
            else if (jugador1->ordenmano < jugador2->ordenmano){
                ganador12 = 2;
            }
            else {
                // Si las manos y los ordenes son iguales, comparamos las cartas altas
                if (jugador1->cartaAlta > jugador2->cartaAlta) {
                    ganador12 = 1; // Jugador 1 gana sobre jugador 2
                } else if (jugador1->cartaAlta < jugador2->cartaAlta) {
                    ganador12 = 2; // Jugador 2 gana sobre jugador 1
                } else {
                    ganador12 = 0; // Empate entre jugador 1 y jugador 2
                }

            }
        } else {
            // Si las manos son iguales, comparamos las cartas altas
            if (jugador1->cartaAlta > jugador2->cartaAlta) {
                ganador12 = 1; // Jugador 1 gana sobre jugador 2
            } else if (jugador1->cartaAlta < jugador2->cartaAlta) {
                ganador12 = 2; // Jugador 2 gana sobre jugador 1
            } else {
                ganador12 = 0; // Empate entre jugador 1 y jugador 2
            }
        }

        //########### fin comparacion 1-2 ################

        // Ahora comparamos el ganador de jugador1 y jugador2 contra jugador3
        // caso en el que 1 gana
        if (ganador12 == 1){
            if (jugador1->mano > jugador3->mano) {
               return 1; // Jugador 1 gana sobre jugador 3
            } else if (jugador1->mano < jugador3->mano) {
                return 3; // Jugador 3 gana sobre jugador 1
            } else if (jugador1->mano == 1 || jugador1->mano == 2 || jugador1->mano == 3 || jugador1->mano == 7){
                if (jugador1->ordenmano > jugador3->ordenmano){
                    return 1;
                }
                else if (jugador1->ordenmano < jugador3->ordenmano){
                    return 3;
                }
                else {
                    // Si las manos y los ordenes son iguales, comparamos las cartas altas
                    if (jugador1->cartaAlta > jugador3->cartaAlta) {
                        return 1; // Jugador 1 gana sobre jugador 3
                    } else if (jugador1->cartaAlta < jugador3->cartaAlta) {
                        return 3; // Jugador 3 gana sobre jugador 1
                    } else {
                        return 0; // Empate entre jugador 1 y jugador 3
                    }

                }
            } else {
                // Si las manos y los ordenes son iguales, comparamos las cartas altas
                if (jugador1->cartaAlta > jugador3->cartaAlta) {
                    return 1; // Jugador 1 gana sobre jugador 3
                } else if (jugador1->cartaAlta < jugador3->cartaAlta) {
                    return 3; // Jugador 3 gana sobre jugador 1
                } else {
                    return 0; // Empate entre jugador 1 y jugador 3
                }
            }
        }
        //###################### fin comparación 1-3 ####################

        if (ganador12 == 2){
            if (jugador2->mano > jugador3->mano) {
               return 2; // Jugador 2 gana sobre jugador 3
            } else if (jugador2->mano < jugador3->mano) {
                return 3; // Jugador 3 gana sobre jugador 2
            } else if (jugador2->mano == 1 || jugador2->mano == 2 || jugador2->mano == 3 || jugador2->mano == 7){
                if (jugador2->ordenmano > jugador3->ordenmano){
                    return 2;
                }
                else if (jugador2->ordenmano < jugador3->ordenmano){
                    return 3;
                }
                else {
                    // Si las manos y los ordenes son iguales, comparamos las cartas altas
                    if (jugador2->cartaAlta > jugador3->cartaAlta) {
                        return 2; // Jugador 2 gana sobre jugador 3
                    } else if (jugador2->cartaAlta < jugador3->cartaAlta) {
                        return 3; // Jugador 3 gana sobre jugador 2
                    } else {
                        return 0; // Empate entre jugador 2 y jugador 3
                    }

                }
            } else {
                // Si las manos y los ordenes son iguales, comparamos las cartas altas
                if (jugador2->cartaAlta > jugador3->cartaAlta) {
                    return 2; // Jugador 2 gana sobre jugador 3
                } else if (jugador2->cartaAlta < jugador3->cartaAlta) {
                    return 3; // Jugador 3 gana sobre jugador 2
                } else {
                    return 0; // Empate entre jugador 2 y jugador 3
                }
            }
        }
        //###################### fin comparación 2-3 ####################
    }

    //##################### caso 3 se retira ###################
    if (jugador1->retirado == 0 && jugador2->retirado == 0 && jugador3->retirado == 1){
        if (jugador1->mano > jugador2->mano) {
           return 1; // Jugador 1 gana sobre jugador 2
        } else if (jugador1->mano < jugador2->mano) {
            return 2; // Jugador 2 gana sobre jugador 1
        } else if (jugador1->mano == 1 || jugador1->mano == 2 || jugador1->mano == 3 || jugador1->mano == 7){
            if (jugador1->ordenmano > jugador2->ordenmano){
                return 1;
            }
            else if (jugador1->ordenmano < jugador2->ordenmano){
                return 2;
            }
            else {
                // Si las manos y los ordenes son iguales, comparamos las cartas altas
                if (jugador1->cartaAlta > jugador2->cartaAlta) {
                    return 1; // Jugador 1 gana sobre jugador 2
                } else if (jugador1->cartaAlta < jugador2->cartaAlta) {
                    return 2; // Jugador 2 gana sobre jugador 1
                } else {
                    return 0; // Empate entre jugador 1 y jugador 2
                }

            }
        } else {
            // Si las manos y los ordenes son iguales, comparamos las cartas altas
            if (jugador1->cartaAlta > jugador2->cartaAlta) {
                return 1; // Jugador 1 gana sobre jugador 2
            } else if (jugador1->cartaAlta < jugador3->cartaAlta) {
                return 2; // Jugador 2 gana sobre jugador 1
            } else {
                return 0; // Empate entre jugador 1 y jugador 2
            }
        }
    }



    // ############### caso 2 se retira #################
    if (jugador1->retirado == 0 && jugador2->retirado == 1 && jugador3->retirado == 0){
        if (jugador1->mano > jugador3->mano) {
           return 1; // Jugador 1 gana sobre jugador 3
        } else if (jugador1->mano < jugador3->mano) {
            return 3; // Jugador 3 gana sobre jugador 1
        } else if (jugador1->mano == 1 || jugador1->mano == 2 || jugador1->mano == 3 || jugador1->mano == 7){
            if (jugador1->ordenmano > jugador3->ordenmano){
                return 1;
            }
            else if (jugador1->ordenmano < jugador3->ordenmano){
                return 3;
            }
            else {
                // Si las manos y los ordenes son iguales, comparamos las cartas altas
                if (jugador1->cartaAlta > jugador3->cartaAlta) {
                    return 1; // Jugador 1 gana sobre jugador 3
                } else if (jugador1->cartaAlta < jugador3->cartaAlta) {
                    return 3; // Jugador 3 gana sobre jugador 1
                } else {
                    return 0; // Empate entre jugador 1 y jugador 3
                }

            }
        } else {
            // Si las manos y los ordenes son iguales, comparamos las cartas altas
            if (jugador1->cartaAlta > jugador3->cartaAlta) {
                return 1; // Jugador 1 gana sobre jugador 3
            } else if (jugador1->cartaAlta < jugador3->cartaAlta) {
                return 3; // Jugador 3 gana sobre jugador 1
            } else {
                return 0; // Empate entre jugador 1 y jugador 3
            }
        }
    }

    // ############### caso 1 se retira #################
    if (jugador1->retirado == 1 && jugador2->retirado == 0 && jugador3->retirado == 0){
        if (jugador2->mano > jugador3->mano) {
           return 2; // Jugador 2 gana sobre jugador 3
        } else if (jugador2->mano < jugador3->mano) {
            return 3; // Jugador 3 gana sobre jugador 2
        } else if (jugador2->mano == 1 || jugador2->mano == 2 || jugador2->mano == 3 || jugador2->mano == 7){
            if (jugador2->ordenmano > jugador3->ordenmano){
                return 2;
            }
            else if (jugador2->ordenmano < jugador3->ordenmano){
                return 3;
            }
            else {
                // Si las manos y los ordenes son iguales, comparamos las cartas altas
                if (jugador2->cartaAlta > jugador3->cartaAlta) {
                    return 2; // Jugador 2 gana sobre jugador 3
                } else if (jugador2->cartaAlta < jugador3->cartaAlta) {
                    return 3; // Jugador 3 gana sobre jugador 2
                } else {
                    return 0; // Empate entre jugador 2 y jugador 3
                }

            }
        } else {
            // Si las manos y los ordenes son iguales, comparamos las cartas altas
            if (jugador2->cartaAlta > jugador3->cartaAlta) {
                return 2; // Jugador 2 gana sobre jugador 3
            } else if (jugador2->cartaAlta < jugador3->cartaAlta) {
                return 3; // Jugador 3 gana sobre jugador 2
            } else {
                return 0; // Empate entre jugador 2 y jugador 3
            }
        }
    }
    //##################### caso 1 no se retira ###################
    if (jugador1->retirado == 0 && jugador2->retirado == 1 && jugador3->retirado == 1){
        return 1;
    }
    //##################### caso 2 no se retira ###################
    if (jugador1->retirado == 1 && jugador2->retirado == 0 && jugador3->retirado == 1){
        return 2;
    }
    //##################### caso 3 no se retira ###################
    if (jugador1->retirado == 1 && jugador2->retirado == 1 && jugador3->retirado == 0){
        return 3;
    }

}




int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkImage *imagen_carta_jugador1_1, *imagen_carta_jugador1_2;
    GtkImage *imagen_carta_jugador2_1, *imagen_carta_jugador2_2;
    

    GtkImage *imagen_carta_jugador3_1, *imagen_carta_jugador3_2;
    

    GtkButton *generate_button, *button_rcomunitaria;
    GtkWidget *spin_apostar, *button_apostar, *button_retirarse,*button_actualizar;
    GtkWidget *label_turno, *label_pozo;

    GtkImage *carta_comunitaria1, *carta_comunitaria2, *carta_comunitaria3, *carta_comunitaria4, *carta_comunitaria5;
    GtkWidget *creditos_jugador1, *creditos_jugador2, *creditos_jugador3, *label_ganador;
                       
    inicializar_jugadores();
    
    

    // Inicializa GTK
    gtk_init(&argc, &argv);

    
    
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
    label_ganador = GTK_WIDGET(gtk_builder_get_object(builder, "label_ganador"));

  
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
    g_object_set_data(G_OBJECT(button_actualizar), "label_ganador", label_ganador);

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

