#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

typedef struct nodo {

    void* dato;
    struct nodo* proximo;

}nodo_t;

struct hash{

};
struct hash_iter{

};

nodo_t *crear_nodo(void *dato){

    nodo_t* nodo = malloc(sizeof(nodo_t));

    if (!nodo)     return NULL;

    nodo->dato = dato;
    nodo->proximo = NULL;

    return nodo;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

    hash_t* hash = malloc(sizeof(hash_t));

    if(!hash)  return NULL;
    //aca va todo lo que tenga en el struct pero ni idea que tiene

    return hash;
}

