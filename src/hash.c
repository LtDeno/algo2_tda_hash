#include "hash.h"
#include "abb.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define CANT_MIN_CONTENEDORES 3
#define MULTIPLICADOR_TAMANIO_REHASH 4
#define INDICE_REHASH 0.75

typedef struct contenedor{
    void* elemento;
    char* clave;
}contenedor_t;

struct hash{
    abb_t** abbs_de_contenedores;
    hash_destruir_dato_t destructor;
    size_t cant_hashes;
    size_t cant_abb_usados;
    size_t cant_abb;
};

typedef struct contenedor_funciones{
    hash_t* hash;
    bool (*funcion)(hash_t*, const char*, void*);
    void* aux;
    bool parar;
    size_t contador;
}contenedor_funciones_t;

int comparar_contenedores(void* _contenedor1, void* _contenedor2){
    if (!_contenedor1){
        return -1;
    } else if (!_contenedor2){
        return 1;
    }
    contenedor_t* contenedor1 = _contenedor1;
    contenedor_t* contenedor2 = _contenedor2;
    return strcmp(contenedor1->clave, contenedor2->clave);
}

size_t funcion_hash(const char* clave){
    size_t resultado = (size_t)(*clave);
    size_t char_mas_grande = (size_t)(*clave);
    size_t long_clave = strlen(clave);

    for (size_t i = 1; i < long_clave; i++){
        size_t char_actual = (size_t)(*(clave+i));
        resultado = resultado + char_actual;

        if (char_actual > char_mas_grande){
            char_mas_grande = char_actual;
        }
    }

    return (resultado % char_mas_grande);
}

contenedor_t* crear_contenedor(const char* clave, void* elemento){
    if (!clave){
        return NULL;
    }

    contenedor_t* nuevo_contenedor = (contenedor_t*)calloc(1, sizeof(contenedor_t));
    if (!nuevo_contenedor){
        return NULL;
    }

    char* copia_clave = (char*)calloc(strlen(clave)+1, sizeof(char));
    if (!copia_clave){
        free(nuevo_contenedor);
        return NULL;
    }

    strcpy(copia_clave, clave);

    nuevo_contenedor->clave = copia_clave;
    nuevo_contenedor->elemento = elemento;

    return nuevo_contenedor;
}

/* Funcion adapatada para funcionar con el iterador de lista.

   Destruye la memoria almacenada para el contenedor,
   luego de aplicar el destructor a su elemento.
   El contenedor no puede ser nulo. Si el destructor
   no es nulo, lo aplica al elemento del contenedor.*/
bool destruir_contenedor(void* _contenedor, void* _destructor){
    if (!_contenedor){
        return false;
    }

    contenedor_t* contenedor = (contenedor_t*)_contenedor;
    if (_destructor){
        hash_destruir_dato_t destructor = *((hash_destruir_dato_t*)_destructor);

        if (destructor){
            destructor(contenedor->elemento);
        }
    }

    free(contenedor->clave);
    free(contenedor);
    return true;
}

/* Destruye la memoria almacenada para la lista,
   luego de aplicar el destructor de contenedores a
   cada contenedor propio.
   La lista no puede ser nula.*/
void destruir_abb_y_contenedores(abb_t* arbol, void* _destructor){
    if (!arbol){
        return;
    }

    abb_con_cada_elemento(arbol, PREORDEN, destruir_contenedor, _destructor);
    abb_destruir(arbol);
    return;
}

/* Funcion adaptada para funcionar con los iteradores de lista.
   El segundo parametro ha de ser un hash_t* y el primer parametro
   un contenedor_t*, ambos casteados a void* por la adaptacion.
   Inserta el elemento del contenedor en el hash auxiliar.
   Devuelve false si no se pudo insertar o true si se pudo.*/
bool insercion_de_rehash(void* _contenedor, void* _hash_aux){
    hash_t* hash_aux = _hash_aux;
    contenedor_t* contenedor = _contenedor;

    int estado_insercion = hash_insertar(hash_aux, contenedor->clave, contenedor->elemento);
    if (estado_insercion != 0){
        return false;
    }

    return true;
}

