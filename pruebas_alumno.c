#define _POSIX_C_SOURCE 200809L
#include "pa2m.h"
#include "src/tp1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // mkstemp, unlink

#define ARCHIVO_PRUEBA_INEXISTENTE "ejemplos/asdkasjhfskladjhfksdfhksdf.csv"
#define ARCHIVO_EJEMPLO "ejemplos/normal.csv"

/* -------------------------------------------------------------------------- */
/* Helpers para archivos temporales                                           */
/* -------------------------------------------------------------------------- */
static char *crear_archivo_temporal(const char *contenido) {
    char plantilla[] = "tp1_tmp_XXXXXX";
    int fd = mkstemp(plantilla);
    if (fd == -1)
        return NULL;

    FILE *f = fdopen(fd, "w");
    if (!f) {
        close(fd);
        unlink(plantilla);
        return NULL;
    }
    fputs(contenido, f);
    fclose(f);

    return strdup(plantilla); /* liberar con free() */
}

static void eliminar_archivo(const char *ruta) {
    if (ruta)
        unlink(ruta);
}

/* -------------------------------------------------------------------------- */
/* Pruebas tp1_leer_archivo / tp1_cantidad                                    */
/* -------------------------------------------------------------------------- */
void tp1_leer_archivo_devuelve_null_cuando_el_archivo_no_existe() {
    tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA_INEXISTENTE);
    pa2m_afirmar(tp1 == NULL, "Abrir un archivo inexistente devuelve NULL");
}

void tp1_leer_archivo_valido_devuelve_estructura() {
    tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_EJEMPLO);
    pa2m_afirmar(tp1 != NULL, "Abrir un archivo válido devuelve una estructura");
    if (tp1)
        tp1_destruir(tp1);
}

void tp1_cantidad_en_null_es_cero() {
    pa2m_afirmar(tp1_cantidad(NULL) == 0, "tp1_cantidad(NULL) devuelve 0");
}

/* -------------------------------------------------------------------------- */
/* Pruebas tp1_guardar_archivo                                                */
/* -------------------------------------------------------------------------- */
void guardar_con_tp1_null_devuelve_null() {
    pa2m_afirmar(tp1_guardar_archivo(NULL, "salida.csv") == NULL,
                 "Guardar con tp1 NULL devuelve NULL");
}

void guardar_con_nombre_null_devuelve_null() {
    tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_EJEMPLO);
    pa2m_afirmar(tp1 != NULL, "Se pudo leer archivo para probar guardado");
    pa2m_afirmar(tp1_guardar_archivo(tp1, NULL) == NULL, "Guardar con nombre NULL devuelve NULL");
    tp1_destruir(tp1);
}

void guardar_y_releer_mantiene_cantidad() {
    tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_EJEMPLO);
    pa2m_afirmar(tp1 != NULL, "Archivo de ejemplo leído correctamente");
    size_t cant_original = tp1_cantidad(tp1);

    char *temp = crear_archivo_temporal("");
    pa2m_afirmar(temp != NULL, "Se crea archivo temporal para guardar");

    pa2m_afirmar(tp1_guardar_archivo(tp1, temp) == tp1,
                 "tp1_guardar_archivo devuelve el mismo puntero en éxito");

    tp1_t *releido = tp1_leer_archivo(temp);
    pa2m_afirmar(releido != NULL, "Se puede releer el archivo guardado");
    pa2m_afirmar(releido && tp1_cantidad(releido) == cant_original,
                 "La cantidad coincide tras guardar y releer");

    tp1_destruir(tp1);
    tp1_destruir(releido);
    eliminar_archivo(temp);
    free(temp);
}

/* -------------------------------------------------------------------------- */
/* Pruebas tp1_union                                                          */
/* -------------------------------------------------------------------------- */
void union_con_parametros_null_devuelve_null() {
    pa2m_afirmar(tp1_union(NULL, NULL) == NULL, "tp1_union(NULL, NULL) devuelve NULL");
    tp1_t *a = tp1_leer_archivo(ARCHIVO_EJEMPLO);
    pa2m_afirmar(a != NULL, "Se lee un tp1 válido para probar NULL");
    pa2m_afirmar(tp1_union(a, NULL) == NULL, "tp1_union(a, NULL) devuelve NULL");
    pa2m_afirmar(tp1_union(NULL, a) == NULL, "tp1_union(NULL, a) devuelve NULL");
    tp1_destruir(a);
}

