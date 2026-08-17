using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Process-local editor used by the Host: open a save from a path, export to a path.
/// Command-line Open and the native Open dialog both call <see cref="OpenFromPath"/>.
/// </summary>
public sealed class EditorApp
{
    public EditorApp()
        : this(UserConfig.Load())
    {
    }

    public EditorApp(UserConfig config)
    {
        ArgumentNullException.ThrowIfNull(config);
        Config = config;
    }

    public UserConfig Config { get; }
    public EditorSession? Session { get; private set; }

    public bool PathIsSave(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        return SaveUtil.TryGetSaveFile(path, out _);
    }
    public bool NeedsClosePrompt => Session?.NeedsClosePrompt == true;
    public bool NeedsOpenPrompt => Session?.NeedsOpenPrompt == true;
    public bool NeedsExportPrompt => Session?.NeedsExportPrompt(Config.CheckUnsavedEntityOnExport) == true;

    public void OpenFromPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var previous = Session;
        Session = EditorSession.OpenDropped(Session, path);
        if (!ReferenceEquals(previous, Session))
            TryBackupOnOpen();
    }

    public void SaveToPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var session = RequireSession();
        File.WriteAllBytes(path, session.Export());
        session.SetLoadedPath(path);
        session.MarkClean();
    }

    public void SaveEntityToPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        FileBackup.MoveAsideIfNeeded(path);
        File.WriteAllBytes(path, RequireSession().ExportEntity());
    }

    public void ExportBackup(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var src = RequireSession().LoadedPath;
        if (string.IsNullOrEmpty(src) || !File.Exists(src))
            throw new FileNotFoundException("Original file has been moved; unable to copy a backup.", src);
        File.Copy(src, path, overwrite: true);
    }

    public string SuggestedBackupName => RequireSession().SuggestedBackupName;

    public void SaveConfig() => Config.Save();

    private void TryBackupOnOpen()
    {
        if (Session is null || !Config.BackupEnabled)
            return;
        var dir = Config.BackupDirectory;
        if (!Directory.Exists(dir))
            return;
        Session.TryCopyOpenBackup(dir);
    }

    private EditorSession RequireSession()
        => Session ?? throw new InvalidOperationException("No save is open.");
}
