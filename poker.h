#ifndef POKER_H
#define POKER_H

#include <stdbool.h>

// Definiciones
#define TAM_MAZO 52

// Estructuras
typedef struct {
    int valor;        // Valor de 1 (As) a 13 (Rey)
    const char *palo; // Palo de la carta
} Carta;

// Prototipos de funciones
void inicializar_mazo(Carta mazo[]);
void barajar_mazo(Carta mazo[], int tam);
Carta repartir_carta(Carta mazo[]);

#endif // POKER_H
