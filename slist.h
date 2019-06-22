#ifndef __SLIST_H__
#define __SLIST_H__

#include <stddef.h>

typedef struct _SNodo {
  char* dato;
  struct _SNodo* sig;
} SNodo;

typedef SNodo* SList;

/**
 * Devuelve una lista vacía.
 */
SList slist_crear();

/**
 * Destruccion de la lista.
 */
void slist_destruir(SList lista);

/**
 * Determina si la lista es vacía.
 */
int slist_vacia(SList lista);

/**
 * Agrega un elemento al final de la lista.
 */
SList slist_agregar_final(SList lista, char* dato);

/**
 * Agrega un elemento al inicio de la lista.
 */
SList slist_agregar_inicio(SList lista, char* dato);

#endif /* __SLIST_H__ */
