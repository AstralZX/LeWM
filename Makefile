CC      ?= c++
CFLAGS  ?= -std=c++20 -O2 -Wall -Wextra
PKGS     = wlroots wayland-server libinput libxkbcommon pixman-1

PREFIX  ?= /usr/local
DESTDIR ?=

SRC := $(wildcard src/*.cpp) \
       $(wildcard src/config/*.cpp) \
       $(wildcard src/compositor/*.cpp) \
       $(wildcard src/layout/*.cpp) \
       $(wildcard src/ipc/*.cpp)

OBJ := $(SRC:.cpp=.o)
BIN := LeWM

CPPFLAGS += $(shell pkg-config --cflags $(PKGS))
LDLIBS   += $(shell pkg-config --libs $(PKGS))

.PHONY: all clean install uninstall

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(BIN)

install: $(BIN)
	install -Dm755 $(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)
	install -Dm644 LeWM.desktop $(DESTDIR)$(PREFIX)/share/wayland-sessions/LeWM.desktop

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)
	rm -f $(DESTDIR)$(PREFIX)/share/wayland-sessions/LeWM.desktop
