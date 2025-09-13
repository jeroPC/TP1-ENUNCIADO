#define _POSIX_C_SOURCE 200809L
#include "tp1.h"
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int CAPACIDAD_INICIAL = 100;
const int LARGO_BUFFER = 1024;
const int TOTAL_POKEMONES = 6;

void tp1_destruir(tp1_t *tp);
static void mergesort_pokemones_directo(struct pokemon *arr, size_t n);
typedef bool (*criterio_pokemon_t)(struct pokemon *a, struct pokemon *b);

struct tp1 {
	struct pokemon *pokemones; // Array dinámico de pokémones
	size_t cantidad; // Cantidad actual de pokémones
	size_t capacidad; // Capacidad máxima del array
};

enum tipo_pokemon string_a_tipo(const char *tipo_str)
{
	if (!tipo_str) {
		errno = EINVAL;
		return TIPO_NORM;
	}
	if (strcmp(tipo_str, "ELEC") == 0)
		return TIPO_ELEC;
	else if (strcmp(tipo_str, "FUEG") == 0)
		return TIPO_FUEG;
	else if (strcmp(tipo_str, "PLAN") == 0)
		return TIPO_PLAN;
	else if (strcmp(tipo_str, "AGUA") == 0)
		return TIPO_AGUA;
	else if (strcmp(tipo_str, "NORM") == 0)
		return TIPO_NORM;
	else if (strcmp(tipo_str, "FANT") == 0)
		return TIPO_FANT;
	else if (strcmp(tipo_str, "PSI") == 0)
		return TIPO_PSI;
	else if (strcmp(tipo_str, "LUCH") == 0)
		return TIPO_LUCH;
	errno = EINVAL;
	return TIPO_NORM;
}

const char *tipo_a_string(enum tipo_pokemon tipo)
{
	switch (tipo) {
	case TIPO_ELEC:
		return "ELEC";
	case TIPO_FUEG:
		return "FUEG";
	case TIPO_PLAN:
		return "PLAN";
	case TIPO_AGUA:
		return "AGUA";
	case TIPO_NORM:
		return "NORM";
	case TIPO_FANT:
		return "FANT";
	case TIPO_PSI:
		return "PSI";
	case TIPO_LUCH:
		return "LUCH";
	default:
		return "NORM";
	}
}

static FILE *abrir_archivo_lectura(const char *nombre)
{
	if (!nombre)
		return NULL;
	return fopen(nombre, "r");
}

static char *leer_linea(FILE *archivo)
{
	if (!archivo)
		return NULL;

	size_t capacidad = CAPACIDAD_INICIAL, usados = 0;
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
		if (leidos > 0 && buffer[leidos - 1] == '\n')
			terminada = 1;
	}

	if (usados == 0) {
		free(linea);
		return NULL;
	}
	linea[usados] = '\0';
	if (usados > 0 && linea[usados - 1] == '\n')
		linea[usados - 1] = '\0';
	if (ferror(archivo)) {
		free(linea);
		return NULL;
	}
	return linea;
}
static bool redimensionar_Array(tp1_t *tp1)
{
	size_t nueva_capacidad = tp1->capacidad * 2;

	struct pokemon *nuevo_array = realloc(
		tp1->pokemones, sizeof(struct pokemon) * nueva_capacidad);

	if (!nuevo_array) {
		return false;
	}

	tp1->pokemones = nuevo_array;
	tp1->capacidad = nueva_capacidad;

	return true;
}

static bool string_a_entero(const char *cadena, int *numero)
{
	if (!cadena || !numero)
		return false;
	char *fin = NULL;
	errno = 0;
	long valor = strtol(cadena, &fin, 10);
	if (errno != 0 || fin == cadena || *fin != '\0')
		return false;
	if (valor < INT_MIN || valor > INT_MAX)
		return false;
	*numero = (int)valor;
	return true;
}

static bool mapear_tipo(const char *cadena, enum tipo_pokemon *numero)
{
	if (!cadena || !numero)
		return false;
	enum tipo_pokemon tipo = string_a_tipo(cadena);
	if (tipo == TIPO_NORM && strcmp(cadena, "NORM") != 0)
		return false;
	*numero = tipo;
	return true;
}

static struct pokemon *crear_pokemon(char *campos[6])
{
	struct pokemon *pk = malloc(sizeof *pk);
	if (!pk)
		return NULL;

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

static struct pokemon *parsear_pokemon(const char *linea)
{
	if (!linea)
		return NULL;

