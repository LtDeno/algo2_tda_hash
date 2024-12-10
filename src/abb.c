#include "abb.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

abb_t* abb_crear(abb_comparador comparador){
  if (!comparador){
    return NULL;
  }

  abb_t* arbol = (abb_t*)calloc(1, sizeof(abb_t));
  if (!arbol){
    return NULL;
  }

  arbol->comparador = comparador;

  return arbol;
}

/* Recibe un nodo del arbol para comenzar la insercion y el elemento a insertar en el nodo.
   El comparador no puede ser nulo. Si el nodo es nulo, crea un nuevo nodo y le asigna el
   elemento.
   
   Devuelve el nodo insertado o NULL si algo no funciono.*/
nodo_abb_t* insertar_nodo(nodo_abb_t* nodo_inicio, void* elemento, abb_comparador comparador){
  if (!nodo_inicio){
    nodo_abb_t* nuevo_nodo = (nodo_abb_t*)calloc(1, sizeof(nodo_abb_t));
    if (!nuevo_nodo){
      return NULL;
    }

    nuevo_nodo->elemento = elemento;
    return nuevo_nodo;
  }
  
  int comparacion = comparador(elemento, nodo_inicio->elemento);
  if ((comparacion <= 0)){
    nodo_inicio->izquierda = insertar_nodo(nodo_inicio->izquierda, elemento, comparador);
  } else {
    nodo_inicio->derecha = insertar_nodo(nodo_inicio->derecha, elemento, comparador);
  }

  return nodo_inicio;
}

abb_t* abb_insertar(abb_t* arbol, void* elemento){
  if (!arbol){
    return NULL;
  }

  nodo_abb_t* nuevo_nodo = insertar_nodo(arbol->nodo_raiz, elemento, arbol->comparador);
  if (!nuevo_nodo){
    return NULL;
  }

  arbol->nodo_raiz = nuevo_nodo;
  arbol->tamanio++;

  return arbol;
}

/* Devuelve el nodo mas a la derecha del nodo recibido y
   asigna su padre al segundo parametro*/
nodo_abb_t* conseguir_nodo_mas_derecho_y_padre(nodo_abb_t* nodo_inicio, nodo_abb_t** nodo_padre){
  if (!nodo_inicio){
    return NULL;
  }

  nodo_abb_t* actual = nodo_inicio;

  while (actual->derecha){
    (*nodo_padre) = actual;
    actual = actual->derecha;
  }

  return actual;
}

/* Libera un nodo que tenga uno o cero hijos y devuelve su hijo (en caso de tener)*/
nodo_abb_t* destruir_nodo_sin_dos_hijos(nodo_abb_t* nodo_inicio){
  nodo_abb_t* nodo_hijo = NULL;
  if (nodo_inicio->derecha){
    nodo_hijo = nodo_inicio->derecha;
  } else {
    nodo_hijo = nodo_inicio->izquierda;
  }
      
  free(nodo_inicio);
  return nodo_hijo;
}

/* Libera un nodo que tenga 2 hijos, devuelve el predecesor inorder y acomoda
   el predecesor en el arbol*/   
nodo_abb_t* destruir_nodo_con_dos_hijos(nodo_abb_t* nodo_inicio){
  nodo_abb_t* padre_predecesor = nodo_inicio;
  nodo_abb_t* nodo_predecesor = conseguir_nodo_mas_derecho_y_padre(nodo_inicio->izquierda, &padre_predecesor);

  if (nodo_predecesor != nodo_inicio->izquierda){
    padre_predecesor->derecha = nodo_predecesor->izquierda;
    nodo_predecesor->izquierda = nodo_inicio->izquierda; 
  }
  nodo_predecesor->derecha = nodo_inicio->derecha;
  
  free(nodo_inicio);
  return nodo_predecesor;
}

nodo_abb_t* quitar_nodo(nodo_abb_t* nodo_inicio, void* elemento_buscado, void** elemento_encontrado, abb_comparador comparador){
  if (!nodo_inicio){
    return NULL;
  }

  int comparacion = comparador(elemento_buscado, nodo_inicio->elemento);

  if (comparacion == 0){
    (*elemento_encontrado) = nodo_inicio->elemento;

    if (nodo_inicio->izquierda && nodo_inicio->derecha){
      return destruir_nodo_con_dos_hijos(nodo_inicio);

    } else {
      return destruir_nodo_sin_dos_hijos(nodo_inicio);
    } 
  } else if (comparacion < 0){
    nodo_inicio->izquierda = quitar_nodo(nodo_inicio->izquierda, elemento_buscado, elemento_encontrado, comparador);
  } else {
    nodo_inicio->derecha = quitar_nodo(nodo_inicio->derecha, elemento_buscado, elemento_encontrado, comparador);
  }

  return nodo_inicio;
}

