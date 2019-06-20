#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colasenlazadas.h"

typedef struct {
  int conexiones;
  int* nCiudad;
  int* precio;
} Ciudad;

Ciudad* crea_ciudad(int size) {
  Ciudad* city = malloc(sizeof(Ciudad));
  city->conexiones = 0;
  size--;  // Como maximo puede conectarse a todas las ciudades menos si misma.
  city->nCiudad = malloc(sizeof(int) * size);
  city->precio = malloc(sizeof(int) * size);
  return city;
}

void destruye_ciudad(void* dato) {
  Ciudad* city = dato;
  free(city->nCiudad);
  free(city->precio);
  free(city);
}

char* copia_palabra(char* palabra) {
  char* word = malloc(sizeof(char) * (strlen(palabra) + 1));
  strcpy(word, palabra);
  return word;
}

void ciudad_agregar_conexion(Ciudad* city, int ciudad, int precio) {
  int max = city->conexiones, i, termine = 0;
  for (i = 0; i < max && !termine; i++) {
    if (city->precio[i] > precio) termine = 1;
  }
  if (termine)
    i--;  // Para invertir el i++ del final del for, solo si corte el for
  for (int j = max; j > i; j--) {
    city->nCiudad[j] = city->nCiudad[j - 1];
    city->precio[j] = city->precio[j - 1];
  }
  city->nCiudad[i] = ciudad;
  city->precio[i] = precio;
}

// int revisa_conexion(Ciudad* city, char ciudad) {
//   int max = city->conexiones;
//   for (int i = 0; i < max; i++)
//     if (city->ciudadesVisit[i] == ciudad) return 0;
//   return 1;
// }

// Ciudad* buscar_ciudad(SList ciudades, char ciudad) {
//   int termine = 0;
//   while (!termine) {
//     Ciudad* city = ciudades->dato;
//     if (city->ciudad == ciudad)
//       termine = 1;
//     else
//       ciudades = ciudades->sig;
//   }
//   return ciudades->dato;
// }

#define SIZEBUFFER 256

int recupera_pos(Cola nombresCiudades, char* ciudad) {
  SNodo* nodoActual = cola_primero(nombresCiudades);
  while (strcmp(nodoActual->dato, ciudad)) nodoActual = nodoActual->sig;
  return nodoActual->n;
  // Nunca evaluo el NULL ya que se que la palabra existe en algun nodo;
}

Ciudad** lectura_archivo(char* archivoEntrada, Cola nombresCiudades) {
  FILE* archivo = fopen(archivoEntrada, "r");
  char* buffer = malloc(sizeof(char) * SIZEBUFFER);
  int cantidadPalabras = 0;
  fscanf(archivo, "%s", buffer);  // Saltea la linea de "Ciudades"
  fscanf(archivo, "%s", buffer);  // Comienzo a leer ciudades
  while (strcmp(buffer, "Costos")) {
    int largobuffer = strlen(buffer);
    if (largobuffer != 0) {
      if (buffer[largobuffer - 1] == ',') buffer[largobuffer - 1] = '\0';
      cola_encolar(nombresCiudades, cantidadPalabras, copia_palabra(buffer));
      cantidadPalabras++;
    }
    fscanf(archivo, "%s", buffer);
  }

  Ciudad** ciudades = malloc(sizeof(Ciudad*) * cantidadPalabras);
  for (int i = 0; i < cantidadPalabras; i++)
    ciudades[i] = crea_ciudad(cantidadPalabras);

  while (fscanf(archivo, "%s", buffer) != EOF) {
    char* iteraBuffer = buffer;
    int i = 0, largo = strlen(buffer);
    while (iteraBuffer[i] != ',') i++;
    char* ciudad1 = malloc(sizeof(char) * i);
    strncpy(ciudad1, iteraBuffer, i);
    ciudad1[i] = '\0';
    iteraBuffer = iteraBuffer + i + 1;
    i = 0;
    while (iteraBuffer[i] != ',') i++;
    char* ciudad2 = malloc(sizeof(char) * i);
    strncpy(ciudad2, iteraBuffer, i);
    ciudad2[i] = '\0';
    iteraBuffer = iteraBuffer + i + 1;
    i = 0;
    int costo = atoi(iteraBuffer);
    int indice1, indice2;
    indice1 = recupera_pos(nombresCiudades, ciudad1);
    indice2 = recupera_pos(nombresCiudades, ciudad2);
    ciudad_agregar_conexion(ciudades[indice1], indice2, costo);
    ciudad_agregar_conexion(ciudades[indice2], indice1, costo);
    free(ciudad1);
    free(ciudad2);
  }
  free(buffer);
  fclose(archivo);
  return ciudades;
}

int main() {
  Cola test = cola_crear();
  Ciudad** ciudades = lectura_archivo("ejemplo.txt", test);
  // free(ciudades);
  // cola_encolar(test, 2, "meme");
  // printf("HOLACOLA %d\n", cola_es_vacia(test));
  return 1;
}