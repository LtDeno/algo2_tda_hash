#include "src/hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//strdup no lo podemos usar porque es POSIX pero no es C99
char* duplicar_string(const char* s){
  if(!s)
    return NULL;

  char* p = malloc(strlen(s)+1);
  strcpy(p,s);
  return p;
}

void destruir_string(void* elemento){
  if(elemento){
    printf("(Destructor) Libero el vehiculo: %s\n", (char*)elemento);
    free(elemento);
  }
}

bool mostrar_patente(hash_t* hash, const char* clave, void* aux){
  if(!clave)
    return true;

  aux=aux;
  hash=hash;

  printf("Patente en el hash: %s\n", clave);

  return false;
}

void guardar_vehiculo(hash_t* garage, const char* patente, const char* descripcion){
  int retorno = hash_insertar(garage, patente, duplicar_string(descripcion));
  printf("Guardando vehiculo patente %s (%s): ", patente, descripcion);
  printf("%s\n", retorno==0?"OK":"ERROR");
}

void quitar_vehiculo(hash_t* garage, const char* patente){
  int retorno = hash_quitar(garage, patente);
  printf("Retirando vehiculo patente %s: ", patente);
  printf("%s\n", retorno==0?"OK":"ERROR");
}

void verificar_vehiculo(hash_t* garage, const char* patente, bool deberia_existir){
  printf("Verifico el vehiculo patente %s: ", patente);
  bool retorno = hash_contiene(garage, patente);
  printf("%s\n", (retorno==deberia_existir)?"OK":"ERROR");
}

int main(){
  hash_t* garage = hash_crear(destruir_string, 3);

  printf("Agrego autos al garage\n");

  guardar_vehiculo(garage, "AC123BD", "Auto de Mariano");
  guardar_vehiculo(garage, "OPQ976", "Auto de Lucas");
  guardar_vehiculo(garage, "A421ACB", "Moto de Manu");
  guardar_vehiculo(garage, "AA442CD", "Auto de Guido");
  guardar_vehiculo(garage, "AC152AD", "Auto de Agustina");
  guardar_vehiculo(garage, "DZE443", "Auto de Jonathan");
  guardar_vehiculo(garage, "AA436BA", "Auto de Gonzalo");
  guardar_vehiculo(garage, "QDM443", "Auto de Daniela");
  guardar_vehiculo(garage, "BD123AC", "Auto de Pablo");
  guardar_vehiculo(garage, "CD442AA", "Auto de Micaela");
  guardar_vehiculo(garage, "PQO697", "Auto de Juan");
  guardar_vehiculo(garage, "DZE443", "Auto de Jonathan otra vez");
  guardar_vehiculo(garage, "AC152AD", "Auto de Agustina otra vez");

  verificar_vehiculo(garage, "QDM443", true);
  verificar_vehiculo(garage, "PQO697", true);

  
  quitar_vehiculo(garage, "QDM443");
  quitar_vehiculo(garage, "PQO697");

  verificar_vehiculo(garage, "QDM443", false);
  verificar_vehiculo(garage, "PQO697", false);
  

  size_t impresas = hash_con_cada_clave(garage, mostrar_patente, NULL);
  printf("Se mostraron %zu patentes con el iterador interno\n\n", impresas);

  hash_destruir(garage);

  return 0;
}
