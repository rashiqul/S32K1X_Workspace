# S32K1X Workspace

Embedded firmware development workspace for NXP S32K1xx microcontrollers.
Supports both a host-side x86_64 build/test pipeline for fast local iteration and CI, and a cross-compiled ARM firmware build targeting the S32K144 (Cortex-M4).

## Target Platform

| Item | Detail |
|------|--------|
| MCU Family | NXP S32K1xx |
| Current Target | S32K144 (ARM Cortex-M4 @ up to 112 MHz) |
| Evaluation Board | S32K144EVB-Q100 |
| AUTOSAR RTD | 4.7.0 (RTD 3.0.0) |
| CI Execution Target | x86_64 Linux (build, unit test, coverage, SonarQube) |

## Prerequisites

### Build Environment
- **OS**: Linux (Ubuntu 20.04+ recommended) / WSL2
- **GCC/G++**: 11+ (x86_64 host builds)
- **arm-none-eabi-gcc**: 10.2 (ARM target builds — NXP bundled at `~/NXP/gcc-10.2-arm32-eabi/bin` or system install)
- **CMake**: 3.23+ (via Poetry)
- **Ninja**: Build system
- **Python**: 3.11+ with Poetry
- **Conan**: 2.x
- **Git**: Version control

### ARM Target Dependencies
- **NXP S32DS 3.6.6** installed (provides Platform SDK headers at `/mnt/c/NXP/S32DS.3.6.6/`)
  - Required headers: `PlatformSDK_S32K1_S32M24/RTD/BaseNXP_TS_T40D2M30I0R0/`
  - Required headers: `PlatformSDK_S32K1_S32M24/RTD/Platform_TS_T40D2M30I0R0/`
- **usbipd-win** (Windows) for attaching USB debug probe to WSL2 (flash/debug — future)

### Development Tools (Recommended)
- **clang-format**: C/C++ formatting
- **shellcheck**: Shell script linting
- **S32 Design Studio 3.6.6**: BSP generation, reference for flash/debug configurations

## Project Structure

```
S32K1X_Workspace/
├── bsp/                         # S32K144 Board Support Package
│   ├── startup/                 #   Startup assembly + system init (from S32DS)
│   ├── linker/                  #   Linker script for FLASH execution
│   ├── drivers/rtd/             #   NXP RTD AUTOSAR 4.7 MCAL drivers
│   │   ├── include/             #     Driver headers (Clock, MCU, Power, Port, DIO...)
│   │   └── src/                 #     Driver sources
│   ├── config/
│   │   ├── generated/           #   S32DS-generated peripheral configuration
│   │   │   ├── include/
│   │   │   └── src/
│   │   └── board/               #   Board-level pin configuration
│   └── CMakeLists.txt
├── cmake/
│   ├── toolchains/
│   │   └── arm-none-eabi.cmake  # ARM Cortex-M4 cross-compile toolchain
│   └── CodeCoverage.cmake
├── src/
│   ├── main.c                   # S32K144 firmware entry point (LED blink, MCU+Port init)
│   └── cbd/template/            # x86 host application code
├── test/cbd/unit_tests/         # Unit tests (Google Test, x86 only)
├── include/                     # Public headers
├── libs/                        # Local third-party dependencies (gtest, spdlog, fmt, autosar)
├── .conan/profiles/
│   ├── c_cpp_workspace_linux    #   x86_64 Linux host profile
│   ├── c_cpp_workspace_macos    #   x86_64 macOS host profile
│   └── s32k1x_arm_cortex_m4    #   ARM Cortex-M4 bare-metal profile
├── .github/workflows/           # CI pipelines
├── scripts/                     # Utility and setup scripts
├── build_x86_64/{Debug,Release} # x86 build artifacts (generated)
└── build_s32k1/build_armv7/    # ARM target build artifacts (generated)
```

## Quick Start

### Setup

```bash
make setup       # Validate and install developer dependencies
make configure   # Install Poetry + Conan deps, configure x86 build
```

### x86 Host — Build and Test

```bash
make build          # Build Debug profile
make build-release  # Build Release profile
make test           # Run all unit tests
make coverage       # Full coverage analysis
```

### ARM Target — S32K144 Firmware

```bash
# Build firmware (MCU init + LED blink on PTD0)
make build_all_tgt

# Outputs:
#   build_s32k1/build_armv7/Release/src/s32k144_firmware    (ELF with debug symbols)
#   build_s32k1/build_armv7/Release/bin/s32k144_firmware.hex (Intel HEX for flashing)
#   build_s32k1/build_armv7/Release/bin/s32k144_firmware.bin (raw binary)
#   build_s32k1/build_armv7/Release/s32k144_firmware.map     (linker map)

# Override SDK/toolchain paths if needed:
make build_all_tgt \
    S32DS_SDK_ROOT=/mnt/c/NXP/S32DS.3.6.6/S32DS/software/PlatformSDK_S32K1_S32M24 \
    NXP_GCC_PATH=/home/rashiqul/NXP/gcc-10.2-arm32-eabi/bin
```

## Build Targets

### Configuration
- `make pre-configure` - Install Poetry dependencies
- `make conan-install` - Install Conan dependencies and generate build files
- `make configure` - Complete full project configuration
- `make clean` - Remove x86 build artifacts
- `make clean-all` - Remove all artifacts (x86 + ARM)

### Build — x86 Host
- `make build` - Build Debug profile
- `make build-debug` - Build Debug profile explicitly
- `make build-release` - Build Release profile
- `make list-targets` - List available CMake targets

