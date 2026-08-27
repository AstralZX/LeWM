# LeWM

LeWM is a Wayland compositor for people who want a tiling window manager
that gets out of the way. It is written in C++ and built directly on
[wlroots](https://gitlab.freedesktop.org/wlroots/wlroots), without
wrapping an existing X11 toolset or sitting on another compositor.

The goal is simple: low memory footprint, predictable latency, and a
config file you can actually read.

## Why another compositor

Most Wayland compositors either hide everything behind a GUI or expect you
to recompile to change a keybinding. LeWM keeps a small, readable core
and adds a real configuration language so behavior lives in a file, not
in a header you have to edit and rebuild.

What you get:

- Static tiling with multiple layouts, no floating-by-default surprises
- A config language called `le`, parsed at startup from `config.le`
- A Unix IPC socket for runtime control (layout switches, focus, close)
- Memory use that stays under what a typical Qt or GTK shell pulls in
  before it has drawn a single window

LeWM is not a desktop environment. It does not ship a bar, a launcher, or
a settings panel. Pipewire screen sharing, xdg-desktop-portal, and your
favorite launcher are expected to come from the rest of your system.

## Building

LeWM needs wlroots and the usual Wayland plumbing, plus a C++20 compiler.

### Dependencies

- wlroots >= 0.17
- wayland, wayland-protocols
- libinput, libxkbcommon
- pixman
- pkg-config
- a C++20 toolchain (gcc 11+ or clang 14+)

On Debian and derivatives:

```sh
sudo apt install libwayland-dev wayland-protocols libwlroots-dev \
  libinput-dev libxkbcommon-dev libpixman-1-dev pkg-config \
  g++ meson
```

### Compile and install

The build is a plain Makefile. It expects wlroots to be discoverable
through pkg-config.

```sh
make
sudo make install
```

`PREFIX` and `DESTDIR` are honored, so packagers can do the usual:

```sh
make PREFIX=/usr DESTDIR=pkgroot install
```

## Running

LeWM is a compositor, so it wants to be your session. From a TTY:

```sh
LeWM
```

Or hand it to your login manager by dropping the included
`LeWM.desktop` into `/usr/share/wayland-sessions/`.

The first run looks for `config.le` in `$XDG_CONFIG_HOME/LeWM/`. If it is
not there, a built-in default layout is used and a warning is logged.

## Configuring with `le`

Configuration lives in `~/.config/LeWM/config.le`. The `le` language is
intentionally small: assignments, blocks, lists, and a few built-in
functions. No semicolons, no braces-as-control-flow confusion.

A minimal `config.le`:

```le
modkey "Super"

border_width 2
border_color_active  "#3c3836"
border_color_normal  "#1d2021"

layout default "tile"

key "Super+Enter"   exec "foot"
key "Super+d"       exec "fuzzel"
key "Super+Space"   layout_next
key "Super+Shift+q" kill_focused

workspace "1" "term"
workspace "2" "web"
workspace "3" "code"

rule "foot"     workspace "1"
rule "firefox"  workspace "2" float false
```

The parser is documented in [`docs/config-language.md`](docs/config-language.md),
and a larger example lives in [`examples/config.le`](examples/config.le).

## IPC

LeWM exposes a Unix socket at `$XDG_RUNTIME_DIR/LeWM.sock`. Clients send
single-line commands; `layout_next`, `kill_focused`, `focus_next` and
`focus_prev` are understood. Sending `layout_next` over the socket does the
same thing as the keybinding above. The wire format is LeWM-specific, so
write to the socket directly or use a small helper script.

## Status

LeWM is usable as a daily driver on a single monitor and on basic
multi-monitor setups. Known gaps:

- No built-in wallpaper (use a separate background tool)
- VRR is untested
- XWayland is supported but off by default; flip `xwayland true` in config

Bug reports and config snippets are welcome. Pull requests that keep the
memory budget honest get looked at first.

## License

LeWM is licensed under the GNU Affero General Public License version 3.
See [LICENSE](LICENSE). Configuration examples and the `le` grammar
description in `docs/` are likewise AGPL-3.0 unless stated otherwise.
