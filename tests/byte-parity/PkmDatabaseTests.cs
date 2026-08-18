using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class PkmDatabaseTests
{
    [Fact]
    public void box_search_returns_the_same_occupied_slots_as_core_loader()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var second = sav.BlankPKM;
        second.Species = (ushort)Species.Ivysaur;
        second.SetUnshiny();
        second.RefreshChecksum();
        sav.SetBoxSlotAtIndex(second, 1, 0);

        var expected = new List<SlotCache>();
        SlotInfoLoader.AddFromSaveFile(sav, expected);
        var want = expected
            .Where(z => z.IsDataValid() && z.Source is SlotInfoBox)
            .Select(z => (((SlotInfoBox)z.Source).Box, ((SlotInfoBox)z.Source).Slot, z.Entity.Species))
            .OrderBy(z => z)
            .ToArray();

        var session = EditorSession.FromSave(sav);
        session.OpenPkmDatabase(BoxOnlyOptions());
        var got = session.SearchPkmDatabase(new PkmDatabaseQuery { SearchBoxes = true, SearchDatabase = false, SearchBackups = false })
            .Where(z => z.Kind == "box")
            .Select(z => (z.Box, z.Slot, z.Species))
            .OrderBy(z => z)
            .ToArray();

        Assert.Equal(want, got);
    }

    [Fact]
    public void species_filter_keeps_only_matching_box_hits()
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
        session.OpenPkmDatabase(BoxOnlyOptions());
        var hits = session.SearchPkmDatabase(new PkmDatabaseQuery
        {
            Species = (ushort)Species.Ivysaur,
            SearchBoxes = true,
            SearchDatabase = false,
            SearchBackups = false,
        });

        Assert.Single(hits);
        Assert.Equal((ushort)Species.Ivysaur, hits[0].Species);
        Assert.Equal(1, hits[0].Box);
        Assert.Equal(0, hits[0].Slot);
    }

    [Fact]
    public void loading_a_hit_sets_the_current_entity_like_view()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var ivy = sav.BlankPKM;
        ivy.Species = (ushort)Species.Ivysaur;
        ivy.SetUnshiny();
        ivy.RefreshChecksum();
        sav.SetBoxSlotAtIndex(ivy, 1, 0);
        ivy = sav.GetBoxSlotAtIndex(1, 0);
        ivy.ForcePartyData();

        var converted = EntityConverter.ConvertToType(ivy, sav.PKMType, out _)
            ?? throw new InvalidOperationException();
        sav.AdaptToSaveFile(converted);
        converted.RefreshChecksum();

        var session = EditorSession.FromSave(sav);
        session.SelectBoxSlot(0, 0);
        session.OpenPkmDatabase(BoxOnlyOptions());
        var hits = session.SearchPkmDatabase(new PkmDatabaseQuery
        {
            Species = (ushort)Species.Ivysaur,
            SearchBoxes = true,
            SearchDatabase = false,
            SearchBackups = false,
        });
        session.LoadPkmDatabaseHit(hits[0].Index);

        Assert.Equal(converted.Data.ToArray(), session.CurrentEntity);
        Assert.Equal(((int)Species.Ivysaur).ToString(), session.GetField("CB_Species"));
    }

    [Fact]
    public void backup_search_uses_user_config_backup_folder()
    {
        using var dir = new TempDir();
        var (currentBytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(currentBytes, out var sav))
            throw new InvalidOperationException();

        var backup = OccupiedSave.WithBoxSlot(pk => pk.Species = (ushort)Species.Charmander);
        File.WriteAllBytes(Path.Combine(dir.Path, "other.sav"), backup.Save);

        var session = EditorSession.FromSave(sav);
        session.OpenPkmDatabase(new PkmDatabaseOptions
        {
            DatabasePath = Path.Combine(dir.Path, "missing-pkmdb"),
            BackupPath = dir.Path,
            SearchBackups = true,
            SearchExtraSaves = false,
            FilterUnavailableSpecies = false,
            InitialSortMode = DatabaseSortMode.SlotIdentity,
        });
        var hits = session.SearchPkmDatabase(new PkmDatabaseQuery
        {
            SearchBoxes = false,
            SearchDatabase = false,
            SearchBackups = true,
        });

        Assert.Contains(hits, z => z.Species == (ushort)Species.Charmander);
        Assert.DoesNotContain(hits, z => z.Species == (ushort)Species.Bulbasaur);
    }

    private static PkmDatabaseOptions BoxOnlyOptions() => new()
    {
        DatabasePath = Path.Combine(Path.GetTempPath(), "pkhexqt-missing-pkmdb"),
        BackupPath = Path.Combine(Path.GetTempPath(), "pkhexqt-missing-bak"),
        SearchBackups = false,
        SearchExtraSaves = false,
        FilterUnavailableSpecies = false,
        InitialSortMode = DatabaseSortMode.SlotIdentity,
    };

    private sealed class TempDir : IDisposable
    {
        public string Path { get; } = System.IO.Path.Combine(System.IO.Path.GetTempPath(), "pkhexqt-pkmdb-" + Guid.NewGuid().ToString("N"));
        public TempDir() => Directory.CreateDirectory(Path);
        public void Dispose()
        {
            try { Directory.Delete(Path, recursive: true); }
            catch (IOException) { }
        }
    }
}
