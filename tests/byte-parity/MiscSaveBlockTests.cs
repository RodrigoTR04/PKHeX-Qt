using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class MiscSaveBlockTests
{
    [Fact]
    public void crystal_lists_misc_without_opening_the_editor()
    {
        var session = EditorSession.FromSave(BlankSaveFile.Get(GameVersion.C));
        Assert.Contains("misc", session.SaveBlockPages().Split('\n'));
    }

    [Fact]
    public void crystal_gs_ball_matches_core_enable()
    {
        var sav = BlankSaveFile.Get(GameVersion.C);
        var expected = sav.Clone();
        ((SAV2)expected).EnableGSBallMobileEvent();

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("misc");
        session.SaveBlock.Modify("B_VirtualConsoleGSBall");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void gen3_misc_coins_match_winforms_save()
    {
        var sav = BlankSaveFile.Get(GameVersion.E);
        var expected = BlankSaveFile.Get(GameVersion.E);
        ((SAV3)expected).Coin = 1234;

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("misc");
        session.SaveBlock.ApplyJson("""{"page":"misc","fields":[{"name":"coins","value":1234}]}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Data.ToArray(), sav.Data.ToArray());
    }

    [Fact]
    public void gen4_misc_coins_match_winforms_save()
    {
        var sav = BlankSaveFile.Get(GameVersion.D);
        var expected = BlankSaveFile.Get(GameVersion.D);
        ((SAV4)expected).Coin = 4321;

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("misc");
        session.SaveBlock.ApplyJson("""{"page":"misc","fields":[{"name":"coins","value":4321}]}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Data.ToArray(), sav.Data.ToArray());
    }

    [Fact]
    public void gen5_record16_save_reencrypts_like_winforms()
    {
        var sav = BlankSaveFile.Get(GameVersion.B);
        var expected = sav.Clone();
        var records = ((SAV5)expected).Records;
        records.SetRecord16(0, 123);
        records.EndAccess();

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("misc");
        session.SaveBlock.ApplyJson("""{"page":"misc","fields":[{"name":"record16","value":123}]}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void bdsp_spiritomb_unlock_matches_event_unlocker()
    {
        var sav = BlankSaveFile.Get(GameVersion.BD);
        var expected = sav.Clone();
        new EventUnlocker8b((SAV8BS)expected).UnlockSpiritomb();

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("misc");
        session.SaveBlock.Modify("B_Spiritomb");
        session.SaveSaveBlock();

        Assert.Equal(expected.Data.ToArray(), sav.Data.ToArray());
    }

    [Fact]
    public void xy_opower_points_match_winforms_save()
    {
        var sav = BlankSaveFile.Get(GameVersion.X);
        var expected = sav.Clone();
        ((SAV6XY)expected).OPower.Points = 77;

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("opowers");
        session.SaveBlock.ApplyJson("""{"page":"opowers","fields":[{"name":"points","value":77}]}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void xy_pokepuff_slot_matches_core_write()
    {
        var sav = BlankSaveFile.Get(GameVersion.X);
        var expected = sav.Clone();
        ((SAV6XY)expected).Puff.GetPuffs()[0] = 9;

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("pokepuffs");
        session.SaveBlock.ApplyJson("""{"page":"pokepuffs","fields":[{"name":"puff0","value":9}]}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void sv_fashion_unlock_base_matches_core()
    {
        var sav = BlankSaveFile.Get(GameVersion.SL);
        var expected = sav.Clone();
        var want = (SAV9SV)expected;
        PlayerFashionUnlock9.UnlockBase(want.Blocks, want.Gender);

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("fashion");
        session.SaveBlock.Modify("B_SetAllOwned");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void misc_cancel_leaves_the_save_unchanged()
    {
        var sav = BlankSaveFile.Get(GameVersion.E);
        var session = EditorSession.FromSave(sav);
        var before = sav.Data.ToArray();
        session.OpenSaveBlock();
        session.SaveBlock.Select("misc");
        session.SaveBlock.ApplyJson("""{"page":"misc","fields":[{"name":"coins","value":9}]}""");
        session.CancelSaveBlock();
        Assert.Equal(before, sav.Data.ToArray());
    }

    [Fact]
    public void gen5_record_cancel_leaves_crypto_intact()
    {
        var sav = BlankSaveFile.Get(GameVersion.B);
        var session = EditorSession.FromSave(sav);
        var before = session.Export();
        session.OpenSaveBlock();
        session.SaveBlock.Select("misc");
        session.SaveBlock.ApplyJson("""{"page":"misc","fields":[{"name":"record16","value":99}]}""");
        session.CancelSaveBlock();
        Assert.Equal(before, session.Export());
    }
}
