using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class CryPathTests
{
    [Fact]
    public void bulbasaur_uses_the_species_wav_name()
    {
        Assert.Equal("1", CryPath.FileName((ushort)Species.Bulbasaur, 0, EntityContext.Gen5));
    }

    [Fact]
    public void urshifu_rapid_strike_keeps_the_form_suffix_even_though_the_sprite_does_not()
    {
        Assert.Equal("892-1", CryPath.FileName((ushort)Species.Urshifu, 1, EntityContext.Gen8));
        Assert.Equal("892", CryPath.FileName((ushort)Species.Urshifu, 0, EntityContext.Gen8));
    }

    [Fact]
    public void existing_form_wav_wins_then_falls_back_to_species_wav()
    {
        using var dir = new TempDir();
        File.WriteAllBytes(Path.Combine(dir.Path, "25-1.wav"), [1]);
        File.WriteAllBytes(Path.Combine(dir.Path, "25.wav"), [2]);
        File.WriteAllBytes(Path.Combine(dir.Path, "1.wav"), [3]);

        Assert.Equal(Path.Combine(dir.Path, "25-1.wav"), CryPath.ExistingWav((ushort)Species.Pikachu, 1, EntityContext.Gen7, dir.Path));
        Assert.Equal(Path.Combine(dir.Path, "25.wav"), CryPath.ExistingWav((ushort)Species.Pikachu, 2, EntityContext.Gen7, dir.Path));
        Assert.Equal(Path.Combine(dir.Path, "1.wav"), CryPath.ExistingWav((ushort)Species.Bulbasaur, 0, EntityContext.Gen5, dir.Path));
    }

    [Fact]
    public void missing_cry_and_species_zero_are_empty_not_an_error()
    {
        using var dir = new TempDir();
        Assert.Equal(string.Empty, CryPath.ExistingWav((ushort)Species.Bulbasaur, 0, EntityContext.Gen5, dir.Path));
        Assert.Equal(string.Empty, CryPath.ExistingWav(0, 0, EntityContext.Gen5, dir.Path));
        Assert.Equal(string.Empty, CryPath.ExistingWav((ushort)Species.Bulbasaur, 0, EntityContext.Gen5, Path.Combine(dir.Path, "missing-folder")));
    }

    [Fact]
    public void hover_on_an_occupied_slot_returns_the_matching_wav_when_enabled()
    {
        using var dir = new TempDir();
        var sounds = Path.Combine(dir.Path, "sounds");
        Directory.CreateDirectory(sounds);
        var wav = Path.Combine(sounds, "1.wav");
        File.WriteAllBytes(wav, [1]);
        var input = Path.Combine(dir.Path, "named.sav");
        File.WriteAllBytes(input, OccupiedSave.WithBoxSlot().Save);

        var config = new UserConfig { Root = dir.Path, HoverSlotPlayCry = true };
        var app = new EditorApp(config);
        app.DetectRoots = [Path.Combine(dir.Path, "no-drives")];
        app.OpenFromPath(input);

        Assert.Equal(wav, app.SlotCryPath(party: false, box: 0, slot: 0));
        config.HoverSlotPlayCry = false;
        Assert.Equal(string.Empty, app.SlotCryPath(party: false, box: 0, slot: 0));
        Assert.Equal(string.Empty, app.SlotCryPath(party: false, box: 0, slot: 1));
    }

    private sealed class TempDir : IDisposable
    {
        public string Path { get; } = Directory.CreateTempSubdirectory("pkhexqt-cry-").FullName;

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
