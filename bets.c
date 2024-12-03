#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

// Variables para apuestas y turno
int apuesta_jugador1 = 0, apuesta_jugador2 = 0, apuesta_jugador3 = 0;
int turno = 0; // 0 = jugador 1, 1 = jugador 2, 2 = jugador 3

// Función para realizar una apuesta
void apostar(GtkButton *button, gpointer user_data) {
    GtkEntry *entry_apuesta;
    int apuesta = 0;

    // Dependiendo de quién sea el turno, se asigna al jugador correspondiente
    if (turno == 0) {
        // Obtener la apuesta del jugador 1
        entry_apuesta = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "entry_apuesta_jugador1"));
        const char *texto_apuesta = gtk_entry_get_text(entry_apuesta);
        apuesta = atoi(texto_apuesta);
        apuesta_jugador1 += apuesta;
        gtk_label_set_text(label_cartajugador_11, g_strdup_printf("Apuesta: %d", apuesta_jugador1));
    } else if (turno == 1) {
        // Obtener la apuesta del jugador 2
        entry_apuesta = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "entry_apuesta_jugador2"));
        const char *texto_apuesta = gtk_entry_get_text(entry_apuesta);
        apuesta = atoi(texto_apuesta);
        apuesta_jugador2 += apuesta;
        gtk_label_set_text(label_cartajugador_21, g_strdup_printf("Apuesta: %d", apuesta_jugador2));
    } else if (turno == 2) {
        // Obtener la apuesta del jugador 3
        entry_apuesta = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "entry_apuesta_jugador3"));
        const char *texto_apuesta = gtk_entry_get_text(entry_apuesta);
        apuesta = atoi(texto_apuesta);
        apuesta_jugador3 += apuesta;
        gtk_label_set_text(label_cartajugador_31, g_strdup_printf("Apuesta: %d", apuesta_jugador3));
    }

    // Cambiar turno
    turno = (turno + 1) % 3;
}
