#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"

#define SIZEBUFFER 256

typedef struct _Ciudad {
  char* ciudad;
  int conexiones;
} * Ciudad;

typedef struct _Error {
  int tipo;
  int datoAuxiliar;
} * Error;

char* copia_palabra(char* palabra) {
  char* word = malloc(sizeof(char) * (strlen(palabra) + 1));
  strcpy(word, palabra);
  return word;
}

Ciudad crea_ciudad(char* nombre) {
  Ciudad c = malloc(sizeof(struct _Ciudad));
  c->ciudad = copia_palabra(nombre);
  c->conexiones = 0;
  return c;
}

void destruye_ciudad(void* dato) {
  Ciudad c = dato;
  free(c->ciudad);
  free(c);
}

Error crea_error(int tipo, int dato) {
  Error e = malloc(sizeof(struct _Error));
  e->tipo = tipo;
  e->datoAuxiliar = dato;
  return e;
}

void destruye_error(void* dato) { free(dato); }

int ciudad_en_lista(SList lista, char* nombreCiudad) {
  while (lista != NULL) {
    if (!strcmp(nombreCiudad, (char*)lista->dato)) return 1;
    lista = lista->sig;
  }
  return 0;
}

void imprime_errores(SList errores) {
  while (errores != NULL) {
    Error e = errores->dato;
    switch (e->tipo) {
      default:
        printf("Nada");
        break;
      case -1:
        printf("El archico no posee toda la informacion necesaria.\n");
        break;
      case 0:
        printf(
            "La palabra \"Ciudades\" no se encuentra en la primer línea o esta "
            "unidad otra palabra.\n");
        break;
      case 1:
        printf("En la primer línea solo debe estar la palabra \"Ciudades\".\n");
        break;
      case 2:
        printf("Se encontró un espacio en la palabra n°%d.\n", e->datoAuxiliar);
        break;
      case 3:
        printf("Se encontraron 2 comas seguidas antes de la palabra n°%d.\n",
               e->datoAuxiliar);
        break;
      case 4:
        printf("Falto ingresar un espacio entre las palabras %d y %d.\n",
               e->datoAuxiliar - 1, e->datoAuxiliar);
        break;
      case 5:
        printf("La palabra \"Costos\" no se encuentra en la línea 3.\n");
        break;
      case 6:
        printf("La palabra \"Costos\" no se encuentra.\n");
        break;
      case 7:
        printf("La palabra \"Costos\" debe ser la única en su línea.\n");
        break;
    }
    errores = errores->sig;
  }
}

int funcion_eof(SList errores, char bufferc) {
  if (bufferc == EOF) {
    errores = slist_agregar_final(errores, crea_error(-1, 0));
    imprime_errores(errores);
    slist_destruir(errores, destruye_error);
    return 1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 2) return 0;
  FILE* archivo = fopen(argv[1], "r");
  if (archivo == NULL) {
    printf("El archivo \"%s\" no existe.\n", argv[1]);
    return 0;
  }
  char buffer[SIZEBUFFER], bufferc;
  SList errores = slist_crear();
  int hayerror = 0;
  fscanf(archivo, "%s", buffer);
  if (strcmp(buffer, "Ciudades")) {
    errores = slist_agregar_final(errores, crea_error(0, 0));
    hayerror = 1;
  } else {
    bufferc = fgetc(archivo);
    if (funcion_eof(errores, bufferc))
      return 0;
    else if (bufferc == '\r' || bufferc == '\n') {
      // Como cambiar esto, maldita logica
    } else {
      hayerror = 1;
      errores = slist_agregar_final(errores, crea_error(1, 0));
    }
  }

  if (!hayerror) {
    if (bufferc == '\r') fgetc(archivo);
  } else {
    while (bufferc != '\n' && bufferc != EOF) bufferc = fgetc(archivo);
  }

  if (funcion_eof(errores, bufferc)) return 0;

  bufferc = fgetc(archivo);  // Comienza la nueva linea
  int i = 0, estado = 1, linea = 2, palabra = 1;
  SList ciudades = slist_crear();
  while (bufferc != '\r' && bufferc != '\n' && bufferc != EOF) {
    if (bufferc == ',') {
      if (estado == 1) {
        buffer[i] = '\0';
        i = 0;
        ciudades = slist_agregar_final(ciudades, crea_ciudad(buffer));
        palabra++;
        estado = 2;
        bufferc = fgetc(archivo);
      } else {
        errores = slist_agregar_final(errores, crea_error(3, palabra));
        bufferc = fgetc(archivo);
      }
    } else if (bufferc == ' ') {
      if (estado == 1) {
        errores = slist_agregar_final(errores, crea_error(2, palabra));
        bufferc = fgetc(archivo);
        if (bufferc == '\n' || bufferc == '\r') {
          buffer[i] = '\0';
          ciudades = slist_agregar_final(ciudades, crea_ciudad(buffer));
        }
      } else {
        estado = 1;
        bufferc = fgetc(archivo);
      }
    } else {
      buffer[i] = bufferc;
      i++;
      bufferc = fgetc(archivo);
      if (bufferc == '\n' || bufferc == '\r') {
        buffer[i] = '\0';
        ciudades = slist_agregar_final(ciudades, crea_ciudad(buffer));
      }
      if (estado != 1) {
        errores = slist_agregar_final(errores, crea_error(4, palabra));
        estado = 1;
      }
    }
  }

  if (funcion_eof(errores, bufferc)) return 0;

  while (bufferc == '\r' || bufferc == '\n') {
    if (bufferc == '\n') linea++;
    bufferc = fgetc(archivo);
  }
  if (funcion_eof(errores, bufferc)) return 0;

  if (linea != 3) {
    errores = slist_agregar_final(errores, crea_error(5, linea));
  }

  hayerror = 0;
  fscanf(archivo, "%s", buffer);
  if (bufferc != 'C' || strcmp(buffer, "ostos")) {
    errores = slist_agregar_final(errores, crea_error(6, linea));
    hayerror = 1;
  } else {
    bufferc = fgetc(archivo);
    if (funcion_eof(errores, bufferc))
      return 0;
    else if (bufferc != '\r' || bufferc != '\n') {
      // como cambiar esto RT
    } else {
      errores = slist_agregar_final(errores, crea_error(7, linea));
      hayerror = 1;
    }
  }

  if (!hayerror) {
    if (bufferc == '\r') fgetc(archivo);
  } else {
    while (bufferc != '\n' && bufferc != EOF) bufferc = fgetc(archivo);
  }
  if (funcion_eof(errores, bufferc)) return 0;

  while (bufferc != EOF) {
    //IR leyendo, fijarse comas, int, agregar conexion en lista,
  }
  //cuando termino checkear que todas tengan almenos 1 conexion
  

  printf("%d\n", slist_vacia(errores));
  imprime_errores(errores);
  slist_destruir(errores, destruye_error);
  slist_destruir(ciudades, destruye_ciudad);
  fclose(archivo);
  return 1;
}