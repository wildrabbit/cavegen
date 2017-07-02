---
layout: default
title: Primer reto de programación FemDevs: Cavegen!
---

[English version](index)

## Introducción

Del 16 de junio al 2 de julio se ha organizado el primer reto de programación de FemDevs. En esencia, se elige una feature o tecnología utilizada en desarrollo de juegos y las participantes desarrollan una pequeña demo.
Durante la semana previa se propusieron una serie de temas a votar, y el ganador fue "Generación de mapas procedurales". 

Es un tema que siempre me ha interesado, y además podía resultarme útil en alguno de mis proyectos en eterno desarrollo (¡Te miro a ti, Rob-like!), así que me propuse participar con algunos algoritmos típicamente utilizados en Roguelikes.

Una vez implementado el primer algoritmo me di cuenta de que necesitaba alguna manera cómoda de jugar con los parámetros del generador, así que aproveché para probar ImGUI, una librería de interfaces de usuario en modo inmediato que ha ganado popularidad últimamente.

### Algoritmos

#### Autómatas celulares

[![Cell-automata example](https://img.youtube.com/vi/olJeZIYZtp8/0.jpg)](https://www.youtube.com/watch?v=olJeZIYZtp8)

El primero de ellos utiliza autómatas celulares. Uno de los más sencillos es un autómata síncrono, que opera en pasos definidos. El estado en un tiempo determinado se calcula a partir del estado en el paso anterior.

// TODO: Insertar gráfico

En este caso decidí probar con dos tipos de cálculo.
* **Básico** - Una celda en un tiempo T será de tipo "pared" si el número de celdas vecinas en T-1 supera un umbral concreto.
* **Extendido** - Una celda en tiempo T será de tipo "pared" si cumple el cálculo básico o hay una o menos celdas de tipo "pared" en un radio de dos celdas. Esto elimina los espacios abiertos del algoritmo básico, y genera niveles más laberínticos.

Podemos elegir si la celda en cuestión cuenta o no en el cómputo.

Como resultado, produce mapas relativamente naturales, como sistemas de cavernas. 

[Ref. 1 - Math ∩ Programming](https://jeremykun.com/2012/07/29/the-cellular-automaton-method-for-cave-generation/)
[Ref. 2 - Roguebasin](http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels/)

#### Caminata del borracho

[![Drunkard-walk example](https://img.youtube.com/vi/9tIFzq-guG0/0.jpg)](https://www.youtube.com/watch?v=9tIFzq-guG0)

El concepto de este algoritmo es bastante simple también. En cada turno un agente va desplazándose en una dirección al azar y excavando celdas vacías mientras no haya completado un porcentaje dado (o, si estamos visualizando paso a paso la simulación, un número de pasos).

En mi código utilizo un solo agente que no existe como tal. Va implícito en la clase del generador como una serie de miembros de la misma (básicamente en la posición actual y el número de celdas excavadas).

Este algoritmo genera también entornos más o menos orgánicos, pero son bastante irregulares. Por accidente apliqué a continuación un paso de generación del autómata celular, obteniendo como resultado niveles más suaves.

[![Drunkard-cell example](https://img.youtube.com/vi/Jba5-RIkkJw/0.jpg)](https://www.youtube.com/watch?v=Jba5-RIkkJw)

Algunas pegas que le he encontrado son la probabilidad de atasco. Al moverse de uno en uno y no tener en cuenta ningún tipo de condición adicional, es posible que el agente se quede atascado intentando "cavar en el vacío". Quería haber implementado un cuarto generador basado también en varios agentes más especializados en funciones concretas y con tiempos de vida finitos, pero al final lo dejé para otro momento. 
Asimismo, con mapas grandes (Después de unas pruebas preliminares al principio del reto con el generador basado en autómatas celulares pasé de un tamaño 40x30 a 200x150) puede concentrarse demasiado en un área concreta. Quizás con más de un agente se pueda mejorar el resultado.

[Ref.1 - Roguebasin](http://www.roguebasin.com/index.php?title=Random_Walk_Cave_Generation)

#### Árboles BSP

[![BSP example](https://pasteboard.co/Gz8oGWP.png)](https://www.youtube.com/watch?v=FI07QY25OwE)

La generación basada en árboles BSP se utiliza con frecuencia para generar mazmorras basadas en salas rectangulares. 

En primer lugar, vamos dividiendo en cada paso el mapa en dos. Las divisiones atienden a una serie de criterios, como el cociente ancho/alto (y viceversa), para garantizar que no tengamos niveles demasiado estrechos, o el tamaño mínimo de una sala.

Cuando terminamos de dividir, generamos salas en cada una de las divisiones finales, y las conectamos con pasillos. La generación de pasillos resultó la parte más desafiante.

En función de los parámetros de ajuste (probabilidad de división horizontal vs vertical, tamaño de divisiones y de salas a generar dentro de una división, etc) este tipo de generador aprovecha bastante bien el espacio del mapa, y genera mapas relativamente regulares (a veces igual demasiado regulares).

[Ref.1 - Tutsplus](https://gamedevelopment.tutsplus.com/tutorials/how-to-use-bsp-trees-to-generate-game-maps--gamedev-12268)

[Ref.1 - Eskerda](https://eskerda.com/bsp-dungeon-generation/)

#### Referencias adicionales

EN CONSTRUCCIÓN -_-U

### Conclusiones

Me habría gustado probar varios otros, alterarlos para conseguir otros resultados o combinarlos, pero mi disponibilidad estaba un poco limitada. Sin embargo, es un muy buen entrenamiento.

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

### Instrucciones

EN CONSTRUCCIÓN -_-U

* * *

[Volver a la página principal](http://wildrabbit.github.io)

[Volver a mi portfolio](http://stealthcoder.tumblr.com)
