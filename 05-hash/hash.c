#include <stdlib.h>
#include <stdio.h>
#include "hash.h"
#define LARGO_INICIAL 10

/* *****************************************************************
 *                             STRUCTS
 * *****************************************************************/


/*typedef struct estado{
    char* vacio;
    char* ocupado;
    char* borrado;    

}estado_t;*/

/*typedef struct nodo {

    void* dato;
    struct nodo* proximo;

}nodo_t;*/

typedef struct hash_campo {
    char *clave;
    void *valor;
    int estado ; // va de 0 a 2 ocupado, vacio, borrado en ese orden no se si es lo mas correcto pero safa por el momento
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

size_t hashing(unsigned char *str)
{
    size_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


/* *****************************************************************
 *                            PRIMITIVAS
 * *****************************************************************/


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));

    if(!hash)  return NULL;

    hash->largo = LARGO_INICIAL;
    hash->destruir_dato = destruir_dato;
    hash->cantidad = 0;
    hash->carga = 0;

    hash->tabla = crear_tabla_hash();

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
    
    int clave_hash = hashing(clave);

    hash->tabla[clave_hash]->clave = clave_hash;
    hash->tabla[clave_hash]->valor = dato;
    hash->tabla[clave_hash]->estado->estado = 0;

    return true;
}

void hash_destruir(hash_t *hash){
    for (int t = 0 , i<hash->largo, i++){
        hash->destruir_dato(hash->tabla[i]);
    }
    free(hash)
}