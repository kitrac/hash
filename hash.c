#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#define LARGO 10
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
    const hash_t* hash;
    size_t indice;
};



/* *****************************************************************
 *                              EXTRAS
 * *****************************************************************/

hash_campo_t* crear_tabla_hash (size_t largo){

    hash_campo_t* tabla = malloc(largo*sizeof(hash_campo_t));

    if (!tabla) return NULL;

    for (int i=0;i<largo;i++){

        tabla[i].clave = NULL;
        tabla[i].valor = NULL; 
        tabla[i].estado = VACIO;
    }

    return tabla;
}

size_t hashing(const char *str, const hash_t* hash)
{
     size_t indice = 11;
     int c;

     while ((c = *str++))
         indice = ((indice << 5) + indice) + c; /* hash * 33 + c */
    
    /*size_t indice = hash->largo;
    for (size_t i = 0; i< strlen(str); i++){
        indice = 31 * (indice + str[i]);
    }/*/
    return indice%hash->largo;
}


/* *****************************************************************
 *                            PRIMITIVAS
 * *****************************************************************/


size_t busqueda(const hash_t* hash, char* clave){ 

    size_t indice = hashing(clave,hash);

    while(hash->tabla[indice].estado != VACIO){

        if(strcmp(hash->tabla[indice].clave,clave) == 0)    return indice;
        indice++;
        if (indice == hash->largo)  indice = 0;
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
    hash->carga = (hash->cantidad+hash->borrados)/hash->largo;

    hash->tabla = crear_tabla_hash(hash->largo);

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
    //printf("tam_nuevo: %ld\n", tam_nuevo);
    size_t largo = hash->largo;
    hash->largo = tam_nuevo;
    //hash_campo_t* tabla_nuevo = realloc(hash->tabla, tam_nuevo * sizeof(int));
    hash_campo_t* tabla_nueva = crear_tabla_hash(tam_nuevo);
    for ( int i = 0; i<largo; i++){
        if(hash->tabla[i].estado == OCUPADO){
            size_t indice  = hashing(hash->tabla[i].clave,hash);
            while(tabla_nueva[indice].estado != VACIO){
                indice++;
                if (indice == hash->largo)  indice = 0;
            }
            tabla_nueva[indice].estado = OCUPADO;
            tabla_nueva[indice].clave = hash->tabla[i].clave;
            tabla_nueva[indice].valor = hash->tabla[i].valor;
        }
    }
    hash->tabla =  tabla_nueva;
    hash->borrados = 0;
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    //a = (double) 5 / 2; /* a == 2.5*/
    size_t divv = hash->cantidad+hash->borrados;

    float red =((float)(divv)/(float)(hash->largo));

    if(red >= 0.7){
        hash_redimensionar(hash,hash->largo*FACTOR_MULT);
    }
    
    char *clave_aux;

    clave_aux = strdup(clave);

    size_t indice = busqueda(hash,clave_aux);

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

    size_t indice = busqueda(hash,clave_aux);
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
    while (hash->tabla[indice].estado != VACIO){
        if(strcmp(hash->tabla[indice].clave,clave) == 0 && hash->tabla[indice].estado == BORRADO){
            return NULL;
        }
        if (strcmp(hash->tabla[indice].clave,clave) == 0) return hash->tabla[indice].valor;
        indice++;
        if(indice == hash->largo)  indice = 0;
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
        if(indice == hash->largo)  indice = 0;
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

size_t obtener_ocupado(hash_iter_t* iter){
    if(iter->indice!=0)     iter->indice++;
    while(iter->hash->tabla[iter->indice].estado != OCUPADO && !hash_iter_al_final(iter)){
        iter->indice++;
    }
    if(hash_iter_al_final(iter)){
        return -1;
    }
    return iter->indice;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){

    hash_iter_t* iter = malloc(sizeof(hash_iter_t));

    if(!iter){
        return NULL;
    }

    iter->hash = hash;

    iter->indice = 0;

    obtener_ocupado(iter);

    return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){

    if(hash_iter_al_final(iter)) return false;

    if (obtener_ocupado(iter) == -1 ){
        return false;
    }

    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){

    if(hash_iter_al_final(iter)) return NULL;

    return iter->hash->tabla[iter->indice].clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
    return iter->indice == iter->hash->largo;
}

void hash_iter_destruir(hash_iter_t* iter){
    free(iter);
}