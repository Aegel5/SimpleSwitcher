import polib
import sys
from pathlib import Path

#pip install polib

curpath = Path(__file__).parent.resolve()
name = 'ru.po'
if(len(sys.argv) > 1): name = f"{sys.argv[1]}.po" 
path = curpath / name
cont = ""

Path(curpath / ".last_name").write_text(name)

po = polib.pofile(path)
po.encoding = "utf-8"

cnt = 0
# Translate each entry
for entry in po:
	if entry.msgstr == "":  # Only translate if there's no existing translation
		cont += entry.msgid
		cont += "\n\n"
		cnt+=1
	if cnt >= 30:break

# Save the translated .po file
Path(curpath / "flat.txt").write_text(cont, encoding='utf-8')
print(f"added {cnt} records")

