#include "src/tp1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void mostrar_uso(void)
{
	puts("Uso:");
	puts("./tp1 pokedex.csv buscar nombre <nombre>");
	puts("./tp1 pokedex.csv buscar id <id>");
	puts("./tp1 pokedex.csv union <otra.csv> <salida.csv>");
	puts("./tp1 pokedex.csv interseccion <otra.csv> <salida.csv>");
	puts("./tp1 pokedex.csv diferencia <otra.csv> <salida.csv>");
	puts("./tp1 pokedex.csv mostrar nombre");
	puts("./tp1 pokedex.csv mostrar id");
}

static void imprimir_pokemon(struct pokemon *pokemon)
{
	if (!pokemon)
		return;
	printf("%d,%s,%d,%d,%d,%d\n", pokemon->id,
	       pokemon->nombre ? pokemon->nombre : "", (int)pokemon->tipo,
	       pokemon->ataque, pokemon->defensa, pokemon->velocidad);
}

static bool callback_imprimir(struct pokemon *pokemon, void *extra)
{
	(void)extra;
	imprimir_pokemon(pokemon);
	return true;
}

struct contexto_llenado {
	struct pokemon **vector;
	size_t indice;
};

static bool callback_llenar_vector(struct pokemon *pokemon, void *extra)
{
	struct contexto_llenado *ctx = extra;
	ctx->vector[ctx->indice++] = pokemon;
	return true;
}

static int comparar_por_nombre(const void *a, const void *b)
{
	const struct pokemon *pa = *(const struct pokemon *const *)a;
	const struct pokemon *pb = *(const struct pokemon *const *)b;
	return strcmp(pa->nombre, pb->nombre);
}

static void mostrar_por_nombre(tp1_t *coleccion)
{
	size_t cantidad = tp1_cantidad(coleccion);
	if (!cantidad)
		return;

	struct pokemon **vector = malloc(cantidad * sizeof(*vector));
	if (!vector)
		return;

	struct contexto_llenado ctx = { vector, 0 };
	tp1_con_cada_pokemon(coleccion, callback_llenar_vector, &ctx);

	qsort(vector, ctx.indice, sizeof(*vector), comparar_por_nombre);

	for (size_t indice = 0; indice < ctx.indice; indice++)
		imprimir_pokemon(vector[indice]);

	free(vector);
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		mostrar_uso();
		return 1;
	}

	tp1_t *pokedex = tp1_leer_archivo(argv[1]);
	if (!pokedex) {
		fprintf(stderr, "Error leyendo %s\n", argv[1]);
		return 1;
	}

	int resultado = 0;
	const char *comando = argv[2];

	if (strcmp(comando, "buscar") == 0 && argc == 5) {
		struct pokemon *pokemon = NULL;
		if (strcmp(argv[3], "nombre") == 0)
			pokemon = tp1_buscar_nombre(pokedex, argv[4]);
		else if (strcmp(argv[3], "id") == 0)
			pokemon = tp1_buscar_id(pokedex, atoi(argv[4]));
		else {
			mostrar_uso();
			resultado = 1;
		}

		if (pokemon)
			imprimir_pokemon(pokemon);
		else
			puts("No encontrado");
	} else if ((strcmp(comando, "union") == 0 ||
		    strcmp(comando, "interseccion") == 0 ||
		    strcmp(comando, "diferencia") == 0) &&
		   argc == 5) {
		tp1_t *otra_coleccion = tp1_leer_archivo(argv[3]);
		if (!otra_coleccion) {
			fprintf(stderr, "Error leyendo %s\n", argv[3]);
			resultado = 1;
		} else {
			tp1_t *(*operacion)(tp1_t *, tp1_t *) = NULL;
			if (strcmp(comando, "union") == 0)
				operacion = tp1_union;
			else if (strcmp(comando, "interseccion") == 0)
				operacion = tp1_interseccion;
			else
				operacion = tp1_diferencia;

			tp1_t *resultado_coleccion =
				operacion(pokedex, otra_coleccion);
			if (!resultado_coleccion ||
			    !tp1_guardar_archivo(resultado_coleccion,
						 argv[4])) {
				fprintf(stderr,
					"Error generando/guardando resultado\n");
				resultado = 1;
			}
			if (resultado_coleccion)
				tp1_destruir(resultado_coleccion);
			tp1_destruir(otra_coleccion);
		}
	} else if (strcmp(comando, "mostrar") == 0 && argc == 4) {
		if (strcmp(argv[3], "id") == 0)
			tp1_con_cada_pokemon(pokedex, callback_imprimir, NULL);
		else if (strcmp(argv[3], "nombre") == 0)
			mostrar_por_nombre(pokedex);
		else {
			mostrar_uso();
			resultado = 1;
		}
	} else {
		mostrar_uso();
		resultado = 1;
	}

	tp1_destruir(pokedex);
	return resultado;
}
