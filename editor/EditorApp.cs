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
        DetectRoots = SaveDetect.DefaultRoots();
    }

    public UserConfig Config { get; }
    public EditorSession? Session { get; private set; }
    public IReadOnlyList<string> DetectRoots { get; set; }

    public bool PathIsSave(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        return SaveUtil.TryGetSaveFile(path, out _);
    }

    public bool NeedsClosePrompt => Session?.NeedsClosePrompt == true;
    public bool NeedsOpenPrompt => Session?.NeedsOpenPrompt == true;
    public bool NeedsExportPrompt => Session?.NeedsExportPrompt(Config.CheckUnsavedEntityOnExport) == true;
    public bool IsExportable => Session?.IsExportable == true;

    public string SlotCryPath(bool party, int box, int slot)
    {
        if (!Config.HoverSlotPlayCry || Session is null)
            return string.Empty;
        return Session.SlotCryPath(Config.SoundDirectory, party, box, slot);
    }

    public void OpenFromPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var previous = Session;
        Session = EditorSession.OpenDropped(Session, path);
        if (!ReferenceEquals(previous, Session))
        {
            TryBackupOnOpen();
            if (Session.LoadedPath is { } loaded)
                Config.RememberLoaded(loaded);
            Session.LoadTemplate(Config.TemplateDirectory);
        }
    }

    public void TryStartup(IReadOnlyList<string> args)
    {
        ArgumentNullException.ThrowIfNull(args);
        string? cliSave = null;
        string? cliEntity = null;
        foreach (var arg in args)
        {
            if (string.IsNullOrWhiteSpace(arg) || !File.Exists(arg))
                continue;
            if (SaveUtil.TryGetSaveFile(arg, out _))
                cliSave = arg;
            else
                cliEntity = arg;
        }

        if (cliSave is not null)
            OpenFromPath(cliSave);
        else
            OpenAuto();

        if (cliEntity is not null && Session is not null)
            Session.ImportEntityFromPath(cliEntity);
    }

    public bool TakeBackupPrompt()
    {
        if (Config.AskedCreateBackupFolder)
            return false;
        Config.AskedCreateBackupFolder = true;
        return !Directory.Exists(Config.BackupDirectory);
    }

    public void CreateBackupFolder() => Directory.CreateDirectory(Config.BackupDirectory);

    public void SaveToPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var session = RequireSession();
        File.WriteAllBytes(path, session.Export());
        session.SetLoadedPath(path);
        session.MarkClean();
        Config.RememberLoaded(path);
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

    private void OpenAuto()
    {
        switch (Config.AutoLoadSaveOnStartup)
        {
            case SaveFileLoadSetting.LastLoaded:
                foreach (var path in Config.RecentlyLoaded)
                {
                    if (File.Exists(path) && PathIsSave(path))
                    {
                        OpenFromPath(path);
                        return;
                    }
                }
                OpenBlank();
                return;
            case SaveFileLoadSetting.RecentBackup:
                using (var cts = new CancellationTokenSource(TimeSpan.FromSeconds(5)))
                {
                    var detected = SaveDetect.Detect(DetectRoots, Config.OtherBackupPaths, cts.Token)
                        .FirstOrDefault();
                    if (detected?.Metadata.FilePath is { } path && File.Exists(path))
                    {
                        OpenFromPath(path);
                        return;
                    }
                }
                OpenBlank();
                return;
            default:
                OpenBlank();
                return;
        }
    }

    private void OpenBlank()
    {
        Session = EditorSession.Blank(Config.DefaultSaveVersion);
        Session.LoadTemplate(Config.TemplateDirectory);
    }

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
