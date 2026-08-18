using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class BoxExportTests
{
    [Fact]
    public void current_box_export_skips_empty_slots_and_matches_party_payload()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        sav.CurrentBox = 0;
        var entity = sav.GetBoxSlotAtIndex(0, 0);
        var session = EditorSession.FromSave(sav);
        using var dir = new TempDir();
        var settings = BoxExportSettings.Default with { Scope = BoxExportScope.Current };
        var count = session.ExportBoxes(dir.Path, settings);

        Assert.Equal(1, count);
        var files = Directory.GetFiles(dir.Path);
        Assert.Single(files);
        Assert.Equal(ExpectedName(entity), Path.GetFileName(files[0]));
        Assert.Equal(ExpectedPayload(sav, entity), File.ReadAllBytes(files[0]));
    }

    [Fact]
    public void all_boxes_export_includes_every_occupied_slot()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var second = sav.BlankPKM;
        second.Species = (ushort)Species.Ivysaur;
        second.SetUnshiny();
        second.RefreshChecksum();
        sav.SetBoxSlotAtIndex(second, 1, 0);

        var session = EditorSession.FromSave(sav);
        using var dir = new TempDir();
        var count = session.ExportBoxes(dir.Path, BoxExportSettings.Default with { Scope = BoxExportScope.All });

        Assert.Equal(2, count);
        Assert.Equal(2, Directory.GetFiles(dir.Path).Length);
    }

    [Fact]
    public void include_empty_slots_writes_one_file_per_box_slot()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var session = EditorSession.FromSave(sav);
        using var dir = new TempDir();
        var settings = BoxExportSettings.Default with
        {
            Scope = BoxExportScope.Current,
            EmptySlots = BoxExportEmptySlots.Include,
            FileIndexPrefix = BoxExportIndexPrefix.InBox,
        };
        var count = session.ExportBoxes(dir.Path, settings);
        Assert.Equal(sav.BoxSlotCount, count);
        Assert.Equal(sav.BoxSlotCount, Directory.GetFiles(dir.Path).Length);
    }

    [Fact]
    public void gen1_current_box_uses_the_same_namer_and_payload()
    {
        var sav = BlankSaveFile.Get(GameVersion.RD);
        var pk = sav.BlankPKM;
        pk.Species = (ushort)Species.Bulbasaur;
        sav.SetBoxSlotAtIndex(pk, 0, 0);
        pk = sav.GetBoxSlotAtIndex(0, 0);

        var session = EditorSession.FromSave(sav);
        using var dir = new TempDir();
        var count = session.ExportBoxes(dir.Path, BoxExportSettings.Default with { Scope = BoxExportScope.Current });

        Assert.Equal(1, count);
        var files = Directory.GetFiles(dir.Path);
        Assert.Equal(ExpectedName(pk), Path.GetFileName(files[0]));
        Assert.Equal(ExpectedPayload(sav, pk), File.ReadAllBytes(files[0]));
    }

    private static string ExpectedName(PKM pk)
        => PathUtil.CleanFileName(EntityFileNamer.GetName(pk)) + "." + pk.Extension;

    private static byte[] ExpectedPayload(SaveFile sav, PKM pk)
    {
        var copy = pk.Clone();
        if (sav.SIZE_BOXSLOT != sav.SIZE_PARTY)
            copy.ForcePartyData();
        var data = new byte[sav.SIZE_PARTY];
        copy.WriteDecryptedDataParty(data);
        return data;
    }

    private sealed class TempDir : IDisposable
    {
        public string Path { get; } = System.IO.Path.Combine(System.IO.Path.GetTempPath(), "pkhexqt-boxexport-" + Guid.NewGuid().ToString("N"));
        public TempDir() => Directory.CreateDirectory(Path);
        public void Dispose()
        {
            try { Directory.Delete(Path, recursive: true); }
            catch (IOException) { }
        }
    }
}
