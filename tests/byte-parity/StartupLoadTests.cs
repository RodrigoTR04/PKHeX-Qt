using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class StartupLoadTests
{
    [Fact]
    public void last_loaded_skips_missing_paths_and_opens_the_first_existing_save()
    {
        using var dir = new TempDir();
        var keep = Path.Combine(dir.Path, "keep.sav");
        File.WriteAllBytes(keep, OccupiedSave.WithBoxSlot().Save);
        var config = Isolated(dir);
        config.AutoLoadSaveOnStartup = SaveFileLoadSetting.LastLoaded;
        config.RecentlyLoaded.Add(Path.Combine(dir.Path, "gone.sav"));
        config.RecentlyLoaded.Add(keep);

        var app = App(dir, config);
        app.TryStartup([]);

        Assert.Equal(keep, app.Session!.LoadedPath);
        Assert.Equal(keep, config.RecentlyLoaded[0]);
    }

    [Fact]
    public void recent_backup_opens_the_newest_save_under_jksv_on_a_supplied_root()
    {
        using var dir = new TempDir();
        var sd = Path.Combine(dir.Path, "sd");
        var jksv = Path.Combine(sd, "JKSV");
        Directory.CreateDirectory(jksv);
        var older = Path.Combine(jksv, "older.sav");
        var newer = Path.Combine(jksv, "newer.sav");
        File.WriteAllBytes(older, OccupiedSave.WithBoxSlot(pk => pk.TID16 = 1).Save);
        File.WriteAllBytes(newer, OccupiedSave.WithBoxSlot(pk => pk.TID16 = 2).Save);
        File.SetLastWriteTimeUtc(older, DateTime.UtcNow.AddMinutes(-5));
        File.SetLastWriteTimeUtc(newer, DateTime.UtcNow);

        var config = Isolated(dir);
        config.AutoLoadSaveOnStartup = SaveFileLoadSetting.RecentBackup;
        var app = App(dir, config, [sd]);
        app.TryStartup([]);

        Assert.Equal(newer, app.Session!.LoadedPath);
    }

    [Fact]
    public void recent_backup_also_scans_custom_backup_paths()
    {
        using var dir = new TempDir();
        var extra = Path.Combine(dir.Path, "extra");
        Directory.CreateDirectory(extra);
        var sav = Path.Combine(extra, "custom.sav");
        File.WriteAllBytes(sav, OccupiedSave.WithBoxSlot().Save);

        var config = Isolated(dir);
        config.AutoLoadSaveOnStartup = SaveFileLoadSetting.RecentBackup;
        config.OtherBackupPaths.Add(extra);
        var emptyRoot = Path.Combine(dir.Path, "empty-root");
        Directory.CreateDirectory(emptyRoot);
        var app = App(dir, config, [emptyRoot]);
        app.TryStartup([]);

        Assert.Equal(sav, app.Session!.LoadedPath);
    }

    [Fact]
    public void disabled_autoload_does_not_open_detected_or_recent_saves()
    {
        using var dir = new TempDir();
        var sav = Path.Combine(dir.Path, "named.sav");
        File.WriteAllBytes(sav, OccupiedSave.WithBoxSlot().Save);
        var config = Isolated(dir);
        config.AutoLoadSaveOnStartup = SaveFileLoadSetting.Disabled;
        config.RecentlyLoaded.Add(sav);
        var jksv = Path.Combine(dir.Path, "sd", "JKSV");
        Directory.CreateDirectory(jksv);
        File.WriteAllBytes(Path.Combine(jksv, "card.sav"), OccupiedSave.WithBoxSlot().Save);

        var app = App(dir, config, [Path.Combine(dir.Path, "sd")]);
        app.TryStartup([]);

        Assert.NotNull(app.Session);
        Assert.Null(app.Session!.LoadedPath);
        Assert.False(app.Session.IsExportable);
    }

    [Fact]
    public void command_line_save_overrides_last_loaded()
    {
        using var dir = new TempDir();
        var recent = Path.Combine(dir.Path, "recent.sav");
        var cli = Path.Combine(dir.Path, "cli.sav");
        File.WriteAllBytes(recent, OccupiedSave.WithBoxSlot(pk => pk.TID16 = 11).Save);
        File.WriteAllBytes(cli, OccupiedSave.WithBoxSlot(pk => pk.TID16 = 22).Save);
        var config = Isolated(dir);
        config.AutoLoadSaveOnStartup = SaveFileLoadSetting.LastLoaded;
        config.RecentlyLoaded.Add(recent);

        var app = App(dir, config);
        app.TryStartup([cli]);

        Assert.Equal(cli, app.Session!.LoadedPath);
    }

    [Fact]
    public void opening_a_save_seeds_the_editor_from_the_template_folder()
    {
        using var dir = new TempDir();
        var input = Path.Combine(dir.Path, "named.sav");
        File.WriteAllBytes(input, OccupiedSave.WithBoxSlot().Save);
        var templateDir = Path.Combine(dir.Path, "template");
        Directory.CreateDirectory(templateDir);

        var seeded = OccupiedSave.WithBoxSlot(pk => pk.Species = (ushort)Species.Charmander);
        var session = EditorSession.Load(seeded.Save);
        session.SelectBoxSlot(0, 0);
        File.WriteAllBytes(Path.Combine(templateDir, "template.pk5"), session.ExportEntity());

        var app = App(dir, Isolated(dir));
        app.OpenFromPath(input);

        Assert.Equal(((int)Species.Charmander).ToString(), app.Session!.GetField("CB_Species"));
        Assert.NotEqual("1", app.Session.GetField("CB_Species"));
    }

    [Fact]
    public void missing_template_uses_the_same_blank_fallback_as_core()
    {
        using var dir = new TempDir();
        var input = Path.Combine(dir.Path, "named.sav");
        var bytes = OccupiedSave.WithBoxSlot().Save;
        File.WriteAllBytes(input, bytes);
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var expected = sav.LoadTemplate(Path.Combine(dir.Path, "no-template"));
        if (expected.Data.SequenceEqual(sav.BlankPKM.Data))
            EntityTemplates.TemplateFields(expected, sav);

        var app = App(dir, Isolated(dir));
        app.OpenFromPath(input);

        Assert.Equal(expected.Species.ToString(), app.Session!.GetField("CB_Species"));
        Assert.NotEqual("1", app.Session.GetField("CB_Species"));
    }

    [Fact]
    public void startup_settings_round_trip_in_user_config()
    {
        using var dir = new TempDir();
        var config = Isolated(dir);
        config.AutoLoadSaveOnStartup = SaveFileLoadSetting.LastLoaded;
        config.RecentlyLoaded.Add("/tmp/a.sav");
        config.OtherBackupPaths.Add("/tmp/backups");
        config.AskedCreateBackupFolder = true;
        config.Save();

        var loaded = UserConfig.Load(dir.Path);
        Assert.Equal(SaveFileLoadSetting.LastLoaded, loaded.AutoLoadSaveOnStartup);
        Assert.Equal(["/tmp/a.sav"], loaded.RecentlyLoaded);
        Assert.Equal(["/tmp/backups"], loaded.OtherBackupPaths);
        Assert.True(loaded.AskedCreateBackupFolder);
    }

    [Fact]
    public void backup_prompt_is_consumed_once_and_yes_creates_the_folder()
    {
        using var dir = new TempDir();
        var app = App(dir, Isolated(dir));
        Assert.True(app.TakeBackupPrompt());
        Assert.False(app.TakeBackupPrompt());
        Assert.False(Directory.Exists(app.Config.BackupDirectory));
        app.CreateBackupFolder();
        Assert.True(Directory.Exists(app.Config.BackupDirectory));
    }

    private static UserConfig Isolated(TempDir dir)
        => new() { Root = dir.Path };

    private static EditorApp App(TempDir dir, UserConfig config, IReadOnlyList<string>? roots = null)
    {
        var app = new EditorApp(config);
        app.DetectRoots = roots ?? [Path.Combine(dir.Path, "no-drives")];
        return app;
    }

    private sealed class TempDir : IDisposable
    {
        public string Path { get; } = Directory.CreateTempSubdirectory("pkhexqt-start-").FullName;

        public void Dispose()
        {
            try
            {
                Directory.Delete(Path, recursive: true);
            }
            catch (IOException)
            {
            }
        }
    }
}
