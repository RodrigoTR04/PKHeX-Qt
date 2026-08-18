#!/bin/sh
set -eu
# Usage: check-appimage.sh APPIMAGE
# Extracts without FUSE and reuses check-release.sh on the AppDir.

appimage=${1:-}
if [ -z "$appimage" ] || [ ! -f "$appimage" ]; then
  echo "usage: check-appimage.sh APPIMAGE" >&2
  exit 2
fi
if [ ! -x "$appimage" ]; then
  echo "AppImage is not executable: $appimage" >&2
  exit 1
fi

here=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
work=$(mktemp -d)
cleanup() { rm -rf "$work"; }
trap cleanup EXIT

export APPIMAGE_EXTRACT_AND_RUN=1
appimage=$(CDPATH= cd -- "$(dirname "$appimage")" && pwd)/$(basename "$appimage")
(
  cd "$work"
  "$appimage" --appimage-extract
)

root="$work/squashfs-root"
if [ ! -x "$root/AppRun" ]; then
  echo "missing AppRun" >&2
  exit 1
fi
if [ ! -f "$root/pkhex-qt.desktop" ]; then
  echo "missing pkhex-qt.desktop" >&2
  exit 1
fi
if [ ! -f "$root/pkhex-qt.png" ]; then
  echo "missing pkhex-qt.png" >&2
  exit 1
fi

"$here/check-release.sh" "$root"
echo "appimage ok: $appimage"
