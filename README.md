Nombre / Proyecto: Containerized Edge-Android Gateway (CEAG) — Pasarela de Borde Basada en Android Contenedorizado.  
Arquitectura Objetivo: Hardware de escritorio/PC estándar x86_64 (64 bits).  
Enfoque de Despliegue: Contenedorización liviana sobre un sistema operativo anfitrión (Opción B: Contenedor sobre Linux Base con Waydroid), evitando máquinas virtuales convencionales, emuladores pesados o distribuciones precompiladas de terceros (como BlissOS o PrimeOS).  
Componentes y Arquitectura Técnica
Núcleo (Kernel): Kernel Linux minimalista de 64 bits (instalado sin entorno de escritorio pesado).  
Aislamiento y Contenedores: Motor de contenedores LXC ejecutando el entorno aislado de Waydroid (Android Runtime), gestionado mediante el aislamiento de memoria, cgroups y namespaces de usuario.  
Subsistema Gráfico: Compositor de ventanas Wayland (específicamente Weston) con aceleración gráfica por hardware mediante la asignación directa de nodos DRM (GPU) al contenedor.  
Comunicación Interprocesos (IPC) y Memoria:
Interfaces nativas del kernel Linux habilitadas/compiladas: binder / binderfs y ashmem / memfd para memoria compartida.  
Sockets de comunicación IPC diseñados entre un daemon en el sistema Linux anfitrión (para lectura de hardware/E/S) y las aplicaciones del contenedor Android.  
Interfaz de Usuario (HMI): Aplicaciones móviles Android (APK) orientadas a paneles táctiles, consolas industriales o estaciones de control/monitorización (Edge Computing).  
Herramientas de Auditoría y Depuración en Vivo
El sistema integra tres herramientas clave para evaluar latencia, llamadas al sistema (syscalls) y rendimiento:  
1. top / htop: Monitoreo de CPU y RAM de los procesos waydroid-daemon y system_server (top -p $(pgrep waydroid)).  
2. strace: Interceptación de operaciones ioctl sobre el dispositivo binder para medir retardos IPC (strace -f -e trace ioctl -p <PID_BINDER>).  
3. adb logcat: Conexión a través de la interfaz de red virtual waydroid0 (adb connect 192.168.240.112:5555) para capturar eventos y errores de renderizado (adb logcat *:E).  
Plan de Implementación (16 Semanas)
Fase 1 (Semanas 1–4): Configuración de Git, instalación de Linux minimalista x86_64 y compilación de módulos IPC (binderfs, ashmem/memfd).  
Fase 2 (Semanas 5–8): Inicialización de LXC/Waydroid, despliegue de Wayland (Weston) y asignación directa de nodos DRM.  
Fase 3 (Semanas 9–12): Creación del daemon anfitrión, sockets IPC Host-Android y desarrollo de la aplicación de control (APK).  
Fase 4 (Semanas 13–16): Pruebas de latencia/memoria (strace, top), ajuste de permisos en cgroups/namespaces y cierre documental

# Containerized Edge-Android Gateway (CEAG)

**Pasarela de Borde Basada en Android Contenedorizado para la Optimización de la Comunicación Interprocesos e Interfaz Gráfica en Sistemas Operativos x86_64**[span_0](start_span)[span_0](end_span)

- **Asignatura:** Sistemas Operativos — Proyecto Integrador de Cátedra (16 Semanas)[span_1](start_span)[span_1](end_span)
- **Estudiantes:**
  - Henyer Martinez — C.I. 31.092.075
  - Yairubys Mijares — C.I. 31.232.485
- **Sección:** D3
- **Arquitectura:** Linux Minimalista `x86_64` + Wayland + LXC/Waydroid + IPC (`binderfs`, `memfd`, sockets)[span_2](start_span)[span_2](end_span)[span_3](start_span)[span_3](end_span)[span_4](start_span)[span_4](end_span)

## Adelanto del Repositorio
- `ipc-bridge/host-daemon/src/main.c`: Prototipo en **C** del *daemon* anfitrión sobre socket `AF_UNIX` para consulta de estado y recursos (`binder`, memoria y carga)[span_5](start_span)[span_5](end_span)[span_6](start_span)[span_6](end_span).
- `ipc-bridge/host-daemon/Makefile`: Compilación del *daemon* en C.
- `kernel/scripts/verificar-modulos.sh`: Verificación de soporte de `binderfs`, memoria compartida, `cgroups` y nodos DRM[span_7](start_span)[span_7](end_span)[span_8](start_span)[span_8](end_span).
- 
