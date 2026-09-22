# CLAUDE.md — DFRCore

## Project Overview

DFRCore (Dialable Fidelity and Resolution Core) is a discrete event simulation (DES) framework in C++17 that supports modeling large numbers of entities (lifeforms, vehicles, units) with dynamic fidelity and multi-resolution modeling. It can run in batch mode (as fast as possible) or real-time federation with external inputs.

## Build System

CMake, minimum version 3.10, C++17.

```bash
# Configure
cmake -S . -B build

# Build
cmake --build build

# Run tests
ctest --test-dir build

# Build documentation (requires Doxygen)
cmake --build build --target doxygen
```

## Coding Conventions

- C++17, no extensions
- Header-only declarations in `.hpp`, implementations in `.cpp`
- Doxygen `//!` comments on public API
- `#pragma once` for include guards
- Member variables prefixed with `m` (e.g., `mX`, `mY`, `mZ`)
- Static factory methods preferred over constructors for coordinate conversions
