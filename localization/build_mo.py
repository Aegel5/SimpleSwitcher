import pathlib
from pathlib import Path
import os

def run(exe, arg):
    cmd = f'{exe} {arg}'
    os.system(cmd)

curpath = pathlib.Path(__file__).parent.resolve()
loc_folder = curpath
for root, dirs, files in os.walk(loc_folder):
    for file in files:
        if file.endswith(".po"):      
            path = os.path.join(root, file)
            path_mo = path.replace(".po", ".mo")
            run(loc_folder / "gnuwin32" / "msgfmt.exe", f"-o {path_mo} {path}")
            print(f"build {path}")