#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#define LARGO_INICIAL 10

/* *****************************************************************
 *                             STRUCTS
 * *****************************************************************/


/*typedef struct estado{
    int estado; 

}estado_t;*/

/*typedef struct nodo {

    void* dato;
    struct nodo* proximo;

}nodo_t;*/

typedef struct hash_campo {
    char *clave;
    void *valor;
    int estado ; // va de 0 a 2 vacio, ocupado, borrado en ese orden no se si es lo mas correcto pero safa por el momento
} hash_campo_t;



struct hash {
    size_t cantidad;                  
    size_t largo;                     
    size_t carga;                     
    hash_campo_t *tabla; 
    hash_destruir_dato_t destruir_dato; 
};

struct hash_iter{

};



/* *****************************************************************
 *                              EXTRAS
 * *****************************************************************/

hash_campo_t* crear_tabla_hash (hash_t* hash){

    hash_campo_t* tabla = malloc((hash->largo)*sizeof(hash_campo_t*));

    if (!tabla) return NULL;

    for (int i=0;i<hash->largo;i++){

        hash->tabla[i].clave = NULL;
        hash->tabla[i].valor = NULL; 
        hash->tabla[i].estado = 1;
    }

    return tabla;
}

/*nodo_t *crear_nodo(void *dato){

    nodo_t* nodo = malloc(sizeof(nodo_t));

    if (!nodo)     return NULL;

    nodo->dato = dato;
    nodo->proximo = NULL;

    return nodo;
}*/

size_t hashing(char *str)
{
    size_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


/* *****************************************************************
 *                            PRIMITIVAS
 * *****************************************************************/

double hash_carga(const hash_t *hash){
    return hash->cantidad/hash->largo;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));

    if(!hash)  return NULL;

    hash->largo = LARGO_INICIAL;
    hash->destruir_dato = destruir_dato;
    hash->cantidad = 0;
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

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    
    char *clave_aux = malloc(sizeof(char)*strlen(clave));

    if (!clave_aux) return false;

    strcpy(clave_aux,clave);    // esto porque no me deja pasar clave porque es const

    size_t indice = hashing(clave_aux);

    while(hash->tabla[indice].estado == 0){
        indice++;
    }

    hash->tabla[indice].clave = clave_aux;
    hash->tabla[indice].valor = dato;
    hash->tabla[indice].estado = 0;

    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if(hash->cantidad == 0) return NULL;

    void* valor;

    size_t indice = hashing(clave);

    while ( hash->tabla[indice].estado != 0){
        if (hash->tabla[indice].clave == clave){
            hash->tabla[indice].estado = 2;
            valor = hash->tabla[indice].valor;
            return valor;
        }
        indice++;
    }
}

// void *hash_obtener(const hash_t *hash, const char *clave){
//     size_t clave_aux = hash(clave);
//     for( int i = 0 ; i < hash->largo;  i++){
//         hash->tabla[i] == hash->tabla[hash(clave)]
//     }
//}

// bool hash_pertenece(const hash_t *hash, const char *clave){

// }
/*void hash_destruir(hash_t *hash){
    for (int i = 0 ; i<hash->largo; i++){
        hash->destruir_dato(hash->tabla[i]);
    }
    free(hash);
}*/