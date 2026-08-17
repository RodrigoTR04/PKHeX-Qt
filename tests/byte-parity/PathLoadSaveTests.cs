using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class PathLoadSaveTests
{
    [Fact]
    public void open_from_path_loads_that_save_into_the_editor_session()
    {
        using var dir = new TempDir();
        var path = Path.Combine(dir.Path, "input.sav");
        var fixture = GoldenSave.Gen5BlankExport();
        File.WriteAllBytes(path, fixture);

        var app = new EditorApp();
        app.OpenFromPath(path);

        Assert.NotNull(app.Session);
        Assert.Equal(fixture, app.Session!.Export());
    }

    [Fact]
    public void save_to_path_writes_bytes_identical_to_editor_export()
    {
        using var dir = new TempDir();
        var input = Path.Combine(dir.Path, "input.sav");
        var output = Path.Combine(dir.Path, "exported.sav");
        File.WriteAllBytes(input, GoldenSave.Gen5BlankExport());

        var app = new EditorApp();
        app.OpenFromPath(input);
        var expected = app.Session!.Export();

        app.SaveToPath(output);

        Assert.Equal(expected, File.ReadAllBytes(output));
    }

    private sealed class TempDir : IDisposable
    {
        public string Path { get; } = Directory.CreateTempSubdirectory("pkhexqt-").FullName;

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
