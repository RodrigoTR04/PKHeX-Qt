using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class RemainingPokedexTests
{
    [Fact]
    public void gen4_seen_none_matches_modify_all_none()
    {
        Run(GameVersion.D, sav =>
        {
            var lang = Zukan4.GetGen4LanguageBitIndex(sav.Language);
            for (ushort i = 1; i <= 493; i++)
                ((SAV4)sav).Dex.ModifyAll(i, Zukan4.SetDexArgs.None, lang);
        });
    }

    [Fact]
    public void gen6_xy_seen_none_writes_initial_species()
    {
        Run(GameVersion.X, sav =>
        {
            var dex = ((SAV6XY)sav).Zukan;
            dex.SeenNone();
            dex.InitialSpecies = 25;
        }, select: 25);
    }

    [Fact]
    public void gen6_oras_seen_none_writes_initial_species()
    {
        Run(GameVersion.OR, sav =>
        {
            var dex = ((SAV6AO)sav).Zukan;
            dex.SeenNone();
            dex.InitialSpecies = 25;
        }, select: 25);
    }

    [Fact]
    public void gen7b_seen_none_clears_form_rows_and_size_records()
    {
        var sav = BlankSaveFile.Get(GameVersion.GP);
        var dex = ((SAV7b)sav).Zukan;
        dex.SetCaught(25, true);
        dex.SetSeen(25, 0, true);
        dex.SetSizeData(DexSizeType.MinHeight, 25, 0, 10, 20, true);

        var expected = BlankSaveFile.Get(GameVersion.GP);
        ClearGen7Entries(((SAV7b)expected).Zukan, expected.MaxSpeciesID);
        ResetGgSizeRecords(((SAV7b)expected).Zukan, expected.MaxSpeciesID);

        var session = EditorSession.FromSave(sav);
        session.OpenPokedex();
        session.Pokedex.Bulk("mnuSeenNone");
        session.SavePokedex();

        Assert.False(((SAV7b)sav).Zukan.GetCaught(25));
        Assert.False(((SAV7b)sav).Zukan.GetSizeData(DexSizeType.MinHeight, 25, 0, out _, out _, out var flagged) && flagged);
        Assert.Equal(expected.Data.ToArray(), sav.Data.ToArray());
    }

    [Fact]
    public void swsh_seen_none_matches_core()
    {
        Run(GameVersion.SW, sav => ((SAV8SWSH)sav).Zukan.SeenNone());
    }

    [Fact]
    public void bdsp_seen_none_uses_set_all_seen_false()
    {
        Run(GameVersion.BD, sav => ((SAV8BS)sav).Zukan.SetAllSeen(false));
    }

    [Fact]
    public void la_pokedex_is_reachable_and_cancel_restores()
    {
        var sav = BlankSaveFile.Get(GameVersion.PLA);
        var before = sav.Data.ToArray();
        var session = EditorSession.FromSave(sav);
        Assert.True(session.HasPokedexEditor);
        session.OpenPokedex();
        session.CancelPokedex();
        Assert.Equal(before, sav.Data.ToArray());
    }

    [Fact]
    public void sv_seen_none_matches_core()
    {
        Run(GameVersion.SL, sav => ((SAV9SV)sav).Zukan.SeenNone());
    }

    [Fact]
    public void za_seen_none_matches_core()
    {
        Run(GameVersion.ZA, sav => ((SAV9ZA)sav).Zukan.SeenNone());
    }

    [Fact]
    public void sv_blank_uses_kitakami_form_string()
    {
        var session = EditorSession.FromSave(BlankSaveFile.Get(GameVersion.SL));
        session.OpenPokedex();
        Assert.Contains("SAV_PokedexSVKitakami", session.Pokedex.ToJson(), StringComparison.Ordinal);
    }

    private static void Run(GameVersion version, Action<SaveFile> applyExpected, ushort select = 0)
    {
        var sav = BlankSaveFile.Get(version);
        var expected = BlankSaveFile.Get(version);
        applyExpected(expected);
        var session = EditorSession.FromSave(sav);
        Assert.True(session.HasPokedexEditor);
        session.OpenPokedex();
        if (select != 0)
            session.Pokedex.SelectSpecies(select);
        session.Pokedex.Bulk("mnuSeenNone");
        session.SavePokedex();
        Assert.Equal(expected.Data.ToArray(), sav.Data.ToArray());
    }

    private static void ClearGen7Entries(Zukan7 dex, int maxSpecies)
    {
        var names = dex.GetEntryNames(GameInfo.Strings.Species);
        for (var i = 0; i < names.Count; i++)
        {
            var id = (ushort)(i + 1);
            if (id <= maxSpecies)
            {
                dex.SetCaught(id, false);
                for (var lang = 0; lang < 9; lang++)
                    dex.SetLanguageFlag(i, lang, false);
            }
            for (var region = 0; region < 4; region++)
            {
                dex.SetSeen(id, region, false);
                dex.SetDisplayed(i, region, false);
            }
        }
    }

    private static void ResetGgSizeRecords(Zukan7b dex, int maxSpecies)
    {
        for (ushort species = 1; species <= maxSpecies; species++)
        {
            if (!Zukan7b.TryGetSizeEntryIndex(species, 0, out var index))
                continue;
            foreach (DexSizeType group in Enum.GetValues<DexSizeType>())
                dex.SetSizeData(group, index, Zukan7b.DefaultEntryValueH, Zukan7b.DefaultEntryValueW);
        }
    }
}
