# Caso 1: exploración 2D — versión Sprint 1

Primera versión funcional del videojuego de escritorio en C++20 para Windows, desarrollada con Visual Studio 2026 y la API Win32. No requiere un motor, servicios externos ni recursos descargados. El área de juego tiene 800 × 600 píxeles.

## Ejecutar

1. Abrir `JuegoCaso01.slnx` en Visual Studio 2026 con la carga de trabajo **Desarrollo para el escritorio con C++**.
2. Seleccionar `Debug | x64` y ejecutar con **F5** o **Ctrl+F5**.
3. También se puede compilar con MSBuild desde una terminal de Visual Studio: `MSBuild JuegoCaso01.slnx /p:Configuration=Debug /p:Platform=x64`.

**Controles:** WASD o flechas para moverse en cuatro direcciones, mouse para apuntar, R para reiniciar la sala. Al perder el foco de la ventana se liberan las teclas para evitar movimiento retenido.

El personaje verde es el explorador; el segmento amarillo indica el apuntado. Las áreas grises son obstáculos sólidos. La vida aparece arriba a la derecha. El paso verde en el borde conecta dos salas; la vida se conserva al cruzarlo. En este sprint no hay combate, enemigos, llaves ni puertas con cerradura; están planificados para los sprints siguientes.

## Arquitectura

La explicación detallada de clases, funciones, colisiones, tecnología y diseño visual está en [`EXPLICACION_TECNICA_SPRINT1.md`](EXPLICACION_TECNICA_SPRINT1.md).

- `GameLogic.hpp`: estado de entrada, jugador, sala, obstáculos, colisiones AABB, movimiento, apuntado y reglas de vida. No depende de la interfaz de Windows.
- `JuegoCaso01.cpp`: ventana Win32, teclado y mouse, ciclo de actualización y dibujo con GDI. No contiene reglas de colisión.
- `GameLogicTests.cpp`: pruebas aisladas de movimiento, límites, obstáculos, apuntado, vida y reinicio.

La vida se conserva en `Player` y queda limitada a 0–100, también al cambiar de sala. El movimiento diagonal se resuelve como horizontal para mantener las cuatro direcciones del requisito. El tiempo de actualización se limita a 50 ms para evitar saltos excesivos tras una pausa del sistema. No hay persistencia en disco.

## Pruebas

La solución incluye `GameLogicTests.vcxproj`. Compilar la solución y ejecutar `x64\Debug\GameLogicTests.exe`. Un resultado correcto imprime `Sprint 1 logic tests passed`.

Comprobar manualmente que WASD y flechas mueven al personaje, que los bordes y cajas bloquean el avance, que el segmento amarillo sigue el cursor y que la ventana conserva el área cliente de 800 × 600.

## Alcance y estado

Esta versión cubre RF-01, RF-02, RF-03, RF-04 y RF-08. Aplica RNF-03, RNF-07, RNF-11, RNF-12 y RNF-13 mediante resolución fija, ejecución local, responsabilidades separadas y lógica probada sin interfaz. No se afirma aceptación formal de requisitos ni métricas de rendimiento sin la prueba correspondiente.
