#!/bin/sh
set -eu
dotnet_exe=$1
configuration=$2
output_dir=$3
project=$4
rid=${5:-}
self_contained=${6:-0}
mkdir -p "$output_dir"
set -- "$dotnet_exe" publish "$project" \
  -c "$configuration" \
  -p:PublishDir="$output_dir/" \
  -p:UseAppHost=false
if [ "$self_contained" = 1 ]; then
  if [ -z "$rid" ]; then
    echo "self-contained publish requires a RID" >&2
    exit 1
  fi
  set -- "$@" -r "$rid" --self-contained true
fi
exec "$@"