	char *copia = strdup(linea);
	if (!copia)
		return NULL;

	char *campos[6];
	char *p = copia;
	int i = 0;

	for (; i < 5; i++) {
		campos[i] = p;
		char *coma = strchr(p, ',');
		if (!coma) {
			free(copia);
			return NULL;
		}
		*coma = '\0';
		if (*campos[i] == '\0') {
			free(copia);
			return NULL;
		}
		p = coma + 1;
	}
	campos[5] = p;
	if (*p == '\0' || strchr(p, ',')) {
		free(copia);
		return NULL;
	}

	struct pokemon *pk = crear_pokemon(campos);
	free(copia);
	return pk;
}

tp1_t *tp1_leer_archivo(const char *nombre)
{
	if (!nombre)
		return NULL;

	FILE *archivo = abrir_archivo_lectura(nombre);
	if (!archivo)
		return NULL;

	tp1_t *tp1 = malloc(sizeof(tp1_t));
	if (!tp1) {
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
			bool repetido = false;
			for (size_t i = 0; i < tp1->cantidad && !repetido; i++)
				if (tp1->pokemones[i].id == p->id)
					repetido = true;

			if (!repetido) {
				if (tp1->cantidad == tp1->capacidad &&
				    !redimensionar_Array(tp1)) {
					free(p->nombre);
					free(p);
					for (size_t i = 0; i < tp1->cantidad;
					     i++)
						free(tp1->pokemones[i].nombre);
					free(tp1->pokemones);
					free(tp1);
					fclose(archivo);
					return NULL;
				}
				tp1->pokemones[tp1->cantidad] = *p;
				tp1->pokemones[tp1->cantidad].nombre =
					strdup(p->nombre);
				tp1->cantidad++;
			}
			free(p->nombre);
			free(p);
		}
	}

	fclose(archivo);
	mergesort_pokemones_directo(tp1->pokemones, tp1->cantidad);
	return tp1;
}

size_t tp1_cantidad(tp1_t *tp1)
{
	if (!tp1)
		return 0;
	return tp1->cantidad;
}

static FILE *abrir_archivo_escritura(const char *nombre)
{
	if (!nombre)
		return NULL;
	return fopen(nombre, "w");
}

static bool escribir_pokemon(FILE *archivo, struct pokemon *p)
{
	if (!archivo || !p)
		return false;
	const char *nombre = p->nombre ? p->nombre : "";
	if (fprintf(archivo, "%d,%s,%s,%d,%d,%d\n", p->id, nombre,
		    tipo_a_string(p->tipo), p->ataque, p->defensa,
		    p->velocidad) < 0)
		return false;
	return true;
}

tp1_t *tp1_guardar_archivo(tp1_t *tp1, const char *nombre)
{
	if (!tp1)
		return NULL;

	FILE *archivo = abrir_archivo_escritura(nombre);
	if (!archivo)
		return NULL;

	for (size_t i = 0; i < tp1->cantidad; i++) {
		if (!escribir_pokemon(archivo, &tp1->pokemones[i])) {
			fclose(archivo);
			return NULL;
		}
	}
	fclose(archivo);
	return tp1;
}

static tp1_t *tp1_combinar_capacidad(size_t capacidad)
{
	tp1_t *tp = malloc(sizeof(tp1_t));
	if (!tp)
		return NULL;
	tp->pokemones = malloc(sizeof(struct pokemon) * capacidad);
	if (!tp->pokemones) {
		free(tp);
		return NULL;
	}

	tp->capacidad = capacidad;
	tp->cantidad = 0;
	return tp;
}

static bool agregar_pokemon(tp1_t *dest, struct pokemon *src)
{
	struct pokemon *d = &dest->pokemones[dest->cantidad++];
	*d = *src;
	d->nombre = strdup(src->nombre);
	if (!d->nombre) {
		tp1_destruir(dest);
		return false;
	}
	return true;
}

static tp1_t *combinar_tp1(tp1_t *a, tp1_t *b, criterio_pokemon_t criterio)
{
	if (!a || !b)
		return NULL;
	tp1_t *nuevo = tp1_combinar_capacidad(a->cantidad + b->cantidad);
	if (!nuevo)
		return NULL;
	size_t i = 0, j = 0;
	while (i < a->cantidad || j < b->cantidad) {
		struct pokemon *pa = (i < a->cantidad) ? &a->pokemones[i] :
							 NULL;
		struct pokemon *pb = (j < b->cantidad) ? &b->pokemones[j] :
							 NULL;
		struct pokemon *a_agregar = NULL;

		if (pa && pb && pa->id == pb->id) {
			a_agregar = pa;
			i++;
			j++;
		} else if (pb == NULL || (pa && pa->id < pb->id)) {
			a_agregar = pa;
			i++;
		} else {
			a_agregar = pb;
			j++;
		}

		if (a_agregar && criterio(pa, pb)) {
			if (!agregar_pokemon(nuevo, a_agregar)) {
				tp1_destruir(nuevo);
				return NULL;
			}
		}
	}
	return nuevo;
}

