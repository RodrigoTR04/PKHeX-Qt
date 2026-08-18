using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class EventFlagsTests
{
    [Fact]
    public void gen2_flag_save_matches_event_workspace()
    {
        var sav = BlankSaveFile.Get(GameVersion.C);
        var expected = sav.Clone();
        var want = new EventWorkspace<SAV2, byte>((SAV2)expected, expected.Version);
        want.Flags[0] = true;
        want.Save();

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("flags");
        session.SaveBlock.ApplyJson("""{"page":"flags","setFlags":[{"index":0,"value":true}]}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void gen5_flag_and_work_save_matches_event_workspace()
    {
        var sav = BlankSaveFile.Get(GameVersion.B);
        var expected = sav.Clone();
        var want = new EventWorkspace<IEventFlag37, ushort>(((SAV5)expected).EventWork, expected.Version);
        want.Flags[0] = true;
        want.Values[0] = 7;
        want.Save();

        var session = EditorSession.FromSave(sav);
        session.OpenSaveBlock();
        session.SaveBlock.Select("flags");
        session.SaveBlock.ApplyJson("""{"page":"flags","setFlags":[{"index":0,"value":true}],"setWork":[{"index":0,"value":7}]}""");
        session.SaveSaveBlock();

        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void flag_cancel_leaves_the_save_unchanged()
    {
        var sav = BlankSaveFile.Get(GameVersion.C);
        var session = EditorSession.FromSave(sav);
        var before = session.Export();
        session.OpenSaveBlock();
        session.SaveBlock.Select("flags");
        session.SaveBlock.ApplyJson("""{"page":"flags","setFlags":[{"index":0,"value":true}]}""");
        session.CancelSaveBlock();
        Assert.Equal(before, session.Export());
    }

    [Fact]
    public void gen5_lists_trainer_and_flags_pages()
    {
        var session = EditorSession.FromSave(BlankSaveFile.Get(GameVersion.B));
        Assert.Contains("trainer", session.SaveBlockPages().Split('\n'));
        Assert.Contains("flags", session.SaveBlockPages().Split('\n'));
    }
}
