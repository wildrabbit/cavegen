---
layout: default
title: Primer reto de programación FemDevs: Cavegen!
---

[English version](index)

## Introducción

Del 16 de junio al 2 de julio se ha organizado el primer reto de programación de FemDevs. En esencia, dada una mecánica o técnica utilizada en desarrollo de juegos, las participantes desarrollan una pequeña demo.

Durante la semana previa se propusieron una serie de temas a votar, y el ganador fue "Generación de mapas procedurales". 

Es un tema que siempre me ha interesado, y además podía resultarme útil en alguno de mis proyectos en eterno desarrollo (¡Te miro a ti, [Rob-like](https://ithildin.itch.io/rob-like)!), así que me propuse participar con algunos algoritmos típicamente utilizados en Roguelikes.

Una vez implementado el primer algoritmo me di cuenta de que necesitaba alguna manera cómoda de trastear con los parámetros del generador, así que aproveché para probar ImGUI, una librería de interfaces de usuario en modo inmediato que ha ganado popularidad últimamente frente a soluciones tradicionales, como QT.

## Algoritmos

### Autómatas celulares

[![Cell-automata example](https://img.youtube.com/vi/olJeZIYZtp8/0.jpg)](https://www.youtube.com/watch?v=olJeZIYZtp8)

El primero de ellos utiliza autómatas celulares. Uno de los más sencillos es un autómata síncrono, que opera en pasos definidos. El estado del sistema en un instante determinado se calcula a partir del estado en el paso anterior.

![Maravilloso ejemplo de programmer art](http://i68.tinypic.com/fxxnvn.png)

En este caso decidí probar con dos variaciones a la regla que determina el tipo de salida de una celda.
* **Básico** - Una celda en un tiempo T será de tipo "pared" si el número de celdas vecinas en T-1 supera un umbral concreto.
* **Extendido** - Una celda en tiempo T será de tipo "pared" si cumple el cálculo básico o hay una o menos celdas de tipo "pared" en un radio de dos celdas. Esto elimina los espacios abiertos del algoritmo básico, y genera niveles más laberínticos.

Podemos elegir si la celda en cuestión cuenta o no en el cómputo.

Como resultado, produce mapas relativamente naturales, lo que permite representar archipiélagos o sistemas de cavernas. 

[Ref. 1 - Math ∩ Programming](https://jeremykun.com/2012/07/29/the-cellular-automaton-method-for-cave-generation/)

[Ref. 2 - Roguebasin](http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels/)

### Caminata del borracho

[![Drunkard-walk example](https://img.youtube.com/vi/9tIFzq-guG0/0.jpg)](https://www.youtube.com/watch?v=9tIFzq-guG0)

El concepto de este algoritmo es bastante simple también. En cada turno un agente va desplazándose en una dirección al azar y excavando celdas vacías mientras no haya completado un porcentaje dado (o, si estamos visualizando paso a paso la simulación, un número de pasos).

En mi código utilizo un solo agente que no existe como tal, sino que va implícito en la clase del generador como una serie de miembros de la misma (básicamente, la posición actual y el número de celdas excavadas).

Este algoritmo genera también entornos más o menos orgánicos, pero son bastante irregulares. Es bastante probable que esto se deba a que me pasé con la borrachera del agente, si me permitís la licencia, y forcé que en cada paso escogiera una nueva dirección. En otras implementaciones que he visto el agente preserva la dirección en la que excava durante un cierto número de pasos, antes de escoger una nueva, lo cual probablemente genere pasillos más definidos.

Por accidente apliqué a continuación un paso de generación del autómata celular, obteniendo como resultado niveles más suaves.

[![Drunkard-cell example](https://img.youtube.com/vi/Jba5-RIkkJw/0.jpg)](https://www.youtube.com/watch?v=Jba5-RIkkJw)

Algunas pegas que le he encontrado son la relativamente elevada probabilidad de atasco. Al moverse de uno en uno, como comentaba antes, y no tener en cuenta ningún tipo de condición adicional, es posible que el agente se quede atascado intentando excavar en celdas ya procesadas. Quería haber implementado un cuarto generador basado también en varios agentes más especializados en funciones concretas y con tiempos de vida finitos, pero al final lo dejé para otro momento. 

Asimismo, con mapas grandes (Después de unas pruebas preliminares al principio del reto con el generador basado en autómatas celulares pasé de un tamaño 40x30 a 200x150) puede concentrarse demasiado en un área concreta, desaprovechando mucho espacio disponible. Quizás con más de un agente se pueda mejorar el resultado.

[Ref.1 - Roguebasin](http://www.roguebasin.com/index.php?title=Random_Walk_Cave_Generation)

### Árboles BSP

[![BSP example](https://cdn.pbrd.co/images/Gz8oGWP.png)](https://www.youtube.com/watch?v=FI07QY25OwE)

La generación basada en árboles BSP se utiliza con frecuencia para generar mazmorras basadas en estancias regulares, típicamente rectangulares, comunicadas a través de corredores. 

En primer lugar, vamos dividiendo en cada paso el mapa en dos. Las divisiones atienden a una serie de criterios, como el cociente ancho/alto (y viceversa), para garantizar que no tengamos niveles demasiado estrechos, o el tamaño mínimo de una sala.

Cuando terminamos de dividir, generamos salas en cada una de las divisiones finales, y las conectamos con pasillos. La generación de pasillos resultó la parte más desafiante.

En función de los parámetros de ajuste (probabilidad de división horizontal vs vertical, tamaño de divisiones y de salas a generar dentro de una división, etc) este tipo de generador aprovecha bastante bien el espacio del mapa, y genera mapas relativamente regulares (a veces igual demasiado regulares).

[Ref.1 - Tutsplus](https://gamedevelopment.tutsplus.com/tutorials/how-to-use-bsp-trees-to-generate-game-maps--gamedev-12268)

[Ref.1 - Eskerda](https://eskerda.com/bsp-dungeon-generation/)

## Conclusiones

Me habría gustado probar varios otros, alterarlos para conseguir otros resultados o combinarlos, pero mi disponibilidad estaba un poco limitada. Sin embargo, es un muy buen entrenamiento.

Una vez damos el salto a aspectos de implementación más cercanos al diseño de un juego, entra al campo otro factor, que es la definición de puntos de interés o colocación de entidades. Recientemente vi un artículo de la [segunda edición de Game AI Pro](http://gameaipro.com) sobre resolutores de restricciones para dominios finitos que me pareció bastante relevante.

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

Una vez descomprimido el zip de la demo, es necesario hacer doble click para lanzarla.

Si no hay problemas de compatibilidad o librerías, aparecerá la ventana principal con el menú de opciones desplegado. 


* * *

[Volver a la página principal](http://wildrabbit.github.io)

[Volver a mi portfolio](http://stealthcoder.tumblr.com)
