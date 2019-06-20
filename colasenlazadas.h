#ifndef __ECOLA_H__
#define __ECOLA_H__

#include <stddef.h>

typedef struct _SNodo {
  char* dato;
  struct _SNodo* sig;
} SNodo;

typedef struct _Cola {
  SNodo* primero;
  SNodo* ultimo;
} * Cola;

Cola cola_crear();

int cola_es_vacia(Cola cola);

char* cola_primero(Cola cola);

void cola_encolar(Cola cola, char* dato);

void cola_desencolar(Cola cola);

void cola_destruir(Cola cola);

#endif /* __ECOLA_H__ */