void union_de_dos_archivos_sin_repetidos_tiene_suma_de_cantidades() {
    /* Dos archivos sin IDs repetidos */
    const char *contA = "10,Pikachu,ELEC,55,40,90\n"
                        "11,Charmander,FUEG,52,43,65\n";
    const char *contB = "12,Bulbasaur,PLAN,49,49,45\n";

    char *a = crear_archivo_temporal(contA);
    char *b = crear_archivo_temporal(contB);
    tp1_t *tpA = tp1_leer_archivo(a);
    tp1_t *tpB = tp1_leer_archivo(b);
    size_t cantA = tp1_cantidad(tpA);
    size_t cantB = tp1_cantidad(tpB);

    tp1_t *u = tp1_union(tpA, tpB);
    pa2m_afirmar(u != NULL, "Unión de dos tp1 válidos devuelve un tp1");
    pa2m_afirmar(tp1_cantidad(u) == cantA + cantB, "Unión sin duplicados = suma de cantidades");

    tp1_destruir(tpA);
    tp1_destruir(tpB);
    tp1_destruir(u);
    eliminar_archivo(a);
    eliminar_archivo(b);
    free(a);
    free(b);
}

void union_de_dos_archivos_con_repetidos_no_duplica() {
    /* ID 20 repetido, debe priorizar el primero */
    const char *contA = "20,Eevee,NORM,55,50,55\n"
                        "21,Gastly,FANT,35,30,80\n";
    const char *contB = "20,Eevee,NORM,1,1,1\n" /* Repetido, datos distintos */
                        "22,Mew,PSI,100,100,100\n";

    char *a = crear_archivo_temporal(contA);
    char *b = crear_archivo_temporal(contB);
    tp1_t *tpA = tp1_leer_archivo(a);
    tp1_t *tpB = tp1_leer_archivo(b);

    size_t cantA = tp1_cantidad(tpA); // 2
    size_t cantB = tp1_cantidad(tpB); // 2

    tp1_t *u = tp1_union(tpA, tpB);
    pa2m_afirmar(u != NULL, "Unión con repetidos devuelve un tp1 válido");
    pa2m_afirmar(tp1_cantidad(u) == (cantA + cantB - 1),
                 "Cantidad en la unión descuenta el duplicado");

    tp1_destruir(tpA);
    tp1_destruir(tpB);
    tp1_destruir(u);
    eliminar_archivo(a);
    eliminar_archivo(b);
    free(a);
    free(b);
}

void buscar_nombre_devuelve_puntero_si_existe() {
    const char *contenido = "1,Pikachu,ELEC,55,40,90\n"
                            "2,Charmander,FUEG,52,43,65\n";
    char *archivo = crear_archivo_temporal(contenido);
    tp1_t *tp = tp1_leer_archivo(archivo);

    struct pokemon *p = tp1_buscar_nombre(tp, "Pikachu");
    pa2m_afirmar(p != NULL && strcmp(p->nombre, "Pikachu") == 0,
                 "Buscar por nombre devuelve puntero correcto si existe");

    struct pokemon *noexiste = tp1_buscar_nombre(tp, "Bulbasaur");
    pa2m_afirmar(noexiste == NULL, "Buscar por nombre devuelve NULL si no existe");

    tp1_destruir(tp);
    eliminar_archivo(archivo);
    free(archivo);
}

void interseccion_devuelve_tp1_con_pokemones_comunes() {
    const char *a = "1,Pikachu,ELEC,55,40,90\n"
                    "2,Charmander,FUEG,52,43,65\n";
    const char *b = "2,Charmander,FUEG,52,43,65\n"
                    "3,Bulbasaur,PLAN,49,49,45\n";
    char *fa = crear_archivo_temporal(a);
    char *fb = crear_archivo_temporal(b);
    tp1_t *tpA = tp1_leer_archivo(fa);
    tp1_t *tpB = tp1_leer_archivo(fb);

    tp1_t *inter = tp1_interseccion(tpA, tpB);
    pa2m_afirmar(inter != NULL, "Intersección devuelve un tp1 válido");
    pa2m_afirmar(tp1_cantidad(inter) == 1,
                 "Intersección contiene solo los pokemones comunes (por id)");

    struct pokemon *p = tp1_buscar_nombre(inter, "Charmander");
    pa2m_afirmar(p != NULL, "El pokemon común está en la intersección");

    tp1_destruir(tpA);
    tp1_destruir(tpB);
    tp1_destruir(inter);
    eliminar_archivo(fa);
    eliminar_archivo(fb);
    free(fa);
    free(fb);
}

