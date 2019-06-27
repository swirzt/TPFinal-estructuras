#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colasenlazadas.h"

typedef struct _Ciudades {
  int cantidad;
  char** nombres;
  int* matrizCostos;
} * Ciudades;

typedef struct _Solucion {
  int costo;
  int* movimientos;
} * Solucion;

#define SIZEBUFFER 256

/*
 * crea_ciudades : Int -> Ciudades
 * Recibe un tamaño, devuelve una estructura Ciudades con la memoria alocada
 * necesaria para ese tamaño.
 */
Ciudades crea_ciudades(int cantidad) {
  Ciudades nuevaCiudades = malloc(sizeof(struct _Ciudades));
  nuevaCiudades->cantidad = cantidad;
  if (cantidad >= 0) {
    nuevaCiudades->nombres = malloc(sizeof(char*) * cantidad);
    nuevaCiudades->matrizCostos = calloc(cantidad * cantidad, sizeof(int));
    // La funcion calloc inicializa la matriz con valor 0
    // Segun el PDF todos los costos son Enteros positivos
  }
  return nuevaCiudades;
}

/*
 * ciudades_destruir : Ciudades -> Void
 * Recibe una estructura Ciudades, libera toda su memoria alocada.
 */
void ciudades_destruir(Ciudades c) {
  for (int i = 0; i < c->cantidad; i++) free(c->nombres[i]);
  free(c->nombres);
  free(c->matrizCostos);
  free(c);
}

/*
 * ciudades_obtiene_pos : Ciudades Char* -> Int
 * Recibe una estructura Ciudades y un nombre de ciudad.
 * Si lo encuentra devuelve su valor indice, de lo contrario devuelve -1.
 */
int ciudades_obtiene_pos(Ciudades c, char* nombre) {
  int termine = 0, i;
  int max = c->cantidad;
  for (i = 0; i < max && !termine; i++) {
    if (!strcmp(nombre, c->nombres[i])) termine = 1;
  }
  if (termine)
    return --i;
  else
    return -1;
  // Esto esta dado como un seguro pero si el archivo esta bien escrito no
  // deberia darse esta situación.
}

/*
 * ciudades_agregar_costo : Ciudades Char* Char* Int -> Void
 * Recibe una estructura Ciudades, dos nombres de ciudad y un costo entre ellas.
 * Ingresa el costo en la tabla Ciudades->matrizCostos.
 */
void ciudades_agregar_costo(Ciudades c, char* city1, char* city2, int costo) {
  int indice1 = ciudades_obtiene_pos(c, city1);
  int indice2 = ciudades_obtiene_pos(c, city2);
  c->matrizCostos[indice1 * c->cantidad + indice2] = costo;
  c->matrizCostos[indice2 * c->cantidad + indice1] = costo;
}

/*
 * ciudades_devuelve_costos : Ciudades Int Int -> Int
 * Recibe una estructura Ciudades y dos nombres de ciudad.
 * Devuelve el costo de viaje entre ellas.
 */
int ciudades_devuelve_costo(Ciudades c, int c1, int c2) {
  return c->matrizCostos[c1 * c->cantidad + c2];
}

/*
 * crea_solucion : Int -> Solucion
 * Recibe un tamaño, devuelve una estructura Solucion con memoria alocada para
 * ese tamaño.
 */
Solucion crea_solucion(int size) {
  Solucion s = malloc(sizeof(struct _Solucion));
  s->costo = -1;
  s->movimientos = malloc(sizeof(int) * size);
  return s;
}

/*
 * solucion_destruir : Solucion -> Void
 * Recibe una estructura Solucion, libera toda su memoria alocada.
 */
