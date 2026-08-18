#!/bin/sh
set -eu
# Usage: pack-appimage.sh FOLDER_DIR OUT_APPIMAGE
# Wraps a packed Host folder as a Type-2 AppImage. Layout stays flat so
# applicationDirPath() still finds dotnet/, lang/, assets/, and Qt.

src=${1:-}
out=${2:-}
if [ -z "$src" ] || [ -z "$out" ]; then
  echo "usage: pack-appimage.sh FOLDER_DIR OUT_APPIMAGE" >&2
  exit 2
fi
if [ ! -x "$src/pkhex-qt" ]; then
  echo "packed folder is missing pkhex-qt: $src" >&2
  exit 1
fi

here=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
repo=$(CDPATH= cd -- "$here/../.." && pwd)
version=$(sed -n 's/^project(PKHeXQt VERSION \([0-9.]*\).*/\1/p' "$repo/CMakeLists.txt")
if [ -z "$version" ]; then
  version=0.0.0
fi

appdir=$(mktemp -d)
cleanup() { rm -rf "$appdir"; }
trap cleanup EXIT

cp -a "$src"/. "$appdir/"
cp -a "$repo/packaging/linux/pkhex-qt.desktop" "$appdir/pkhex-qt.desktop"

icon_src="$repo/PKHeX-26.07.07/PKHeX.WinForms/Resources/icon.ico"
icon_dst="$appdir/pkhex-qt.png"
if [ -f "$icon_src" ] && command -v convert >/dev/null 2>&1; then
  convert "$icon_src[1]" -filter point -resize 256x256 "$icon_dst"
elif [ -f "$icon_src" ] && command -v magick >/dev/null 2>&1; then
  magick "$icon_src[1]" -filter point -resize 256x256 "$icon_dst"
else
  python3 - "$icon_dst" <<'PY'
import struct, sys, zlib
path = sys.argv[1]
w = h = 256
rgb = (32, 74, 135)
def chunk(tag, data):
    crc = zlib.crc32(tag + data) & 0xffffffff
    return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", crc)
raw = b"".join(b"\x00" + (bytes(rgb) * w) for _ in range(h))
open(path, "wb").write(
    b"\x89PNG\r\n\x1a\n"
    + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
    + chunk(b"IDAT", zlib.compress(raw, 9))
    + chunk(b"IEND", b"")
)
PY
fi

printf '%s\n' '#!/bin/sh' 'HERE=$(dirname "$(readlink -f "$0")")' 'exec "$HERE/pkhex-qt" "$@"' >"$appdir/AppRun"
chmod +x "$appdir/AppRun" "$appdir/pkhex-qt"

tool=${APPIMAGETOOL:-}
if [ -z "$tool" ]; then
  cache=${XDG_CACHE_HOME:-"$HOME/.cache"}/pkhex-qt
  mkdir -p "$cache"
  tool="$cache/appimagetool-1.9.1-x86_64.AppImage"
  if [ ! -x "$tool" ]; then
    url=https://github.com/AppImage/appimagetool/releases/download/1.9.1/appimagetool-x86_64.AppImage
    if command -v curl >/dev/null 2>&1; then
      curl -fsSL -o "$tool" "$url"
    else
      wget -q -O "$tool" "$url"
    fi
    chmod +x "$tool"
  fi
fi

mkdir -p "$(dirname "$out")"
out=$(CDPATH= cd -- "$(dirname "$out")" && pwd)/$(basename "$out")
export APPIMAGE_EXTRACT_AND_RUN=1
export ARCH=x86_64
export VERSION="$version"
"$tool" "$appdir" "$out"
chmod +x "$out"
echo "appimage: $out"
