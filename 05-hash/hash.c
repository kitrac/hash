#include <stdlib.h>
#include <stdio.h>
#include "hash.h"
#define LARGO_INICIAL 10

/* *****************************************************************
 *                             STRUCTS
 * *****************************************************************/


typedef struct estado{
    char* vacio;
    char* ocupado;
    char* borrado;

}estado_t;

typedef struct nodo {

    void* dato;
    struct nodo* proximo;

}nodo_t;

typedef struct hash_campo {
    char *clave;
    void *valor;
    estado_t estado; 
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

hash_campo_t* crear_tabla_hash (){

    hash_campo_t* tabla = malloc((hash->largo)*sizeof(hash_campo_t*));

    if (!tabla) return NULL;

    for (int i=0;i<hash->largo;i++)

      tabla[i]=VACIO;           //EL vacio esta mal 

    tabla->clave = NULL;     //         no tengo ni 
    tabla->valor = NULL;     //         idea de como
    tabla->estado = "vacio"; //         van estos tres
    return tabla;
}

nodo_t *crear_nodo(void *dato){

    nodo_t* nodo = malloc(sizeof(nodo_t));

    if (!nodo)     return NULL;

    nodo->dato = dato;
    nodo->proximo = NULL;

    return nodo;
}

   
int Hashing(char *cad){	
    int valor;
    for (valor=0; ; c++)
   	  valor += (int)*c;
    return (valor%hash->largo);
}


/* *****************************************************************
 *                            PRIMITIVAS
 * *****************************************************************/


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));

    if(!hash)  return NULL;

    hash->tabla = crear_tabla_hash();

    if(!tabla){

        free(hash);

        return NULL;
    }
    hash->destruir_dato = destruir_dato;

    hash->largo = LARGO_INICIAL;

    hash->cantidad = 0;

    hash->carga = 0;

    return hash;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    nodo_t* nodo = crear_nodo(dato);
    if (nodo == NULL){
        return false;
    }
}

void hash_destruir(hash_t *hash){
    for (int t = 0 , i<hash->largo, i++){
        hash->destruir_dato(hash->tabla[i]);
    }
    free(hash)
}