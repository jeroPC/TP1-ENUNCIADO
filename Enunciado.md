<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP1: Archivos, strings y muchos punteros

Se pide implementar la funcionalidad del archivo **.h** provisto según la descripción presente en el mismo y lo charlado en clase.

Adicionalmente se pide la implementación de ***main.c*** que dado un archivo de pokedex y otros parámetros, carga los datos y ejecuta la operación pedida.

Por último, es obligatorio implementar ***pruebas_alumno.c*** con pruebas de la funcionalidad de ***tp1.h***.

## Estructura del archivo

El archivo a leer tiene un formato como se muestra a continuación:

```csv
1,Pikachu,ELEC,55,40,90
2,Charmander,FUEG,52,43,65
3,Bulbasaur,PLAN,49,49,45
4,Squirtle,AGUA,48,65,43
5,Jigglypuff,NORM,45,20,20
```

Donde la primer columna corresponde a un número identificador, la segunda columna es el nombre del pokemon, luego le sigue el tipo y por último 3 números que representan ataque, defensa y velocidad del pokemon.

Los tipos válidos son los siguientes:

| Código | Tipo      |
|--------|-----------|
| ELEC   | Eléctrico |
| FUEG   | Fuego     |
| PLAN   | Planta    |
| AGUA   | Agua      |
| NORM   | Normal    |
| FANT   | Fantasma  |
| PSI    | Psíquico  |
| LUCH   | Lucha     |


## Respecto de los pokemones repetidos

Al cargar un archivo, si se encuentran pokemones repetidos, se debe conservar solamente uno de los pokemones (no importa cual). La detección de repetidos se realiza en base al id del pokemon.

## Ejemplos de uso

  - **./tp1 pokedex.csv buscar nombre pikachu**: Busca un pokemon con el nombre pikachu y si lo encuentra muestra la información por pantalla.
  - **./tp1 pokedex.csv buscar id 25**: Busca un pokemon con el id 25 y si lo encuentra muestra la información por pantalla.
  - **./tp1 pokedex.csv union pokedex2.csv resultado.csv**: Carga la pokedex y la pokedex2 y guarda en resultado la union de ambas.
  - **./tp1 pokedex.csv interseccion pokedex2.csv resultado.csv**: Carga la pokedex y la pokedex2 y guarda en resultado la inserseccion de ambas.
  - **./tp1 pokedex.csv diferencia pokedex2.csv resultado.csv**: Carga la pokedex y la pokedex2 y guarda en resultado la diferencia de ambas.
  - **./tp1 pokedex.csv mostrar nombre**: Muestra todos los pokemon por orden alfabetico
  - **./tp1 pokedex.csv mostrar id**: Muestra todos los pokemon por orden numerico

---

## Consejos para la elaboración del trabajo

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 

**LEER EL ENUNCIADO** primero y luego **la descripción de las funciones**. Importantísimo. 


**NO** escribas código a lo loco sin compilar cada tanto. Implementá la solución de a poco y compilando a cada paso. Dejar la compilación para el final es uno de los peores errores que podés cometer.

**NO** avances en la implementación si quedan errores sin resolver en alguna prueba. Cada vez que escribas una prueba implementá toda la funcionalidad necesaria para que funcione correctamente. Esto incluye liberar memoria y accesos inválidos a la misma. Solamente una vez que hayas logrado que la prueba pase exitosamente es que podés comenzar a escribir la próxima prueba para continuar el trabajo.

**NO** está permitido modificar los archivos **.h** provistos con el enunciado.

---

## Puntos teóricos

 - Explicar la elección de la estructura para implementar la funcionalida dpedida. Justifique el uso de cada uno de los campos de la estructura.
 - Explicar con diagramas cómo quedan dispuestas las estructuras y elementos en memoria.
 - Justificar la complejidad computacional temporal de **cada una** de las funciones que se piden implementar.
 - Explique qué dificultades tuvo para implementar las funcionalidades pedidas en el main (si tuvo alguna) y explique si alguna de estas dificultades se podría haber evitado modificando la definición del .h

## Restricciones

Para este TP no se permiten utilizar las funciones de la biblioteca estándar de ***C*** **qsort**, **bsearch** y **strtok**. Si se requieren dichas funcionalidades deben ser implementadas por el alumno.

Tampoco se permite presuponer un longitúd máxima de línea, nombres, vectores, etc. Debe utilizarse memoria dinámica.

Al leer un archivo con líneas inválidas, dichas líneas se deben saltear e ignorar para continuar leyendo el resto de las líneas.
