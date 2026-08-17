using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class ShowdownClipboardTests
{
    [Fact]
    public void showdown_export_matches_core_for_the_current_entity()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot(pk =>
        {
            pk.Move1 = (ushort)Move.Tackle;
            pk.CurrentLevel = 50;
        });
        var expected = ShowdownParsing.GetShowdownText(entity);

        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);

        Assert.Equal(expected, session.ExportShowdown());
        Assert.False(string.IsNullOrWhiteSpace(expected));
    }

    [Fact]
    public void showdown_import_applies_the_same_set_details_as_core()
    {
        const string text = """
            Venusaur @ Leftovers
            Ability: Overgrow
            Level: 50
            EVs: 252 HP / 4 Def / 252 SpA
            Modest Nature
            - Giga Drain
            - Sludge Bomb
            - Sleep Powder
            - Synthesis
            """;

        var (save, entity) = OccupiedSave.WithBoxSlot();
        var expected = entity.Clone();
        var set = BattleTemplateTeams.TryGetSets(text).First();
        expected.ApplySetDetails(set);

        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        session.ImportShowdown(text);

        Assert.Equal(ShowdownParsing.GetShowdownText(expected), session.ExportShowdown());
        Assert.Equal("3", session.GetField("CB_Species"));
    }

    [Fact]
    public void party_and_box_showdown_export_match_core_lists()
    {
        var (save, _) = OccupiedSave.WithPartySlot(pk => pk.Move1 = (ushort)Move.Tackle, slot: 0);
        if (!SaveUtil.TryGetSaveFile(save, out var sav))
            throw new InvalidOperationException();
        sav.SetBoxSlotAtIndex(sav.GetPartySlotAtIndex(0), 0, 1);

        var partyExpected = ShowdownParsing.GetShowdownSets(sav.PartyData, Environment.NewLine + Environment.NewLine);
        var boxExpected = ShowdownParsing.GetShowdownSets(sav.GetBoxData(0), Environment.NewLine + Environment.NewLine);

        var session = EditorSession.Load(sav.Write().ToArray());
        session.SelectPartySlot(0);

        Assert.Equal(partyExpected, session.ExportPartyShowdown());
        Assert.Equal(boxExpected, session.ExportCurrentBoxShowdown());
    }

    [Fact]
    public void entity_clipboard_bytes_round_trip_like_a_pk5_file()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot(pk =>
        {
            pk.Move1 = (ushort)Move.Tackle;
            pk.Nickname = "CLIPTEST";
            pk.IsNicknamed = true;
        });
        var expected = new byte[entity.SIZE_PARTY];
        entity.WriteDecryptedDataParty(expected);

        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        var copied = session.ExportEntity();
        Assert.Equal(expected, copied);
        Assert.EndsWith(".pk5", session.EntityFileName, StringComparison.OrdinalIgnoreCase);

        var blank = OccupiedSave.WithBoxSlot();
        var paste = EditorSession.Load(blank.Save);
        paste.SelectBoxSlot(0, 0);
        paste.ImportEntity(copied, ".pk5");
        paste.CommitCurrent();

        if (!SaveUtil.TryGetSaveFile(blank.Save, out var sav))
            throw new InvalidOperationException();
        if (!FileUtil.TryGetPKM(copied, out var pk, ".pk5", sav))
            throw new InvalidOperationException();
        var converted = EntityConverter.ConvertToType(pk, sav.PKMType, out _) ?? pk;
        if (ReferenceEquals(pk, converted))
            sav.AdaptToSaveFile(converted);
        sav.SetBoxSlotAtIndex(converted, 0, 0);

        Assert.Equal(sav.Write().ToArray(), paste.Export());
        Assert.Equal("CLIPTEST", paste.GetField("TB_Nickname"));
    }
}
