using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class SlotDropTests
{
    [Fact]
    public void dropping_an_entity_file_onto_a_box_slot_matches_core()
    {
        var (sourceSave, entity) = OccupiedSave.WithBoxSlot(pk =>
        {
            pk.Move1 = (ushort)Move.Tackle;
            pk.Nickname = "DROPTEST";
            pk.IsNicknamed = true;
        });
        var source = EditorSession.Load(sourceSave);
        source.SelectBoxSlot(0, 0);
        var payload = source.ExportEntity();

        var (destSave, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(destSave, out var sav))
            throw new InvalidOperationException();
        if (!FileUtil.TryGetPKM(payload, out var pk, ".pk5", sav))
            throw new InvalidOperationException();
        var converted = EntityConverter.ConvertToType(pk, sav.PKMType, out _) ?? pk;
        if (ReferenceEquals(pk, converted))
            sav.AdaptToSaveFile(converted);
        sav.SetBoxSlotAtIndex(converted, 0, 1);

        var session = EditorSession.Load(destSave);
        session.SelectBoxSlot(0, 0);
        session.DropEntityOnSlot(party: false, box: 0, slot: 1, payload, ".pk5");

        Assert.Equal(sav.Write().ToArray(), session.Export());
        session.SelectBoxSlot(0, 1);
        Assert.Equal("DROPTEST", session.GetField("TB_Nickname"));
    }

    [Fact]
    public void shift_set_and_alt_delete_match_core_slot_writes()
    {
        var (save, _) = OccupiedSave.WithBoxSlot(pk => pk.Nickname = "SETTABS");
        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        session.SetField("CHK_NicknamedFlag", "1");
        session.SetField("TB_Nickname", "SETTABS");
        session.WriteCurrentToSlot(party: false, box: 0, slot: 2);

        if (!SaveUtil.TryGetSaveFile(save, out var sav))
            throw new InvalidOperationException();
        var pk = sav.GetBoxSlotAtIndex(0, 0);
        pk.IsNicknamed = true;
        pk.Nickname = "SETTABS";
        pk.FixMoves();
        pk.RefreshChecksum();
        sav.SetBoxSlotAtIndex(pk, 0, 2);
        Assert.Equal(sav.Write().ToArray(), session.Export());

        session.DeleteSlot(party: false, box: 0, slot: 2);
        sav.SetBoxSlotAtIndex(sav.BlankPKM, 0, 2);
        Assert.Equal(sav.Write().ToArray(), session.Export());
    }

    [Fact]
    public void slot_hover_preview_matches_core_localized_preview()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot(pk => pk.Move1 = (ushort)Move.Tackle);
        var expected = ShowdownParsing.GetLocalizedPreviewText(entity, BattleTemplateExportSettings.Showdown);

        var session = EditorSession.Load(save);
        Assert.Equal(expected, session.SlotPreview(party: false, box: 0, slot: 0));
        Assert.Equal(string.Empty, session.SlotPreview(party: false, box: 0, slot: 1));
    }

    [Fact]
    public void swapping_two_box_slots_matches_core()
    {
        var (save, _) = OccupiedSave.WithBoxSlot(pk =>
        {
            pk.Nickname = "ALPHA";
            pk.IsNicknamed = true;
        }, box: 0, slot: 0);
        if (!SaveUtil.TryGetSaveFile(save, out var sav))
            throw new InvalidOperationException();
        var other = sav.BlankPKM;
        other.Species = (ushort)Species.Ivysaur;
        other.SetUnshiny();
        other.Nickname = "BETA";
        other.IsNicknamed = true;
        other.RefreshChecksum();
        sav.SetBoxSlotAtIndex(other, 0, 3);
        var twoSlots = sav.Write().ToArray();

        var a = sav.GetBoxSlotAtIndex(0, 0);
        var b = sav.GetBoxSlotAtIndex(0, 3);
        sav.SetBoxSlotAtIndex(b, 0, 0);
        sav.SetBoxSlotAtIndex(a, 0, 3);

        var session = EditorSession.Load(twoSlots);
        session.SwapSlots(partyA: false, boxA: 0, slotA: 0, partyB: false, boxB: 0, slotB: 3);
        Assert.Equal(sav.Write().ToArray(), session.Export());
    }
}
