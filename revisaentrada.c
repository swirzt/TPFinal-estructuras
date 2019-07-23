#include <ctype.h>
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
 * crea_ciudad : Char* -> Ciudad
 * Recibe un nombre de ciudad, devuelve una estructura Ciudad con la memoria
 * necesaria alocada y el nombre copiado en la estructura.
 * -No depende del char* ingresado.
 */
Ciudad crea_ciudad(char* nombre) {
  Ciudad c = malloc(sizeof(struct _Ciudad));
  c->ciudad = copia_palabra(nombre);
  c->conexiones = 0;
  return c;
}

/*
 * destruye_ciudad : Ciudad -> Void
 * Funcion que recibe una estructura Ciudad y libera toda su memoria.
 * Se define con void* para hacerla compatible con slist_destruir.
 */
void destruye_ciudad(void* dato) {
  Ciudad c = dato;
  free(c->ciudad);
  free(c);
}

/*
 * crea_error : Int Int -> Error
 * Recibe un tipo de error y su dato auxiliar.
 * Devuelve una estructura Error con la memoria necesaria alocada y los datos
 * ingresados dentro.
 */
Error crea_error(int tipo, int dato) {
  Error e = malloc(sizeof(struct _Error));
  e->tipo = tipo;
  e->datoAuxiliar = dato;
  return e;
}

/*
 * destruye_error : Error -> Void
 * Recibe una estructura Error y libera toda su memoria alocada.
 * Se define con void* para hacerla compatible con slist_destruir.
 */
void destruye_error(void* dato) { free(dato); }

/*
 * ciudad_en_lista : SList Char* -> Int
 * Recibe una lista de estructuras Ciudad y un nombre de ciudad.
 * Devuelve 1 si la ciudad dada se encuentra en la lista, de lo contrario 0.
 */
int ciudad_en_lista(SList lista, char* nombreCiudad) {
  while (lista != NULL) {
    Ciudad c = lista->dato;
    if (!strcmp(nombreCiudad, c->ciudad)) return 1;
    lista = lista->sig;
  }
  return 0;
}

/*
 * ciudad_agrega_conexion : SList Char* Char* -> Void
 * Recibe una lista de estructuras Ciudad y el nombre de dos ciudades.
 * Aumenta en 1 la cantidad de conexiones de cada ciudad.
 */
void ciudad_agrega_conexion(SList ciudades, char* ciudad1, char* ciudad2) {
  int colocadas = 0, c1 = 0, c2 = 0;
  while (ciudades != NULL && colocadas != 2) {
    Ciudad c = ciudades->dato;
    if (c1 == 0)
      if (!strcmp(c->ciudad, ciudad1)) {
        c->conexiones++;
        c1 = 1;
        colocadas++;
      }
    if (c2 == 0)
      if (!strcmp(c->ciudad, ciudad2)) {
        c->conexiones++;
        c2 = 1;
        colocadas++;
      }
    ciudades = ciudades->sig;
  }
}

/*
 * imprime_errores : SList -> Void
 * Por cada Error en la lista, imprime su advertencia relacionada.
 */
