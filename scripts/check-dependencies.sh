#!/usr/bin/env bash
set -euo pipefail

platform=${1:?Usage: check-dependencies.sh linux|windows executable}
binary=${2:?Missing executable}
case "$platform" in
  linux)
    mapfile -t libraries < <(readelf -d "$binary" | sed -n 's/.*Shared library: \[\(.*\)\]/\1/p')
    ;;
  windows)
    mapfile -t libraries < <(x86_64-w64-mingw32-objdump -p "$binary" | sed -n 's/.*DLL Name: //p' | tr '[:upper:]' '[:lower:]' | tr -d '\r')
    ;;
  *) exit 2 ;;
esac
if ((${#libraries[@]} == 0)); then
  echo "Could not inspect dependencies: $binary" >&2
  exit 1
fi
for library in "${libraries[@]}"; do
  printf '%s\n' "$library"
  case "$platform:$library" in
    linux:libc.so.6|linux:libm.so.6|linux:libdl.so.2|linux:libpthread.so.0|linux:ld-linux-x86-64.so.2) ;;
    windows:advapi32.dll|windows:comdlg32.dll|windows:gdi32.dll|windows:imm32.dll|windows:kernel32.dll|windows:msvcrt.dll|windows:ole32.dll|windows:oleaut32.dll|windows:setupapi.dll|windows:shell32.dll|windows:user32.dll|windows:version.dll|windows:winmm.dll) ;;
    *) echo "Unexpected external dependency: $library" >&2; exit 1 ;;
  esac
done
