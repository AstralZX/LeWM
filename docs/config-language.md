# The `le` configuration language

LeWM is configured with a file called `config.le`, read once at startup
from `$XDG_CONFIG_HOME/LeWM/config.le`. The language is line oriented.
There is no statement terminator and no block syntax; every directive is
one line.

## Lexical rules

- Lines starting with `#` are comments and are ignored.
- Tokens are separated by whitespace.
- Double-quoted strings are used for keys, labels and commands that
  contain spaces (e.g. `"Super+Enter"`, `"foot"`).
- Booleans are the bare words `true` and `false`.

## Directives

### modkey

```
modkey "Super"
```

Sets the primary modifier. Use the XKB key name. Common values:
`"Super"`, `"Alt"`, `"Control"`.

### Borders

```
border_width 2
border_color_active  "#3c3836"
border_color_normal  "#1d2021"
```

Width in pixels. Colors are hex RGB or RGBA.

### Layout

```
layout default "tile"
```

Selects the layout applied to new workspaces. Valid names: `tile`,
`mono`, `grid`.

### Keybindings

```
key "Combo" action [args...]
```

`Combo` is the modifier sequence. `action` is one of:

- `exec "command"` - run a program
- `layout_next` - cycle the active layout
- `kill_focused` - close the focused surface
- `focus_next` / `focus_prev` - move focus
- `workspace "n"` - switch workspace

Example:

```
key "Super+Enter"   exec "foot"
key "Super+Shift+q" kill_focused
key "Super+Space"   layout_next
```

### Workspaces

```
workspace "1" "term"
workspace "2" "web"
```

The first argument is the id used by `key ... workspace`, the second is
a human label shown by status tools.

### Rules

```
rule "app_id" workspace "2" float false
```

Rules match a client's `app_id` (the Wayland surface class) and assign
it a workspace and floating state at spawn time. Multiple properties may
follow the app id.

## Behavior notes

The parser is strict about unknown directives: it logs them and keeps
going, so a typo will not abort startup but will be silent in the output
unless you watch the log. Order matters only for keybindings; the first
matching binding wins.

Config reload is not live. Edit the file and restart the compositor, or
send commands over the IPC socket for runtime changes.
