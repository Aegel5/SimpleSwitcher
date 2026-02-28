
public static class ProgramUtils {
    public static string TypeSession => IsDebug ? "DEBUG" : "RELEASE";

    static public bool IsRelease => !IsDebug;

    static public bool IsDebug {
        get {
#if DEBUG
            return true;
#else
            return false;
#endif
        }
    }

    public static string? BinFolder => Path.GetDirectoryName(ExePath);

    public static string? ExePath => Environment.ProcessPath;

    static public string FindOurPath(string name, bool except = true) {
        var path = BinFolder;
        while (true) {
            if (path == null)
                return except ? throw new Exception("can't find " + name) : "";
            var cur = Path.Combine(path, name);

            if (File.Exists(cur) || Directory.Exists(cur)) {
                return cur;
            }

            path = Path.GetDirectoryName(path);
            if (path == null) {
                return except ? throw new Exception("can't find " + name):"";
            }
        }
    }

    static public string FindOurPath(params string[] parms) {
        var path = FindOurPath(parms[0]);
        for (int i = 1; i < parms.Length; i++) {
            path = Path.Combine(path, parms[i]);
        }
        if (!File.Exists(path) && !Directory.Exists(path))
            throw new Exception("can't find " + path);
        return path;
    }

    static public bool HasArg(string cmd_arg) => Environment.GetCommandLineArgs().FirstOrDefault(x => x.Contains(cmd_arg)) != null;

    public static DateTime StartTime { get; private set; } = DateTime.UtcNow;


}

