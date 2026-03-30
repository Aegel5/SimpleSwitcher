using System.Runtime.CompilerServices;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace LocalizationBuild; 
internal class Program {

	static string GetCurPath([CallerFilePath] string path = "") => Path.GetDirectoryName(path);

	public static HashSet<string> GetLocsFromFolder(string path) {
		var foundStrings = new HashSet<string>();
		string pattern = @"LOC\(""([^""]*)""\)";

		// Собираем все .cpp и .h файлы во всех подпапках
		var files = Directory.EnumerateFiles(path, "*.*", SearchOption.AllDirectories)
							 .Where(f => f.EndsWith(".cpp") || f.EndsWith(".h"));

		foreach (var file in files) {
			string content = File.ReadAllText(file);
			var matches = Regex.Matches(content, pattern);

			foreach (Match match in matches) {
				foundStrings.Add(match.Groups[1].Value);
			}
		}

		return foundStrings;
	}
	class Info {
		public Dictionary<string, string> complete = new();
	}
	static void Main(string[] args) {

		var path = GetCurPath();

		// получаем актуальные данные
		var english = GetLocsFromFolder(Path.Combine(path, "..", "src"));

		Dictionary<string, Info> simpleLoc = new();
		Info get(string lang) {
			if (!simpleLoc.TryGetValue(lang, out var dict)) { dict = new(); simpleLoc.Add(lang, dict); }
			return dict;
		}

		foreach (var f_ in Directory.GetFiles(path, "*.json")) {

			var curDict = new Dictionary<string, string>();
			try {
				using var stream = File.OpenRead(f_);
				curDict = JsonSerializer.Deserialize<Dictionary<string, string>>(stream);
			} catch (Exception) { }
			var name = Path.GetFileName(f_).Trim('_');
			name = name[..name.IndexOf('.')];
			var info = get(name);
			foreach (var item in curDict) {
				if (!string.IsNullOrEmpty(item.Value)) {
					if (!english.Contains(item.Key)) continue;
					info.complete[item.Key] = item.Value;
				}
			}
		}

		HashSet<string> rewriten = new();

		var opt = new JsonSerializerOptions { WriteIndented = true, Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping };

		int to_translate = 0;
		foreach (var item in simpleLoc) {
			var name = item.Key;
			var info = item.Value;
			var file = Path.Combine(path, name + ".json");
			File.WriteAllText(file, JsonSerializer.Serialize(info.complete, opt));
			rewriten.Add(file);

			int curI = -1;
			foreach (var chunk in english.Where(x => !info.complete.ContainsKey(x)).Chunk(40)){
				to_translate += chunk.Count();
				curI++;
				file = Path.Combine(path, name + $".{curI}.json");
				File.WriteAllText(file, JsonSerializer.Serialize(chunk.ToDictionary(x => x, x => ""), opt));
				rewriten.Add(file);
			}

			var header = LocalizationGenerator.GenerateCppFile(name, info.complete);
			File.WriteAllText(Path.Combine(path, name + ".h"), header);

		}

		foreach (var f_ in Directory.GetFiles(path, "*.json")) {
			if (!rewriten.Contains(f_)) File.Delete(f_);
		}

		Console.WriteLine("To translate: " + to_translate);



	}
}
