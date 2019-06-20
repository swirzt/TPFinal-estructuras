#include "colasenlazadas.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

Cola cola_crear() {
  Cola nuevaCola = malloc(sizeof(struct _Cola));
  nuevaCola->primero = NULL;
  nuevaCola->ultimo = NULL;
}

int cola_es_vacia(Cola cola) {
  assert(cola != NULL);
  return cola->primero == NULL;
}

char* cola_primero(Cola cola) {
  assert(!cola_es_vacia(cola));
  return cola->primero->dato;
}

void cola_encolar(Cola cola, char* dato) {
  SNodo* nuevoNodo = malloc(sizeof(SNodo));
  nuevoNodo->dato = dato;
  nuevoNodo->sig = NULL;
  if (cola_es_vacia(cola)) {
    cola->primero = nuevoNodo;
  } else {
    cola->ultimo->sig = nuevoNodo;
  }
  cola->ultimo = nuevoNodo;
}

void cola_desencolar(Cola cola) {
  if (!cola_es_vacia(cola)) {
    SNodo* buffer = cola->primero->sig;
    if (buffer == NULL) cola->ultimo = NULL;
    free(cola->primero);
    cola->primero = buffer;
  }
}

void cola_destruir(Cola cola) {
  for (SNodo* buffer = cola->primero; buffer != NULL;) {
    SNodo* tmp = buffer->sig;
    free(buffer);
    buffer = tmp;
  }
  free(cola);
}
