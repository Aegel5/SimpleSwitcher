
using System.Text.RegularExpressions;

Console.WriteLine("Start build");

var curpath = Path.GetFullPath(Path.Combine(ProgramUtils.FindOurPath("SimpleSwitcherBuild"), ".."));

Console.WriteLine($"Current path: {curpath}");

string curv2 = "unknown"; 

{
	var ver_cont = File.ReadAllText(Path.Combine(curpath, "src", "ver.h"));
	curv2 = Regex.Match(ver_cont, @"\""(.*)\""").Groups[1].Value;
}

var package_build_folder = Path.Combine(curpath, "package_build");

Console.WriteLine($"Build version {curv2}. folder: {package_build_folder}");