void imprime_errores(SList errores) {
  while (errores != NULL) {
    Error e = errores->dato;
    switch (e->tipo) {
      case -1:
        printf("El archico no posee toda la informacion necesaria.\n");
        break;
      case 0:
        printf(
            "La palabra \"Ciudades\" no se encuentra en la primer línea o "
            "esta "
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
        printf("La ciudad %d esta repetida\n", e->datoAuxiliar);
        break;
      case 6:
        printf("La palabra \"Costos\" no se encuentra en la línea 3.\n");
        break;
      case 7:
        printf("La palabra \"Costos\" no se encuentra.\n");
        break;
      case 8:
        printf("La palabra \"Costos\" debe ser la única en su línea.\n");
        break;
      case 9:
        printf("Se encontro un espacio en la línea %d.\n", e->datoAuxiliar);
        break;
      case 10:
        printf("Falta una o mas ciudades en la línea %d.\n", e->datoAuxiliar);
        break;
      case 11:
        printf("Existe una coma extra en la línea %d.\n", e->datoAuxiliar);
        break;
      case 12:
        printf("La línea %d terminó sin contener todos los datos necesarios.\n",
               e->datoAuxiliar);
        break;
      case 13:
        printf("La línea %d no contiene el costo entre las 2 ciudades.\n",
               e->datoAuxiliar);
        break;
      case 14:
        printf("La primer ciudad en línea %d no se definio anteriormente.\n",
               e->datoAuxiliar);
        break;
      case 15:
        printf("La segunda ciudad en línea %d no se definio anteriormente.\n",
               e->datoAuxiliar);
        break;
      case 16:
        printf("Existe un caracter no numérico en el costo de la línea %d.\n",
               e->datoAuxiliar);
        break;
      case 17:
        printf("Las ciudades en la línea %d tienen un costo de 0.\n",
               e->datoAuxiliar);
        break;
    }
    errores = errores->sig;
  }
}

/*
 * funcion_eof : SList Char -> Int
 * Recibe una lista de Errores y un caracter.
 * Si el caracter ingresado es el valor EOF, se agrega un error a la lista y
 * devuelve 1.
 * De lo contrario devuelve 0.
 */
int funcion_eof(SList errores, char bufferc) {
  if (bufferc == EOF) {
    errores = slist_agregar_final(errores, crea_error(-1, 0));
    imprime_errores(errores);
    slist_destruir(errores, destruye_error);
    return 1;
  }
  return 0;
}

/*
 * falta_conexion : SList -> Void
 * Recibe una lista de ciudades.
 * Revisa una por una, si alguna ciudad tiene menos de una conexión se le
 * notifica al usuario por consola.
 */
void falta_conexion(SList ciudades) {
  while (ciudades != NULL) {
    Ciudad c = ciudades->dato;
    if (c->conexiones == 0)
      printf("La ciudad \"%s\" no está conectada a ninguna otra.\n", c->ciudad);
    ciudades = ciudades->sig;
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("La cantidad de argumentos es incorrecta.\n");
    return 0;
  }

  FILE* archivo = fopen(argv[1], "r");
  if (archivo == NULL) {
    printf("El archivo \"%s\" no existe.\n", argv[1]);
    return 0;
  }

  SList errores = slist_crear();  // Lista de Errores

  /* Reviso la primer línea en busca de "Ciudades" */
  char buffer[SIZEBUFFER], bufferc;
  int hayerror = 0, basura;
  basura = fscanf(archivo, "%s", buffer);
  if (strcmp(buffer, "Ciudades")) {
    errores = slist_agregar_final(errores, crea_error(0, 0));
    hayerror = 1;
    bufferc = fgetc(archivo);
  } else {
    bufferc = fgetc(archivo);
    if (funcion_eof(errores, bufferc))
      return 0;  // Si es verdadero, termino el programa abruptamente.
    else if (bufferc == ' ') {
      hayerror = 1;
      errores = slist_agregar_final(errores, crea_error(1, 0));
    }
  }
  if (!hayerror) {  // Si no hay error termino la línea facilmente
    if (bufferc == '\r') fgetc(archivo);
  } else {  // En caso contrario, avanzo hasta terminar linea
    while (bufferc != '\n' && bufferc != EOF) bufferc = fgetc(archivo);
  }
  if (funcion_eof(errores, bufferc))
    return 0;  // Termino el programa abruptamente.
  /*************************************************/

  /* Leo la línea siguiente, almaceno las ciudades */
  bufferc = fgetc(archivo);  // Comienza la nueva linea.
  int i = 0, estado = 1, linea = 2, palabra = 1;
  SList ciudades = slist_crear();  // Creo la lista de ciudades.
  while (bufferc != '\r' && bufferc != '\n' && bufferc != EOF) {
    if (bufferc == ',') {
      if (estado == 1) {
        buffer[i] = '\0';
        i = 0;
        if (ciudad_en_lista(ciudades, buffer))
          errores = slist_agregar_final(errores, crea_error(5, palabra));
        else
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
          if (ciudad_en_lista(ciudades, buffer))
            errores = slist_agregar_final(errores, crea_error(5, palabra));
          else
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
        if (ciudad_en_lista(ciudades, buffer))
          errores = slist_agregar_final(errores, crea_error(5, palabra));
        else
          ciudades = slist_agregar_final(ciudades, crea_ciudad(buffer));
      }
      if (estado != 1) {
        errores = slist_agregar_final(errores, crea_error(4, palabra));
        estado = 1;
      }
    }
  }
  if (funcion_eof(errores, bufferc)) return 0;  // Termino abruptamente.
  while (bufferc == '\r' || bufferc == '\n') {  // Finalizo la linea.
    if (bufferc == '\n') linea++;
    bufferc = fgetc(archivo);
  }
  if (funcion_eof(errores, bufferc)) return 0;
  /***********************************************/

  /* Busco la línea de "Costos" */
  if (linea != 3) {
    errores = slist_agregar_final(errores, crea_error(6, linea));
  }
  hayerror = 0;
  basura = fscanf(archivo, "%s", buffer);
  if (bufferc != 'C' || strcmp(buffer, "ostos")) {
    errores = slist_agregar_final(errores, crea_error(7, linea));
    hayerror = 1;
  } else {
    bufferc = fgetc(archivo);
    if (funcion_eof(errores, bufferc))
      return 0;
    else if (bufferc == ' ') {
      errores = slist_agregar_final(errores, crea_error(8, linea));
      hayerror = 1;
    }
  }
  bufferc = fgetc(archivo);
  if (!hayerror) {
    if (bufferc == '\r') fgetc(archivo);
  } else {
    while (bufferc != '\n' && bufferc != EOF) bufferc = fgetc(archivo);
  }
  linea++;
  if (funcion_eof(errores, bufferc)) return 0;
  /******************************************************/

  /* Reviso la linea que almacena los costos */
  /* Estado puede tener 3 valores: 1,2,3.
    1-Significa que estoy leyendo la primer ciudad
    2-Significa que estoy leyendo la segunda ciudad
    3-Significa que estoy leyendo el costo
  */
  char buffer2[SIZEBUFFER];
  estado = 1, i = 0;
  int buscandofin = 0, cantidadInt = 0, faltaCiudad = 0, anteriorComa = 0;
  int numeroCosto = 0;
  while (bufferc != EOF) {
    if (bufferc == ' ') {
      errores = slist_agregar_final(errores, crea_error(9, linea));
      bufferc = fgetc(archivo);
      anteriorComa = 0;
    } else if (bufferc == ',') {
      if (anteriorComa)
        errores = slist_agregar_final(errores, crea_error(11, linea));
      else if (estado == 1) {
        if (i == 0) {
          errores = slist_agregar_final(errores, crea_error(10, linea));
          faltaCiudad = 1;
        }
        buffer[i] = '\0';
        i = 0;
        estado = 2;
      } else if (estado == 2) {
        if (i == 0) {
          errores = slist_agregar_final(errores, crea_error(10, linea));
          faltaCiudad = 1;
        }
        buffer2[i] = '\0';
        i = 0;
        estado = 3;
      }
      bufferc = fgetc(archivo);
      anteriorComa = 1;
    } else if (bufferc == '\r') {
      if (estado != 3)
        errores = slist_agregar_final(errores, crea_error(12, linea));
      buscandofin = 1;
      bufferc = fgetc(archivo);
      anteriorComa = 0;
    } else if (bufferc == '\n') {
      if (buscandofin)
        buscandofin = 0;
      else if (estado != 3)
        errores = slist_agregar_final(errores, crea_error(12, linea));
      else if (!faltaCiudad) {
        int buscaCiudad1 = ciudad_en_lista(ciudades, buffer);
        int buscaCiudad2 = ciudad_en_lista(ciudades, buffer2);
        if (!buscaCiudad1 || !buscaCiudad2) {
          if (!buscaCiudad1)
            errores = slist_agregar_final(errores, crea_error(14, linea));
          if (!buscaCiudad2)
            errores = slist_agregar_final(errores, crea_error(15, linea));
        } else
          ciudad_agrega_conexion(ciudades, buffer, buffer2);
        if (cantidadInt == 0)
          errores = slist_agregar_final(errores, crea_error(13, linea));
        else if (numeroCosto == 0)
          errores = slist_agregar_final(errores, crea_error(17, linea));
      }
      estado = 1;
      bufferc = fgetc(archivo);
      cantidadInt = 0;
      numeroCosto = 0;
      anteriorComa = 0;
      faltaCiudad = 0;
      linea++;
    } else {
      if (estado == 1) {
        buffer[i] = bufferc;
        i++;
        bufferc = fgetc(archivo);
      } else if (estado == 2) {
        buffer2[i] = bufferc;
        i++;
        bufferc = fgetc(archivo);
      } else {
        if (!isdigit(bufferc))
          errores = slist_agregar_final(errores, crea_error(16, linea));
        else {
          cantidadInt++;
          numeroCosto = numeroCosto * 10 + (bufferc - '0');
        }
        bufferc = fgetc(archivo);
      }
      anteriorComa = 0;
    }
  }
  /*******************************************************/

  /* Imprimo errores y reviso ciudades aisladas */
  imprime_errores(errores);
  falta_conexion(ciudades);
  /******************************************/

  slist_destruir(errores, destruye_error);
  slist_destruir(ciudades, destruye_ciudad);
  fclose(archivo);
  basura++;
  return 1;
}