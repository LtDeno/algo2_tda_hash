#include "pa2mm.h"
#include "src/hash.h"
#include <string.h>

typedef struct queso{
    const char* nombre;
    const char* pais_de_origen;
}queso_t;

void destructor_elementos(void* elemento){
    if (!elemento){
        return;
    }

    queso_t* queso = elemento;
    printf("\nELIMINAÇÃO DO QUEIJO!!!\n");
    printf("Nombre: %s, pais de origen: %s\n", queso->nombre, queso->pais_de_origen);
    free(queso);
} 

bool son_claves_coincidentes(hash_t* hash, const char* clave1, void* _clave2){
    if (!hash || !clave1 || !_clave2){
        return false;
    }

    const char* clave2 = _clave2;
    if (strcmp(clave1, clave2) == 0){
        printf("\nTenemos una coincidencia de claves!\n");
        return true;
    }

    return false;
}

bool almacenar_queso(hash_t* hash, const char* nombre, const char* pais_de_origen){
    queso_t* quesito = calloc(1, sizeof(queso_t));
    if (!quesito){
        return false;
    }

    quesito->nombre = nombre;
    quesito->pais_de_origen = pais_de_origen;

    int estado_insercion = hash_insertar(hash, quesito->nombre, (void*)quesito);
    if (estado_insercion == 0){
        return true;
    }

    return false;
}

void pruebas_de_crear_y_destruir(){
    hash_t* hash = hash_crear(destructor_elementos, 2);
    pa2m_afirmar(hash, "Se pudo crear la tabla de hash vacio.");

    hash_destruir(hash);    
}

void pruebas_de_hash_nulo(){
    hash_t* hash = NULL;
    
    void* elemento_inexistente = hash_obtener(hash, "pocahontas");
    pa2m_afirmar(!elemento_inexistente, "No obtengo un elemento cualquiera.");

    bool contienent = hash_contiene(hash, "pocahontas");
    pa2m_afirmar(!contienent, "No contiene un elemento cualquiera.");

    int quitado = hash_quitar(hash, "pocahontas");
    pa2m_afirmar(quitado != 0, "No puedo quitar un elemento inexistente");

    size_t cant_hashes = hash_cantidad(hash);
    pa2m_afirmar(cant_hashes == 0, "Hay 0 elementos.");

    const char* nombre_buscado = "pocahontas";
    size_t cant_aplicaciones_funcion = hash_con_cada_clave(hash, son_claves_coincidentes, (void*)nombre_buscado);
    pa2m_afirmar(cant_aplicaciones_funcion == 0, "Nunca se aplica la funcion.");

    hash_destruir(hash);
}

void pruebas_de_hash_vacio(){
    hash_t* hash = hash_crear(destructor_elementos, 10);
    pa2m_afirmar(hash, "Se pudo crear la tabla de hash vacio.");

    void* elemento_inexistente = hash_obtener(hash, "pocahontas");
    pa2m_afirmar(!elemento_inexistente, "No obtengo un elemento cualquiera.");

    bool contienent = hash_contiene(hash, "pocahontas");
    pa2m_afirmar(!contienent, "No contiene un elemento cualquiera.");

    int quitado = hash_quitar(hash, "pocahontas");
    pa2m_afirmar(quitado != 0, "No puedo quitar un elemento inexistente");

    size_t cant_hashes = hash_cantidad(hash);
    pa2m_afirmar(cant_hashes == 0, "Hay 0 elementos.");

    const char* nombre_buscado = "pocahontas";
    size_t cant_aplicaciones_funcion = hash_con_cada_clave(hash, son_claves_coincidentes, (void*)nombre_buscado);
    pa2m_afirmar(cant_aplicaciones_funcion == 0, "Nunca se aplica la funcion.");

    hash_destruir(hash);
}

