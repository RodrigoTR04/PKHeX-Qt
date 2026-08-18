using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class RibbonTests
{
    [Fact]
    public void gen5_save_writes_the_same_bytes_as_winforms_setvalue()
    {
        var (save, _) = OccupiedSave.WithBoxSlot();
        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        var expected = new PK5(session.CurrentEntity);
        ReflectUtil.SetValue(expected, "RibbonChampionSinnoh", true);
        expected.RefreshChecksum();

        session.OpenAccessory();
        session.Accessory.SetRibbon("RibbonChampionSinnoh", true);
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void gen9_save_writes_ribbon_and_affixed()
    {
        var session = EditorSession.FromSave(OccupiedGen9());
        session.SelectBoxSlot(0, 0);
        var expected = new PK9(session.CurrentEntity);
        ReflectUtil.SetValue(expected, "RibbonChampionKalos", true);
        expected.AffixedRibbon = 0;
        expected.RefreshChecksum();

        session.OpenAccessory();
        session.Accessory.SetRibbon("RibbonChampionKalos", true);
        session.Accessory.SetAffixed(0);
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void cancel_leaves_the_entity_and_save_unchanged()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot();
        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        var beforeExport = session.Export();
        session.OpenAccessory();
        session.Accessory.SetRibbon("RibbonChampionSinnoh", true);
        session.CancelAccessory();

        Assert.Equal(entity.Data.ToArray(), session.CurrentEntity);
        Assert.Equal(beforeExport, session.Export());
    }

    [Fact]
    public void give_all_then_save_matches_every_ribbon_maxed()
    {
        var (save, _) = OccupiedSave.WithBoxSlot();
        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        var expected = new PK5(session.CurrentEntity);
        MaxEveryRibbon(expected);
        expected.RefreshChecksum();
        session.OpenAccessory();
        session.Accessory.Modify("B_All");
        session.SaveAccessory();

        Assert.Equal(expected.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void accessory_shell_accepts_another_page()
    {
        var (save, _) = OccupiedSave.WithBoxSlot();
        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        session.OpenAccessory();
        session.Accessory.Register(new StubPage());
        Assert.Contains("stub", session.Accessory.PageIds);
        Assert.Contains("ribbons", session.Accessory.PageIds);
    }

    private static SaveFile OccupiedGen9()
    {
        var sav = GoldenSave.Gen9Blank();
        var pk = sav.BlankPKM;
        pk.Species = (ushort)Species.Sprigatito;
        pk.RefreshChecksum();
        sav.SetBoxSlotAtIndex(pk, 0, 0);
        return sav;
    }

    private static void MaxEveryRibbon(PKM pk)
    {
        foreach (var rib in RibbonInfo.GetRibbonInfo(pk))
        {
            if (rib.Type is RibbonValueType.Boolean)
                ReflectUtil.SetValue(pk, rib.Name, true);
            else
                ReflectUtil.SetValue(pk, rib.Name, (byte)rib.MaxCount);
        }
    }

    private sealed class StubPage : IAccessoryPage
    {
        public string Id => "stub";
        public string LangForm => "Stub";
        public string ToJson() => "{}";
        public void ApplyJson(string json) { }
        public void Modify(string action) { }
        public void Save(PKM pk) { }
    }
}
