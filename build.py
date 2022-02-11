﻿import pathlib
from pathlib import Path
import re
import os
import shutil 
import sys

is_publ = False
for arg in sys.argv:
    if arg == "/publish":
        is_publ = True


curpath = pathlib.Path(__file__).parent.resolve()
print(f"curpath is {curpath}");

os.chdir(curpath)

# пропатчим версию
ver_path = curpath / "SimpleSwitcher/src/ver.h"

contents = Path(ver_path).read_text()

curv = re.search('"(.*)"', contents).group(1)

#a = curv.replace(".","")
#aa = int(a)
#aa+=1
#aaa = str(aa)
#curv2 = ""
#for b in aaa:
#    curv2 += b
#    curv2 += '.'
#curv2 = curv2[:-1]

curvsp = curv.split('.')
curv2 = curvsp[0] + "." + str(int(curvsp[1])+1)
contents = contents.replace(curv,curv2)
print(curv2)
Path(ver_path).write_text(contents)

package_build_folder = pathlib.Path("package_build")
result_dir_root = package_build_folder / "OUT"
result_dir = result_dir_root / "SimpleSwitcher"
try: 
    shutil.rmtree(result_dir_root) 
except FileNotFoundError: 
    pass
result_dir.mkdir(parents=True, exist_ok=True)    

def build(subfold, is64):

    print(f'*** BUILD {subfold} {("64" if is64 else "32")} ***');
    

    build_folder = subfold
    if is64:
        build_folder += "_64"
    path = package_build_folder / build_folder

    pathlib.Path(path).mkdir(parents=True, exist_ok=True)
    
    os.system(f'cmake -G "Visual Studio 17 2022"  {" " if is64 else "-A Win32"} -DCMAKE_BUILD_TYPE=Release ./{subfold} -B {path}')
    os.system(f'cmake --build {path} --parallel --config Release')    
    
    release_folder = path / "Release"
    tocopy = ['.exe', '.dll']
    for root, dirs, files in os.walk(release_folder):
        for file in files:
            for ext in tocopy:
                if file.endswith(ext):    
                    file2 = file
                    if is64:
                        file2 = file.replace(ext, "64"+ext)
                    print("copy ", file2)
                    shutil.copy(os.path.join(root, file), os.path.join(result_dir, file2))
            

build("loader_dll", is64=False)
build("loader_dll", is64=True)    
build("SimpleSwitcher", is64=False)

add_to_bin = "SimpleSwitcher/binfiles"
for file in os.listdir(add_to_bin):
    shutil.copy(os.path.join(add_to_bin, file), os.path.join(result_dir, file))
    
zippath = result_dir_root / f"SimpleSwitcher_v{curv2}"
    
shutil.make_archive(zippath, 'zip', result_dir) 

def publish():
    print('*** publish release ***')
    
    # pip install PyGithub requests
    from github import Github
    
    tok = Path("D:/yy/token.txt").read_text() 
    g = Github(tok)
    
    repo = g.get_repo("aegel5/SimpleSwitcher")
    rel_message = "## Changes\n- minor fixes"
    
    #last_r = repo.get_latest_release()
    #if last_r.body == rel_message:
    #    print("DEL PREV RELEASE")
    #    last_r.delete_release()
    
    last_com = repo.get_branch("master").commit
    print(last_com)
    newrel = repo.create_git_tag_and_release(curv2, curv2, release_name=f"SimpleSwitcher {curv2}", release_message=rel_message, object=last_com.sha, type="commit")
    newrel.upload_asset(str(zippath) + ".zip")
    
if is_publ:  
    publish()    

print("\n***DONE***")