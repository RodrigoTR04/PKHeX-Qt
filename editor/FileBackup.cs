namespace PKHeX.Editor;

/// <summary>
/// Original PKHeX moves an existing destination aside to path.bak once, then writes the new file.
/// </summary>
public static class FileBackup
{
    public static void MoveAsideIfNeeded(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        if (!File.Exists(path))
            return;
        var bak = path + ".bak";
        if (!File.Exists(bak))
            File.Move(path, bak);
    }
}