void pruebas_de_un_solo_elemento(){
    hash_t* hash = hash_crear(destructor_elementos, 1);

    pa2m_afirmar(almacenar_queso(hash, "Bandel", "India"), "Se pudo almacenar un queso.");
    pa2m_afirmar(hash_cantidad(hash) == 1, "Hay un queso en el hash.");
    pa2m_afirmar(hash_contiene(hash, "Bandel"), "Esta el queso insertado.");
    pa2m_afirmar(hash_obtener(hash, "Bandel"), "Se obtiene el queso buscado.");
    pa2m_afirmar(hash_con_cada_clave(hash, son_claves_coincidentes, "Bandel") == 1, "Se aplica 1 vez la funcion al elemento que si existe");
    pa2m_afirmar(hash_quitar(hash, "Bandel") == 0, "Se puede quitar el queso.");
    pa2m_afirmar(hash_cantidad(hash) == 0, "No hay quesos en el hash");
    almacenar_queso(hash, "Tuco", "Bangladesh");
    pa2m_afirmar(hash_contiene(hash, "Tuco"), "Se contiene el elemento buscado insertado nuevamente");
    pa2m_afirmar(hash_con_cada_clave(hash, son_claves_coincidentes, "Bandel") == 1, "Se aplica 1 vez la funcion pero no se llega al elemento que quiero");
    hash_quitar(hash, "Tuco");
    pa2m_afirmar(hash_con_cada_clave(hash, son_claves_coincidentes, "Tuco") == 0, "Se aplica 0 veceses la funcion porque esta vacio");
    pa2m_afirmar(!hash_contiene(hash, "Tuco"), "No se contiene el elemento buscado");

    hash_destruir(hash);
}

void pruebas_de_muchos_elementos(){
    hash_t* hash = hash_crear(destructor_elementos, 5);
    
    pa2m_afirmar(almacenar_queso(hash, "Cremoso", "Argentina"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Sardo", "Argentina"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Goya", "Argentina"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Feta", "Grecia"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Danbo", "Dinamarca"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Roquefort danes", "Dinamarca"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Fynbo", "Dinamarca"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Cheddar", "Reino Unido"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Untable", "Mi Corazon"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Chubut", "Argentina"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Reggianito", "Argentina"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Orda", "Argentina"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Provolone", "Italia"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Stracchino", "Italia"), "Se pudo almacenar un queso");
    pa2m_afirmar(almacenar_queso(hash, "Queso de soja", "D:"), "Se pudo almacenar un queso");

    pa2m_afirmar(hash_cantidad(hash) == 15, "La cant de hashes es 15");
    pa2m_afirmar(hash_quitar(hash, "Chubut") == 0, "Se puede quitar el queso Chubut");
    pa2m_afirmar(hash_cantidad(hash) == 14, "Despues de sacar el Chubut, la cant de hashes es 14");

    pa2m_afirmar(almacenar_queso(hash, "Queso de soja", "Japon"), "Puedo ingresar un elemento nuevo bajo la misma clave y me destruye el que ya tenia");
    pa2m_afirmar(hash_cantidad(hash) == 14, "La cantidad no se inmuta");

    pa2m_afirmar(hash_con_cada_clave(hash, son_claves_coincidentes, "Danbo") == 1, "Se aplica la funcion 1 vez porque mi primer elemento es el que quiero");
    pa2m_afirmar(hash_con_cada_clave(hash, son_claves_coincidentes, "Macaco") == 14, "Se aplica la funcion a todos los elementos porque nunca encuentro el que busco");

    hash_destruir(hash);
}

int main(){
    
    pa2m_nuevo_grupo("Pruebas: crear y destruir");
    pruebas_de_crear_y_destruir();

    pa2m_nuevo_grupo("Pruebas: hash nulo");
    pruebas_de_hash_nulo();
    
    pa2m_nuevo_grupo("Pruebas: hash vacio");
    pruebas_de_hash_vacio();

    pa2m_nuevo_grupo("Pruebas: un solo queso");
    pruebas_de_un_solo_elemento();

    pa2m_nuevo_grupo("Pruebas: muchos quesos");
    pruebas_de_muchos_elementos();

    return pa2m_mostrar_reporte();
}
