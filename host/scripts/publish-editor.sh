#!/bin/sh
set -eu
dotnet_exe=$1
configuration=$2
output_dir=$3
project=$4
mkdir -p "$output_dir"
exec "$dotnet_exe" msbuild "$project" -restore -t:Publish \
  -p:Configuration="$configuration" \
  -p:PublishDir="$output_dir/" \
  -p:UseAppHost=false
