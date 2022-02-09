import pathlib
from pathlib import Path
import re
import os

curpath = pathlib.Path(__file__).parent.resolve()
# пропатчим версию
ver_path = curpath / "SimpleSwitcher/src/ver.h"

contents = Path(ver_path).read_text()

curv = re.search("\"(.*)\"", contents).group(1)
a = curv.replace(".","")
aa = int(a)
aa+=1
aaa = str(aa)

curv2 = ""
for b in aaa:
    curv2 += b
    curv2 += '.'

curv2 = curv2[:-1]
contents = contents.replace(curv,curv2)
print(curv2)
Path(ver_path).write_text(contents)

os.system("build.cmd")

print("***DONE***")
