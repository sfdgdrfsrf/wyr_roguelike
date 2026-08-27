# wyr_roguelike

A dependency-free C11 libretro roguelike prototype for RetroArch.

## Build

Install Python 3 and a C compiler.

```sh
make verify
make linux-x86_64
```

Android ARM64:

```sh
export ANDROID_NDK_HOME=/path/to/android-ndk
make android-arm64
```

The resulting `wyr_roguelike_libretro.so` is an ARM64 Android libretro core.

Windows with MinGW:

```sh
make windows-x86_64
```

## Scenarios

The source of truth is `tools/generate_400_scenarios.py`. Never hand-edit
`scenarios.h`; regenerate it with:

```sh
python3 tools/generate_400_scenarios.py --output scenarios.h --verify
```

The generator enforces exactly 400 scenarios, 80 in each category, unique
IDs, and text under 256 bytes.

## RetroArch

Copy the platform-specific core into RetroArch's core directory and load it
as a libretro core. The core does not require a game ROM.

## Save states

The serialized state is versioned and includes the mutable run state:
level, scenario index, RNG state, frame counter, HP, buffs, and curses.
