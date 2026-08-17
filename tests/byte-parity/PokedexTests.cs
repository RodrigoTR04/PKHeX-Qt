using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class PokedexTests
{
    [Fact]
    public void cancel_leaves_the_save_unchanged()
    {
        var fixture = GoldenSave.Gen5BlankExport();
        var session = EditorSession.Load(fixture);
        session.OpenPokedex();
        session.Pokedex.SelectSpecies(25);
        session.Pokedex.Bulk("mnuSeenNone");
        session.CancelPokedex();

        Assert.Equal(fixture, session.Export());
    }

    [Fact]
    public void gen5_bulk_seen_none_save_writes_initial_species()
    {
        var fixture = GoldenSave.Gen5BlankExport();
        var session = EditorSession.Load(fixture);
        session.OpenPokedex();
        session.Pokedex.SelectSpecies(25);
        session.Pokedex.Bulk("mnuSeenNone");
        session.SavePokedex();

        Assert.Equal(ExpectedGen5SeenNone(fixture, 25), session.Export());
    }

    [Fact]
    public void gen7_seen_none_clears_form_bits_core_seen_none_does_not()
    {
        var sav = GoldenSave.Gen7Blank();
        var dex = ((SAV7SM)sav).Zukan;
        dex.SetCaught(25, true);
        dex.SetSeen(25, 0, true);
        var formSpecies = (ushort)(sav.MaxSpeciesID + 1);
        dex.SetSeen(formSpecies, 0, true);

        var coreOnly = GoldenSave.Gen7Blank();
        ((SAV7SM)coreOnly).Zukan.SetCaught(25, true);
        ((SAV7SM)coreOnly).Zukan.SetSeen(25, 0, true);
        ((SAV7SM)coreOnly).Zukan.SetSeen(formSpecies, 0, true);
        ((SAV7SM)coreOnly).Zukan.SeenNone();
        Assert.True(((SAV7SM)coreOnly).Zukan.GetSeen(formSpecies, 0));

        var session = EditorSession.FromSave(sav);
        session.OpenPokedex();
        session.Pokedex.Bulk("mnuSeenNone");
        session.SavePokedex();

        var after = ((SAV7SM)sav).Zukan;
        Assert.False(after.GetCaught(25));
        Assert.False(after.GetSeen(25, 0));
        Assert.False(after.GetSeen(formSpecies, 0));
        Assert.Equal(ExpectedGen7SeenNoneUi(GoldenSave.Gen7Blank(), 25, formSpecies).Write().ToArray(), sav.Write().ToArray());
    }

    private static byte[] ExpectedGen5SeenNone(byte[] fixture, ushort species)
    {
        if (!SaveUtil.TryGetSaveFile(fixture, out var sav))
            throw new InvalidOperationException("Fixture was not a save.");
        var clone = sav.Clone();
        var dex = ((SAV5)clone).Zukan;
        dex.SeenNone();
        dex.InitialSpecies = species;
        sav.CopyChangesFrom(clone);
        return sav.Write().ToArray();
    }

    private static SaveFile ExpectedGen7SeenNoneUi(SaveFile blank, ushort species, ushort formSpecies)
    {
        var sav = blank;
        var dex = ((SAV7SM)sav).Zukan;
        dex.SetCaught(species, true);
        dex.SetSeen(species, 0, true);
        dex.SetSeen(formSpecies, 0, true);
        var names = dex.GetEntryNames(GameInfo.Strings.Species);
        for (var i = 0; i < names.Count; i++)
        {
            var id = (ushort)(i + 1);
            if (id <= sav.MaxSpeciesID)
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
        return sav;
    }
}
