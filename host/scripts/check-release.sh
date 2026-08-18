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
need plugins/platforms/libqxcb.so
if ! ldd "$dest/plugins/platforms/libqoffscreen.so" 2>/dev/null | grep -q 'libQt6Gui.so'; then
  echo "bundled offscreen plugin is not linked to Qt 6" >&2
  fail=1
fi
if ! ldd "$dest/plugins/platforms/libqxcb.so" 2>/dev/null | grep -q 'libQt6Gui.so'; then
  echo "bundled xcb plugin is not linked to Qt 6" >&2
  fail=1
fi
if [ -e "$dest/plugins/multimedia/libffmpegmediaplugin.so" ] || \
   [ -e "$dest/plugins/multimedia/libgstreamermediaplugin.so" ]; then
  echo "do not bundle Qt FFmpeg/GStreamer plugins; they crash on startup" >&2
  fail=1
fi
if [ ! -e "$dest/dotnet/libSkiaSharp.so" ]; then
  skia=$(find "$dest/dotnet" -name 'libSkiaSharp.so' 2>/dev/null | head -n 1)
  if [ -z "$skia" ]; then
    echo "missing dotnet/libSkiaSharp.so" >&2
    fail=1
  fi
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
qt_multimedia=$(find "$dest" -maxdepth 1 -name 'libQt6Multimedia.so*' | head -n 1)
if [ -z "$qt_multimedia" ]; then
  echo "missing bundled libQt6Multimedia" >&2
  fail=1
fi

qt_core=$(find "$dest" -maxdepth 1 -name 'libQt6Core.so*' | head -n 1)
if [ -n "$qt_core" ] && command -v readelf >/dev/null 2>&1; then
  if ! readelf -d "$qt_core" | grep -q '\$ORIGIN'; then
    echo "bundled QtCore has no \$ORIGIN runpath; host ICU will be required" >&2
    fail=1
  fi
fi
icu=$(find "$dest" -maxdepth 1 -name 'libicui18n.so*' | head -n 1)
if [ -z "$icu" ]; then
  echo "missing bundled libicui18n" >&2
  fail=1
fi

if [ -x "$dest/pkhex-qt" ]; then
  if command -v timeout >/dev/null 2>&1; then
    status=0
    QT_QPA_PLATFORM=offscreen timeout 3 "$dest/pkhex-qt" >/dev/null 2>&1 || status=$?
    case "$status" in
      0|124)
        ;;
      127|132|134|136|139)
        echo "pkhex-qt failed to start (exit $status)" >&2
        fail=1
        ;;
    esac
  fi
else
  echo "pkhex-qt is not executable" >&2
  fail=1
fi

if [ "$fail" -ne 0 ]; then
  exit 1
fi
echo "release folder ok: $dest"
