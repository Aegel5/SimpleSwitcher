import pathlib
from pathlib import Path
import re
import os
import shutil 
import sys

print("start...")

# http://gnuwin32.sourceforge.net/packages/gettext.htm

util = "\"C:/Program Files (x86)/GnuWin32/bin/xgettext.exe\""

file_list = ""

need_ext = ['.cpp', '.h']
for root, dirs, files in os.walk("../src"):
    for file in files:
        for ext in need_ext:
            if file.endswith(ext):
                path = os.path.join(root, file)
                print(path) 
                #os.system(util + " -d ss -s --keyword=wxT -o mess.pot " + path) 
                file_list += f"{path}"
                file_list += " "
cmd =     util + " -d ss -s --keyword=_ -o mess.pot " + file_list
print(cmd)            
os.system(cmd) 