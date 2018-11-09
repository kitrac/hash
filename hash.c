#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#define LARGO 30
#define FACTOR_RED_MAX 0.7
#define FACTOR_RED_MIN 0.2
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
    bool inicio;
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
void claves_destruir(hash_campo_t *tabla,size_t largo){
    for (int i = 0 ; i<largo; i++){
        if(tabla[i].estado == OCUPADO){
            free(tabla[i].clave);
        }
    }
}

// uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
//   size_t i = 0;
//   uint32_t hash = 0;
//   while (i != length) {
//     hash += key[i++];
//     hash += hash << 10;
//     hash ^= hash >> 6;
//   }
//   hash += hash << 3;
//   hash ^= hash >> 11;
//   hash += hash << 15;
//   return hash;
// }


size_t hashing(const char * clave, const hash_t* hash)
{
    //  size_t indice = 11;
    //  int c;
    //  while ((c = *str++))
    //      indice = ((indice << 5) + indice) + c; /* hash * 33 + c */
    
    size_t indice = hash->largo;
    for (size_t i = 0; i< strlen(clave); i++){
        indice = 31 * (indice + clave[i]);
    }
    return indice%hash->largo;
}


/* *****************************************************************
 *                            PRIMITIVAS HASH
 * *****************************************************************/
bool busqueda (const hash_t* hash, const char* clave, size_t* indice){

    while (hash->tabla[*indice].estado != VACIO){

        if (hash->tabla[*indice].estado == OCUPADO){
            if (strcmp(hash->tabla[*indice].clave,clave) == 0) return true;
        }

        *indice = *indice + 1 ;

        if (*indice== hash->largo)  *indice = 0;

    }
    return false;
}

// size_t busqueda(const hash_t* hash, const char* clave){ 

//     size_t indice = hashing(clave,hash);

//     while(hash->tabla[indice].estado != VACIO){

//         if (hash->tabla[indice].estado == BORRADO){
//             indice++;
//             continue;
//         }
//         if (strcmp(hash->tabla[indice].clave,clave) == 0)    return indice;

//         indice++;

//         if (indice == hash->largo)  indice = 0;
//     }
//     return indice;
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
    size_t largo_viejo = hash->largo;

    hash->largo = tam_nuevo;

    hash_campo_t* tabla_nueva = crear_tabla_hash(tam_nuevo);

    hash_campo_t* tabla_vieja = hash->tabla;
    
    hash->tabla = tabla_nueva;

    hash->cantidad = 0;

    hash->borrados = 0;
    
    for ( int i = 0; i<largo_viejo; i++){
        if(tabla_vieja[i].estado == OCUPADO){
            hash_guardar(hash,tabla_vieja[i].clave,tabla_vieja[i].valor);
            // char* clave_aux = strdup(hash->tabla[i].clave);
            // size_t indice  = hashing(clave_aux,hash);
            // while(tabla_nueva[indice].estado != VACIO){
            //     indice++;
            //     if (indice == hash->largo)  indice = 0;
            // }
            // tabla_nueva[indice].estado = OCUPADO;
            // tabla_nueva[indice].clave = clave_aux;
            // tabla_nueva[indice].valor = hash->tabla[i].valor;
        }
    }
    claves_destruir(tabla_vieja,largo_viejo);
    free(tabla_vieja);
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    size_t divv = hash->cantidad+hash->borrados;

    float red =((float)(divv)/(float)(hash->largo));

    if(red >= FACTOR_RED_MAX){
        hash_redimensionar(hash,hash->largo*FACTOR_MULT);
    }

    // if (red <= FACTOR_RED_MIN){
    //     hash_redimensionar(hash,hash->largo/2);
    // }

    size_t indice = hashing(clave,hash);

    if (busqueda(hash,clave,&indice)){
        void* dato_viejo = hash->tabla[indice].valor;
        hash->tabla[indice].valor = dato;
        if(hash->destruir_dato) hash->destruir_dato(dato_viejo);
        return true;
    }

    char *clave_aux;

    clave_aux = strdup(clave);


    hash->tabla[indice].clave = clave_aux;
    hash->tabla[indice].valor = dato;
    hash->tabla[indice].estado = OCUPADO;
    hash->cantidad++;

    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if(hash->cantidad == 0) return NULL;

    size_t indice = hashing(clave,hash);

    if (!busqueda(hash,clave,&indice)){
        return NULL;
    }

    hash->tabla[indice].estado = BORRADO;
    
    free(hash->tabla[indice].clave);

    hash->cantidad--;

    hash->borrados ++;

    return hash->tabla[indice].valor;
}

void *hash_obtener(const hash_t *hash, const char *clave){

    if(hash->cantidad == 0) return NULL; 

    size_t indice = hashing(clave,hash);

    if (!busqueda(hash,clave,&indice)){
        return NULL;
    }

    return hash->tabla[indice].valor;
    // while (hash->tabla[indice].estado != VACIO){
    //     if(hash->tabla[indice].estado == BORRADO  && strcmp(hash->tabla[indice].clave,clave) == 0){
    //         return NULL;
    //     }
    //     if (strcmp(hash->tabla[indice].clave,clave) == 0) return hash->tabla[indice].valor;
    //     indice++;
    //     if(indice == hash->largo)  indice = 0;
    // }

    //return hash->tabla[indice].valor;
}

bool hash_pertenece(const hash_t *hash, const char *clave){

    size_t indice = hashing(clave, hash);

    return busqueda(hash,clave,&indice);
    // while (hash->tabla[indice].estado != VACIO){
    //     if (strcmp(hash->tabla[indice].clave,clave) == 0 && hash->tabla[indice].estado == BORRADO){
    //         return false;
    //     }
    //     if (strcmp(hash->tabla[indice].clave,clave) == 0){
    //         return true;
    //     }
    //     indice++;
    //     if(indice == hash->largo)  indice = 0;
    // }
    // if (hash->tabla[indice].estado != OCUPADO){
    //     return false;
    // }
    // if (strcmp(hash->tabla[indice].clave,clave) == 0){
    //     return true;
    // }
    // else{
    //     return false;
    // }

}

void hash_destruir(hash_t *hash){
    for (int i = 0 ; i<hash->largo; i++){
        if(hash->tabla[i].estado == OCUPADO){
            if (hash->destruir_dato) hash->destruir_dato(hash->tabla[i].valor);
            free(hash->tabla[i].clave);
        }
            
    }

    free(hash->tabla);
    free(hash);
}



/* *****************************************************************
 *                            PRIMITIVAS ITERADOR
 * *****************************************************************/



size_t obtener_ocupado(hash_iter_t* iter){
    if (!iter->inicio){
        iter->indice++;
    }

    while (!hash_iter_al_final(iter) && iter->hash->tabla[iter->indice].estado != OCUPADO){
        iter->indice++;
    }
    if(hash_iter_al_final(iter)){
        return -1;
    }
    iter->inicio = false;
    return iter->indice;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){

    hash_iter_t* iter = malloc(sizeof(hash_iter_t));

    if(!iter){
        return NULL;
    }

    iter->hash = hash;

    iter->indice = 0;

    iter->inicio = true;

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