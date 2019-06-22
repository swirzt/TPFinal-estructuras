#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"

#define SIZEBUFFER 256

int en_lista(SList lista, char* ciudad) {
  while (lista != NULL) {
    if (!strcmp(ciudad, lista->dato)) return 1;
    lista = lista->sig;
  }
  return 0;
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

void llama_error(int error, int linea) {
  switch (error) {
    case 0:
      printf("El archivo es incorrecto, en la primer linea solo debe estar");
      printf(" la palabra \"Ciudades\"\n");
      break;
    case 1:
      printf("ERROR: hay un caracter despues de una coma");
      printf(" en la linea de Ciudades.\n");
      break;
    case 2:
      printf("ERROR: hay una coma despues de otra coma");
      printf(" en la linea de Ciudades.\n");
      break;
    case 3:
      printf("ERROR: hay un espacio en el nombre de una ciudad");
      printf(" o antes de una coma.\n");
      break;
    case 4:
      printf("El archivo es incorrecto, la palabra \"Costos\" debe");
      printf(" situarse en la tercer linea\n");
      break;
    case 5:
      printf("El archivo es incorrecto, en la tercer linea solo debe estar");
      printf(" la palabra \"Costos\"\n");
      break;
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) return 0;
  FILE* archivo = fopen(argv[1], "r");
  if (archivo == NULL) {
    printf("El archivo \"%s\" no existe.\n", argv[1]);
    return 0;
  }
  char buffer[SIZEBUFFER];
  fscanf(archivo, "%s", buffer);
  if (strcmp(buffer, "Ciudades")) {
    llama_error(0, 1);  // La linea no es necesaria en este caso
    return 0;
  }
  char bufferc = fgetc(archivo);  // Saltea el \r
  bufferc = fgetc(archivo);       // Saltea el \n
  bufferc = fgetc(archivo);       // Comienza la nueva linea
  int i = 0, estado = 1, linea = 2;
  SList ciudades = slist_crear();
  while (bufferc != '\r' && bufferc != '\n') {
    if (bufferc != ',' && bufferc != ' ') {
      if (estado == 1) {
        buffer[i] = bufferc;
        i++;
        bufferc = fgetc(archivo);
        if (bufferc == '\n' || bufferc == '\r') {
          buffer[i] = '\0';
          ciudades = slist_agregar_final(ciudades, copia_palabra(buffer));
        }
      } else {
        llama_error(1, linea);
        return 0;
      }
    } else if (bufferc == ',') {
      if (estado == 1) {
        buffer[i] = '\0';
        i = 0;
        ciudades = slist_agregar_final(ciudades, copia_palabra(buffer));
        estado = 2;
        bufferc = fgetc(archivo);
      } else {
        llama_error(2, linea);
        return 0;
      }
    } else if (bufferc == ' ') {
      if (estado == 1) {
        llama_error(3, linea);
        return 0;
      } else {
        estado = 1;
        bufferc = fgetc(archivo);
      }
    }
  }
  while (bufferc == '\r' || bufferc == '\n') {
    if (bufferc == '\n') linea++;
    bufferc = fgetc(archivo);
  }
  if (linea != 3) {
    llama_error(4, linea);
    return 0;
  }
  fscanf(archivo, "%s", buffer);
  if (bufferc != 'C' || strcmp(buffer, "ostos")) {
    llama_error(5, linea);
    return 0;
  }

  slist_destruir(ciudades);
  fclose(archivo);
  return 1;
}