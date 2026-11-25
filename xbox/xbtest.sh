#!/bin/bash
# Copyright (c) 2026 Skitzelles - part of Game Vault, https://github.com/skitzellez/xbox-game-vault
# Ymir Xbox test loop: build, package <version>, install on the console, start it (optional link), wait, show the log.
# xbox/xbtest.sh <version> [seconds] [link]   (nobuild=1 skips the build)
set -e
V=$1; W=${2:-20}; LINK=$3
X=/c/Users/GamerDad/XboxEmu
if [ -z "$nobuild" ]; then
  (cd $X/ymir-src && cmd //c "xbox\build_gv.bat" > $X/ymir_build.log 2>&1) || { grep -E " error |FAILED" $X/ymir_build.log | head; exit 1; }
fi
bash $X/ymir-src/xbox/package.sh $V | tail -1
cd $X/XemuUWP/tools
export XB_FAMILY=GamerDad.YmirForGameVault MSYS_NO_PATHCONV=1 XB_DIR=/LocalState/Ymir
PY=/c/Users/GamerDad/AppData/Local/Programs/Python/Python312/python.exe
$PY xb.py stop >/dev/null 2>&1 || true
$PY xb.py install "C:/Users/GamerDad/XboxEmu/ymir-src/dist/YmirForGameVault_$V""_x64.appx" 2>&1 | tail -1
if [ -n "$LINK" ]; then $PY xb.py launch "$LINK" 2>&1 | tail -1; else $PY xb.py launch 2>&1 | tail -1; fi
timeout $W tail -f /dev/null || true
echo "== xbox_start.txt"; $PY xb.py log xbox_start.txt
$PY xb.py procs 2>&1 | grep -i "Ymir.exe" || echo "(Ymir not running)"
$PY xb.py shot $X/ymir-src/build-gv/xb_shot.png >/dev/null 2>&1 || true
