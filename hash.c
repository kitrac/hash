#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#define LARGO 29
#define FACTOR_RED 0.7
#define FACTOR_MULT 2
/* *****************************************************************
 *                             STRUCTS
 * *****************************************************************/



typedef enum {
    VACIO,
    OCUPADO,
    BORRADO,
}estado_t;


typedef struct hash_campo {
    char *clave;
    void *valor;
    estado_t estado;
} hash_campo_t;



struct hash {
    size_t cantidad;
    size_t borrados;            
    size_t largo;                     
    size_t carga;                     
    hash_campo_t *tabla; 
    hash_destruir_dato_t destruir_dato; 
};

struct hash_iter{
    hash_t* hash;
    size_t indice;

};



/* *****************************************************************
 *                              EXTRAS
 * *****************************************************************/

hash_campo_t* crear_tabla_hash (hash_t* hash){

    hash_campo_t* tabla = malloc((hash->largo)*sizeof(hash_campo_t));

    if (!tabla) return NULL;

    for (int i=0;i<hash->largo;i++){

        tabla[i].clave = NULL;
        tabla[i].valor = NULL; 
        tabla[i].estado = VACIO;
    }

    return tabla;
}

size_t hashing(const char *str, const hash_t* hash)
{
    // size_t hash = 11;
    // int c;

    // while ((c = *str++))
    //     hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    size_t indice = hash->largo;
    for (size_t i = 0; i< strlen(str); i++){
        indice = 31 * (indice + str[i]);
    }
    return indice%hash->largo;
}


/* *****************************************************************
 *                            PRIMITIVAS
 * *****************************************************************/


size_t buscar(const hash_t* hash, char* clave){ 

    size_t indice = hashing(clave,hash);

    while(hash->tabla[indice].estado != VACIO){

        if(strcmp(hash->tabla[indice].clave,clave) == 0)    return indice;
        indice++;
    }

    return indice;
}
// long unsigned int hash_carga(const hash_t *hash){
//     return 
// }

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));

    if(!hash)  return NULL;

    hash->largo = LARGO;
    hash->destruir_dato = destruir_dato;
    hash->cantidad = 0;
    hash->borrados = 0;
    hash->carga = 0;

    hash->tabla = crear_tabla_hash(hash);

    if(!hash->tabla){

        free(hash);
        return NULL;
    }

    return hash;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}


void hash_redimensionar(hash_t* hash, size_t tam_nuevo) {
    hash->largo = tam_nuevo;
    hash_campo_t* tabla_nuevo = realloc(hash->tabla, tam_nuevo * sizeof(int));
    for ( int i = 0; i<hash->largo; i++){
        if(hash->tabla[i].estado == OCUPADO){
            //char* hola = hash->tabla[i].clave;
            size_t indice  = hashing(hash->tabla[i].clave,hash);
            tabla_nuevo[indice].estado = OCUPADO;
            tabla_nuevo[indice].clave = hash->tabla[i].clave;
            tabla_nuevo[indice].valor = hash->tabla[i].valor;

        }
    }
    hash->tabla =  tabla_nuevo;
    hash->borrados = 0;
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if(((hash->cantidad+hash->borrados)/hash->largo)>= FACTOR_RED){
        hash_redimensionar(hash,hash->largo*FACTOR_MULT);
    }
    
    char *clave_aux = malloc(sizeof(char)*strlen(clave));
    
    if (!clave_aux) return false; 

    clave_aux = strdup(clave);

    size_t indice = buscar(hash,clave_aux);

    if (hash_pertenece(hash,clave)){
        hash->tabla[indice].valor = dato;
        return true;
    }

    hash->tabla[indice].clave = clave_aux;
    hash->tabla[indice].valor = dato;
    hash->tabla[indice].estado = OCUPADO;
    hash->cantidad++;

    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if(hash->cantidad == 0) return NULL;

    char* clave_aux;

    clave_aux = strdup(clave);

    size_t indice = buscar(hash,clave_aux);

    if (strcmp(hash->tabla[indice].clave,clave) == 0 && hash->tabla[indice].estado == BORRADO){
            return NULL;
        }

    hash->tabla[indice].estado = BORRADO;  
    
    hash->cantidad--;

    hash->borrados ++;

    return hash->tabla[indice].valor;
}

void *hash_obtener(const hash_t *hash, const char *clave){

    if(hash->cantidad == 0) return NULL; 

    char* clave_aux;

    clave_aux = strdup(clave);

    size_t indice = hashing(clave_aux,hash);

    if (strcmp(hash->tabla[indice].clave,clave) == 0 && hash->tabla[indice].estado == BORRADO){
            return NULL;
        }

    return hash->tabla[indice].valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){

    size_t indice = hashing(clave,hash);
    while (hash->tabla[indice].estado != VACIO){
        if (strcmp(hash->tabla[indice].clave,clave) == 0 && hash->tabla[indice].estado == BORRADO){
            return false;
        }
        if (strcmp(hash->tabla[indice].clave,clave) == 0){
            return true;
        }
        indice++;
    }
  
    return false;

}

void hash_destruir(hash_t *hash){
    for (int i = 0 ; i<hash->largo; i++){
        if(hash->tabla[i].estado != VACIO){
            if (hash->destruir_dato!=NULL) hash->destruir_dato(hash->tabla[i].valor);
        }
    }
    free(hash->tabla);
    free(hash);
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
    hash_iter_t* iter = malloc(sizeof(hash_iter_t));
    // if(!iter){
    //     return NULL;
    // }
    // iter->hash = hash;
    // iter->indice = 0;
    return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    return "adjsldjas";
}

bool hash_iter_al_final(const hash_iter_t *iter){
    return true;
}


void hash_iter_destruir(hash_iter_t* iter){
    free(iter);
}