void* abb_quitar(abb_t* arbol, void *elemento){
  if (!arbol || abb_vacio(arbol)){
    return NULL;
  }

  void* elemento_a_quitar = NULL;
  arbol->nodo_raiz = quitar_nodo(arbol->nodo_raiz, elemento, &elemento_a_quitar, arbol->comparador);

  if (arbol->comparador(elemento_a_quitar, elemento) == 0){
    arbol->tamanio--;
    return elemento_a_quitar;
  }

  return NULL;
}

/* Busca el primer nodo que contenga el elemento buscado en el arbol.
   El comparador no puede ser nulo. 

   Devuelve el primer nodo contenedor del elemento o NULL en caso de error.*/
nodo_abb_t* buscar_nodo(nodo_abb_t* nodo_inicio, void* elemento, abb_comparador comparador){
  if (!nodo_inicio){
    return NULL;
  }

  int comparacion = comparador(elemento, nodo_inicio->elemento);

  if (comparacion == 0){
    return nodo_inicio;
  } else if (comparacion < 0){
    return buscar_nodo(nodo_inicio->izquierda, elemento, comparador);
  } else {
    return buscar_nodo(nodo_inicio->derecha, elemento, comparador);
  }

  return NULL;
}

void* abb_buscar(abb_t* arbol, void* elemento){
  if (!arbol){
    return NULL;
  }

  nodo_abb_t* nodo_con_tenedor = buscar_nodo(arbol->nodo_raiz, elemento, arbol->comparador);
  if (!nodo_con_tenedor){
    return NULL;
  }

  return nodo_con_tenedor->elemento;
}

bool abb_vacio(abb_t* arbol){
  return ((!arbol) || (arbol->tamanio == 0));
}

size_t abb_tamanio(abb_t *arbol){
  if (!arbol){
    return 0;
  }
  return arbol->tamanio;
}

/* Libera nodos recursivos y, en caso de haber,
   aplica el destructor al elemento de cada nodo.*/
void destruir_nodos(nodo_abb_t* nodo_inicio, void (*destructor)(void*)){
  if (!nodo_inicio){
    return;
  }

  destruir_nodos(nodo_inicio->izquierda, destructor);
  destruir_nodos(nodo_inicio->derecha, destructor);
  if (destructor){
    destructor(nodo_inicio->elemento);
  }

  free(nodo_inicio);
}

void abb_destruir(abb_t *arbol){
  abb_destruir_todo(arbol, NULL);
}

void abb_destruir_todo(abb_t *arbol, void (*destructor)(void *)){
  if (!arbol){
    return;
  }

  destruir_nodos(arbol->nodo_raiz, destructor);
  free(arbol);
}

/* Recorre en INORDEN nodos recursivos aplicando la funcion pasada por parametro,
   y deteniendose cuando la funcion devuelva FALSE.

   Devuelve la cantidad de veces que se haya aplicado la funcion haya devuelto
   TRUE o FALSE.*/
void funcion_a_elementos_inorder(nodo_abb_t* nodo_inicio, bool (*funcion)(void *, void *), void *aux, bool* continuar, size_t* contador){
  if (!nodo_inicio || !(*continuar)){
    return;
  }
  
  funcion_a_elementos_inorder(nodo_inicio->izquierda, funcion, aux, continuar, contador);

  if (*continuar){
    *continuar = funcion(nodo_inicio->elemento, aux);
    (*contador)++;
    if (!(*continuar)){
      return;
    }
  }

  funcion_a_elementos_inorder(nodo_inicio->derecha, funcion, aux, continuar, contador);

  return;
}

/* Recorre en PREORDER nodos recursivos aplicando la funcion pasada por parametro,
   y deteniendose cuando la funcion devuelva FALSE.

   Devuelve la cantidad de veces que se haya aplicado la funcion haya devuelto
   TRUE o FALSE.*/
void funcion_a_elementos_preorder(nodo_abb_t* nodo_inicio, bool (*funcion)(void *, void *), void *aux, bool* continuar, size_t* contador){
  if (!nodo_inicio || !(*continuar)){
    return;
  }

  *continuar = funcion(nodo_inicio->elemento, aux);
  (*contador)++;

  funcion_a_elementos_preorder(nodo_inicio->izquierda, funcion, aux, continuar, contador);
 
  funcion_a_elementos_preorder(nodo_inicio->derecha, funcion, aux, continuar, contador);

  return;
}

/* Recorre en POSTORDEN nodos recursivos aplicando la funcion pasada por parametro,
   y deteniendose cuando la funcion devuelva FALSE.

   Devuelve la cantidad de veces que se haya aplicado la funcion haya devuelto
   TRUE o FALSE.*/
