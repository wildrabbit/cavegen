---
layout: default
title: Primer reto de programación FemDevs: Cavegen!
---

## Introducción

### Algoritmos

[English version](index.md)

En esta demo puede verse el funcionamiento de algunos algoritmos básicos de generación de mapas procedurales:

#### Autómatas celulares

[![Cell-automata example](https://img.youtube.com/vi/olJeZIYZtp8/0.jpg)](https://www.youtube.com/watch?v=olJeZIYZtp8)

El primero de ellos utiliza autómatas celulares. Como resultado, produce mapas relativamente naturales, como sistemas de cavernas. 

[Referencia](https://jeremykun.com/2012/07/29/the-cellular-automaton-method-for-cave-generation/)

#### Caminata del borracho

[![Drunkard-walk example](https://img.youtube.com/vi/9tIFzq-guG0/0.jpg)](https://www.youtube.com/watch?v=9tIFzq-guG0)

"Paseo del borracho" (Drunkard walk). Genera también entornos más o menos orgánicos, pero bastante irregulares. Por accidente apliqué a continuación el primer algoritmo, generando niveles más suaves.

[![Drunkard-cell example](https://img.youtube.com/vi/Jba5-RIkkJw/0.jpg)](https://www.youtube.com/watch?v=Jba5-RIkkJw)

#### Árboles BSP

[![BSP example](https://img.youtube.com/vi/FI07QY25OwE/0.jpg)](https://www.youtube.com/watch?v=FI07QY25OwE)

Árboles BSP. Útiles para generar mazmorras basadas en salas rectangulares generando una distribución regular del espacio.


Me habría gustado probar muchos otros, o combinarlos, pero mi disponibilidad estaba un poco limitada. Asimismo, la mejor prueba es utilizar los mapas generados en un juego real.

## Descargas

### [Código fuente](https://github.com/wildrabbit/cavegen)

Dependencias:

* Toolset VS 2017 (v141) (probablemente funcione con la v140 y VS 2015, pero no está probado)

* [SFML SDK 2.4](https://www.sfml-dev.org/download/sfml/2.4.2)

* [ImGUI](https://github.com/ocornut/imgui)

* [Bindings de ImGUI con SFML por Elias Daler](https://github.com/eliasdaler/imgui-sfml)


### [Descargar demo](https://www.dropbox.com/s/wq5vp4t00aleis6/cavegen.zip?dl=0)

Probado en un PC con Windows 10 64b.

Requisitos:

* Probablemente las [librerías redistribuibles de VS2017](https://go.microsoft.com/fwlink/?LinkId=746571)

* * *

[Volver a la página principal](http://wildrabbit.github.io)

[Volver a mi portfolio](http://stealthcoder.tumblr.com)
