# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Working rules (team conventions — shared with leg_hero_organepi/CLAUDE.md)

* Do not modify source code or comments without the user's explicit permission. When the user is asking a question or describing a problem, **diagnose and report findings only; do not apply a fix unless requested.**
* Before every response, state whether code or comments were modified, using `代码改动：...` and `注释改动：...`.
* 永远不要动我的注释，除非我让你单独修改注释 (Never modify the user's comments unless explicitly asked to modify comments).

## Project overview

This is the STM32H723VGT6 firmware for a RoboMaster wheeled-leg chassis. The project is generated/configured with STM32CubeMX 6.15.0 and STM32CubeIDE, uses STM32Cube H7 firmware V1.12.1, HAL, FreeRTOS through the CMSIS-RTOS v1 API, and the ARM Cortex-M DSP library. The target is a Cortex-M7 running at 480 MHz with hard-float compiler options.

The hardware configuration of record is `wheel_leg_sys.ioc`; it is the source for peripheral and RTOS configuration. The root `CMakeLists.txt` is an alternate GCC cross-compilation build definition and emits ELF, HEX, and BIN images.

Repository context: this directory is one of three sub-projects sharing a git repo rooted at `Leg_Hero` (sibling changes appear as `../*` paths in `git status`): `leg_new_hero_chassis` (this firmware), `leg_hero_organepi` (a C++ companion project with its own `CLAUDE.md` and frequently large in-flight changes), and `leg_new_hero_gimbal`. Stay inside this project unless the task explicitly crosses project boundaries.

## Build and validation commands

The normal build workflow is STM32CubeIDE: import the repository as an existing STM32 project, select a Debug or Release configuration, and use **Project > Build Project**. Flash/debug through the CubeIDE probe configuration for the STM32H723VGT6 board.

A command-line CMake build is also defined. It requires an `arm-none-eabi-*` toolchain on `PATH` and CMake 3.31 or newer:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
cmake --build build -j
```

Use `Release`, `RelWithDebInfo`, or `MinSizeRel` as appropriate. The post-build outputs are `build/CtrlBoard-H7_IMU.hex` and `build/CtrlBoard-H7_IMU.bin`; the link map is `build/CtrlBoard-H7_IMU.map`. On Windows, the same commands work from Git Bash if CMake and the GNU Arm Embedded toolchain are available.

There is no project-maintained unit-test target, `enable_testing()` configuration, lint script, or host simulator. `cmake --build build -j` is the available compile/link smoke test. Individual source files are not useful standalone tests because they depend on HAL, FreeRTOS, device handles, linker symbols, and the target hardware.

## Repository architecture

- `Core/` contains CubeMX-generated STM32 startup, clock, interrupt, DMA, GPIO, SPI, FDCAN, timer, and UART integration. `Core/Src/main.c` performs HAL/peripheral initialization, initializes DWT, the remote control, FDCAN callbacks, and BMI088, then creates/starts the RTOS scheduler.
- `Middlewares/`, `Drivers/`, and `USB_DEVICE/` contain the STM32 HAL, CMSIS, FreeRTOS, USB CDC, and ARM DSP dependencies. Treat these as vendor/generated code unless the task explicitly targets them.
- `User/APP/` is the application layer and RTOS task layer. `Core/Src/freertos.c` creates the seven tasks: `INS_TASK`, `ChassisL_TASK`, `Other_TASK`, `Host_Comm_TASK`, `Can_Comm_TASK`, `UI_TASK`, and the default USB task. Task priorities and stack sizes are configured both there and in the `.ioc`; keep those definitions synchronized if changing scheduling. Note that `Other_Task` is defined in `Uart_Send_Task.c`, and `Led_Flow_Task.c` (WS2812 LED effects, `LED_Flow()`) is not an RTOS thread — it is called from `Other_Task`.
- `User/Algorithm/` contains estimation and control: quaternion EKF/Mahony/Kalman/filter code, PID, finite-state-machine logic, and the VMC/LQR leg kinematics/control calculations. `ChassisL_Task.c` is the main control-loop integration point: it reads motor/IMU state, updates leg geometry, computes outputs, and sends joint/wheel commands at approximately 1 ms cadence. The generic FSM (`User/Algorithm/fsm/fsm.c`, `FSM_Init`/`FSM_Set_Status`) is consumed across the APP tasks; the leg robot modes it drives (normal / over_turn / above_ground / ready_to_jump → gather_strength → take_off_to_jump → landing_cushioning) are diagrammed in `README.md`.
- `User/Devices/` contains board/device protocols and drivers: BMI088 over SPI/DMA, DJI C620 and DM motors over FDCAN, remote control UART parsing, referee UART/protocol/CRC handling, super-cap communication, WS2812, and CAN callbacks/receive state. Interrupt callbacks update shared device state; tasks consume that state and produce periodic commands.
- `User/Bsp/`, `User/Controller/`, `User/Lib/`, and `User/Devices/*` provide board support, shared structures/utilities, controller glue, and reusable protocol/device abstractions used by the application tasks.

### Runtime data flow

1. `main()` initializes hardware and starts FreeRTOS.
2. BMI088 and remote-control input feed the INS and control state. `INS_Task` publishes the global `INS` state; `ChassisL_Task` waits for INS initialization before entering its loop.
3. FDCAN receive callbacks populate motor and referee/CAN state. `ChassisL_Task` maps joint feedback into VMC leg coordinates, applies safety gating (host communication, robot control status, and motor status), and transmits motor/super-cap commands.
4. `Host_Comm_Task` sends sensor, RC, motor, and attitude telemetry over USB CDC and accepts CRC-checked torque/wheel commands. Missing host packets zero the commanded torques after its timeout counter expires.
5. `Can_Comm_Task` periodically exchanges referee-related data over FDCAN. `UI_Task` consumes motor/referee/chassis status and sends static/dynamic referee UI graphics. `Other_Task`/`Uart_Send_Task.c` handles auxiliary periodic serial output.

The application uses shared globals rather than a message-queue architecture. When changing shared state, inspect the producer callback/task and every consumer, and preserve the existing timing, timeout, and motor-output safety gates.

## Editing and generated-code boundaries

CubeMX-generated files use `USER CODE BEGIN/END` sections. Put application additions in those sections when modifying generated `Core/`, USB, or peripheral files so regeneration does not erase them. Prefer implementing behavior in `User/` and changing `wheel_leg_sys.ioc` for peripheral/FreeRTOS configuration rather than hand-editing generated initialization. After changing the `.ioc`, regenerate with the matching CubeMX/CubeIDE version and review the complete generated diff.

The root CMake source list uses `GLOB_RECURSE` over `Core`, `Middlewares`, `Drivers`, `USB_DEVICE`, and `User`, so new source files under those trees are normally picked up automatically. Keep the linker script (`STM32H723VGTX_FLASH.ld`) and target flags aligned with the MCU; the CMake file currently enables Cortex-M4 DSP library linkage while compiling for Cortex-M7, matching the checked-in project configuration.

## Useful files to inspect first

- `wheel_leg_sys.ioc` — authoritative MCU, peripheral, pin, DMA, FDCAN, UART, USB, and FreeRTOS configuration.
- `CMakeLists.txt` — command-line cross-build and image generation.
- `Core/Src/main.c` and `Core/Src/freertos.c` — boot sequence and task topology.
- `User/APP/ChassisL_Task.c` — chassis control loop and motor safety/output path.
- `User/APP/INS_Task.c`, `User/APP/Host_Comm_Task.c`, and `User/APP/Can_Comm_Task.c` — estimation and communication producers/consumers.
- `README.md` — Mermaid state diagram of the leg robot mode FSM (normal / over_turn / above_ground / jump chain); pairs with `User/Algorithm/fsm`.
- `ROBOT_UI_README.md` — referee UI behavior and motor-status index mapping.
