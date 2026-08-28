#!/bin/sh
# Install a fresh user config for LeWM. It never overwrites an existing
# configuration so a user's tweaks survive reinstall/upgrade.
set -e

SAMPLE="${1:-examples/config.le}"

XDG="${XDG_CONFIG_HOME:-$HOME/.config}"
DEST="$XDG/LeWM/config.le"

if [ -z "$HOME" ]; then
    echo "install-config: \$HOME is not set; skipping user config install" >&2
    exit 0
fi

if [ ! -f "$SAMPLE" ]; then
    echo "install-config: sample config not found: $SAMPLE (skipping)" >&2
    exit 0
fi

mkdir -p "$XDG/LeWM"

if [ -f "$DEST" ]; then
    echo "install-config: keeping existing $DEST" >&2
    exit 0
fi

cp "$SAMPLE" "$DEST"
echo "install-config: wrote $DEST" >&2
