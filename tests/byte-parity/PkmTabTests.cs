using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class PkmTabTests
{
    [Fact]
    public void committing_nickname_matches_core_mutation_of_the_same_slot()
    {
        var (save, _) = OccupiedSave.WithBoxSlot();
        const string nickname = "TESTNAME";

        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        session.SetField("CHK_NicknamedFlag", "1");
        session.SetField("TB_Nickname", nickname);
        session.CommitCurrent();

        if (!SaveUtil.TryGetSaveFile(save, out var sav))
            throw new InvalidOperationException();
        var pk = sav.GetBoxSlotAtIndex(0, 0);
        pk.IsNicknamed = true;
        pk.Nickname = nickname;
        pk.RefreshChecksum();
        sav.SetBoxSlotAtIndex(pk, 0, 0);

        Assert.Equal(sav.Write().ToArray(), session.Export());
        Assert.Equal(nickname, session.GetField("TB_Nickname"));
    }

    [Fact]
    public void legality_report_matches_core_english_report_for_the_current_entity()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(save, out var sav))
            throw new InvalidOperationException();
        var expected = new LegalityAnalysis(entity, sav.Personal).Report("en", verbose: false);

        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);

        Assert.Equal(expected, session.LegalityReport(verbose: false));
        Assert.Equal(new LegalityAnalysis(entity, sav.Personal).Valid, session.LegalityValid);
    }

    [Fact]
    public void selecting_a_slot_exposes_species_on_the_main_tab_field()
    {
        var (save, _) = OccupiedSave.WithBoxSlot();
        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);

        Assert.Equal("1", session.GetField("CB_Species"));
    }
}
