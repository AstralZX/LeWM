# LeWM - Wayland compositor
#
# Targets:
#   make            - configure + build (dev-friendly)
#   make install    - build and install to $(PREFIX) [$(DESTDIR)$(PREFIX)]
#   make uninstall  - remove installed files (best effort)
#   make clean      - remove build artifacts
#   make run        - build and run from the build dir (nested example uses the
#                     Louvre wayland backend; see comments below)
#
# Options:
#   PREFIX=/usr|/usr/local   install prefix  (default: /usr/local)
#   DESTDIR=/some/stage      install into a staging dir (packaging)
#   CONFIG=path              copy this file as the user config
#                            (default: examples/config.le)
#   BACKENDS_DIR=...         Louvre backends dir override for `make run`

PREFIX      ?= /usr/local
DESTDIR     ?=
BINDIR       = $(DESTDIR)$(PREFIX)/bin
CONFIG      ?= examples/config.le
BUILD_DIR   ?= build

# For the nested backend aimed at `make run`: pick up the install prefix's
# backends if set, else rely on the default path compiled into Louvre.
BACKENDS_DIR ?= $(shell pkg-config --variable=prefix Louvre 2>/dev/null)/lib64/Louvre/backends

CMAKEFLAGS   = -DCMAKE_INSTALL_PREFIX=$(PREFIX)
ifneq ($(DESTDIR),)
	CMAKEFLAGS += -DCMAKE_INSTALL_PREFIX=$(PREFIX)
endif

.PHONY: all default build install uninstall clean run configure

all: default

default: build

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release $(CMAKEFLAGS)

build: configure
	cmake --build $(BUILD_DIR)

install: build
	cmake --install $(BUILD_DIR)
	@if [ -n "$(DESTDIR)" ]; then \
		echo ">> Staged install complete (DESTDIR=$(DESTDIR))"; \
	else \
		echo ">> Installed LeWM to $(PREFIX)/bin/LeWM"; \
	fi
	@./scripts/install-config.sh "$(CONFIG)"

uninstall:
	@echo ">> Removing $(BINDIR)/LeWM"
	rm -f "$(BINDIR)/LeWM"

clean:
	rm -rf $(BUILD_DIR)

# Runs LeWM nested inside your current Wayland desktop: Louvre's wayland
# backend opens a window on the host compositor. Requires the wayland.so
# graphic + input backends and a working GL surface.
run: build
	@if [ -n "$(BACKENDS_DIR)" ] && [ -d "$(BACKENDS_DIR)" ]; then \
		export LOUVRE_BACKENDS_PATH="$(BACKENDS_DIR)"; \
		export LOUVRE_GRAPHIC_BACKEND=wayland; \
		export LOUVRE_INPUT_BACKEND=wayland; \
	fi; \
	export LD_LIBRARY_PATH="$(shell pkg-config --variable=libdir Louvre 2>/dev/null):$${LD_LIBRARY_PATH}"; \
	LD_LIBRARY_PATH="$$LD_LIBRARY_PATH" ./$(BUILD_DIR)/LeWM

help:
	@echo "LeWM targets: all/build, configure, install, uninstall, clean, run, help"
	@echo "  PREFIX=$(PREFIX)  DESTDIR=$(DESTDIR)  CONFIG=$(CONFIG)"
