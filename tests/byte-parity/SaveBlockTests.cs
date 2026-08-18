using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class SaveBlockTests
{
    [Fact]
    public void gen1_simple_trainer_save_matches_winforms_field_writes()
    {
        var sav = BlankSaveFile.Get(GameVersion.RD);
        var expected = sav.Clone();
        expected.OT = "ASH";
        expected.TID16 = 12345;
        expected.Money = 5000;
        var e1 = (SAV1)expected;
        e1.Badges = 3;
        e1.Coin = 100;

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("trainer");
        session.SaveBlock.ApplyJson("""{"page":"trainer","ot":"ASH","tid":12345,"money":5000,"badges":3,"coins":100}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void gen5_simple_trainer_save_matches_winforms_field_writes()
    {
        var sav = BlankSaveFile.Get(GameVersion.B);
        var expected = sav.Clone();
        expected.OT = "Hilbert";
        expected.TID16 = 22222;
        expected.SID16 = 33333;
        expected.Money = 99999;
        var e5 = (SAV5)expected;
        e5.Misc.Badges = 0xA5;
        e5.BattleSubway.BP = 50;
        e5.Country = 1;
        e5.Region = 2;

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("trainer");
        session.SaveBlock.ApplyJson(
            """{"page":"trainer","ot":"Hilbert","tid":22222,"sid":33333,"money":99999,"badges":165,"coins":50,"country":1,"region":2}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void trainer_cancel_leaves_the_save_unchanged()
    {
        var sav = BlankSaveFile.Get(GameVersion.RD);
        var session = EditorSession.FromSave(sav);
        var before = session.Export();
        session.OpenSaveBlock();
        session.SaveBlock.Select("trainer");
        session.SaveBlock.ApplyJson("""{"page":"trainer","ot":"RED"}""");
        session.CancelSaveBlock();
        Assert.Equal(before, session.Export());
    }

    [Fact]
    public void save_block_shell_accepts_another_page()
    {
        var session = EditorSession.FromSave(BlankSaveFile.Get(GameVersion.RD));
        session.OpenSaveBlock();
        session.SaveBlock.Register(new StubPage());
        Assert.Contains("trainer", session.SaveBlock.PageIds);
        Assert.Contains("stub", session.SaveBlock.PageIds);
    }

    [Fact]
    public void gen9_has_no_simple_trainer_page()
    {
        var session = EditorSession.FromSave(BlankSaveFile.Get(GameVersion.SL));
        Assert.False(session.HasSaveBlockEditor);
    }

    private sealed class StubPage : ISaveBlockPage
    {
        public string Id => "stub";
        public string LangForm => "Stub";
        public string ToJson() => "{}";
        public void ApplyJson(string json) { }
        public void Modify(string action) { }
        public void Save(SaveFile sav) { }
    }
}
