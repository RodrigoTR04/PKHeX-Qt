using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class AccessoryEditorTests
{
    [Fact]
    public void gen6_lists_memories_and_medals()
    {
        var session = Occupied(GameVersion.X, Species.Froakie);
        session.OpenAccessory();
        Assert.Contains("memories", session.Accessory.PageIds);
        Assert.Contains("medals", session.Accessory.PageIds);
        Assert.DoesNotContain("tech", session.Accessory.PageIds);
    }

    [Fact]
    public void gen6_memory_save_writes_ot_friendship()
    {
        var session = Occupied(GameVersion.X, Species.Froakie);
        session.SelectBoxSlot(0, 0);
        var expected = new PK6(session.CurrentEntity) { OriginalTrainerFriendship = 200 };
        expected.RefreshChecksum();

        session.OpenAccessory();
        session.Accessory.Select("memories");
        session.Accessory.ApplyJson("""{"page":"memories","otFriendship":200}""");
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void gen6_medal_save_sets_first_regimen()
    {
        var session = Occupied(GameVersion.X, Species.Froakie);
        session.SelectBoxSlot(0, 0);
        var expected = new PK6(session.CurrentEntity);
        expected.SetRegimenState(0, true);
        expected.RefreshChecksum();

        session.OpenAccessory();
        session.Accessory.Select("medals");
        session.Accessory.SetFlag(0, true);
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void swsh_tech_record_save_sets_flag()
    {
        var session = Occupied(GameVersion.SW, Species.Grookey);
        session.SelectBoxSlot(0, 0);
        var expected = new PK8(session.CurrentEntity);
        expected.SetMoveRecordFlag(0, true);
        expected.RefreshChecksum();

        session.OpenAccessory();
        session.Accessory.Select("tech");
        session.Accessory.SetFlag(0, true);
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void pla_move_shop_save_sets_purchased_and_mastered()
    {
        var session = Occupied(GameVersion.PLA, Species.Rowlet);
        session.SelectBoxSlot(0, 0);
        var expected = new PA8(session.CurrentEntity);
        expected.SetPurchasedRecordFlag(0, true);
        expected.SetMasteredRecordFlag(0, true);
        expected.RefreshChecksum();

        session.OpenAccessory();
        session.Accessory.Select("shop");
        session.Accessory.SetPair(0, purchased: true, mastered: true);
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void za_plus_record_save_sets_flag()
    {
        var session = Occupied(GameVersion.ZA, Species.Sprigatito);
        session.SelectBoxSlot(0, 0);
        var expected = new PA9(session.CurrentEntity);
        expected.SetMovePlusFlag(0, true);
        expected.RefreshChecksum();

        session.OpenAccessory();
        session.Accessory.Select("plus");
        session.Accessory.SetFlag(0, true);
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void memory_cancel_leaves_entity_unchanged()
    {
        var session = Occupied(GameVersion.X, Species.Froakie);
        session.SelectBoxSlot(0, 0);
        var before = session.CurrentEntity.ToArray();
        session.OpenAccessory();
        session.Accessory.Select("memories");
        session.Accessory.ApplyJson("""{"page":"memories","otFriendship":12}""");
        session.CancelAccessory();
        Assert.Equal(before, session.CurrentEntity);
    }

    private static EditorSession Occupied(GameVersion version, Species species)
    {
        var sav = BlankSaveFile.Get(version);
        var pk = sav.BlankPKM;
        pk.Species = (ushort)species;
        pk.RefreshChecksum();
        sav.SetBoxSlotAtIndex(pk, 0, 0);
        var session = EditorSession.FromSave(sav);
        session.SelectBoxSlot(0, 0);
        return session;
    }
}
