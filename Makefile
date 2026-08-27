PYTHON ?= python3
CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -fPIC -O2
OUT=wyr_roguelike_libretro
GEN=tools/generate_400_scenarios.py

.PHONY: all generate verify clean android-arm64 linux-x86_64 windows-x86_64
all: linux-x86_64

generate:
	$(PYTHON) $(GEN) --output scenarios.h

verify:
	$(PYTHON) $(GEN) --output scenarios.h --verify

linux-x86_64: generate
	$(CC) $(CFLAGS) -shared -o $(OUT).so wyr_roguelike.c

android-arm64: generate
	@if [ -z "$(ANDROID_NDK_HOME)" ]; then echo "Set ANDROID_NDK_HOME"; exit 1; fi
	$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang $(CFLAGS) -shared -o $(OUT).so wyr_roguelike.c

windows-x86_64: generate
	x86_64-w64-mingw32-gcc $(CFLAGS) -shared -o $(OUT).dll wyr_roguelike.c

clean:
	rm -f $(OUT).so $(OUT).dll scenarios.h