### Build — ARM Target (S32K144)
- `make build_all_tgt` - Cross-compile S32K144 firmware (ARM GCC, Release)
- `make clean_tgt` - Remove ARM target build artifacts

### Test and Quality
- `make test` - Run all tests
- `make coverage` - Run full coverage workflow
- `make lint` - Run Python/tooling lint checks
- `make clang-format` - Format all C/C++ sources

### Package
- `make package` - Create Conan package

### Help
- `make help` - Show all available targets

## ARM Firmware Details

The S32K144 firmware (`src/main.c`) demonstrates:
1. MCU driver init (`Mcu_Init`, `Mcu_InitClock`)
2. Port driver init (`Port_Init`) — configures PTD0 as GPIO output (LED)
3. Infinite LED blink via `Dio_WriteChannel` on `DioConf_DioChannel_DioChannel_0`

The BSP is sourced from two NXP S32DS example projects:
- `Mcu_Example_S32K144` — MCU/Clock/Power RTD drivers and startup code
- `Port_Example_S32K144` — Port/DIO RTD drivers and pin configuration

### Memory Map (S32K144)

| Region | Base | Size |
|--------|------|------|
| Vector table (FLASH) | `0x00000000` | 1 KB |
| Flash config | `0x00000400` | 16 B |
| Code / rodata | `0x00000410` | ~511 KB |
| SRAM (data+BSS) | `0x1FFF8100` | ~56 KB |
| Stack (Core 0) | `0x20006000` | 4 KB |

### Toolchain Configuration

| Setting | Value |
|---------|-------|
| Compiler | `arm-none-eabi-gcc 10.2` |
| CPU flags | `-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16` |
| ABI | `-fno-short-enums` (32-bit enum, matches NXP RTD) |
| ASM | `-x assembler-with-cpp` (C preprocessor on `.s` files) |
| Linker | `-specs=nano.specs -specs=nosys.specs -nostartfiles` |

## Testing

The project uses Google Test and CTest integration (x86 host only).

- Console output: `make test`
- JUnit XML output: `build_x86_64/{Debug,Release}/gtest_junit_ut_cicd.xml`

## Code Coverage

Coverage reports are generated with gcovr (x86 host only).

- Debug HTML: `build_x86_64/Debug/coverage/coverage_report.html`
- Release HTML: `build_x86_64/Release/coverage/coverage_report.html`
- Cobertura XML: `build_x86_64/{Debug,Release}/coverage/cobertura.xml`
- SonarQube XML: `build_x86_64/{Debug,Release}/coverage/sonarqube.xml`

## Local Libraries

The workspace includes the following local libraries in `libs/`:
- **gtest** — Google Test framework (x86 builds only)
- **spdlog** — Header-only logging library
- **fmt** — Header-only string formatting library
- **autosar-platform-types** — AUTOSAR standard types (x86 builds only; ARM uses NXP SDK types)

## CI/CD Pipeline

The GitHub Actions pipeline in `.github/workflows/linux-gcc-pipeline.yml` runs:

1. Repository compliance checks
2. Environment validation
3. Format and shell lint checks
4. Build with warnings-as-errors
5. Conan package creation and validation
6. Unit tests with test result artifacts
7. SonarQube analysis (optional, enabled when `SONAR_TOKEN` is configured)

## SonarQube Setup (Optional)

To enable SonarQube analysis:

1. Sign up at [SonarCloud](https://sonarcloud.io/) and create or select organization `rashiqul-sonar-workspace`
2. Create/import the project for this repository
   - Project key format used by CI: `<github_owner>_S32K1X_Workspace`
3. Add the repository secret in GitHub:
   - Settings -> Secrets and variables -> Actions -> `SONAR_TOKEN`
4. Ensure the GitHub repository is bound in SonarCloud to enable PR decoration

Note: SonarScanner properties are passed directly from the GitHub workflow; there is no `sonar-project.properties` file in this repository.

## Development Workflow

### Linux/WSL2 — x86 Build, Test, Static Analysis
1. Make code changes in `src/cbd/`, `include/`, and `test/`
2. Run `make build` and `make test`
3. Run `make coverage` and review generated reports

### ARM Target — Firmware Development
1. Generate BSP/configuration assets using S32 Design Studio (S32K1xx project)
2. Copy generated files to `bsp/config/generated/` and `bsp/config/board/`
3. Update `src/main.c` with application logic
4. Build with `make build_all_tgt`
5. (Future) Flash with OpenOCD via `make flash` — see Roadmap

## Roadmap

- [x] Add S32K144 BSP structure (startup, linker scripts, AUTOSAR RTD drivers)
- [x] Add ARM Cortex-M4 cross-compilation toolchain
- [x] Add `make build_all_tgt` firmware build workflow
- [x] Firmware: MCU init + Port/DIO init + LED blink (PTD0)
- [ ] Flash firmware via OpenOCD from WSL2 (`make flash`)
- [ ] Attach GDB debugger from WSL2 (`make debug`)
- [ ] VS Code tasks and launch.json for one-click build/flash/debug
- [ ] Expand MCAL module coverage (UART, SPI, CAN-FD...)
- [ ] Rename template-era Conan profile/package naming

## License

See [LICENSE](LICENSE) for details.

## Author

**Mohammad Rashiqul Alam**

## Contributing

This is a project workspace under active development. For issues or suggestions, contact the repository owner.
