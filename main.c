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
    i--;  // Para invertir el i++ del final del for, solo si no termine el for
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
  char bufferc = fgetc(archivo);  // LLega al primer \n
  bufferc = fgetc(archivo);       // Comienza la nueva linea.
  while (bufferc != '\n') {
    int i = 0;
    while (i < SIZEBUFFER && bufferc != '\n' && bufferc != ',') {
      buffer[i] = bufferc;
      bufferc = fgetc(archivo);
      i++;
    }
    buffer[i] = '\0';

    if (i != 0) {
      cola_encolar(nombresCiudades, cantidadPalabras, copia_palabra(buffer));
      cantidadPalabras++;
    }
    while (bufferc == ' ' || bufferc == ',') bufferc = fgetc(archivo);
  }

  Ciudad** ciudades = malloc(sizeof(Ciudad*) * cantidadPalabras);
  for (int i = 0; i < cantidadPalabras; i++)
    ciudades[i] = crea_ciudad(cantidadPalabras);

  // hasta aca funciona

  fscanf(archivo, "%s", buffer);  // Saltea la linea de "Costos"
  bufferc = fgetc(archivo);       // Alcanza el /n
  bufferc = fgetc(archivo);       // Comienza la nueva linea
  while (bufferc != EOF) {
    int i = 0, valorEnLinea = 1;
    int ciudad1, ciudad2, costo;
    while (bufferc != '\n' && bufferc != EOF) {
      if (bufferc != ',')
        buffer[i] = bufferc;
      else {
        buffer[i] = '\0';
        if (valorEnLinea == 1)
          ciudad1 = recupera_pos(nombresCiudades, buffer);
        else if (valorEnLinea == 2)
          ciudad2 = recupera_pos(nombresCiudades, buffer);
        else
          costo = atoi(buffer);
        i = 0;
      }
      i++;
      bufferc = fgetc(archivo);
    }
    ciudad_agregar_conexion(ciudades[ciudad1], ciudad2, costo);
    ciudad_agregar_conexion(ciudades[ciudad2], ciudad1, costo);
    while (bufferc == '\n') bufferc = fgetc(archivo);
  }

  fclose(archivo);
  free(buffer);
  return ciudades;
}

int main() {
  Cola test = cola_crear();
  Ciudad** ciudades = lectura_archivo("ejemplo.txt", test);
  // cola_encolar(test, 2, "meme");
  // printf("HOLACOLA %d\n", cola_es_vacia(test));
  return 1;
}