/* Transforma magicamente el hash pasado por parametro para que
   contenga una nueva cantidad de sublistas, siendo esta nueva
   cantidad la antigua multiplicada por MULTIPLICADOR_TAMANIO_REHASH.
   
   Los elementos pueden no estar ubicados en el indice de hash
   donde se encontraban ubicados previos al rehasheo.

   Operacin muy costosa: alta complejidad algoritmica, al tener que
   trasladar todos los elementos de un hash a otro. Alto uso de memoria,
   ya que necesita de un hash auxiliar completamente nuevo para
   poder operar la reasignacion de elementos al nuevo tamanio.
   
   Devuelve 0 en caso de salir tudo bom o -1 en caso contrario.*/
int rehash(hash_t* hash){
    if (!hash){
        return -1;
    }

    hash_t* hash_aux = hash_crear(hash->destructor, hash->cant_abb * MULTIPLICADOR_TAMANIO_REHASH);
    if (!hash_aux){
        return -1;
    }

    for (size_t i = 0; i < hash->cant_abb; i++){
        abb_con_cada_elemento(*(hash->abbs_de_contenedores+i), PREORDEN, insercion_de_rehash, (void*)hash_aux);
        destruir_abb_y_contenedores(*(hash->abbs_de_contenedores+i), NULL);
    }

    free(hash->abbs_de_contenedores);

    hash->abbs_de_contenedores = hash_aux->abbs_de_contenedores;
    hash->cant_abb = hash_aux->cant_abb;
    hash->cant_abb_usados = hash_aux->cant_abb_usados;
    hash->cant_hashes = hash_aux->cant_hashes;

    free(hash_aux);
    return 0;
}

/* Funcion adaptada para funcionar con el iterador de ABB.
   
   El primer parametro debe ser un contenedor_t* y el segundo un
   contenedor_funciones_t*. Ninguno puede ser nulo. Aplica la
   funcion del segundo parametro pasandole el hash y auxilair del
   mismo junto a la clave del contendor del primer parametro.

   Incrementa el contador de aplicaciones del contenedor_funciones
   y asigna el resultado de la funcion al "parar".

   Invierte el valor de retorno del "parar" de forma que coincida
   la condicion de iteracion del ABB con la de Hash.
   La funcion del segundo parametro debe tener las condiciones del
   hash_con_cada_elemento.*/
bool funcion_de_aplicar_funcion(void* _contenedor, void* _funcional){
    if (!_contenedor || !_funcional){
        return false;
    }

    contenedor_t* contenedor = (contenedor_t*)_contenedor;
    contenedor_funciones_t* funcional = (contenedor_funciones_t*)_funcional;
    funcional->parar = funcional->funcion(funcional->hash, contenedor->clave, funcional->aux);
    funcional->contador++;

    return !funcional->parar;
}

hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad_inicial){
    hash_t* nuevo_hash = (hash_t*)calloc(1, sizeof(hash_t));
    if (!nuevo_hash){
        return NULL;
    }

    nuevo_hash->destructor = destruir_elemento;
    if (capacidad_inicial > CANT_MIN_CONTENEDORES){
        nuevo_hash->cant_abb = capacidad_inicial;
    } else {
        nuevo_hash->cant_abb = CANT_MIN_CONTENEDORES;
    }

    nuevo_hash->abbs_de_contenedores = (abb_t**)calloc(nuevo_hash->cant_abb, sizeof(abb_t*));
    if (!nuevo_hash->abbs_de_contenedores){
        free(nuevo_hash);
        return NULL;
    }

    return nuevo_hash;
}

int hash_insertar(hash_t* hash, const char* clave, void* elemento){
    if (!hash || !clave){
        return -1;
    }

    double factor_de_carga = (double)hash->cant_abb_usados / (double)hash->cant_abb;
    if (factor_de_carga > INDICE_REHASH){

        int estado_rehash = rehash(hash);
        if (estado_rehash != 0){
            return -1;
        }
    }

    size_t indice_insercion = funcion_hash(clave) % hash->cant_abb;
    contenedor_t* nuevo_contenedor = crear_contenedor(clave, elemento);
    if (!nuevo_contenedor){
        return -1;
    }

    contenedor_t* posible_contenedor_ya_existente = abb_buscar(*(hash->abbs_de_contenedores+indice_insercion), nuevo_contenedor);
    if (posible_contenedor_ya_existente){
        if (hash->destructor){
            hash->destructor(posible_contenedor_ya_existente->elemento);
        }
        posible_contenedor_ya_existente->elemento = elemento;
        destruir_contenedor(nuevo_contenedor, NULL);
        return 0;
    }

    if (!(*(hash->abbs_de_contenedores+indice_insercion))){

        *(hash->abbs_de_contenedores+indice_insercion) = abb_crear(comparar_contenedores);
        if (!(*(hash->abbs_de_contenedores+indice_insercion))){
            destruir_contenedor(nuevo_contenedor, NULL);
            return -1;
        }

        hash->cant_abb_usados++;
    }

    if (!abb_insertar(*(hash->abbs_de_contenedores+indice_insercion), (void*)nuevo_contenedor)){
        destruir_contenedor(nuevo_contenedor, NULL);
        return -1;
    }

    hash->cant_hashes++;

    return 0;
}

