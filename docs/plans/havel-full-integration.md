---
plan name: havel-full-integration
plan description: Fix linking, implement scripting support
plan status: active
---

## Idea
Fully integrate Havel language into game engine with working VM, proper linking, and script callbacks.

## Implementation
- Debug linking issue - Use nm/objdump to verify symbols in Havel libraries. Check if havel::VM constructor exists in libhavel_core.a. Compare mangled names.
- Rebuild Havel with -fPIC - Static libraries may have position-dependent code. Rebuild: cd havel && CXXFLAGS='-fPIC' ./build.sh 6 build
- Try shared libraries - Modify CMakeLists.txt to link against libhavel.so instead of .a. Shared libs handle dependencies better.
- Alternative: Include Havel sources directly - Add Havel source files to game build (original approach). Fix include paths and dependencies.
- Implement actual Havel VM - Replace stub in HavelIntegration.cpp with real implementation using havel::VM, registerFn, etc.
- Register game engine functions - Add functions scripts can call: game_print(), get_time(), etc. Map to game functionality.
- Test with Havel script - Create test.hv script, load from game, call functions, verify bidirectional communication.
- Document integration - Update docs with working examples, build instructions, troubleshooting guide.

## Required Specs
<!-- SPECS_START -->
<!-- SPECS_END -->