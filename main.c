#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colasenlazadas.h"

typedef struct _Ciudades {
  int cantidad;
  char** nombres;
  int* matrizCostos;  // Es una matriz en un arreglo unidimensional
} * Ciudades;

#define SIZEBUFFER 256

/*
 * matriz_escribir : Int* Int Int Int Int -> void
 * Recibe una matriz NxN representada por un arreglo unidimensional.
 * Modifica el valor de la matriz en la posicion (fila,columna).
 */
void matriz_escribir(int* matriz, int n, int fila, int columna, int valor) {
  int indice = (fila * n) + columna;
  matriz[indice] = valor;
}

/*
 * crea_ciudades : Int -> Ciudades
 * Recibe un tamaño, devuelve una estructura Ciudades con la memoria alocada
 * necesaria para ese tamaño.
 */
Ciudades crea_ciudades(int cantidad) {
  Ciudades nuevaCiudades = malloc(sizeof(struct _Ciudades));
  nuevaCiudades->cantidad = cantidad;
  nuevaCiudades->nombres = malloc(sizeof(char*) * cantidad);
  nuevaCiudades->matrizCostos = malloc(sizeof(int) * cantidad * cantidad);
  for (int i = 0; i < cantidad; i++)
    for (int j = 0; j < cantidad; j++)
      matriz_escribir(nuevaCiudades->matrizCostos, cantidad, i, j, -1);
  // Se inicializa la matriz de costos con -1
  return nuevaCiudades;
}

/*
 * ciudades_destruir : Ciudades -> void
 * Recibe una estructura Ciudades, libera toda su memoria alocada.
 */
void ciudades_destruir(Ciudades c) {
  for (int i = 0; i < c->cantidad; i++) free(c->nombres[i]);
  free(c->nombres);
  free(c->matrizCostos);
  free(c);
}

/*
 * ciudades_agregar_nombres : Ciudades Int Char* -> void
 * Recibe una estructura Ciudades, una posicion y un string.
 * Almacena el string en la posicion dada dentro de Ciudades->nombres
 */
void ciudades_agregar_nombre(Ciudades c, int pos, char* nombre) {
  c->nombres[pos] = nombre;
}

/*
 * ciudades_obtiene_pos : Ciudades Char* -> Int
 * Recibe una estructura Ciudades y un nombre de ciudad.
 * Si lo encuentra devuelve su valor indice, de lo contrario devuelve -1.
 */
int ciudades_obtiene_pos(Ciudades c, char* nombre) {
  int termine = 0, i;
  int max = c->cantidad;
  for (i = 0; i < max && !termine; i++) {
    if (!strcmp(nombre, c->nombres[i])) termine = 1;
  }
  if (termine)
    return --i;
  else
    return -1;
}

void ciudades_agregar_costo(Ciudades c, char* city1, char* city2, int costo) {
  int indice1 = ciudades_obtiene_pos(c, city1);
  int indice2 = ciudades_obtiene_pos(c, city2);
  matriz_escribir(c->matrizCostos, c->cantidad, indice1, indice2, costo);
  matriz_escribir(c->matrizCostos, c->cantidad, indice2, indice1, costo);
}

/*
 * copia_palabra : Char* -> Char*
 * Reciba una palabra, devuelve una copia en memoria de la misma.
 */
char* copia_palabra(char* palabra) {
  char* word = malloc(sizeof(char) * (strlen(palabra) + 1));
  strcpy(word, palabra);
  return word;
}

/*
 * lectura_archivo : Char* -> Ciudades
 * Recibe el nombre del archivo a leer.
 * Devuelve una estructura Ciudades con toda la informacion recolectada.
 */
Ciudades lectura_archivo(char* archivoEntrada) {
  FILE* archivo = fopen(archivoEntrada, "r");
  Cola nombresCiudades = cola_crear();
  char* buffer = malloc(sizeof(char) * SIZEBUFFER);
  int cantidadPalabras = 0;
  fscanf(archivo, "%s", buffer);  // Saltea la linea de "Ciudades"
  fscanf(archivo, "%s", buffer);  // Comienzo a leer ciudades
  while (strcmp(buffer, "Costos")) {
    int largobuffer = strlen(buffer);
    if (largobuffer != 0) {
      if (buffer[largobuffer - 1] == ',') buffer[largobuffer - 1] = '\0';
      cola_encolar(nombresCiudades, copia_palabra(buffer));
      cantidadPalabras++;
    }
    fscanf(archivo, "%s", buffer);
  }

  Ciudades c = crea_ciudades(cantidadPalabras);
  for (int i = 0; !cola_es_vacia(nombresCiudades); i++) {
    char* nombre = cola_primero(nombresCiudades);
    cola_desencolar(nombresCiudades);
    ciudades_agregar_nombre(c, i, nombre);
  }
  cola_destruir(nombresCiudades);

  while (fscanf(archivo, "%s", buffer) != EOF) {
    char* iteraBuffer = buffer;
    int i = 0;
    while (iteraBuffer[i] != ',') i++;
    iteraBuffer[i] = '\0';
    char* ciudad1 = copia_palabra(iteraBuffer);
    iteraBuffer = iteraBuffer + i + 1;
    i = 0;
    while (iteraBuffer[i] != ',') i++;
    iteraBuffer[i] = '\0';
    char* ciudad2 = copia_palabra(iteraBuffer);
    iteraBuffer = iteraBuffer + i + 1;
    int costo = atoi(iteraBuffer);
    ciudades_agregar_costo(c, ciudad1, ciudad2, costo);
    free(ciudad1);
    free(ciudad2);
  }
  free(buffer);
  fclose(archivo);
  return c;
}

int main() {
  Ciudades c = lectura_archivo("ejemplo.txt");
  ciudades_destruir(c);
  return 1;
}