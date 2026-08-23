# Lib1 - module template

Every module (chip-aware or not) uses the same shape:

```
Lib1/
  CMakeLists.txt      # orchestrator - dispatches on CHIPS; in plain mode
                      # (CHIPS empty) this file *is* the target definition
  inc/Lib1/Template.hpp
  inc/Lib1/Config.hpp   # default config, reference only in this example
  src/Template.cpp
  ChipA/              # chip-specific implementation
    CMakeLists.txt
    inc/Lib1/Config.hpp   # overrides the common default above
  test/
    CMakeLists.txt    # dispatches on CHIPS, same as the orchestrator
    test_Lib1.cpp             # plain-mode test
    test_Lib1_ChipAware.cpp   # chip-mode test
```

In chip-aware mode, `inc/`/`src/` here are never built as their own
target - each chip subdirectory (`ChipA/`, ...) reaches into them by path
and recompiles the common sources into its own target (see "Common's files
recompiled in" below).

Every file here is plain, live CMake/C++ - **nothing is commented out**.
The orchestrator (`lib/Lib1/CMakeLists.txt`) reads one variable, `CHIPS`:

```cmake
set(CHIPS)          # empty -> plain mode (this is the out-of-the-box default)
set(CHIPS ChipA)    # populated -> chip-aware mode
```

That's the only line you need to change to switch modes. `ChipA/` and
`test_Lib1_ChipAware.cpp` stay inert on their own when `CHIPS` is empty -
the `foreach(_chip ${CHIPS})` loops in both files just never iterate, so
nothing needs commenting out.

`Template.hpp`/`Template.cpp` are the only content file pair in the
template. `newObj` renames them into a module's primary content;
`newHdr`/`newSrc` copy them as a blank scaffold for additional files.

## 1. Create a multi-chip specific library

Use this when different chips need different code, not just different
config - a different register layout, peripheral driver, RTOS binding,
etc.

1. Populate `CHIPS` in the orchestrator, e.g. `set(CHIPS ChipA ChipB)`.
   Add a matching subdirectory per chip (copy `ChipA/`, rename it, adjust
   its own `CMakeLists.txt`'s `LIB_NAME` and `target_sources()`).
2. Each chip subdirectory lists the common `Template.cpp`/`.hpp` alongside
   its own files in `target_sources()` - common code gets recompiled into
   each chip's target, not built once and shared. That's what lets a chip
   override a common header (see `Config.hpp`).
3. `test/CMakeLists.txt`'s `if(CHIPS)` branch picks this up automatically -
   one test executable per chip, always built, so `Lib1-all` (and a plain
   `ctest` run) proves every chip still works. Write your assertions in
   `test_Lib1_ChipAware.cpp` (or add your own file to `target_sources()`).

What you get for free:

- `Lib1-ChipA`, `Lib1-ChipB` - real, `EXCLUDE_FROM_ALL` targets. Building
  one directly (`make tgt Lib1-ChipA`) never touches the other chip.
- `Lib1-all` - builds and tests every chip at once, to catch a broken
  implementation before it ships.
- `Lib1` - an `ALIAS` resolving to whichever chip `-DCHIP=<chip>` names,
  for consumers that don't want to hardcode one (see case 3).

## 2. Create a generic library

This is the default, out-of-the-box state of this template - you don't
have to do anything to get it. Leave `CHIPS` empty:

1. Copy `Lib1/` to `lib/<YourModule>/` (or use **New Module**).
2. Rename `Lib1` to `<YourModule>` throughout (directories, file names,
   file contents - **New Module** does this for you; by hand, it's the
   same rename `updateFiles.sh` does: `Lib1` -> your module name,
   `Template`/`TEMPLATE__` -> your file's name/guard).
3. That's it. `lib/<YourModule>/CMakeLists.txt` defines an ordinary,
   always-built `STATIC` library directly - exactly like
   `Networking`/`Queue`/`Task` already are. No `CHIP`, no
   `EXCLUDE_FROM_ALL`, nothing chip-related at all.

## 3. Create a generic library depending on multi-chip specific libraries

This is the "HAL underneath, ordinary business logic on top" shape - the
module itself has no chip variants of its own, but it needs whichever chip
was selected for something *else* it depends on.

1. Build it exactly like case 2 (`CHIPS` empty, ordinary target) - with
   one addition: `target_link_libraries(${LIB_NAME} PUBLIC SomeHal)`,
   where `SomeHal` is the plain, CHIP-selected `ALIAS` name from a
   chip-aware module built per case 1.
2. **Mark it `EXCLUDE_FROM_ALL`.** Easy to miss, and it'll break a plain
   default build the moment `CHIP` isn't set - `SomeHal`'s `ALIAS` only
   exists when `CHIP` was selected at configure time. Without it, a
   routine "build everything" fails on a missing header. With it, the
   module is skipped until something actually needs it.
3. If it has a test, mark that `EXCLUDE_FROM_ALL` too, for the same
   reason - it depends on the same chip-gated alias, transitively.

This pattern is verified two ways: a plain default build (no `CHIP`) is
left completely unaffected, and building explicitly resolves to whichever
chip `CHIP` names.

## How a consumer includes one specific chip

Once a chip-aware module (case 1) is wired up, you get real CMake targets
- nothing here is a naming convention you construct yourself:

- **`Lib1-ChipA`** - link this directly if you know exactly which chip you
  want: `target_link_libraries(MyApp PRIVATE Lib1-ChipA)`.
- **`Lib1-all`** - a convenience target (not a library) that builds and
  tests every chip variant at once: `make tgt Lib1-all`.
- **`Lib1`** - the CHIP-selected `ALIAS`: `make config CHIP=ChipA`, then
  `target_link_libraries(MyApp PRIVATE Lib1)` resolves to `Lib1-ChipA`. No
  `CHIP` set just prints a `STATUS` note - `Lib1-ChipA`/`Lib1-all` don't
  care either way.

Building only the target you ask for never touches another chip's
sources: each variant is `EXCLUDE_FROM_ALL`, so a plain default build
doesn't compile any of them unless something actually needs one.
