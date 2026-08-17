using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class UnsavedBackupTests
{
    [Fact]
    public void set_field_without_commit_is_unsaved_entity_not_edited_save()
    {
        var session = OccupiedSession();
        session.SetField("CHK_NicknamedFlag", "1");
        session.SetField("TB_Nickname", "UNSAVED");

        Assert.True(session.EntityUnsaved);
        Assert.False(session.SaveEdited);
        Assert.True(session.NeedsClosePrompt);
        Assert.False(session.NeedsOpenPrompt);
        Assert.True(session.NeedsExportPrompt(checkUnsavedEntity: true));
        Assert.False(session.NeedsExportPrompt(checkUnsavedEntity: false));
    }

    [Fact]
    public void commit_marks_the_save_edited_and_the_entity_clean()
    {
        var session = OccupiedSession();
        session.SetField("CHK_NicknamedFlag", "1");
        session.SetField("TB_Nickname", "COMMIT");
        session.CommitCurrent();

        Assert.False(session.EntityUnsaved);
        Assert.True(session.SaveEdited);
        Assert.True(session.NeedsClosePrompt);
        Assert.True(session.NeedsOpenPrompt);
        Assert.False(session.NeedsExportPrompt(checkUnsavedEntity: true));
    }

    [Fact]
    public void export_marks_the_save_clean()
    {
        using var dir = new TempDir();
        var input = Path.Combine(dir.Path, "input.sav");
        File.WriteAllBytes(input, OccupiedSave.WithBoxSlot().Save);
        var config = IsolatedConfig(dir);
        var app = new EditorApp(config);
        app.OpenFromPath(input);
        app.Session!.SelectBoxSlot(0, 0);
        app.Session.SetField("CHK_NicknamedFlag", "1");
        app.Session.SetField("TB_Nickname", "EXPORT");
        app.Session.CommitCurrent();

        var output = Path.Combine(dir.Path, "out.sav");
        app.SaveToPath(output);

        Assert.False(app.Session.SaveEdited);
        Assert.False(app.Session.NeedsClosePrompt);
        Assert.False(File.Exists(output + ".bak"));
        Assert.Equal(app.Session.LoadedPath, output);
    }

    [Fact]
    public void first_open_copies_into_bak_and_second_open_does_not_overwrite()
    {
        using var dir = new TempDir();
        var input = Path.Combine(dir.Path, "named.sav");
        File.WriteAllBytes(input, OccupiedSave.WithBoxSlot().Save);
        var config = IsolatedConfig(dir);
        Directory.CreateDirectory(config.BackupDirectory);

        var app = new EditorApp(config);
        app.OpenFromPath(input);
        var backup = app.Session!.BackupFileName(config.BackupDirectory);
        Assert.True(File.Exists(backup));
        Assert.Equal(File.ReadAllBytes(input), File.ReadAllBytes(backup));

        var stamp = new FileInfo(backup).LastWriteTimeUtc;
        Thread.Sleep(20);
        app.OpenFromPath(input);
        Assert.Equal(stamp, new FileInfo(backup).LastWriteTimeUtc);
    }

    [Fact]
    public void disabled_backup_does_not_copy_on_open()
    {
        using var dir = new TempDir();
        var input = Path.Combine(dir.Path, "named.sav");
        File.WriteAllBytes(input, OccupiedSave.WithBoxSlot().Save);
        var config = IsolatedConfig(dir);
        config.BackupEnabled = false;
        Directory.CreateDirectory(config.BackupDirectory);

        new EditorApp(config).OpenFromPath(input);

        Assert.Empty(Directory.GetFiles(config.BackupDirectory));
    }

    [Fact]
    public void user_config_round_trips_in_the_os_config_root_not_beside_the_binary()
    {
        using var dir = new TempDir();
        var config = IsolatedConfig(dir);
        config.BackupEnabled = false;
        config.AskedCreateBackupFolder = true;
        config.CheckUnsavedEntityOnExport = false;
        config.Save();

        var loaded = UserConfig.Load(dir.Path);
        Assert.False(loaded.BackupEnabled);
        Assert.True(loaded.AskedCreateBackupFolder);
        Assert.False(loaded.CheckUnsavedEntityOnExport);
        Assert.Equal(Path.Combine(dir.Path, "config.json"), loaded.FilePath);
        Assert.DoesNotContain("pkhex.json", loaded.FilePath, StringComparison.OrdinalIgnoreCase);
        Assert.False(loaded.FilePath.StartsWith(AppContext.BaseDirectory, StringComparison.Ordinal));
    }

    [Fact]
    public void overwriting_a_pokemon_file_moves_the_previous_copy_to_bak_once()
    {
        using var dir = new TempDir();
        var path = Path.Combine(dir.Path, "pkm.bin");
        File.WriteAllBytes(path, [1, 2, 3]);
        FileBackup.MoveAsideIfNeeded(path);
        File.WriteAllBytes(path, [9]);
        FileBackup.MoveAsideIfNeeded(path);
        File.WriteAllBytes(path, [8]);

        Assert.Equal([8], File.ReadAllBytes(path));
        Assert.Equal([1, 2, 3], File.ReadAllBytes(path + ".bak"));
    }

    [Fact]
    public void path_is_save_is_true_only_for_save_files()
    {
        using var dir = new TempDir();
        var sav = Path.Combine(dir.Path, "named.sav");
        var pkm = Path.Combine(dir.Path, "pkm.bin");
        File.WriteAllBytes(sav, OccupiedSave.WithBoxSlot().Save);
        File.WriteAllBytes(pkm, [1, 2, 3]);
        var app = new EditorApp(IsolatedConfig(dir));
        Assert.True(app.PathIsSave(sav));
        Assert.False(app.PathIsSave(pkm));
    }

    private static EditorSession OccupiedSession()
    {
        var session = EditorSession.Load(OccupiedSave.WithBoxSlot().Save);
        session.SelectBoxSlot(0, 0);
        return session;
    }

    private static UserConfig IsolatedConfig(TempDir dir)
        => new() { Root = dir.Path };

    private sealed class TempDir : IDisposable
    {
        public string Path { get; } = Directory.CreateTempSubdirectory("pkhexqt-cfg-").FullName;

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
