#ifndef __HASH_H__
#define __HASH_H__

#include <stdbool.h>
#include <stddef.h>

typedef struct hash hash_t;

/*
 * Destructor de los datos almacenados en el hash.  Cada vez que un
 * elemento abandone el hash, debe invocarse al destructor pasandole
 * como parámetro dicho elemento.
*/
typedef void (*hash_destruir_dato_t)(void*);

/*
 * Crea el hash reservando la memoria necesaria para el.
 * Destruir_elemento es un destructor que se utilizará para liberar
 * los elementos que se eliminen del hash.  Capacidad indica la
 * capacidad inicial con la que se crea el hash. La capacidad inicial
 * no puede ser menor a 3. Si se solicita una capacidad menor, el hash
 * se creará con una capacidad de 3.
 *
 * Devuelve un puntero al hash creado o NULL en caso de no poder
 * crearlo.
 */
hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad_inicial);

/*
 * Inserta un elemento en el hash asociado a la clave dada.
 *
 * Nota para los alumnos: Recordar que si insertar un elemento provoca
 * que el factor de carga exceda cierto umbral, se debe ajustar el
 * tamaño de la tabla para evitar futuras colisiones.
 *
 * Devuelve 0 si pudo guardarlo o -1 si no pudo.
 */
int hash_insertar(hash_t* hash, const char* clave, void* elemento);

/*
 * Quita un elemento del hash e invoca la funcion destructora
 * pasandole dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 si no pudo.
 */
int hash_quitar(hash_t* hash, const char* clave);

/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe (o en caso de error).
 */
void* hash_obtener(hash_t* hash, const char* clave);

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario (o en caso de error).
 */
bool hash_contiene(hash_t* hash, const char* clave);

/*
 * Devuelve la cantidad de elementos almacenados en el hash o 0 en
 * caso de error.
 */
size_t hash_cantidad(hash_t* hash);

/*
 * Destruye el hash liberando la memoria reservada y asegurandose de
 * invocar la funcion destructora con cada elemento almacenado en el
 * hash.
 */
void hash_destruir(hash_t* hash);


/*
 * Recorre cada una de las claves almacenadas en la tabla de hash e
 * invoca a la función funcion, pasandole como parámetros el hash, la
 * clave en cuestión y el puntero auxiliar.
 *
 * Mientras que queden mas claves o la funcion retorne false, la
 * iteración continúa. Cuando no quedan mas claves o la función
 * devuelve true, la iteración se corta y la función principal
 * retorna.
 *
 * Devuelve la cantidad de claves totales iteradas (la cantidad de
 * veces que fue invocada la función) o 0 en caso de error.
 *
 */
size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux);

#endif /* __HASH_H__ */
