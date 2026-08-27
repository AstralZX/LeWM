# The `le` configuration language

LeWM is configured with `config.le`, read once at startup from
`~/.config/LeWM/config.le`. The language is line oriented: one directive per
line, `#` starts a comment, and strings containing spaces go in double
quotes. Booleans are the bare words `true` and `false`.

This file is also what the settings panel writes back to, so anything you
can set here can be changed live and persisted.

## Top-level directives

### modkey
```
modkey "Super"
```
The primary modifier for all keybindings. Any XKB key name works; the
common ones are `"Super"`, `"Alt"`, `"Control"`.

### gap
```
gap 6
```
Space in pixels between tiled windows.

### Borders
```
border_width 2
border_color_active  "#d65d0e"
border_color_normal  "#282828"
border_color_urgent  "#cc241d"
```

### xwayland
```
xwayland false
```
Run an XWayland server for legacy X11 clients. Off by default.

### layout
```
layout default "tile"
```
Layout for new workspaces. Currently `"tile"` is the only one wired up; more
land in later releases.

## Animation block
```
animation enabled true
animation open_ms  220
animation close_ms 160
animation move_ms  130
animation easing  "easeOutCubic"
```
Controls the smooth transitions. `easing` is one of: `linear`, `easeOutQuad`,
`easeOutCubic`, `easeInOutCubic`, `easeOutBack`.

## Panel block
```
panel enabled  true
panel position "top"
panel height   28
panel bg      "#1d2021"
panel fg      "#ebdbb2"
```
Configures the built-in settings panel overlay.

## Startup
```
startup "waybar"
startup "swaybg -m fill -i /usr/share/backgrounds/lewm.png"
```
Programs launched after the compositor comes up. Each line is one command.

## Keybindings
```
key "Combo" action [args...]
```
`Combo` is the modifier sequence. Built-in actions:

- `exec "command"` - run a program
- `layout_next` - cycle the active layout
- `kill_focused` - close the focused surface
- `focus_next` / `focus_prev` - move focus
- `workspace "n"` - switch workspace
- `toggle_panel` - open/close the settings panel
- `relayout` - re-run the tiling layout on all outputs

## Workspaces
```
workspace "1" "term"
workspace "2" "web"
```
First argument is the id referenced by `key ... workspace`, second is a label
for status tools.

## Rules
```
rule "firefox" workspace "2" float false
```
Match a client by `app_id` and assign a workspace plus a default floating
state. Multiple properties may follow the app id.

## Live changes

The IPC socket at `$XDG_RUNTIME_DIR/LeWM.sock` accepts lines like:
```
toggle_panel
set gap 12
set anim.open_ms 300
set panel.bg "#000000"
```
`set` writes through to `config.le` immediately.