bool criterio_union(struct pokemon *a, struct pokemon *b)
{
	(void)a;
	(void)b;
	return true;
}

bool criterio_interseccion(struct pokemon *a, struct pokemon *b)
{
	return a && b && a->id == b->id;
}

bool criterio_diferencia(struct pokemon *a, struct pokemon *b)
{
	return a && (!b || a->id < b->id);
}

tp1_t *tp1_union(tp1_t *a, tp1_t *b)
{
	return combinar_tp1(a, b, criterio_union);
}

tp1_t *tp1_interseccion(tp1_t *a, tp1_t *b)
{
	return combinar_tp1(a, b, criterio_interseccion);
}

tp1_t *tp1_diferencia(tp1_t *a, tp1_t *b)
{
	return combinar_tp1(a, b, criterio_diferencia);
}

static struct pokemon *busqueda_pokemon_nombre(tp1_t *tp, const char *nombre)
{
	if (!tp || !nombre)
		return NULL;
	size_t i = 0;
	bool encontrado = false;
	while (i < tp->cantidad && !encontrado) {
		if (strcmp(tp->pokemones[i].nombre, nombre) == 0) {
			encontrado = true;
			return &tp->pokemones[i];
		}
		i++;
	}
	return NULL;
}

struct pokemon *tp1_buscar_nombre(tp1_t *tp, const char *nombre)
{
	if (!tp || !nombre)
		return NULL;
	return (busqueda_pokemon_nombre(tp, nombre));
}

static void mergesort_pokemones_directo(struct pokemon *arr, size_t n)
{
	if (n < 2)
		return;

	size_t mid = n / 2;
	struct pokemon *left = malloc(mid * sizeof(struct pokemon));
	struct pokemon *right = malloc((n - mid) * sizeof(struct pokemon));
	if (!left || !right) {
		free(left);
		free(right);
		return;
	}

	memcpy(left, arr, mid * sizeof(struct pokemon));
	memcpy(right, arr + mid, (n - mid) * sizeof(struct pokemon));

	mergesort_pokemones_directo(left, mid);
	mergesort_pokemones_directo(right, n - mid);

	size_t i = 0, j = 0, k = 0;
	while (i < mid && j < n - mid) {
		if (left[i].id <= right[j].id)
			arr[k++] = left[i++];
		else
			arr[k++] = right[j++];
	}
	while (i < mid)
		arr[k++] = left[i++];
	while (j < n - mid)
		arr[k++] = right[j++];
	free(left);
	free(right);
}

static struct pokemon *busqueda_id(tp1_t *tp, int id, size_t inicio, size_t fin)
{
	if (!tp || inicio > fin) {
		return NULL;
	}

	size_t medio = (inicio + fin) / 2;

	if (tp->pokemones[medio].id == id) {
		return &tp->pokemones[medio];
	} else if (tp->pokemones[medio].id < id) {
		return busqueda_id(tp, id, medio + 1, fin);
	} else {
		if (medio == 0)
			return NULL;
		return busqueda_id(tp, id, inicio, medio - 1);
	}
}

struct pokemon *tp1_buscar_id(tp1_t *tp, int id)
{
	if (!tp)
		return NULL;
	if (tp->cantidad == 0)
		return NULL;

	return busqueda_id(tp, id, 0, tp->cantidad - 1);
}

size_t tp1_con_cada_pokemon(tp1_t *un_tp, bool (*f)(struct pokemon *, void *),
			    void *extra)
{
	if (!un_tp || !f)
		return 0;
	size_t i = 0, n = un_tp->cantidad;
	bool cortar = false;
	while (i < n && !cortar) {
		cortar = !f(&un_tp->pokemones[i], extra);
		i++;
	}
	return i;
}

void tp1_destruir(tp1_t *tp)
{
	if (!tp)
		return;
	for (size_t i = 0; i < tp->cantidad; i++)
		free(tp->pokemones[i].nombre);
	free(tp->pokemones);
	free(tp);
}
