using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class BatchEditorTests
{
    private const string NicknameBoxes = """
        =Species=1
        .Nickname=TESTNAME
        """;

    [Fact]
    public void gen5_box_nickname_matches_core_slot_write()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var expected))
            throw new InvalidOperationException();
        var want = expected.GetBoxSlotAtIndex(0, 0);
        want.Nickname = "TESTNAME";
        want.RefreshChecksum();
        expected.SetBoxSlotAtIndex(want, 0, 0);

        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var session = EditorSession.FromSave(sav);
        var result = session.RunBatch("boxes", NicknameBoxes);

        Assert.True(result.Ok);
        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void gen5_party_nickname_matches_core_slot_write()
    {
        var (bytes, _) = OccupiedSave.WithPartySlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var expected))
            throw new InvalidOperationException();
        var want = expected.GetPartySlotAtIndex(0);
        want.Nickname = "TESTNAME";
        want.RefreshChecksum();
        expected.SetPartySlotAtIndex(want, 0);

        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var session = EditorSession.FromSave(sav);
        var result = session.RunBatch("party", NicknameBoxes);

        Assert.True(result.Ok);
        Assert.Equal(expected.Write().ToArray(), session.Export());
    }

    [Fact]
    public void gen1_box_nickname_uses_the_same_instruction_language()
    {
        var sav = OccupiedGen1();
        var want = sav.GetBoxSlotAtIndex(0, 0).Clone();
        want.Nickname = "TESTNAME";

        var session = EditorSession.FromSave(sav);
        var result = session.RunBatch("boxes", NicknameBoxes);

        Assert.True(result.Ok);
        Assert.Equal("TESTNAME", sav.GetBoxSlotAtIndex(0, 0).Nickname);
        Assert.Equal(want.Data.ToArray(), sav.GetBoxSlotAtIndex(0, 0).Data.ToArray());
    }

    [Fact]
    public void empty_line_does_not_write()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var session = EditorSession.FromSave(sav);
        var before = session.Export();
        var result = session.RunBatch("boxes", NicknameBoxes + "\n\n.OT_Name=ASH");
        Assert.False(result.Ok);
        Assert.Equal(MessageStrings.MsgBEInstructionInvalid, result.Message);
        Assert.Equal(before, session.Export());
    }

    [Fact]
    public void missing_instructions_do_not_write()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var session = EditorSession.FromSave(sav);
        var before = session.Export();
        var result = session.RunBatch("boxes", "=Species=1");
        Assert.False(result.Ok);
        Assert.Equal(MessageStrings.MsgBEInstructionNone, result.Message);
        Assert.Equal(before, session.Export());
    }

    [Fact]
    public void empty_filter_does_not_write()
    {
        var (bytes, _) = OccupiedSave.WithBoxSlot();
        if (!SaveUtil.TryGetSaveFile(bytes, out var sav))
            throw new InvalidOperationException();
        var session = EditorSession.FromSave(sav);
        var before = session.Export();
        var result = session.RunBatch("boxes", "=Species=\n.Nickname=TESTNAME");
        Assert.False(result.Ok);
        Assert.Equal(MessageStrings.MsgBEFilterEmpty, result.Message);
        Assert.Equal(before, session.Export());
    }

    private static SaveFile OccupiedGen1()
    {
        var sav = BlankSaveFile.Get(GameVersion.RD);
        var pk = sav.BlankPKM;
        pk.Species = (ushort)Species.Bulbasaur;
        sav.SetBoxSlotAtIndex(pk, 0, 0);
        return sav;
    }
}
