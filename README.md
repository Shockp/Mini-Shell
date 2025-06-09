# 🐚 MiniShell - Práctica de Sistemas Operativos

**Grado en Ingeniería de Computadores — 2º Curso**  
**Asignatura: Sistemas Operativos (Curso 2023-24)**  
**Práctica Obligatoria 2: Implementación de un intérprete de mandatos tipo shell**

---

## 📋 Descripción

Esta práctica consiste en la implementación de un **intérprete de mandatos**, similar a una shell básica de UNIX, con funcionalidades como ejecución en foreground y background, redirecciones, manejo de pipes, y gestión de procesos. 

El programa debe mostrar un prompt `msh > `, leer líneas desde la entrada estándar, analizarlas usando la librería `parser`, y ejecutar los mandatos correspondientes, gestionando múltiples procesos hijos y las comunicaciones entre ellos.

---

## ⚙️ Características Implementadas

- ✅ Ejecución de comandos con múltiples argumentos
- ✅ Soporte para **pipes (`|`)**
- ✅ Redirección de:
  - Entrada (`<`)
  - Salida estándar (`>`)
  - Salida de errores (`>&`)
- ✅ Ejecución en **background (`&`)** con visualización de PID
- ✅ Manejo de señales (`SIGINT`, `SIGQUIT`)
- ✅ Implementación de mandatos i# MiniShell
nternos:
  - `cd`: cambio de directorio
  - `jobs`: visualización de procesos en background
  - `fg`: traer procesos a foreground
- ✅ Gestión de errores detallada:
  - Mandatos inexistentes
  - Errores de acceso a ficheros

---

## 🏗️ Compilación

El proyecto está desarrollado en **ANSI C**, y se compila con:

```bash
gcc -Wall -Wextra myshell.c libparser.a -o msh -static
```

> ⚠️ Asegúrate de tener en el mismo directorio:  
> - `myshell.c`  
> - `parser.h`  
> - `libparser.a` (o `libparser64.a` para Linux x86_64)

---

## ▶️ Ejecución

Una vez compilado, ejecuta:

```bash
./msh
```

Ejemplos de uso:

```bash
msh > ls -l | grep ".c" > lista.txt
msh > cat < entrada.txt | sort | uniq > salida.txt
msh > find / -name "*.log" >& errores.txt &
msh > cd ~/Documentos
msh > jobs
msh > fg 1
```

---

## 🧪 Funcionalidades evaluadas (y puntuación máxima)

| Funcionalidad | Descripción | Puntos |
|--------------|-------------|--------|
| Mandato simple con argumentos | Ejecución foreground | 0.5 |
| Redirección entrada/salida | Con mandato simple | 1.0 |
| Dos mandatos con `|` | Con redirección opcional | 1.0 |
| Tres o más mandatos con `|` | Con redirección opcional | 4.0 |
| Mandato `cd` interno | Incluye `cd`, `cd ~`, etc. | 0.5 |
| Background (`&`) con pipes | `jobs` y `fg` implementados | 2.0 |
| Manejo de señales | `SIGINT`, `SIGQUIT` gestionadas | 1.0 |

---

## 🗃️ Estructura del proyecto

```
minishell/
├── myshell.c          # Código fuente principal
├── parser.h           # Cabecera de la librería parser
├── libparser.a        # Librería parser (estática)
├── test.c             # Programa de prueba proporcionado
└── README.md          # Este documento
```

---

## 📎 Entrega y Evaluación

- 📝 Entregar **myshell.c** y una memoria PDF explicativa
- 📦 Todo empaquetado en `practica2.zip` a través del **Campus Virtual**
- 📄 La memoria debe incluir:
  - Índice
  - Autores
  - Descripción detallada del código y funciones clave
  - Comentarios personales, dificultades, sugerencias
- 🚨 La calidad de la memoria es **imprescindible** para aprobar
- 🧑‍⚖️ Defensa oral opcional bajo decisión del profesor

---

## 🧑‍💻 Normas de codificación

- Código en **ANSI C** — No usar C++
- Compilación sin *warnings* (`-Wall -Wextra`)
- Buen uso de memoria dinámica (`malloc`, `free`)
- Operaciones sobre strings con funciones estándar (`string.h`)
- Declaraciones al inicio de cada bloque `{}`

---

## 👥 Autoría y Plagio

- Máximo 2 integrantes por grupo
- Plagiar implica:
  - Suspenso automático
  - Posible expediente disciplinario
- Se considerará plagio cualquier copia parcial o total de código o memoria

---

## 📚 Referencias

- Archivos proporcionados en el Campus Virtual:
  - `parser.h`, `libparser.a`, `test.c`
- Guías del sistema UNIX/Linux
- `man exec`, `man fork`, `man pipe`, `man dup2`, `man signal`

---

**¡Buena suerte y a shell-echar código! 🧑‍💻🐚**

