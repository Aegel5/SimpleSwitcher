import pathlib
from pathlib import Path
import re
import os
import shutil 
import sys

resol = sys.argv[1]

result = ''

for root, dirs, files in os.walk('./png{}px'.format(resol)):
    for file in files:
        if file.endswith('.png'):    
            result += '{}{} RCDATA "flags/png{}px/{}"\n'.format(
                Path(file).stem, resol,resol, os.path.basename(file)
            )
            
Path("./flag_res{}.rc".format(resol)).write_text(result)
    