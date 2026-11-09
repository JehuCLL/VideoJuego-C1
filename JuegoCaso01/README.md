# Caso 1 — Exploración y combate 2D

Juego de escritorio en C++20 con raylib 6.2.0. La solución de Visual Studio contiene el juego y las pruebas de lógica.

## Abrir y ejecutar

Abre `JuegoCaso01.slnx` en Visual Studio con la carga de trabajo **Desarrollo para el escritorio con C++**. En el Explorador de soluciones verás los filtros `Model`, `View`, `Controller`, `Inicio` y `Assets`, que agrupan los archivos según su función. Las carpetas físicas están bajo `src`, `Tests` y `assets`. Selecciona `JuegoCaso01` como proyecto de inicio, usa `Debug | x64` y presiona **F5** o **Ctrl+F5**. Si Visual Studio solicita restaurar NuGet, acepta la restauración del paquete `raylib` 6.2.0. El archivo `packages.config` registra la dependencia y la compilación copia los sprites al directorio del ejecutable.

También se puede compilar con MSBuild: `MSBuild JuegoCaso01.slnx /p:Configuration=Debug /p:Platform=x64`.

**Controles:** WASD o flechas para mover, mouse para apuntar, clic izquierdo para disparar, R para reiniciar y Esc para cerrar. El paso verde conecta las dos salas. Al cambiar de sala se conservan la vida y los enemigos ya derrotados.

## Alcance actual

La base del Sprint 1 incluye movimiento, colisiones con paredes y cajas, apuntado, vida y dos salas. De Sprint 2 están implementados solo **RF-05, RF-06, RF-07, RF-09 y RF-10**: disparos del explorador, impactos de sus proyectiles contra enemigos y obstáculos, daño y eliminación de enemigos, movimiento y orientación de enemigos, y disparos enemigos dirigidos al explorador. Los proyectiles enemigos desaparecen al tocar al jugador, pero **no reducen su vida**, ya que RF-11 no forma parte del alcance solicitado.

La presentación utiliza dos sprites pixel art derivados de las referencias proporcionadas para el explorador y el slime. Las texturas tienen fondo transparente y filtro de punto (`TEXTURE_FILTER_POINT`) para mantener bordes nítidos.

## Estructura MVC

```text
JuegoCaso01/
├─ src/
│  ├─ Model/GameModel.hpp             Estado, entidades y reglas del juego
│  ├─ View/GameView.hpp/.cpp           Dibujo, HUD y sprites con raylib
│  ├─ Controller/GameController.hpp/.cpp  Entrada y ciclo del juego
│  └─ main.cpp                         Punto de entrada
├─ Tests/GameLogicTests.cpp            Pruebas de reglas sin ventana
├─ assets/                             Sprites PNG
├─ JuegoCaso01.slnx                    Solución de Visual Studio
└─ README.md
```

`GameController` recibe la entrada y actualiza `game::Game`. `GameView` recibe el modelo como `const` y solo lo dibuja. El modelo no incluye raylib, por lo que sus colisiones y reglas pueden probarse sin interfaz. Los filtros del Explorador de soluciones muestran esta división; `Inicio` agrupa `src/main.cpp`.

## Pruebas

Compila la solución en `Debug | x64` y ejecuta `x64\Debug\GameLogicTests.exe`. Las pruebas cubren las reglas del Sprint 1 y los cinco RF seleccionados del Sprint 2. La documentación técnica y de validación del proyecto se conserva por separado en la carpeta académica `VU2`.
