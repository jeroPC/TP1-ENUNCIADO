#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include "tp1.h"

const int CAPACIDAD_INICIAL = 100;
const int LARGO_BUFFER = 1024;
const int TOTAL_POKEMONES  =6 ;


struct tp1 {
    struct pokemon *pokemones;  // Array dinámico de pokémones
    size_t cantidad;            // Cantidad actual de pokémones
    size_t capacidad;           // Capacidad máxima del array
};

enum tipo_pokemon string_a_tipo(const char *tipo_str) {
    errno = 0;
    if (!tipo_str) { errno = EINVAL; return TIPO_NORM; }
    
    if (strcmp(tipo_str, "ELEC") == 0) return TIPO_ELEC;
    if (strcmp(tipo_str, "FUEG") == 0) return TIPO_FUEG;
    if (strcmp(tipo_str, "PLAN") == 0) return TIPO_PLAN;
    if (strcmp(tipo_str, "AGUA") == 0) return TIPO_AGUA;
    if (strcmp(tipo_str, "NORM") == 0) return TIPO_NORM;
    if (strcmp(tipo_str, "FANT") == 0) return TIPO_FANT;
    if (strcmp(tipo_str, "PSI") == 0) return TIPO_PSI;
    if (strcmp(tipo_str, "LUCH") == 0) return TIPO_LUCH;

    
    errno = EINVAL; 
    return TIPO_NORM;}

const char* tipo_a_string(enum tipo_pokemon tipo) {
    switch (tipo) {
        case TIPO_ELEC: return "ELEC";
        case TIPO_FUEG: return "FUEG";
        case TIPO_PLAN: return "PLAN";
        case TIPO_AGUA: return "AGUA";
        case TIPO_NORM: return "NORM";
        case TIPO_FANT: return "FANT";
        case TIPO_PSI:  return "PSI";
        case TIPO_LUCH: return "LUCH";
        default:        return "NORM";
    }
}

static FILE* abrir_archivo_lectura(const char *nombre) {
    if (!nombre) return NULL;
    return fopen(nombre, "r");
}


char *leer_linea(FILE *archivo)
{
    if (!archivo)
        return NULL;

    size_t capacidad = CAPACIDAD_INICIAL;
    size_t usados = 0;

    char *linea = malloc(capacidad);
    if (!linea)
        return NULL;

    char buffer[LARGO_BUFFER];
    int terminada = 0;

    while (!terminada && fgets(buffer, LARGO_BUFFER, archivo)) {
        size_t leidos = strlen(buffer);

        size_t requerido = usados + leidos + 1;
        if (capacidad < requerido) {
            size_t nueva = capacidad * 2;
            while (nueva < requerido)
                nueva *= 2;
            char *aux = realloc(linea, nueva);
            if (!aux) {
                free(linea);
                return NULL;
            }
            linea = aux;
            capacidad = nueva;
        }
        memcpy(linea + usados, buffer, leidos);
        usados += leidos;

        if (leidos > 0 && buffer[leidos - 1] == '\n') {
            terminada = 1;
        }
    }

    if (usados == 0) {
        free(linea);
        return NULL;
    }

    linea[usados] = '\0';

    if (usados > 0 && linea[usados - 1] == '\n') {
        linea[usados - 1] = '\0';
    }

    if (ferror(archivo)) {
        free(linea);
        return NULL;
    }
    return linea;
}

static bool redimensionar_Array(tp1_t *tp1){

    size_t nueva_capacidad = tp1->capacidad * 2 ;
    
    struct pokemon *nuevo_array = realloc(tp1->pokemones , sizeof(struct pokemon) * nueva_capacidad);

    if (!nuevo_array){
        return false;
    }

    tp1->pokemones = nuevo_array;
    tp1->capacidad = nueva_capacidad;
    
    return true;

}

