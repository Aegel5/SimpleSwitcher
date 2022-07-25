import pathlib
from pathlib import Path
import re
import os
import shutil 
import sys

is_debug = False
is_publ = False
for arg in sys.argv:
    if arg == "/publish":
        is_publ = True
    if arg == "/debug":
        is_debug = True


curpath = pathlib.Path(__file__).parent.resolve()
print(f"curpath is {curpath}");

os.chdir(curpath)

# пропатчим версию
ver_path = curpath / "SimpleSwitcher/src/ver.h"

contents = Path(ver_path).read_text()

curv = re.search('"(.*)"', contents).group(1)


curvsp = curv.split('.')
curv2 = curvsp[0] + "." + str(int(curvsp[1])+1)
curv2_v = "v" + curv2
contents = contents.replace(curv,curv2)
print(curv2)
Path(ver_path).write_text(contents)

package_build_folder = pathlib.Path("package_build")
result_dir_root = package_build_folder / "OUT"
result_dir = result_dir_root / "SimpleSwitcher"

def delfold(fold):
    if os.path.exists(fold): 
        shutil.rmtree(fold) 
        
delfold(result_dir_root)      
result_dir.mkdir(parents=True, exist_ok=True)    

def build(subfold, is64):

    print(f'*** BUILD {subfold} {("64" if is64 else "32")} ***');
    
    rel_name = "Debug" if is_debug else "Release"
    

    build_folder = subfold
    if is64:
        build_folder += "_64"
    path = package_build_folder / build_folder

    pathlib.Path(path).mkdir(parents=True, exist_ok=True)
    
    release_folder = path / rel_name
    delfold(release_folder) # ensure we get only builded now binares
    
    os.system(f'cmake -G "Visual Studio 17 2022"  {" " if is64 else "-A Win32"} -DCMAKE_BUILD_TYPE={rel_name} ./{subfold} -B {path}')
    os.system(f'cmake --build {path} --parallel --config {rel_name}')        

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
            

#build("loader_dll", is64=False)
#build("loader_dll", is64=True)    
build("SimpleSwitcher", is64=False)

add_to_bin = "SimpleSwitcher/binfiles"
for file in os.listdir(add_to_bin):
    shutil.copy(os.path.join(add_to_bin, file), os.path.join(result_dir, file))
    
zippath = result_dir_root / f"SimpleSwitcher_{curv2_v}"
    
shutil.make_archive(zippath, 'zip', result_dir) 

def publish():
    print('*** publish release ***')
    
    # pip install PyGithub requests
    from github import Github
    
    tok = Path("D:/yy/token.txt").read_text() 
    g = Github(tok)
    
    repo = g.get_repo("aegel5/SimpleSwitcher")
    rel_message = "## Changes\n- minor fixes"
    
    last_r = repo.get_latest_release()
    all_download = 0
    for ii in last_r.get_assets():
        all_download += ii.download_count
    print(f"last release was downloaded {all_download} times")
    if all_download <= 1 and last_r.body == rel_message:
        print("DEL PREV RELEASE")
        last_r.delete_release()
    
    last_com = repo.get_branch("master").commit
    print(last_com)
    newrel = repo.create_git_tag_and_release(curv2, curv2, release_name=f"SimpleSwitcher {curv2_v}", release_message=rel_message, object=last_com.sha, type="commit")
    newrel.upload_asset(str(zippath) + ".zip")
    
if is_publ:  
    publish()    

print("\n***DONE***")
