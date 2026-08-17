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
  [ -f "$src" ] || return 0
  base=$(basename "$src")
  if [ -e "$dest/$base" ]; then
    return 0
  fi
  cp -L "$src" "$dest/$base"
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
  ldd "$file" 2>/dev/null | while read -r line; do
    case "$line" in
      *'not found'*)
        continue
        ;;
    esac
    path=$(printf '%s\n' "$line" | awk '{print $3}')
    [ -n "$path" ] || continue
    [ -f "$path" ] || continue
    if is_system "$path"; then
      continue
    fi
    copy_lib "$path"
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

copy_plugin platforms/libqxcb.so
copy_plugin platforms/libqoffscreen.so
copy_plugin platforms/libqwayland.so
copy_plugin xcbglintegrations/libqxcb-glx-integration.so
copy_plugin xcbglintegrations/libqxcb-egl-integration.so
copy_plugin imageformats/libqjpeg.so
copy_plugin imageformats/libqpng.so
copy_plugin tls/libqopensslbackend.so

printf '%s\n' '[Paths]' 'Prefix=.' 'Plugins=plugins' 'Libraries=.' >"$dest/qt.conf"
