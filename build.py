import pathlib
from pathlib import Path
import re
import os
import shutil 
import sys
import subprocess
import requests
import json

is_debug = False
is_publ = False
is_notel = False
ver_suff = ''
for arg in sys.argv[1:]:
    if arg == "/publish":        is_publ = True
    elif arg == "/debug":        is_debug = True
    elif arg == "/notel":        is_notel = True
    elif arg == "/dev":        ver_suff = '_dev'
    else :
        print(f"unknown arg {arg}")
        exit(1)


curpath = pathlib.Path(__file__).parent.resolve()
print(f"curpath is {curpath}");

os.chdir(curpath)

# пропатчим версию
ver_path_1 = curpath / "SimpleSwitcher/src/ver.txt"
ver_path_2 = curpath / "SimpleSwitcher/src/ver.h"

ver_num = int(Path(ver_path_1).read_text())
ver_num+=1
Path(ver_path_1).write_text(str(ver_num))

ver_custom = ''
if not is_publ: ver_custom = ' USER'
curv2 = f'5.{ver_num:03}{ver_suff}{ver_custom}'
Path(ver_path_2).write_text(f'static const char* SW_VERSION = "{curv2}";')

package_build_folder = pathlib.Path("package_build")
result_dir_root = package_build_folder / "OUT"
result_dir = result_dir_root / "SimpleSwitcher"

def run(exe, arg):
    cmd = f'{exe} {arg}'
    print(cmd)
    
    #subprocess.run([exe, arg], capture_output=True, check=True, shell=True)
    os.system(cmd)

def build_localization():
    loc_folder = curpath / "SimpleSwitcher" / "localization"
    for root, dirs, files in os.walk(loc_folder):
        for file in files:
            if file.endswith(".po"):      
                path = os.path.join(root, file)
                path_mo = path.replace(".po", ".mo")
                run(loc_folder / "gnuwin32" / "msgfmt.exe", f"-o {path_mo} {path}")
                print(f"build {path}")

    
build_localization()  

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
    
    cmake_path = "cmake.exe"
    
    #cmake_path_2 = r"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    #if os.path.isfile(cmake_path_2):cmake_path = cmake_path_2
        
    run(cmake_path, f'-G "Visual Studio 17 2022"  {" " if is64 else "-A Win32"} -DCMAKE_BUILD_TYPE={rel_name} ./{subfold} -B {path}')
    run(cmake_path, f'--build {path} --parallel --config {rel_name}')

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


print("Copy bin files")
shutil.copytree("SimpleSwitcher/bin_files", result_dir, dirs_exist_ok=True)

zippath = result_dir_root / f"SimpleSwitcher_{curv2}"
    
shutil.make_archive(zippath, 'zip', result_dir) 

def publish():
    print('*** publish release ***')
    
    # pip install PyGithub requests
    from github import Github
    
    tok = Path("D:/yy/data.txt").read_text() 
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
    newrel = repo.create_git_tag_and_release(curv2, curv2, release_name=f"SimpleSwitcher {curv2}", release_message=rel_message, object=last_com.sha, type="commit")
    newrel.upload_asset(str(zippath) + ".zip")

    if not is_notel:
        bot_token = Path("D:/yy/tok.txt").read_text() 
        msg = f'Новая версия! {curv2}  <a href="https://github.com/Aegel5/SimpleSwitcher/releases">Скачать</a>'
        url = f'https://api.telegram.org/bot{bot_token}/sendMessage'
        # Create the payload as a dictionary
        payload = { 'chat_id': '-1002391595712',    'text': msg,    'parse_mode': 'HTML' }
        # Convert the payload to JSON format
        json_payload = json.dumps(payload)
        # Send the request to the Telegram API
        response = requests.post(url, data=json_payload, headers={'Content-Type': 'application/json'})
        # Print the response from Telegram
        print(response.json())
    
if is_publ:  
    publish()    

print("\n***DONE***")
