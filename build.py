import pathlib
from pathlib import Path
import re
import os
import shutil 
import sys
import subprocess
import requests
import json


#pip install requests

is_debug = False
is_publ = False
is_notel = True
is_clean = True
for arg in sys.argv[1:]:
	if arg == "/publish":		is_publ = True
	elif arg == "/debug":		is_debug = True
	elif arg == "/clean":		is_clean = True
	else :
		print(f"unknown arg {arg}")
		exit(1)
if is_publ: 
    is_clean = True
    
is_upload_github = is_publ and False

if is_upload_github:
	# pip install PyGithub
	from github import Github


curpath = pathlib.Path(__file__).parent.resolve()
print(f"curpath is {curpath}");

os.chdir(curpath)

# пропатчим версию

ver_path = curpath / "src" / "ver.h"
ver_cont = Path(ver_path).read_text()
curv2 = re.search(r'\"(.*)\"', ver_cont).group(1)

package_build_folder = curpath / "package_build"

def run(exe, arg):
	cmd = f'"{exe}" {arg}'
	print(cmd)
	
	#subprocess.run([exe, arg], capture_output=True, check=True, shell=True)
	os.system(cmd)


def delfold(fold):
	def remove_readonly(func, path, excinfo):
		# Функция для обработки ошибок при удалении файлов
		os.chmod(path, 0o777)  # Изменяем права доступа к файлу
		func(path)  # Повторяем попытку удаления
	if os.path.exists(fold): 
		shutil.rmtree(fold, onerror=remove_readonly) 
  
if is_clean: delfold(package_build_folder)  # Полная пересборка!

result_dir_root = package_build_folder / "OUT"
delfold(result_dir_root)

assets = []

def get_vc_env(arch='x64'):
    # Path to vcvarsall.bat - adjust this path based on your Visual Studio installation
    vcvars_path = r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
    
    # Command to run vcvarsall.bat and then output environment variables
    command = f'"{vcvars_path}" {arch} && set'
    
    # Run the command in a shell and capture output
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    
    env = {}
    for line in result.stdout.splitlines():
        key_value = line.split('=', 1)
        if len(key_value) == 2:
            key, value = key_value
            env[key] = value
    return env


def build(subfold, is64):

	xArch = "x86" if not is64 else "x64"

	env = get_vc_env(xArch)
	for e in env:
		os.putenv(e, env[e])

	to_build = subfold
	suff = f"_{xArch}"
	subfold += suff

	print(f'*** BUILD {subfold} ***');

	result_dir = result_dir_root / subfold
	result_dir.mkdir(parents=True, exist_ok=True) 

	path = package_build_folder / subfold
	path.mkdir(parents=True, exist_ok=True)
   
	release_folder = path
	delfold(release_folder) # ensure we get only builded now binares

	publ_define = ""
	if(is_publ):
		publ_define = "-DPUBLIC_RELEASE=ON"	

	subprocess.run(['cmake', f'--preset {xArch}-release', f'-B{path}', publ_define, curpath])
	subprocess.run(f'cmake --build "{path}"')

	tocopy = ['.exe', '.dll']
	for root, dirs, files in os.walk(release_folder):
		for file in files:
			for ext in tocopy:
				if file.endswith(ext):	
					file2 = file
					print("copy ", file2)
					result_path = os.path.join(result_dir, file2)
					shutil.copy(os.path.join(root, file), result_path)
					check_sum_util = curpath / "cert" / "checksum.exe"
					run(check_sum_util, f"sha256 {result_path} toFile")
		break
			
	print("Copy bin files")
	shutil.copytree(curpath / "bin_files", result_dir, dirs_exist_ok=True)

	zippath = result_dir_root / f"SimpleSwitcher{suff}_{curv2}"
	shutil.make_archive(zippath, 'zip', result_dir) 
	assets.append(str(zippath) + ".zip")
  

#build("SimpleSwitcher", is64=False)
build("SimpleSwitcher", is64=True)


def publish():
	print('*** publish release ***')  
	
	tok = Path("D:/yy/data.txt").read_text() 
	g = Github(tok)
	
	repo = g.get_repo("aegel5/SimpleSwitcher")
	rel_message = "## Changes\n- incremental build"
	
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
	for ass in assets:
		newrel.upload_asset(ass)

	if not is_notel:
		bot_token = Path("D:/yy/tok.txt").read_text() 
		msg = f'Новая версия! {curv2}  <a href="https://github.com/Aegel5/SimpleSwitcher/releases">Скачать</a>'
		url = f'https://api.telegram.org/bot{bot_token}/sendMessage'
		# Create the payload as a dictionary
		payload = { 'chat_id': '-1002391595712',	'text': msg,	'parse_mode': 'HTML' }
		# Convert the payload to JSON format
		json_payload = json.dumps(payload)
		# Send the request to the Telegram API
		response = requests.post(url, data=json_payload, headers={'Content-Type': 'application/json'})
		# Print the response from Telegram
		print(response.json())
	
if is_upload_github:  
	publish()	

print("\n***DONE***")
