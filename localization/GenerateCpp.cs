using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public class LocalizationGenerator {
	public static string GenerateCppFile(string languageName, Dictionary<string, string> dictionary) {
		var sb = new StringBuilder();

		sb.AppendLine("#include <utility>");
		sb.AppendLine("#include <array>");
		sb.AppendLine();

		// Используем std::array для поддержки std::span в будущем
		sb.AppendLine($"constexpr std::array<std::pair<const char*, const char*>, {dictionary.Count}> _Localization_{languageName} = {{{{");

		var entries = dictionary.Select(kv =>
			$"    {{\"{Escape(kv.Key)}\", \"{Escape(kv.Value)}\"}}"
		);

		sb.AppendLine(string.Join("," + Environment.NewLine, entries));
		sb.AppendLine("}};");

		return sb.ToString();
	}

	private static string Escape(string s) {
		return s.Replace("\\", "\\\\").Replace("\"", "\\\"");
	}
}
