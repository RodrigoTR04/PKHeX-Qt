namespace PKHeX.Editor;

internal static class AssetLocator
{
    public static IReadOnlyList<string> SpriteDirectories()
    {
        var dirs = new List<string>();
        foreach (var root in Roots())
        {
            AddIfExists(dirs, Path.Combine(root, "sprites"));
            AddIfExists(dirs, root);
        }

        var oracle = OracleSpriteImg();
        if (oracle is not null)
        {
            AddIfExists(dirs, Path.Combine(oracle, "Artwork Pokemon Sprites"));
            AddIfExists(dirs, Path.Combine(oracle, "Artwork Items"));
            AddIfExists(dirs, Path.Combine(oracle, "Artwork Shiny Sprites"));
            AddIfExists(dirs, Path.Combine(oracle, "Pokemon Sprite Overlays"));
            AddIfExists(dirs, Path.Combine(oracle, "ball"));
            AddIfExists(dirs, Path.Combine(oracle, "Big Pokemon Sprites"));
            AddIfExists(dirs, Path.Combine(oracle, "Big Items"));
        }

        return dirs;
    }

    public static IReadOnlyList<string> WallpaperDirectories()
    {
        var dirs = new List<string>();
        foreach (var root in Roots())
        {
            AddIfExists(dirs, Path.Combine(root, "wallpapers"));
            AddIfExists(dirs, root);
        }

        var oracle = OracleMiscBox();
        if (oracle is not null)
            AddIfExists(dirs, oracle);

        return dirs;
    }

    public static string? FindPng(string resourceName, IReadOnlyList<string> directories)
    {
        var names = CandidateFileNames(resourceName);
        foreach (var dir in directories)
        {
            foreach (var name in names)
            {
                var exact = Path.Combine(dir, name);
                if (File.Exists(exact))
                    return exact;
            }

            if (!Directory.Exists(dir))
                continue;
            foreach (var name in names)
            {
                var matches = Directory.GetFiles(dir, name, SearchOption.AllDirectories);
                if (matches.Length > 0)
                    return matches[0];
            }
        }

        return null;
    }

    private static IEnumerable<string> CandidateFileNames(string resourceName)
    {
        yield return resourceName + ".png";
        if (resourceName.EndsWith("_e", StringComparison.Ordinal))
            yield return resourceName[..^2] + "-e.png";
    }

    private static IEnumerable<string> Roots()
    {
        var env = Environment.GetEnvironmentVariable("PKHEXQT_ASSETS");
        if (!string.IsNullOrEmpty(env))
            yield return env;

        var baseDir = AppContext.BaseDirectory;
        yield return Path.Combine(baseDir, "assets");
        yield return Path.GetFullPath(Path.Combine(baseDir, "..", "assets"));

        var repo = FindRepoRoot(baseDir);
        if (repo is not null)
            yield return Path.Combine(repo, "assets");
    }

    private static string? OracleSpriteImg()
    {
        var repo = FindRepoRoot(AppContext.BaseDirectory);
        if (repo is null)
            return null;
        var path = Path.Combine(repo, "PKHeX-26.07.07", "PKHeX.Drawing.PokeSprite", "Resources", "img");
        return Directory.Exists(path) ? path : null;
    }

    private static string? OracleMiscBox()
    {
        var repo = FindRepoRoot(AppContext.BaseDirectory);
        if (repo is null)
            return null;
        var path = Path.Combine(repo, "PKHeX-26.07.07", "PKHeX.Drawing.Misc", "Resources", "img", "box");
        return Directory.Exists(path) ? path : null;
    }

    private static string? FindRepoRoot(string start)
    {
        var dir = new DirectoryInfo(start);
        while (dir is not null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, "PKHeX-26.07.07", "PKHeX.Core"))
                && Directory.Exists(Path.Combine(dir.FullName, "core")))
                return dir.FullName;
            dir = dir.Parent;
        }

        return null;
    }

    private static void AddIfExists(List<string> dirs, string path)
    {
        if (Directory.Exists(path) && !dirs.Contains(path, StringComparer.Ordinal))
            dirs.Add(path);
    }
}
