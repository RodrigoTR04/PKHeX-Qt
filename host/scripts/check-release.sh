#!/bin/sh
set -eu
# Usage: check-release.sh DEST_DIR
# Verifies a PKHeX Qt folder artifact can run without a .NET SDK.

dest=${1:-}
if [ -z "$dest" ] || [ ! -d "$dest" ]; then
  echo "usage: check-release.sh DEST_DIR" >&2
  exit 2
fi

fail=0
need() {
  if [ ! -e "$dest/$1" ]; then
    echo "missing $1" >&2
    fail=1
  fi
}

need pkhex-qt
need LICENSE
need libnethost.so
need qt.conf
need lang/lang_en.txt
need fonts/PGLDings-NormalRegular.ttf
need about/changelog.txt
need dotnet/PKHeX.Editor.dll
need dotnet/PKHeX.Core.dll
need dotnet/PKHeX.Editor.runtimeconfig.json
need dotnet/libhostfxr.so
need dotnet/libcoreclr.so
need plugins/platforms/libqoffscreen.so
if ! ldd "$dest/plugins/platforms/libqoffscreen.so" 2>/dev/null | grep -q 'libQt6Gui.so'; then
  echo "bundled offscreen plugin is not linked to Qt 6" >&2
  fail=1
fi

if [ ! -d "$dest/assets/sprites" ]; then
  echo "missing assets/sprites" >&2
  fail=1
else
  sprite=$(find "$dest/assets/sprites" -name '*.png' | head -n 1)
  if [ -z "$sprite" ]; then
    echo "assets/sprites has no png files" >&2
    fail=1
  fi
fi

qt_widgets=$(find "$dest" -maxdepth 1 -name 'libQt6Widgets.so*' | head -n 1)
if [ -z "$qt_widgets" ]; then
  echo "missing bundled libQt6Widgets" >&2
  fail=1
fi

if [ -x "$dest/pkhex-qt" ]; then
  :
else
  echo "pkhex-qt is not executable" >&2
  fail=1
fi

if [ "$fail" -ne 0 ]; then
  exit 1
fi
echo "release folder ok: $dest"