void funcion_a_elementos_postorder(nodo_abb_t* nodo_inicio, bool (*funcion)(void *, void *), void *aux, bool* continuar, size_t* contador){
  if (!nodo_inicio || !(*continuar)){
    return;
  }

  funcion_a_elementos_postorder(nodo_inicio->izquierda, funcion, aux, continuar, contador);
  
  funcion_a_elementos_postorder(nodo_inicio->derecha, funcion, aux, continuar, contador);

  if (*continuar){
    *continuar = funcion(nodo_inicio->elemento, aux);
    (*contador)++;
  }

  return;
}

size_t abb_con_cada_elemento(abb_t *arbol, abb_recorrido recorrido, bool (*funcion)(void *, void *), void *aux){
  if (!arbol || !funcion){
    return 0;
  }

  bool continuar = true;
  size_t contador = 0;

  if (recorrido == INORDEN){
    funcion_a_elementos_inorder(arbol->nodo_raiz, funcion, aux, &continuar, &contador);
  } else if (recorrido == PREORDEN){
    funcion_a_elementos_preorder(arbol->nodo_raiz, funcion, aux, &continuar, &contador);
  } else{
    funcion_a_elementos_postorder(arbol->nodo_raiz, funcion, aux, &continuar, &contador);
  }

  return contador;
}

/* Recorre el nodos recursivos INORDER depositando sus elementos en el array.
   El contador recursivo debe estar inicializado en 0.

   Se detiene cuando no haya mas nodos a recorrer o el array ya este lleno.
   Devuelve la cantidad de depositos hechos.*/
void nodos_a_array_inorder(nodo_abb_t* nodo_inicio, void** array, size_t tamanio_array, size_t* contador_recursivo){
  if (!nodo_inicio || (tamanio_array == (*contador_recursivo))){
    return;
  }

  nodos_a_array_inorder(nodo_inicio->izquierda, array, tamanio_array, contador_recursivo);

  if (tamanio_array != (*contador_recursivo)){
    array[(*contador_recursivo)] = nodo_inicio->elemento;
    (*contador_recursivo)++;
    if (tamanio_array == (*contador_recursivo)){
      return;
    }
  }
  
  nodos_a_array_inorder(nodo_inicio->derecha, array, tamanio_array, contador_recursivo);

  return;
}

/* Recorre el nodos recursivos PREORDER depositando sus elementos en el array.
   El contador recursivo debe estar inicializado en 0.

   Se detiene cuando no haya mas nodos a recorrer o el array ya este lleno.
   Devuelve la cantidad de depositos hechos.*/
void nodos_a_array_preorder(nodo_abb_t* nodo_inicio, void** array, size_t tamanio_array, size_t* contador_recursivo){
  if (!nodo_inicio || (tamanio_array == (*contador_recursivo))){
    return;
  }

  array[(*contador_recursivo)] = nodo_inicio->elemento;
  (*contador_recursivo)++;

  nodos_a_array_preorder(nodo_inicio->izquierda, array, tamanio_array, contador_recursivo);

  nodos_a_array_preorder(nodo_inicio->derecha, array, tamanio_array, contador_recursivo);

  return;
}

/* Recorre el nodos recursivos POSTORDER depositando sus elementos en el array.
   El contador recursivo debe estar inicializado en 0.

   Se detiene cuando no haya mas nodos a recorrer o el array ya este lleno.
   Devuelve la cantidad de depositos hechos.*/
void nodos_a_array_postorder(nodo_abb_t* nodo_inicio, void** array, size_t tamanio_array, size_t* contador_recursivo){
  if (!nodo_inicio || (tamanio_array == (*contador_recursivo))){
    return;
  }

  nodos_a_array_postorder(nodo_inicio->izquierda, array, tamanio_array, contador_recursivo);

  nodos_a_array_postorder(nodo_inicio->derecha, array, tamanio_array, contador_recursivo);

  if (tamanio_array != (*contador_recursivo)){
    array[(*contador_recursivo)] = nodo_inicio->elemento;
    (*contador_recursivo)++; 
  }

  return;
}

size_t abb_recorrer(abb_t* arbol, abb_recorrido recorrido, void** array, size_t tamanio_array){
  if (!arbol || !array || (tamanio_array == 0)){
    return 0;
  }

  size_t contador_recursivo = 0;

  if (recorrido == INORDEN){
    nodos_a_array_inorder(arbol->nodo_raiz, array, tamanio_array, &contador_recursivo);
  } else if (recorrido == PREORDEN){
    nodos_a_array_preorder(arbol->nodo_raiz, array, tamanio_array, &contador_recursivo);
  } else {
    nodos_a_array_postorder(arbol->nodo_raiz, array, tamanio_array, &contador_recursivo);
  }

  return contador_recursivo;
}