void solucion_destruir(Solucion s) {
  free(s->movimientos);
  free(s);
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

/*
 * lectura_archivo : Char* -> Ciudades
 * Recibe el nombre del archivo a leer.
 * Devuelve una estructura Ciudades con toda la informacion recolectada.
 */
Ciudades lectura_archivo(char* archivoEntrada) {
  FILE* archivo = fopen(archivoEntrada, "r");
  if (archivo == NULL) {
    printf("El archivo \"%s\" no existe, revise su entrada.\n", archivoEntrada);
    return crea_ciudades(-1);
  }

  Cola nombresCiudades = cola_crear();
  char* buffer = malloc(sizeof(char) * SIZEBUFFER);
  int cantidadPalabras = 0;
  fscanf(archivo, "%s", buffer);  // Saltea la linea de "Ciudades"
  fscanf(archivo, "%s", buffer);  // Comienzo a leer ciudades
  while (strcmp(buffer, "Costos")) {
    int largobuffer = strlen(buffer);
    if (largobuffer != 0) {
      if (buffer[largobuffer - 1] == ',') buffer[largobuffer - 1] = '\0';
      cola_encolar(nombresCiudades, copia_palabra(buffer));
      cantidadPalabras++;
    }
    fscanf(archivo, "%s", buffer);
  }

  Ciudades c = crea_ciudades(cantidadPalabras);
  for (int i = 0; !cola_es_vacia(nombresCiudades); i++) {
    char* nombre = cola_primero(nombresCiudades);
    cola_desencolar(nombresCiudades);
    c->nombres[i] = nombre;
  }
  cola_destruir(nombresCiudades);

  char bufferc;
  bufferc = fgetc(archivo);  // Saltea un caracter de fin de linea
  // Si el caracter anterior es \r vuelve a saltear
  if (bufferc == '\r') fgetc(archivo);
  char* buffer2 = malloc(sizeof(char) * SIZEBUFFER);
  int costo;
  while (fscanf(archivo, "%[^,],%[^,],%d", buffer, buffer2, &costo) == 3) {
    ciudades_agregar_costo(c, buffer, buffer2, costo);
    bufferc = fgetc(archivo);  // Saltea un caracter de fin de linea
    // Si el caracter anterior es \r vuelve a saltear
    if (bufferc == '\r') fgetc(archivo);
  }

  free(buffer);
  free(buffer2);
  fclose(archivo);
  return c;
}

/*
 * in_array : Int* Int Int -> Int
 * Recibe un array, su tamaño y un dato.
 * Devuelve 1 si el dato esta en el array, de lo contrario 0.
 */
int in_array(int* array, int size, int dato) {  // SACAR
  for (int i = 0; i < size; i++)
    if (dato == array[i]) return 1;
  return 0;
}

/*
 * nearest_neighbour : Ciudades Solucion Int -> Int
 * Recibe una estructura Ciudades, una Solucion y el nivel de recursion.
 * Aplica una heurística de vecino mas cercano, que permite hallar una cota
 * superior rapida de los datos dados. Esto permite disminuir las búsquedas en
 * la fuerza bruta para problemas grandes.
 * Si encontró un solucion devuelve 1, de lo contrario devuelve 0.
 */
int nearest_neighbour(Ciudades c, Solucion actual, int nivel) {
  if (nivel == 0) {
    actual->movimientos[0] = 0;
    actual->costo = 0;
    return nearest_neighbour(c, actual, 1);
  } else if (nivel == c->cantidad) {
    int costo = c->matrizCostos[actual->movimientos[nivel - 1]];
    if (costo != 0) {
      actual->costo += costo;
      return 1;
    } else
      return 0;
  } else {
    int posibles[c->cantidad], encontrados = 0;
    int anterior = actual->movimientos[nivel - 1];
    for (int i = 1; i < c->cantidad; i++) {
      int esta = 0;
      for (int j = 1; j < nivel && !esta; j++)
        if (i == actual->movimientos[j]) esta = 1;
      if (!esta) {
        int costoI = c->matrizCostos[anterior * c->cantidad + i];
        if (costoI != 0) {
          int j, termine = 0;
          for (j = 0; j < encontrados && !termine; j++) {
            int costoJ = c->matrizCostos[posibles[j] * c->cantidad + anterior];
            if (costoJ > costoI) termine = 1;
          }
          if (termine) {
            j--;
            for (int k = c->cantidad - 1; k > j; k--)
              posibles[k] = posibles[k - 1];
          }
          posibles[j] = i;
          encontrados++;
        }
      }
    }
    for (int i = 0; i < encontrados; i++) {
      int costo = c->matrizCostos[anterior * c->cantidad + posibles[i]];
      actual->costo += costo;
      actual->movimientos[nivel] = posibles[i];
      int resultado = nearest_neighbour(c, actual, nivel + 1);
      if (!resultado) {
        actual->costo -= costo;
      } else
        return 1;
    }
    return 0;
  }
}

/*
 * brute_force : Ciudades Solucion Solucion Int -> Void
 * Funcion recursiva, recibe:
 * *Estructura Ciudad
 * *Estructura Solucion (Donde almacena la mejor solucion conseguida)
 * *Estructura Solucion (Donde trabaja la funcion)
 * *Int (Elemento de control de nivel de recursion)
 * Devuelve el camino mas corto que pasa por todas las ciudades.
 * Si no se encuentra, mejor->costo permanece en -1.
 */
void brute_force(Ciudades c, Solucion mejor, Solucion actual, int nivel) {
  if (nivel == c->cantidad) {
    int costoVuelta = c->matrizCostos[actual->movimientos[nivel - 1]];
    if (costoVuelta != 0 &&
        (mejor->costo == -1 || actual->costo + costoVuelta < mejor->costo)) {
      for (int i = 0; i < c->cantidad; i++)
        mejor->movimientos[i] = actual->movimientos[i];
      mejor->costo = actual->costo + costoVuelta;
    }
  } else {
    for (int i = 1; i < c->cantidad; i++) {
      int esta = 0;
      for (int j = 1; j < nivel && !esta; j++)
        if (i == actual->movimientos[j]) esta = 1;
      if (!esta) {
        int indiceCosto = i * c->cantidad + actual->movimientos[nivel - 1];
        int costoViaje = c->matrizCostos[indiceCosto];
        if (costoViaje != 0 &&
            (mejor->costo == -1 || actual->costo + costoViaje < mejor->costo)) {
          actual->movimientos[nivel] = i;
          actual->costo += costoViaje;
          brute_force(c, mejor, actual, nivel + 1);
          actual->costo -= costoViaje;
        }
      }
    }
  }
}

/*
 * travelling_salesman_problem : Ciudades -> Solucion
 * Inicializa los argumentos necesarios para llamar a la función recursiva.
 * Devuelve la solucion de la recursión.
 */
Solucion travelling_salesman_problem(Ciudades c) {
  Solucion resultado = crea_solucion(c->cantidad);
  int haySolucion = 1;
  if (c->cantidad > 15) {  // Decidi el limite de 15 por que hasta 15 se
                           // consigue un tiempo raznable sin limitar
    int correcto = (nearest_neighbour(c, resultado, 0));
    if (!correcto) {
      resultado->costo = -1;
      haySolucion = 0;
    }
  }
  if (haySolucion) {
    Solucion trabajo = crea_solucion(c->cantidad);
    trabajo->costo = 0;
    trabajo->movimientos[0] = 0;
    brute_force(c, resultado, trabajo, 1);
    solucion_destruir(trabajo);
  }

  return resultado;
}

/*
 * imprime_salida : Char* Ciudades Solucion -> Void
 * Recibe Una estructura Ciudades, una Solucion y el archivo de salida.
 * Imprime la Solucion en el archivo.
 */
void imprime_salida(char* archivoSalida, Ciudades c, Solucion resultado) {
  FILE* archivo = fopen(archivoSalida, "w");
  if (archivo == NULL) {
    printf("No se pudo crear el archivo \"%s\"", archivoSalida);
    printf(" la solución no se almacenó\n");
  }
  int cantidadCiudades = c->cantidad;
  for (int i = 0; i < cantidadCiudades; i++) {
    int indice1 = resultado->movimientos[i % cantidadCiudades];
    int indice2 = resultado->movimientos[(i + 1) % cantidadCiudades];
    char* ciudad1 = c->nombres[indice1];
    char* ciudad2 = c->nombres[indice2];
    int costo = ciudades_devuelve_costo(c, indice1, indice2);
    fprintf(archivo, "%s,%s,%d\n", ciudad1, ciudad2, costo);
  }
  fprintf(archivo, "Costo total: %d\n", resultado->costo);
  fclose(archivo);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("La cantidad de argumentos es incorrecta.\n");
    return 0;
  }

  Ciudades c = lectura_archivo(argv[1]);

  if (c->cantidad == -1) {  // Significa que el archivo no existe en la lectura.
    ciudades_destruir(c);
    return 0;
  }

  // Si se desea resolver TSP de 3, debe comentarse este if.
  if (c->cantidad < 3) {  // Cantidad menor a 3 implica un problema trivial.
    printf("La cantidad de ciudades obtenida es menor a 3.\n");
    ciudades_destruir(c);
    return 0;
  }

  Solucion s = travelling_salesman_problem(c);

  if (s->costo == -1) {
    printf("No se encontro una solucion con los datos dados.\n");
    ciudades_destruir(c);
    solucion_destruir(s);
    return 0;
  }

  imprime_salida(argv[2], c, s);
  ciudades_destruir(c);
  solucion_destruir(s);
  return 1;
}