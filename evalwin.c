#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definir valores de mano
#define CARTA_ALTA 1
#define PAR 2
#define DOBLE_PAREJA 3
#define TRIO 4
#define ESCALERA 5
#define COLOR 6
#define FULL_HOUSE 7
#define POKER 8
#define ESCALERA_COLOR 9
#define ESCALERA_REAL 10

// Estructura de Carta
typedef struct {
    int valor;
    const char *palo;
} Carta;

// Función para comparar dos cartas por valor (y palo en caso de empate de valor)
int comparar_cartas(const void *a, const void *b) {
    Carta *carta1 = (Carta *)a;
    Carta *carta2 = (Carta *)b;

    if (carta1->valor < carta2->valor) return -1;
    if (carta1->valor > carta2->valor) return 1;
    return 0;
}

// Función para verificar si hay una escalera
bool es_escalera(Carta cartas[], int num_cartas) {
    for (int i = 1; i < num_cartas; i++) {
        if (cartas[i].valor != cartas[i-1].valor + 1) {
            return false;
        }
    }
    return true;
}

// Función para verificar si hay un color
bool es_color(Carta cartas[], int num_cartas) {
    const char *palo_inicial = cartas[0].palo;
    for (int i = 1; i < num_cartas; i++) {
        if (strcmp(cartas[i].palo, palo_inicial) != 0) {
            return false;
        }
    }
    return true;
}

// Función para obtener el valor de la mano (Evaluación)
int evaluar_mano(Carta cartas[], int num_cartas) {
    qsort(cartas, num_cartas, sizeof(Carta), comparar_cartas); // Ordenar las cartas por valor

    // Verificar si es una escalera
    if (es_escalera(cartas, num_cartas)) {
        if (es_color(cartas, num_cartas)) {
            if (cartas[0].valor == 1 && cartas[4].valor == 13) {
                return ESCALERA_REAL; // Escalera Real (A, K, Q, J, 10)
            }
            return ESCALERA_COLOR; // Escalera de Color
        }
        return ESCALERA; // Escalera simple
    }

    // Verificar si es un color
    if (es_color(cartas, num_cartas)) {
        return COLOR;
    }

    // Verificar combinaciones de cartas repetidas
    int valores[14] = {0}; // Contador para cada valor de carta (1-13)

    for (int i = 0; i < num_cartas; i++) {
        valores[cartas[i].valor]++;
    }

    bool tiene_par = false, tiene_trio = false, tiene_doble_par = false, tiene_poker = false;

    for (int i = 1; i <= 13; i++) {
        if (valores[i] == 4) {
            tiene_poker = true;
        } else if (valores[i] == 3) {
            tiene_trio = true;
        } else if (valores[i] == 2) {
            if (tiene_par) {
                tiene_doble_par = true;
            } else {
                tiene_par = true;
            }
        }
    }

    // Evaluación de Full House
    if (tiene_trio && tiene_par) {
        return FULL_HOUSE;
    }

    // Evaluación de Poker
    if (tiene_poker) {
        return POKER;
    }

    // Evaluación de Trío
    if (tiene_trio) {
        return TRIO;
    }

    // Evaluación de Doble Pareja
    if (tiene_doble_par) {
        return DOBLE_PAREJA;
    }

    // Evaluación de Pareja
    if (tiene_par) {
        return PAR;
    }

    // Si no se encontró ninguna combinación, es Carta Alta
    return CARTA_ALTA;
}

int comparar_manos(int mano1, int mano2, Carta cartas1[], Carta cartas2[]) {
    // Comparar por tipo de mano (escala, full house, etc.)
    if (mano1 > mano2) {
        return 1; // Jugador 1 gana
    } else if (mano2 > mano1) {
        return 2; // Jugador 2 gana
    } else {
        // Si las manos son del mismo tipo, compararemos por la carta más alta
        // Si es un Full House o Poker, la carta que se repite es la que define
        if (mano1 == FULL_HOUSE || mano1 == POKER) {
            return (cartas1[0].valor > cartas2[0].valor) ? 1 : 2;
        }
        // Si es una mano más simple (par, trío, etc.), comparamos la carta más alta
        for (int i = 4; i >= 0; i--) {
            if (cartas1[i].valor > cartas2[i].valor) {
                return 1; // Jugador 1 gana
            } else if (cartas2[i].valor > cartas1[i].valor) {
                return 2; // Jugador 2 gana
            }
        }
    }
    return 0; // Empate
}
