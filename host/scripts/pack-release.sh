#!/bin/sh
set -eu
# Usage: pack-release.sh BUILD_HOST_DIR DEST_DIR
# Copies a built Host tree into a runnable folder artifact.

build_dir=${1:-}
dest=${2:-}
if [ -z "$build_dir" ] || [ -z "$dest" ]; then
  echo "usage: pack-release.sh BUILD_HOST_DIR DEST_DIR" >&2
  exit 2
fi

here=$(CDPATH= cd -- "$(dirname "$0")" && pwd)

if [ ! -x "$build_dir/pkhex-qt" ]; then
  echo "pkhex-qt was not built in $build_dir" >&2
  exit 1
fi

rm -rf "$dest"
mkdir -p "$dest"
cp -a "$build_dir/pkhex-qt" "$dest/"
cp -a "$build_dir/libnethost.so" "$dest/" 2>/dev/null || cp -a "$build_dir/nethost.dll" "$dest/" 2>/dev/null || true
for dir in assets fonts lang about dotnet; do
  if [ -d "$build_dir/$dir" ]; then
    cp -a "$build_dir/$dir" "$dest/"
  fi
done

"$here/bundle-qt-linux.sh" "$dest" "$dest/pkhex-qt"

printf '%s\n' \
  'PKHeX Qt — incomplete public build' \
  '' \
  'This folder is not v1. Subforms are still missing. It can open, edit, and save.' \
  'chmod +x pkhex-qt && ./pkhex-qt' \
  >"$dest/INCOMPLETE.txt"

chmod +x "$dest/pkhex-qt"
