#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char ciudad;
  int conexiones;
  char* ciudadesVisit;
  int* costo;
} Ciudad;

int size_char(char* array) { return sizeof(array) / sizeof(char); }
int size_int(int* array) { return sizeof(array) / sizeof(int); }

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



int main() {
  Ciudad* citytest = crea_ciudad('a');
  ciudad_agregar_conexion(citytest, 'b', 5);
  return 1;
}