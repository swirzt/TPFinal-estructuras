#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colasenlazadas.h"

// Si queres hace INLINE

typedef struct _Ciudades {
  int cantidad;
  char** nombres;
  int* matrizCostos;
  int costoViaje;
  int* movimientos;
} * Ciudades;

#define SIZEBUFFER 256

/*
 * crea_ciudades : Int -> Ciudades
 * Recibe un tamaño, devuelve una estructura Ciudades con la memoria alocada
 * necesaria para ese tamaño.
 */
Ciudades crea_ciudades(int cantidad) {
  Ciudades nuevaCiudades = malloc(sizeof(struct _Ciudades));
  nuevaCiudades->cantidad = cantidad;
  nuevaCiudades->costoViaje = -1;
  if (cantidad >= 0) {
    nuevaCiudades->nombres = malloc(sizeof(char*) * cantidad);
    nuevaCiudades->matrizCostos = calloc(cantidad * cantidad, sizeof(int));
    // La funcion calloc inicializa la matriz con valor 0
    // Según el PDF todos los costos son Enteros positivos
    nuevaCiudades->movimientos = malloc(sizeof(int) * cantidad);
  } else {
    nuevaCiudades->nombres = NULL;
    nuevaCiudades->matrizCostos = NULL;
    nuevaCiudades->movimientos = NULL;
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
  free(c->movimientos);
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
    return 0;
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
    if (bufferc == '\r') bufferc = fgetc(archivo);
  }

  free(buffer);
  free(buffer2);
  fclose(archivo);
  return c;
}

/*
 * nearest_neighbour : Ciudades Int -> Int
 * Recibe una estructura Ciudades y el nivel de recursion.
 * Aplica una heurística de vecino mas cercano, que permite hallar una cota
 * superior rápida de los datos dados. Esto permite disminuir las búsquedas en
 * la fuerza bruta para problemas grandes.
 * Almacena la solución en la estructura Ciudades.
 * Si encontró un solución devuelve 1, de lo contrario devuelve 0.
 */
int nearest_neighbour(Ciudades c, int nivel) {
  if (nivel == 0) {
    c->movimientos[0] = 0;
    c->costoViaje = 0;
    return nearest_neighbour(c, 1);
  } else if (nivel == c->cantidad) {
    int costo = c->matrizCostos[c->movimientos[nivel - 1]];
    if (costo != 0) {
      c->costoViaje += costo;
      return 1;
    } else
      return 0;
  } else {
    int posibles[c->cantidad], encontrados = 0;
    int anterior = c->movimientos[nivel - 1];
    for (int i = 1; i < c->cantidad; i++) {
      int esta = 0;
      for (int j = 1; j < nivel && !esta; j++)
        if (i == c->movimientos[j]) esta = 1;
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
      c->costoViaje += costo;
      c->movimientos[nivel] = posibles[i];
      int resultado = nearest_neighbour(c, nivel + 1);
      if (!resultado) {
        c->costoViaje -= costo;
      } else
        return 1;
    }
    return 0;
  }
}

/*
 * dfs : Ciudades Int* Int Int* Int -> Void
 * Recibe una estructura Ciudad, un array de ints y 2 ints.
 * El ultimo Int es el iterador de recursión.
 * Almacena la mejor solucion hallada en la estructura Ciudades.
 * Si no encuentra ninguna, costoViaje en la estructura permanece en -1.
 */
void dfs(Ciudades c, int* actual, int costoActual, int* visitados, int nivel) {
  if (nivel == c->cantidad) {
    // No utilizo la funcion ciudades_devuelve_costos porque aumenta el tiempo
    // de ejecución.
    int costoVuelta = c->matrizCostos[actual[nivel - 1]];
    if (costoVuelta != 0 &&
        (c->costoViaje == -1 || costoActual + costoVuelta < c->costoViaje)) {
      for (int i = 0; i < c->cantidad; i++) c->movimientos[i] = actual[i];
      c->costoViaje = costoActual + costoVuelta;
    }
  } else {
    for (int i = 1; i < c->cantidad;
         i++) {  // Prueba todos las ciudades posibles menos 0
      if (visitados[i] == 0) {
        // Mismo caso, no utilizo ciudades_devuelve_costos.
        int indiceCosto = i * c->cantidad + actual[nivel - 1];
        int costoViaje = c->matrizCostos[indiceCosto];

        if (costoViaje != 0 &&
            (c->costoViaje == -1 || costoActual + costoViaje < c->costoViaje)) {
          actual[nivel] = i;
          visitados[i] = 1;
          dfs(c, actual, costoActual + costoViaje, visitados, nivel + 1);
          visitados[i] = 0;
        }
      }
    }
  }
}

/*
 * travelling_salesman_problem : Ciudades -> Void
 * Inicializa los argumentos necesarios para llamar a la función recursiva.
 */
void travelling_salesman_problem(Ciudades c) {
  int haySolucion = 1;
  if (c->cantidad > 15) {
    // Decidí el limite de 15 por que hasta 15 conseguí un tiempo razonable solo
    // con fuerza bruta.
    int correcto = (nearest_neighbour(c, 0));
    if (!correcto) {
      c->costoViaje = -1;
      haySolucion = 0;
    }
  }
  // Si haySolucion = 0, implica que el nearest_neighbour no encontro solución,
  // por lo que no es necesario volver probar toda las posibilidades.
  if (haySolucion) {
    int* visitados = calloc(c->cantidad, sizeof(int));
    int iterador[c->cantidad];
    iterador[0] = 0;
    visitados[0] = 1;
    dfs(c, iterador, 0, visitados, 1);
    free(visitados);
  }
}

/*
 * imprime_salida : Char* Ciudades -> Void
 * Recibe el archivo de salida y una estructura Ciudades.
 * Imprime la solución en el archivo.
 */
void imprime_salida(char* archivoSalida, Ciudades c) {
  FILE* archivo = fopen(archivoSalida, "w");
  if (archivo == NULL) {
    printf("No se pudo crear el archivo \"%s\"", archivoSalida);
    printf(" la solución no se almacenó\n");
  }
  int cantidadCiudades = c->cantidad;
  for (int i = 0; i < cantidadCiudades; i++) {
    int indice1 = c->movimientos[i % cantidadCiudades];
    int indice2 = c->movimientos[(i + 1) % cantidadCiudades];
    char* ciudad1 = c->nombres[indice1];
    char* ciudad2 = c->nombres[indice2];
    int costo = ciudades_devuelve_costo(c, indice1, indice2);
    fprintf(archivo, "%s,%s,%d\n", ciudad1, ciudad2, costo);
  }
  fprintf(archivo, "Costo total: %d\n", c->costoViaje);
  // Línea no pedida pero me pareció conveniente.
  fclose(archivo);
}

/*
 * main : Int Char*[] -> Int
 * Funcion principal del programa.
 */
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

  travelling_salesman_problem(c);

  if (c->costoViaje == -1) {
    printf("No se encontro una solución con los datos dados.\n");
    ciudades_destruir(c);
    return 0;
  }

  imprime_salida(argv[2], c);
  ciudades_destruir(c);
  return 1;
}