void diferencia_devuelve_tp1_con_pokemones_unicos() {
    const char *a = "1,Pikachu,ELEC,55,40,90\n"
                    "2,Charmander,FUEG,52,43,65\n";
    const char *b = "2,Charmander,FUEG,52,43,65\n"
                    "3,Bulbasaur,PLAN,49,49,45\n";
    char *fa = crear_archivo_temporal(a);
    char *fb = crear_archivo_temporal(b);
    tp1_t *tpA = tp1_leer_archivo(fa);
    tp1_t *tpB = tp1_leer_archivo(fb);

    tp1_t *dif = tp1_diferencia(tpA, tpB);
    pa2m_afirmar(dif != NULL, "Diferencia devuelve un tp1 válido");
    pa2m_afirmar(tp1_cantidad(dif) == 1, "Diferencia contiene solo los pokemones únicos de A");

    struct pokemon *p = tp1_buscar_nombre(dif, "Pikachu");
    pa2m_afirmar(p != NULL, "El pokemon único está en la diferencia");

    tp1_destruir(tpA);
    tp1_destruir(tpB);
    tp1_destruir(dif);
    eliminar_archivo(fa);
    eliminar_archivo(fb);
    free(fa);
    free(fb);
}

void buscar_id_devuelve_puntero_si_existe() {
    const char *contenido = "0,yyy,ELEC,55,43,66\n"
                            "1,Pikachu,ELEC,55,40,90\n"
                            "2,Charmander,FUEG,52,43,65\n"
                            "2,Charmand,NORM,65,43,65\n"
                            "3,ABCDEFGHIJK,ELEC,45,43,63\n"
                            "4,Charmander,FUEG,52,43,65\n"
                            "6,ttt,FUEG,52,43,65\n";
    char *archivo = crear_archivo_temporal(contenido);
    tp1_t *tp = tp1_leer_archivo(archivo);

    struct pokemon *p0 = tp1_buscar_id(tp, 0);
    pa2m_afirmar(p0 != NULL && strcmp(p0->nombre, "yyy") == 0, "Buscar por ID 0 devuelve yyy");

    struct pokemon *p1 = tp1_buscar_id(tp, 1);
    pa2m_afirmar(p1 != NULL && strcmp(p1->nombre, "Pikachu") == 0,
                 "Buscar por ID 1 devuelve Pikachu");

    struct pokemon *p2 = tp1_buscar_id(tp, 2);
    pa2m_afirmar(p2 != NULL &&
                     (strcmp(p2->nombre, "Charmander") == 0 || strcmp(p2->nombre, "Charmand") == 0),
                 "Buscar por ID 2 devuelve uno de los pokemones con ID 2");

    struct pokemon *p3 = tp1_buscar_id(tp, 3);
    pa2m_afirmar(p3 != NULL && strcmp(p3->nombre, "ABCDEFGHIJK") == 0,
                 "Buscar por ID 3 devuelve ABCDEFGHIJK");

    struct pokemon *p4 = tp1_buscar_id(tp, 4);
    pa2m_afirmar(p4 != NULL && strcmp(p4->nombre, "Charmander") == 0,
                 "Buscar por ID 4 devuelve Charmander");

    struct pokemon *p6 = tp1_buscar_id(tp, 6);
    pa2m_afirmar(p6 != NULL && strcmp(p6->nombre, "ttt") == 0,
                 "Buscar por ID 6 devuelve pokemon sin nombre");

    struct pokemon *noexiste = tp1_buscar_id(tp, 999);
    pa2m_afirmar(noexiste == NULL, "Buscar por ID inexistente devuelve NULL");

    tp1_destruir(tp);
    eliminar_archivo(archivo);
    free(archivo);
}

static bool contar_pokemon(struct pokemon *p, void *extra) {
    (void)p;
    size_t *contador = extra;
    (*contador)++;
    return true;
}

static bool cortar_en_dos(struct pokemon *p, void *extra) {
    (void)p;
    size_t *contador = extra;
    (*contador)++;
    return *contador < 2; // true para 1, false para 2
}

