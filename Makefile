# Variables
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` 
LIBS = `pkg-config --libs gtk+-3.0`
OUTPUT = exe

# Target principal
p1: proyecto.c
	$(CC) $(CFLAGS) proyecto.c -o $(OUTPUT) $(LIBS)

# Limpiar archivos generados
clean:
	rm -f $(OUTPUT)
