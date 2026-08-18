#!/bin/sh
set -eu
# Copy Qt shared libraries and plugins next to the Host so the folder runs
# without a distro Qt package.

dest=$1
bin=$2

plugin_root=
if command -v qmake6 >/dev/null 2>&1; then
  plugin_root=$(qmake6 -query QT_INSTALL_PLUGINS 2>/dev/null || true)
fi
if [ -z "$plugin_root" ] && command -v qtpaths6 >/dev/null 2>&1; then
  plugin_root=$(qtpaths6 --plugin-dir 2>/dev/null || true)
fi
if [ -z "$plugin_root" ]; then
  for candidate in \
    /usr/lib/qt6/plugins \
    /usr/lib/x86_64-linux-gnu/qt6/plugins \
    /usr/lib64/qt6/plugins
  do
    if [ -f "$candidate/platforms/libqoffscreen.so" ]; then
      plugin_root=$candidate
      break
    fi
  done
fi
if [ -z "$plugin_root" ] || [ ! -d "$plugin_root" ]; then
  echo "could not find Qt 6 plugin directory" >&2
  exit 1
fi
if ! ldd "$plugin_root/platforms/libqoffscreen.so" 2>/dev/null | grep -q 'libQt6Gui.so'; then
  echo "plugin dir $plugin_root is not Qt 6" >&2
  exit 1
fi

mkdir -p "$dest/plugins/platforms"

copy_lib() {
  src=$1
  destname=${2:-}
  [ -f "$src" ] || return 0
  if [ -z "$destname" ]; then
    destname=$(basename "$src")
  fi
  if [ -e "$dest/$destname" ]; then
    return 0
  fi
  cp -L "$src" "$dest/$destname"
}

is_system() {
  case "$1" in
    *ld-linux*|*libc.so*|*libm.so*|*libdl.so*|*libpthread.so*|*librt.so*|*libgcc_s.so*|*libstdc++.so*)
      return 0
      ;;
    *libGL.so*|*libGLdispatch*|*libGLX.so*|*libOpenGL.so*|*libEGL.so*)
      return 0
      ;;
    *libX11.so*|*libXext.so*|*libXau.so*|*libXdmcp.so*|*libxcb.so.*)
      return 0
      ;;
  esac
  return 1
}

copy_deps() {
  file=$1
  [ -f "$file" ] || return 0
  ldd "$file" 2>/dev/null | while IFS= read -r line; do
    case "$line" in
      *'not found'*)
        continue
        ;;
    esac
    soname=$(printf '%s\n' "$line" | awk '{print $1}')
    path=$(printf '%s\n' "$line" | awk '{print $3}')
    [ -n "$path" ] || continue
    [ -f "$path" ] || continue
    case "$soname" in
      linux-vdso*|ld-linux*)
        continue
        ;;
    esac
    if is_system "$path"; then
      continue
    fi
    copy_lib "$path" "$soname"
  done
}

copy_deps "$bin"

copy_plugin() {
  rel=$1
  src="$plugin_root/$rel"
  [ -f "$src" ] || return 0
  mkdir -p "$dest/plugins/$(dirname "$rel")"
  cp -L "$src" "$dest/plugins/$rel"
  copy_deps "$src"
}

copy_plugin_dir() {
  dir=$1
  [ -d "$plugin_root/$dir" ] || return 0
  for src in "$plugin_root/$dir"/*.so; do
    [ -f "$src" ] || continue
    copy_plugin "$dir/$(basename "$src")"
  done
}

# Ship xcb only. A partial Wayland plugin set makes Qt refuse to start on
# Wayland instead of falling back to XWayland.
copy_plugin platforms/libqxcb.so
copy_plugin platforms/libqoffscreen.so
copy_plugin_dir xcbglintegrations
copy_plugin imageformats/libqjpeg.so
copy_plugin imageformats/libqpng.so
copy_plugin imageformats/libqico.so
copy_plugin imageformats/libqgif.so
copy_plugin tls/libqopensslbackend.so
copy_plugin platforminputcontexts/libcomposeplatforminputcontextplugin.so

# Do not ship Qt FFmpeg/GStreamer plugins. Closing over libffmpegmediaplugin.so
# pulls avcodec, Qt Quick/QML, gdk-pixbuf, glycin, … then dlopen segfaults in
# ld.so when MainWindow constructs QSoundEffect. Cries stay silent until a
# small audio backend exists; the Host must still open.

# Close over libs that only appear as deps of other bundled libs (XcbQpa,
# ICU, xcb-cursor, …). Do not walk dest/dotnet.
nprev=-1
while :; do
  n=0
  n=$((n + $(find "$dest" -maxdepth 1 \( -name '*.so' -o -name '*.so.*' \) -type f | wc -l)))
  if [ -d "$dest/plugins" ]; then
    n=$((n + $(find "$dest/plugins" \( -name '*.so' -o -name '*.so.*' \) -type f | wc -l)))
  fi
  [ "$n" -eq "$nprev" ] && break
  nprev=$n
  find "$dest" -maxdepth 1 \( -name '*.so' -o -name '*.so.*' \) -type f \
    | while IFS= read -r file; do
        copy_deps "$file"
      done
  if [ -d "$dest/plugins" ]; then
    find "$dest/plugins" \( -name '*.so' -o -name '*.so.*' \) -type f \
      | while IFS= read -r file; do
          copy_deps "$file"
        done
  fi
done

# Host binary already has RUNPATH $ORIGIN. Distro Qt/ICU do not, so a
# rolling distro with a newer libicu fails even though the files sit
# next to pkhex-qt. Plugins live two directories down.
if ! command -v patchelf >/dev/null 2>&1; then
  echo "patchelf is required to set \$ORIGIN on bundled Qt libraries" >&2
  exit 1
fi
find "$dest" -maxdepth 1 \( -name '*.so' -o -name '*.so.*' \) -type f \
  -exec patchelf --set-rpath '$ORIGIN' {} +
if [ -d "$dest/plugins" ]; then
  find "$dest/plugins" \( -name '*.so' -o -name '*.so.*' \) -type f \
    -exec patchelf --set-rpath '$ORIGIN:$ORIGIN/../..' {} +
fi

printf '%s\n' '[Paths]' 'Prefix=.' 'Plugins=plugins' 'Libraries=.' >"$dest/qt.conf"