/* -------------------------------------------------------------------------- */
/* Pruebas adicionales: nombres largos                                        */
/* -------------------------------------------------------------------------- */
static void prueba_guardar_con_nombre_muy_largo() {
    /* Generamos un nombre significativamente más largo que el buffer anterior (300) */
    char nombre_largo[1024];
    for (size_t i = 0; i < sizeof(nombre_largo) - 2; i++)
        nombre_largo[i] = 'A' + (char)(i % 26);
    nombre_largo[sizeof(nombre_largo) - 2] = 'Z';
    nombre_largo[sizeof(nombre_largo) - 1] = '\0';

    char linea[1400];
    snprintf(linea, sizeof(linea), "999,%s,ELEC,10,10,10\n", nombre_largo);

    char *archivo = crear_archivo_temporal(linea);
    pa2m_afirmar(archivo != NULL, "Se crea archivo temporal con nombre largo");
    tp1_t *tp = tp1_leer_archivo(archivo);
    pa2m_afirmar(tp && tp1_cantidad(tp) == 1, "Se lee pokemon con nombre largo correctamente");

    char *archivo_out = crear_archivo_temporal("");
    pa2m_afirmar(archivo_out != NULL, "Se crea archivo de salida temporal");
    pa2m_afirmar(tp1_guardar_archivo(tp, archivo_out) == tp,
                 "Se guarda archivo con nombre largo sin errores");

    tp1_t *releido = tp1_leer_archivo(archivo_out);
    pa2m_afirmar(releido && tp1_cantidad(releido) == 1, "Releer mantiene cantidad");
    struct pokemon *p = tp1_buscar_id(releido, 999);
    pa2m_afirmar(p && strcmp(p->nombre, nombre_largo) == 0,
                 "El nombre largo se conserva sin truncamiento");

    tp1_destruir(tp);
    tp1_destruir(releido);
    eliminar_archivo(archivo);
    eliminar_archivo(archivo_out);
    free(archivo);
    free(archivo_out);
}

static void prueba_varios_nombres_largos() {
    /* Crear múltiples pokemones con nombres largos distintos para estresar guardado */
    char buffer[8192] = {0};
    for (size_t idx = 0; idx < 5u; idx++) { /* idx como size_t para evitar sign-conversion */
        char nombre[600];
        for (size_t i = 0; i < sizeof(nombre) - 2; i++)
            nombre[i] = (char)('a' + ((i + idx) % 26u));
        nombre[sizeof(nombre) - 2] = (char)('0' + (char)idx);
        nombre[sizeof(nombre) - 1] = '\0';
        char linea[700];
        snprintf(linea, sizeof(linea), "%zu,%s,AGUA,1,1,1\n", (size_t)(2000u + idx), nombre);
        strcat(buffer, linea);
    }
    char *archivo = crear_archivo_temporal(buffer);
    pa2m_afirmar(archivo != NULL, "Se crea archivo temporal con varios nombres largos");
    tp1_t *tp = tp1_leer_archivo(archivo);
    pa2m_afirmar(tp && tp1_cantidad(tp) == 5, "Se leen todos los pokemones de nombres largos");

    char *salida = crear_archivo_temporal("");
    pa2m_afirmar(salida != NULL, "Se crea archivo de salida para varios");
    pa2m_afirmar(tp1_guardar_archivo(tp, salida) == tp,
                 "Se guardan múltiples pokemones de nombres largos");

    tp1_t *releido = tp1_leer_archivo(salida);
    pa2m_afirmar(releido && tp1_cantidad(releido) == 5,
                 "Releer múltiples nombres largos mantiene cantidad");

    /* Verificamos un id intermedio */
    struct pokemon *p_test = tp1_buscar_id(releido, 2002);
    pa2m_afirmar(p_test != NULL, "Se puede buscar un pokemon intermedio tras guardado");

    tp1_destruir(tp);
    tp1_destruir(releido);
    eliminar_archivo(archivo);
    eliminar_archivo(salida);
    free(archivo);
    free(salida);
}

void test_tp1_con_cada_pokemon_recorre_todos() {
    tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_EJEMPLO);
    size_t contador = 0;
    size_t recorridos = tp1_con_cada_pokemon(tp1, contar_pokemon, &contador);
    pa2m_afirmar(recorridos == tp1_cantidad(tp1), "Recorre todos los pokemones");
    pa2m_afirmar(contador == tp1_cantidad(tp1), "Callback se llama por cada pokemon");
    tp1_destruir(tp1);
}

void test_tp1_con_cada_pokemon_corta_antes() {
    tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_EJEMPLO);
    size_t contador = 0;
    size_t recorridos = tp1_con_cada_pokemon(tp1, cortar_en_dos, &contador);
    pa2m_afirmar(recorridos == 2, "Corta el recorrido después de dos pokemones");
    pa2m_afirmar(contador == 2, "Callback se llama dos veces");
    tp1_destruir(tp1);
}

