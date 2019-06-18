#include <stdio.h>
#include <stdlib.h>
#include "slist.h"

typedef struct {
  char ciudad;
  int conexiones;
  char* ciudadesVisit;
  int* costo;
} Ciudad;

Ciudad* crea_ciudad(char ciudad) {
  Ciudad* city = malloc(sizeof(Ciudad));
  city->ciudad = ciudad;
  city->conexiones = 0;
  city->ciudadesVisit = NULL;
  city->costo = NULL;
  return city;
}

void destruye_ciudad(void* dato) {
  Ciudad* city = dato;
  free(city->ciudadesVisit);
  free(city->costo);
  free(city);
}

void ciudad_agregar_conexion(Ciudad* city, char ciudad, int costo) {
  int max = city->conexiones;
  int indice, termine = 0;
  for (indice = 0; indice < max && !termine; indice++)
    if (city->costo[indice] > costo) termine++;
  city->conexiones++;
  int size = city->conexiones;
  city->ciudadesVisit = realloc(city->ciudadesVisit, sizeof(char) * size);
  city->costo = realloc(city->costo, sizeof(int) * size);
  for (int i = indice; i < max; i++) {
    city->ciudadesVisit[i + 1] = city->ciudadesVisit[i];
    city->costo[i + 1] = city->costo[i];
  }
  city->ciudadesVisit[indice] = ciudad;
  city->costo[indice] = costo;
}

int revisa_conexion(Ciudad* city, char ciudad) {
  int max = city->conexiones;
  for (int i = 0; i < max; i++)
    if (city->ciudadesVisit[i] == ciudad) return 0;
  return 1;
}

Ciudad* buscar_ciudad(SList ciudades, char ciudad) {
  int termine = 0;
  while (!termine) {
    Ciudad* city = ciudades->dato;
    if (city->ciudad == ciudad)
      termine = 1;
    else
      ciudades = ciudades->sig;
  }
  return ciudades->dato;
}

SList lectura_archivo(char* archivoEntrada) {
  SList ciudades = slist_crear();
  FILE* archivo = fopen(archivoEntrada, "r");
  char basura[100];
  fscanf(archivo, "%s\n", basura);  // Elimino la linea de "Ciudades"
  char bufferc = fgetc(archivo);
  while (bufferc != '\n') {
    if (bufferc != ',' && bufferc != ' ')
      ciudades = slist_agregar_final(ciudades, crea_ciudad(bufferc));
    bufferc = fgetc(archivo);
  }
  // char primero, segundo;
  // int precio;
  // fscanf(archivo, "%c,%c,%d", &primero, &segundo, &precio);
  // printf("%c %c %d\n", primero, segundo, precio);
  // fscanf(archivo, "%s\n", basura);  // Elimino la linea de "Costos"
  // bufferc = fgetc(archivo);
  // while (bufferc != EOF) {
  //   primero = bufferc;
  //   while (bufferc == ' ' || bufferc == ',') bufferc = fgetc(archivo);
  //   segundo = bufferc;
  //   // while (bufferc == ' ' || bufferc == ',') bufferc = fgetc(archivo);
  //   fscanf(archivo, "%d", &precio);

  //   ciudad_agregar_conexion(buscar_ciudad(ciudades, primero), segundo,
  //   precio); ciudad_agregar_conexion(buscar_ciudad(ciudades, segundo),
  //   primero, precio);

  //   bufferc = fgetc(archivo);
  // }

  fclose(archivo);
  return ciudades;
}

int main() {
  SList resultado = lectura_archivo("ejemplo.txt");
  slist_destruir(resultado, destruye_ciudad);

  return 1;
}