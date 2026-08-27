# LeWM

**LeWM** is a Wayland window manager written in C++ and built on
[Louvre](https://github.com/CuarzoSoftware/Louvre). It tiles windows, stays
out of your RAM, and animates like it means it.

Most compositors make you choose between "tiny and boring" or "huge and
slow". LeWM sits in the corner that nobody else occupies: a C++ compositor
on a C++ compositor library, configured by its own little language, with
transitions that don't make your eyes hurt.

## Why LeWM exists

- **Louvre, not wlroots, not dwl.** Louvre is a C++ library for building
  Wayland compositors. It handles the nasty low-level stuff (buffers,
  backends, protocols) so the actual window manager logic stays readable.
  LeWM is a normal C++ project, not a pile of C macros wrapping another
  compositor.
- **The config is a language, not a header.** `le` is parsed at startup from
  `config.le`. Keybindings, workspaces, rules, panel colors, animation
  curves: all of it lives in one file you can read top to bottom.
- **Smooth by default.** Window open, close and rearrange are short tweens
  driven by Louvre's animation loop. You pick the duration and the easing
  curve. It feels expensive.
- **A settings panel that is actually wired up.** Pop it open, change a
  value, it writes straight back to `config.le`. No restart, no recompile.

LeWM is not a desktop environment. No bar, no launcher, no file manager
ships with it. Those come from the rest of your system (waybar, fuzzel,
thunar, whatever). LeWM just manages windows.

## Features

- Real keybindings. `config.le` keys are matched against the live keymap, so
  `Super+Enter` does what it says. No recompile to add a binding.
- Static tiling with multiple layouts (single-column tile and a grid) and
  animated relayouts when windows open, close or move
- Workspaces. Bind `Super+1`..`Super+9`, windows land on the workspace their
  rule assigns, everything else gets hidden until you switch back
- Window open/close/move transitions with selectable easing
- A built-in settings panel, live-editable over IPC or a keybinding
- A Unix IPC socket (`$XDG_RUNTIME_DIR/LeWM.sock`) for scripting
- Low memory footprint; it does less than the big desktops on purpose
- Small C++ core on Louvre, everything else is a client

## Building

LeWM needs Louvre (2.x) and a C++20 compiler. It uses CMake.

### Dependencies

- [Louvre](https://github.com/CuarzoSoftware/Louvre) >= 2.0 (provides
  `Louvre::Louvre` for CMake and pulls in wlroots/SDL/EGL as needed)
- wayland, wayland-protocols
- libinput, libxkbcommon
- a C++20 toolchain (gcc 11+ or clang 14+)
- CMake >= 3.16

### Compile

```sh
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## Running

From a TTY (it grabs the DRM device through libseat):

```sh
LeWM
```

Or drop `LeWM.desktop` into `/usr/share/wayland-sessions/` and pick it from
your login manager.

On first run it looks for `~/.config/LeWM/config.le`. If it isn't there, a
built-in default is used and a warning is logged. Copy the one in this repo
to start from a sane base:

```sh
mkdir -p ~/.config/LeWM
cp config.le ~/.config/LeWM/config.le
```

## Configuring with `le`

The full grammar is in [`docs/config-language.md`](docs/config-language.md)
and a long, commented example ships as [`config.le`](config.le). A taste:

```le
modkey "Super"
gap 6
border_color_active "#d65d0e"

animation enabled true
animation open_ms 220
animation easing "easeOutCubic"

panel enabled true
panel height 28
panel bg "#1d2021"

key "Super+Enter"   exec "foot"
key "Super+Space"   layout_next
key "Super+Shift+q" kill_focused
key "Super+1"       workspace "1"

workspace "1" "term"
workspace "2" "web"

rule "firefox" workspace "2" float false
```

## Smooth transitions

Every window movement is a tween. The `animator` module lerps position and
fades opacity over the compositor loop; the `easing` table in `config.le`
picks the curve (`linear`, `easeOutQuad`, `easeOutCubic`, `easeInOutCubic`,
`easeOutBack`). Turn the whole thing off with `animation enabled false` if
you want it instant.

## Settings panel

The panel is a live config surface. Open it with the `toggle_panel` IPC
command (or bind a key) and it dims the screen; the options behind it are
the same ones in `config.le`, and any change is written back to disk
immediately. The visual controls on top of the dim layer are still being
built, but the configuration layer is complete and persistent.

## IPC

Send single lines to `$XDG_RUNTIME_DIR/LeWM.sock`:

```
toggle_panel
set gap 12
set anim.open_ms 300
set panel.bg "#000000"
relayout
```

`set` persists to `config.le` on the spot.

## Status

LeWM boots, tiles, animates, switches workspaces and takes IPC commands.
What is still rough:

- The settings panel is a dimming overlay right now; the configuration it
  edits is fully live and persistent, the clickable UI on top of it is not
  built yet
- XWayland works but is off unless you flip `xwayland true`
- Multi-monitor is handled but only lightly tested
- Builds against Louvre 2.x; a few API calls (noted in the source comments)
  should be re-checked against the exact version you install

Bug reports, configs and patches are welcome. Keep the memory budget honest
and the pull requests small.

## License

LeWM is licensed under the GNU Affero General Public License version 3.
See [LICENSE](LICENSE).