void test_tp1_con_cada_pokemon_null() {
    pa2m_afirmar(tp1_con_cada_pokemon(NULL, contar_pokemon, NULL) == 0,
                 "tp1_con_cada_pokemon con tp1 NULL devuelve 0");
    pa2m_afirmar(tp1_con_cada_pokemon((void *)1, NULL, NULL) == 0,
                 "tp1_con_cada_pokemon con callback NULL devuelve 0");
}

static void prueba_lineas_invalidas_y_largas() {
    /* Construimos un archivo con:
     * - Línea inválida (faltan campos)
     * - Línea con id repetido (se ignora duplicado)
     * - Nombre muy largo válido
     * - Línea con tipo inválido
     * - Línea válida final
     */
    char nombre_largo[600];
    for (size_t i = 0; i < sizeof(nombre_largo) - 2; i++)
        nombre_largo[i] = 'A' + (char)(i % 26);
    nombre_largo[sizeof(nombre_largo) - 2] = 'Q';
    nombre_largo[sizeof(nombre_largo) - 1] = '\0';

    char buffer[4096];
    snprintf(buffer, sizeof(buffer),
             "1,Pika\n"                     /* inválida */
             "2,Charmander,FUEG,52,43,65\n" /* válida */
             "2,Otro,FUEG,1,1,1\n"          /* repetido */
             "3,%s,ELEC,10,10,10\n"         /* nombre largo válido */
             "4,Squirtle,XXXX,48,65,43\n"   /* tipo inválida */
             "5,Jigglypuff,NORM,45,20,20\n" /* válida */,
             nombre_largo);

    char *archivo = crear_archivo_temporal(buffer);
    pa2m_afirmar(archivo != NULL, "Se crea archivo temporal mixto");
    tp1_t *tp = tp1_leer_archivo(archivo);
    /* Esperadas válidas: id 2, 3, 5 (la 1 inválida, 2 duplicada ignorada, 4 inválida) => 3 */
    pa2m_afirmar(tp && tp1_cantidad(tp) == 3,
                 "Se cargan solo las líneas válidas ignorando inválidas y duplicados");
    struct pokemon *p3 = tp1_buscar_id(tp, 3);
    pa2m_afirmar(p3 && strcmp(p3->nombre, nombre_largo) == 0,
                 "Nombre largo en línea mixta se preserva");
    struct pokemon *p5 = tp1_buscar_id(tp, 5);
    pa2m_afirmar(p5 && strcmp(p5->nombre, "Jigglypuff") == 0, "Última línea válida se carga");

    tp1_destruir(tp);
    eliminar_archivo(archivo);
    free(archivo);
}

int main() {
    pa2m_nuevo_grupo("Pruebas de lectura de archivos");
    tp1_leer_archivo_devuelve_null_cuando_el_archivo_no_existe();
    tp1_leer_archivo_valido_devuelve_estructura();
    tp1_cantidad_en_null_es_cero();

    pa2m_nuevo_grupo("Pruebas de guardado");
    guardar_con_tp1_null_devuelve_null();
    guardar_con_nombre_null_devuelve_null();
    guardar_y_releer_mantiene_cantidad();

    pa2m_nuevo_grupo("Pruebas de unión");
    union_con_parametros_null_devuelve_null();
    union_de_dos_archivos_sin_repetidos_tiene_suma_de_cantidades();
    union_de_dos_archivos_con_repetidos_no_duplica();

    pa2m_nuevo_grupo("Pruebas de búsqueda por nombre");
    buscar_nombre_devuelve_puntero_si_existe();

    pa2m_nuevo_grupo("Pruebas de intersección");
    interseccion_devuelve_tp1_con_pokemones_comunes();

    pa2m_nuevo_grupo("Pruebas de diferencia");
    diferencia_devuelve_tp1_con_pokemones_unicos();

    pa2m_nuevo_grupo("Pruebas de búsqueda por ID");
    buscar_id_devuelve_puntero_si_existe();

    pa2m_nuevo_grupo("Pruebas adicionales nombres largos");
    prueba_guardar_con_nombre_muy_largo();
    prueba_varios_nombres_largos();

    pa2m_nuevo_grupo("Pruebas mixtas líneas inválidas");
    prueba_lineas_invalidas_y_largas();

    test_tp1_con_cada_pokemon_recorre_todos();
    test_tp1_con_cada_pokemon_corta_antes();
    test_tp1_con_cada_pokemon_null();

    return pa2m_mostrar_reporte();
}
