Descripción del Proyecto

Este proyecto implementa los algoritmos YYC y BT para el cálculo de todos los testores típicos de una matriz básica. Forma parte del curso de Matemáticas Discretas y se fundamenta en el estudio de:

* Matrices booleanas y matrices básicas
* Matrices de diferencias
* Conceptos de testor y tipicidad
* Densidad de matrices
* Operadores sintéticos φ, θ y γ
* Implementación computacional de algoritmos para testores típicos
* Diseño de experimentos para evaluar la eficiencia computacional

Los algoritmos se aplican sobre matrices generadas o ingresadas por el usuario y permiten:

* Calcular la matriz básica (MB)
* Determinar testores y testores típicos
* Ejecutar YYC y BT
* Medir tiempos de ejecución bajo distintos órdenes
* Reordenar matrices por número de unos
* Construir matrices sintéticas mediante los operadores del artículo base

Objetivos del Proyecto

* Adquirir habilidades en la lectura de artículos científicos.
* Implementar computacionalmente algoritmos y manejar GitHub como plataforma de trabajo.
* Desarrollar competencias en edición científica mediante LaTeX.
* Diseñar experimentos que permitan analizar y comparar el rendimiento de algoritmos de búsqueda de testores típicos.

Requisitos Previos
Compiladores C++ compatibles

* MinGW g++ 11 o superior
* MSVC (Visual Studio 2022)
* TDM-GCC (Dev-C++)

Sistema Operativo

* Windows 10 o 11
* Linux (opcional)

Software recomendado

* Visual Studio 2022
* Visual Studio Code
* Dev-C++
* Git o GitHub Desktop

Instrucciones de Instalación
1. Clonar el repositorio
git clone https://github.com/usuario/proyecto-testores.git
cd proyecto-testores

2. Instalar MinGW (opcional para consola)

Descargar desde: https://www.mingw-w64.org/

Agregar a PATH:

C:\mingw64\bin

3. Abrir el proyecto en Visual Studio

Abrir Visual Studio

File → Open → Folder

Seleccionar la carpeta del proyecto

Compilar con Ctrl + Shift + B

4. Abrir en Dev-C++

* File → Open Project
* Cargar archivos .cpp
* Compilar con “Compile & Run”

Instrucciones de Ejecución
En Visual Studio

Ejecutar con Ctrl + F5.

En consola
g++ src/*.cpp -o testores
./testores

Funciones Principales del Programa
1. Generación de matrices booleanas

* Creación de matrices aleatorias de dimensiones dadas por el usuario.

2. Cálculo de la Matriz Básica (MB)

* Eliminación de subfilas
* Detección de filas no informativas
* Aplicación del criterio determinante 01/10

3. Algoritmo YYC

* Identificación de testores y testores típicos
* Ejecución incremental
* Registro de tiempos acumulados
* Comparación entre orden normal y reordenado

4. Algoritmo BT

* Generación del orden lexicográfico
* Identificación de testores y testores típicos
* Cálculo del tiempo total

5. Operadores θ, φ y γ

* Construcción de matrices sintéticas para los experimentos del artículo base.

Autores

* Harvy Molina
* Alex De La Cruz

Proyecto de Matemáticas Discretas – USFQ
Semestre 2025-01
Profesor: Eduardo Alba

Referencias

* YYC: A Fast Performance Incremental Algorithm for Finding Typical Testors
* Generating synthetic test matrices as a benchmark for the computational behavior of typical testor-finding algorithms
* Documento oficial del proyecto del curso
* Material de clase y Syllabus D2L
