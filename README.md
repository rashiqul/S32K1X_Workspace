# C/C++ Template Workspace

A minimal C/C++ project template with CMake build system, local libraries, and unit testing framework.

## Prerequisites

- GCC/G++ 11+
- CMake 3.23+
- Ninja build system
- Python 3.11+ with pip
- Conan 2.x (profile included in repo)
- clang-format (for code formatting)
- shellcheck (for shell script linting)
- Git

### First-Time Setup

**After cloning the repository**, set up git hooks for automated environment validation:

```bash
./scripts/setup-git-hooks.sh
```

This installs a post-checkout hook that automatically validates your development environment after every branch checkout.

**Manual environment check:**

```bash
./scripts/setup-dev-environment.sh
```

This script will:
- Check for all required tools (gcc, cmake, ninja, python3, clang-format, shellcheck, conan)
- Offer to install missing apt packages (requires sudo)
- Offer to install Conan via pip if not present
- Verify all tools are properly configured

The project includes a custom Conan profile (`.conan/profiles/c_cpp_workspace`) that is automatically used for builds, ensuring consistency across different environments (WSL, Linux, Mac).

## Project Structure

```
├── src/cbd/template/     # Source code
├── test/cbd/unit_tests/  # Unit tests
├── include/              # Public headers
├── libs/                 # Local libraries (gtest, spdlog, fmt, autosar-platform-types)
└── build_x86_64/         # Build output directory
    ├── Debug/            # Debug build artifacts and coverage
    └── Release/          # Release build artifacts and coverage
```

## Quick Start

```bash
# Clean previous builds
make clean

# Configure the project
make configure

# Build the project (Debug by default)
make build

# Or build specific profiles
make build-debug
make build-release

# Run tests
make test

# Generate coverage report (runs Debug, Release, and Debug with sanitizer)
make coverage
```

## Build Targets

### Configuration
- `make clean` - Remove build artifacts
- `make clean-all` - Remove all artifacts (pristine state)
- `make configure` - Run CMake configuration
- `make pre-configure` - Install Poetry dependencies

### Build
- `make build` - Build the project (Debug profile by default)
- `make build-debug` - Build Debug profile
- `make build-release` - Build Release profile

### Testing & Quality
- `make test` - Run unit tests
- `make coverage` - Generate code coverage reports (Debug, Release, and Debug with sanitizer)
- `make lint` - Run linting checks
- `make clang-format` - Format all C/C++ code

### Package
- `make package` - Create Conan package

### Help
- `make help` - Show all available targets

## Testing

The project uses Google Test framework. Test results are generated in:
- Console output during `make test`
- JUnit XML: `build_x86_64/{Debug,Release}/gtest_junit_ut_cicd.xml`

Note: `make test` uses the BUILD_TYPE variable. By default, it runs tests from the current build configuration.

## Code Coverage

The `make coverage` target generates comprehensive coverage reports for:
- **Debug profile** (without sanitizer)
- **Release profile** (optimized build)
- **Debug profile with sanitizer** (memory safety checks)

Coverage reports are generated using gcovr:
- **Debug HTML report**: `build_x86_64/Debug/coverage/coverage_report.html`
- **Release HTML report**: `build_x86_64/Release/coverage/coverage_report.html`
- **XML reports**: `build_x86_64/{Debug,Release}/coverage/cobertura.xml`
- **SonarQube reports**: `build_x86_64/{Debug,Release}/coverage/sonarqube.xml`

## Local Libraries

The workspace includes the following libraries in `libs/`:
- **gtest** - Google Test framework for unit testing
- **spdlog** - Fast C++ logging library
- **fmt** - C++ formatting library
- **autosar-platform-types** - AUTOSAR standard type definitions

## Architecture Support

- x86_64 Linux (primary target)

## CI/CD Pipeline

### GitHub Actions - Linux GCC Pipeline

The project includes a comprehensive CI/CD pipeline that runs on Pull Requests. The pipeline includes:

#### Pipeline Stages

1. **Repository Compliance Check**
   - Validates repository structure and required files
   - Ensures critical configuration files exist

2. **Git Linear History Check**
   - Verifies that the PR branch has a linear history (no merge commits)
   - Ensures clean, rebased branches for easier code review

3. **Environment Check**
   - Validates all required build tools (CMake, GCC, Make, Python, Conan)
   - Displays versions for debugging

4. **Lint Check (clang-format)**
   - Enforces code formatting standards across all C/C++ files
   - Uses `.clang-format` configuration (LLVM style)
   - Pipeline fails if any files need formatting

5. **Build Stage**
   - Compiles and links the entire workspace
   - Treats warnings as errors (`-Werror`)
   - Fails on any compiler warnings or linker errors

6. **Conan Package Generation**
   - Creates a Conan package from the built project
   - Runs `conan create` to build and package the library
   - Executes test_package to validate the package
   - Verifies the package can be consumed by downstream projects

7. **Unit Tests**
   - Runs all unit tests using Google Test framework
   - Generates JUnit XML reports for test results
   - Pipeline fails if any test fails

8. **SonarQube Code Quality (Optional)**
   - Performs static code analysis
   - Requires `SONAR_TOKEN` secret to be configured
   - Can be disabled by removing the job from workflow

#### How to Use

The pipeline automatically triggers when:
- A Pull Request is opened
- New commits are pushed to an existing PR
- A PR is synchronized (rebased/updated)

#### Local Testing Before PR

```bash
# Check code formatting
make clang-format

# Or manually with clang-format
find src/ include/ test/ -name '*.c' -o -name '*.cpp' -o -name '*.h' | xargs clang-format --dry-run --Werror

# Fix formatting issues
make clang-format  # Applies formatting automatically

# Build with warnings as errors (Debug by default)
make clean && make build

# Build Release profile
make clean && make build-release

# Run unit tests
make test

# Generate coverage reports
make coverage

# Test Conan package creation (uses custom profile from .conan/profiles/c_cpp_workspace)
conan create . --build=missing -pr:h .conan/profiles/c_cpp_workspace -pr:b .conan/profiles/c_cpp_workspace
```

#### SonarQube Setup (Optional)

To enable SonarQube analysis:

1. Sign up at [SonarCloud](https://sonarcloud.io/) (free for open-source)
2. Create a new project and get your token
3. Add the token as a GitHub secret:
   - Go to your repo → Settings → Secrets and variables → Actions
   - Add new secret: `SONAR_TOKEN` with your SonarQube token
4. Update `sonar-project.properties` with your organization and project key

#### Troubleshooting

**Formatting failures:**
```bash
# Apply formatting using Makefile
make clang-format

# Or manually preview what needs formatting
clang-format --dry-run --Werror src/**/*.c

# Manually apply formatting
clang-format -i src/**/*.c include/**/*.h test/**/*.cpp
```

**Linear history check failures:**
```bash
# Rebase your branch on main
git fetch origin
git rebase origin/main
git push --force-with-lease
```

**Build warnings/errors:**
- Check the build logs in GitHub Actions
- Reproduce locally with: `make clean && make build`
- Fix all compiler warnings before pushing

## Code Owner

**Mohammad Rashiqul Alam**

For questions, suggestions, or contributions to this repository, please reach out to the code owner.

## License

See [LICENSE](LICENSE) file for details.
