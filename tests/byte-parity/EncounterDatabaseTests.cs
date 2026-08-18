using PKHeX.Core;
using PKHeX.Core.Searching;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class EncounterDatabaseTests
{
    [Fact]
    public void gen5_bulbasaur_search_matches_core_moveset_generator()
    {
        if (!SaveUtil.TryGetSaveFile(OccupiedSave.WithBoxSlot().Save, out var sav))
            throw new InvalidOperationException();
        var want = ExpectedFingerprints(sav, (ushort)Species.Bulbasaur);
        var session = EditorSession.FromSave(sav);
        session.SelectBoxSlot(0, 0);
        var hits = session.SearchEncounters(new EncounterDatabaseQuery
        {
            Species = (ushort)Species.Bulbasaur,
            UseTabsAsCriteria = false,
            FilterUnavailableSpecies = false,
        });
        Assert.Equal(want, hits.Select(z => z.Fingerprint).ToArray());
    }

    [Fact]
    public void gen1_bulbasaur_search_matches_core_moveset_generator()
    {
        var sav = BlankSaveFile.Get(GameVersion.RD);
        var want = ExpectedFingerprints(sav, (ushort)Species.Bulbasaur);
        var session = EditorSession.FromSave(sav);
        var hits = session.SearchEncounters(new EncounterDatabaseQuery
        {
            Species = (ushort)Species.Bulbasaur,
            UseTabsAsCriteria = false,
            FilterUnavailableSpecies = false,
        });
        Assert.Equal(want, hits.Select(z => z.Fingerprint).ToArray());
        Assert.NotEmpty(hits);
    }

    [Fact]
    public void taking_an_encounter_fills_the_editor_without_writing_the_save()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var session = EditorSession.FromSave(sav);
        session.SelectBoxSlot(0, 0);
        var before = session.Export();
        var hits = session.SearchEncounters(new EncounterDatabaseQuery
        {
            Species = (ushort)Species.Bulbasaur,
            UseTabsAsCriteria = false,
            FilterUnavailableSpecies = false,
        });
        Assert.NotEmpty(hits);

        session.LoadEncounter(hits[0].Index);

        Assert.Equal(before, session.Export());
        Assert.Equal(hits[0].Species.ToString(), session.GetField("CB_Species"));
    }

    private static string[] ExpectedFingerprints(SaveFile sav, ushort species)
        => ExpectedEncounters(sav, species).Select(enc => $"{enc.GetType().Name}:{enc.Species}:{enc.Form}:{enc.Version}:{enc.LevelMin}:{enc.LevelMax}").ToArray();

    private static List<IEncounterInfo> ExpectedEncounters(SaveFile sav, ushort species)
    {
        var settings = new SearchSettings
        {
            Context = sav.Context,
            Generation = sav.Generation,
            Species = species,
        };
        var versions = settings.GetVersions(sav);
        var pk = sav.BlankPKM;
        var prev = EncounterMovesetGenerator.PriorityList;
        EncounterMovesetGenerator.PriorityList = Enum.GetValues<EncounterTypeGroup>();
        try
        {
            var pi = sav.Personal.GetFormEntry(species, 0);
            var fc = pi.FormCount;
            var results = new List<IEncounterInfo>();
            for (byte f = 0; f < fc; f++)
            {
                if (FormInfo.IsBattleOnlyForm(species, f, pk.Format))
                    continue;
                pk.Species = species;
                pk.Form = f;
                pk.SetGender(pk.GetSaneGender());
                EncounterMovesetGenerator.OptimizeCriteria(pk, sav);
                results.AddRange(EncounterMovesetGenerator.GenerateEncounters(pk, ReadOnlyMemory<ushort>.Empty, versions));
            }
            return results.Distinct(new RefCmp()).ToList();
        }
        finally
        {
            EncounterMovesetGenerator.PriorityList = prev;
        }
    }

    private sealed class RefCmp : IEqualityComparer<IEncounterInfo>
    {
        public bool Equals(IEncounterInfo? x, IEncounterInfo? y) => ReferenceEquals(x, y);
        public int GetHashCode(IEncounterInfo obj) => System.Runtime.CompilerServices.RuntimeHelpers.GetHashCode(obj);
    }
}
