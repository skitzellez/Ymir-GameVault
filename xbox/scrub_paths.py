# Copyright (c) 2026 Skitzelles - part of Game Vault, https://github.com/skitzellez/xbox-game-vault
# PolyForm Noncommercial License 1.0.0: not for sale, and this notice stays in every copy.
# Nothing from the build PC's folders may ship: source paths that the compilers put into the programs (assert texts,
# debug-file names) are replaced by a neutral path of the same length, so the files stay valid.
# python scrub_paths.py <package folder> <build folder prefix, e.g. C:\Users\name\work>
import os
import sys

root, prefix = sys.argv[1], sys.argv[2]
prefix = prefix.rstrip("\\/")
drive_less = prefix[3:] if len(prefix) > 3 and prefix[1] == ":" else prefix   # Users\name\work
neutral = ("build" + "\\gamevault" * 20)[:len(drive_less)]
changed = 0
for base, _, files in os.walk(root):
    for name in files:
        if not name.lower().endswith((".exe", ".dll")):
            continue
        path = os.path.join(base, name)
        data = open(path, "rb").read()
        new = data
        for sep in ("\\", "/"):
            old = drive_less.replace("\\", sep)
            rep = neutral.replace("\\", sep)
            for variant in {old, old.lower(), old.upper()}:
                new = new.replace(variant.encode("ascii"), rep.encode("ascii"))
                new = new.replace(variant.encode("utf-16-le"), rep.encode("utf-16-le"))
        if new != data:
            open(path, "wb").write(new)
            changed += 1
user = drive_less.split("\\")[1] if "\\" in drive_less else drive_less
left = []
for base, _, files in os.walk(root):
    for name in files:
        if name.lower().endswith((".exe", ".dll")) and user.lower().encode() in open(os.path.join(base, name), "rb").read().lower():
            left.append(os.path.join(base, name))
print("scrubbed", changed, "files;", "STILL CONTAINS the user name:" if left else "no user name left", *left)
sys.exit(1 if left else 0)