static bool string_a_entero(const char *cadena, int *numero) {
    if (!cadena || !numero) return false;
    char *fin = NULL;
    errno = 0;
    long valor = strtol(cadena, &fin, 10);
    if (errno != 0 || fin == cadena || *fin != '\0') return false;
    if (valor < INT_MIN || valor > INT_MAX) return false;
    *numero = (int)valor;
    return true;
}

static bool mapear_tipo(const char *cadena, enum tipo_pokemon *numero){
    if(!cadena ||!numero)return false;
    enum tipo_pokemon tipo = string_a_tipo(cadena);
    if (tipo == TIPO_NORM && strcmp(cadena,"NORM") != 0) return false;
    *numero = tipo;
    return true;
}

static struct pokemon *crear_pokemon(char *campos[6])
{
    struct pokemon *pk = malloc(sizeof *pk);
    if (!pk) return NULL;

    if (!string_a_entero(campos[0], &pk->id)) {
        free(pk);
        return NULL;
    }

    pk->nombre = malloc(strlen(campos[1]) + 1);
    if (!pk->nombre) {
        free(pk);
        return NULL;
    }
    strcpy(pk->nombre, campos[1]);

    if (!mapear_tipo(campos[2], &pk->tipo)) {
        free(pk->nombre);
        free(pk);
        return NULL;
    }

    if (!string_a_entero(campos[3], &pk->ataque) ||
        !string_a_entero(campos[4], &pk->defensa) ||
        !string_a_entero(campos[5], &pk->velocidad)) {
        free(pk->nombre);
        free(pk);
        return NULL;
    }
    return pk;
}




static struct pokemon* parsear_pokemon(const char *linea){
    if (!linea) return NULL;

    size_t len = strlen(linea) + 1;
    char *copia = malloc(len);
    if (!copia) return NULL;
    memcpy(copia, linea, len);

    enum { N_CAMPOS = TOTAL_POKEMONES ,SEPARACION_CAMPOS = N_CAMPOS - 1 };
    char *campos[N_CAMPOS] = {0};

    int idx = 0;
    char *p = copia;

    while (idx < SEPARACION_CAMPOS) {
        char *coma = strchr(p, ',');
        if (!coma) { free(copia); return NULL; }     
        *coma = '\0';
        if (*p == '\0') { free(copia); return NULL; } 
        campos[idx++] = p;
        p = coma + 1;
    }

    if (*p == '\0' || strchr(p, ',') != NULL) { free(copia); return NULL; }
    campos[5] = p;

    struct pokemon *pk = crear_pokemon(campos);
    if (!pk) { free(copia); return NULL; }

    free(copia);
    return pk;
}


tp1_t *tp1_leer_archivo(const char *nombre) {
    if (!nombre) return NULL;

    FILE *archivo = abrir_archivo_lectura(nombre);
    if (!archivo) return NULL;
    
    tp1_t *tp1 = malloc(sizeof(tp1_t));
    if (!tp1) {
        free(tp1);
        fclose(archivo);
        return NULL;
    }
    
    tp1->cantidad = 0;
    tp1->capacidad = 10; 
    tp1->pokemones = malloc(sizeof(struct pokemon) * tp1->capacidad);
    if (!tp1->pokemones) {
        free(tp1);
        fclose(archivo);
        return NULL;
    }
    
    char *linea;
     while ((linea = leer_linea(archivo)) != NULL) {
        struct pokemon *p = parsear_pokemon(linea);
        free(linea);

          if (p) {
            if (tp1->cantidad == tp1->capacidad) {
                if (!redimensionar_Array(tp1)) {
                    free(p->nombre);
                    free(p);
                    for (size_t i = 0; i < tp1->cantidad; i++) {
                        free(tp1->pokemones[i].nombre);
                    }
                    free(tp1->pokemones);
                    free(tp1);
                    fclose(archivo);
                    return NULL;
                }
            }
            tp1->pokemones[tp1->cantidad++] = *p; 
            free(p);
        }
    }

    fclose(archivo);
    return tp1;
}
