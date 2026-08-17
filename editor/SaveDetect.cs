using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Host-machine roots for SaveFinder. Core still does the folder names; this supplies Linux/macOS mounts instead of Win32 drive letters.
/// </summary>
public static class SaveDetect
{
    public static IReadOnlyList<string> DefaultRoots()
    {
        if (OperatingSystem.IsWindows())
            return Environment.GetLogicalDrives();

        var roots = new List<string>();
        AddExisting(roots, Environment.GetFolderPath(Environment.SpecialFolder.UserProfile));
        var user = Environment.UserName;
        if (!string.IsNullOrEmpty(user))
        {
            AddExisting(roots, Path.Combine("/media", user));
            AddExisting(roots, Path.Combine("/run/media", user));
        }
        AddExisting(roots, "/mnt");
        AddExisting(roots, "/media");
        AddExisting(roots, "/run/media");
        AddExisting(roots, "/Volumes");
        return roots;
    }

    public static IEnumerable<SaveFile> Detect(
        IReadOnlyList<string> roots,
        IEnumerable<string> extra,
        CancellationToken token)
    {
        ArgumentNullException.ThrowIfNull(roots);
        ArgumentNullException.ThrowIfNull(extra);
        var folders = FoldersToScan(roots, extra);
        return SaveFinder.GetSaveFiles([], detect: false, folders, ignoreBackups: true, token);
    }

    public static IEnumerable<string> FoldersToScan(IReadOnlyList<string> roots, IEnumerable<string> extra)
    {
        ArgumentNullException.ThrowIfNull(roots);
        ArgumentNullException.ThrowIfNull(extra);
        var folders = new List<string>();
        foreach (var path in extra)
        {
            if (!string.IsNullOrWhiteSpace(path))
                folders.Add(path);
        }

        if (OperatingSystem.IsWindows())
        {
            foreach (var folder in SaveFinder.GetFoldersToCheck(roots, extra, CancellationToken.None))
                folders.Add(folder);
            return folders.Distinct(StringComparer.OrdinalIgnoreCase);
        }

        foreach (var root in roots)
        {
            if (string.IsNullOrWhiteSpace(root) || !Directory.Exists(root))
                continue;
            folders.AddRange(SaveFinder.Get3DSBackupPaths(root));
            folders.AddRange(SaveFinder.GetSwitchBackupPaths(root));
        }

        return folders.Distinct(StringComparer.Ordinal);
    }

    private static void AddExisting(List<string> roots, string? path)
    {
        if (string.IsNullOrWhiteSpace(path) || !Directory.Exists(path))
            return;
        if (!roots.Contains(path, StringComparer.Ordinal))
            roots.Add(path);
    }
}