int hash_quitar(hash_t* hash, const char* clave){
    if (!hash || !clave){
        return -1;
    }

    size_t indice_hash = funcion_hash(clave) % hash->cant_abb;
    contenedor_t* contenedor_aux = crear_contenedor(clave, NULL);
    if (!contenedor_aux){
        return -1;
    }

    contenedor_t* contenedor_buscado = (contenedor_t*)abb_quitar(*(hash->abbs_de_contenedores+indice_hash), (void*)contenedor_aux);
    if (!contenedor_buscado || (strcmp(contenedor_buscado->clave, contenedor_aux->clave) != 0)){
        destruir_contenedor(contenedor_aux, NULL);
        return -1;
    }

    destruir_contenedor(contenedor_buscado, &hash->destructor);
    hash->cant_hashes--;   
    destruir_contenedor(contenedor_aux, NULL);

    return 0;
}

void* hash_obtener(hash_t* hash, const char* clave){
    if (!hash || !clave){
        return NULL;
    }

    size_t indice_hash = funcion_hash(clave) % hash->cant_abb;
    contenedor_t* contenedor_aux = crear_contenedor(clave, NULL);
    if (!contenedor_aux){
        return NULL;
    }

    contenedor_t* contenedor_de_elemento = (contenedor_t*)abb_buscar(*(hash->abbs_de_contenedores+indice_hash), (void*)contenedor_aux);
    if (!contenedor_de_elemento || (strcmp(contenedor_de_elemento->clave, clave) != 0)){
        destruir_contenedor(contenedor_aux, NULL);
        return NULL;
    }

    destruir_contenedor(contenedor_aux, NULL);
    return contenedor_de_elemento->elemento;
}

bool hash_contiene(hash_t* hash, const char* clave){
    if (!hash || !clave){
        return false;
    }

    size_t indice_hash = funcion_hash(clave) % hash->cant_abb;
    contenedor_t* contenedor_aux = crear_contenedor(clave, NULL);
    if (!contenedor_aux){
        return false;
    }

    contenedor_t* contenedor_de_elemento = (contenedor_t*)abb_buscar(*(hash->abbs_de_contenedores+indice_hash), (void*)contenedor_aux);
    if (!contenedor_de_elemento || (strcmp(contenedor_de_elemento->clave, clave) != 0)){
        destruir_contenedor(contenedor_aux, NULL);
        return false;
    }

    destruir_contenedor(contenedor_aux, NULL);
    return true;
}

size_t hash_cantidad(hash_t* hash){
    if (!hash){
        return 0;
    }

    return hash->cant_hashes;
}

void hash_destruir(hash_t* hash){
    if (!hash){
        return;
    }

    for (size_t i = 0; i < hash->cant_abb; i++){
        destruir_abb_y_contenedores(*(hash->abbs_de_contenedores+i), &hash->destructor);
    }

    free(hash->abbs_de_contenedores);
    free(hash);
}

size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux){
    if (!hash || !funcion){
        return 0;
    }

    size_t indice_hash = 0;
    contenedor_funciones_t* funcional = (contenedor_funciones_t*)calloc(1, sizeof(contenedor_funciones_t));
    if (!funcional){
        return 0;
    }

    funcional->hash = hash;
    funcional->funcion = funcion;
    funcional->aux = aux;
    funcional->parar = false;

    while ((indice_hash < hash->cant_abb) && !funcional->parar){
        abb_con_cada_elemento(*(hash->abbs_de_contenedores+indice_hash), PREORDEN, funcion_de_aplicar_funcion, funcional);
        indice_hash++;
    }

    size_t recuento = funcional->contador;
    free(funcional);
    return recuento;
}
