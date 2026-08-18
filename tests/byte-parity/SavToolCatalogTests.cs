using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class SavToolCatalogTests
{
    private static string[] Buttons(GameVersion version)
        => EditorSession.FromSave(BlankSaveFile.Get(version)).VisibleSavButtons()
            .Split('\n', StringSplitOptions.RemoveEmptyEntries);

    [Fact]
    public void gen1_keeps_trainer_items_dex_and_hides_later_tools()
    {
        var ids = Buttons(GameVersion.RD);
        Assert.Contains("B_OpenTrainerInfo", ids);
        Assert.Contains("B_OpenItemPouch", ids);
        Assert.Contains("B_OpenPokedex", ids);
        Assert.Contains("B_OpenEventFlags", ids);
        Assert.Contains("B_OpenHallofFame", ids);
        Assert.DoesNotContain("B_OpenWondercards", ids);
        Assert.DoesNotContain("B_OpenFashion", ids);
        Assert.DoesNotContain("B_Raids", ids);
        Assert.DoesNotContain("B_OpenBerryField", ids);
        Assert.DoesNotContain("B_OpenBoxLayout", ids);
    }

    [Fact]
    public void xy_shows_berry_field_and_hides_sv_fashion()
    {
        var ids = Buttons(GameVersion.X);
        Assert.Contains("B_OpenBerryField", ids);
        Assert.Contains("B_OpenFriendSafari", ids);
        Assert.Contains("B_OpenPokepuffs", ids);
        Assert.Contains("B_OpenOPowers", ids);
        Assert.Contains("B_Roamer", ids);
        Assert.DoesNotContain("B_OpenFashion", ids);
        Assert.DoesNotContain("B_OpenPokeblocks", ids);
    }

    [Fact]
    public void sv_shows_raids_and_fashion()
    {
        var ids = Buttons(GameVersion.SL);
        Assert.Contains("B_Raids", ids);
        Assert.Contains("B_RaidsSevenStar", ids);
        Assert.Contains("B_OpenFashion", ids);
        Assert.DoesNotContain("B_OpenPokepuffs", ids);
        Assert.DoesNotContain("B_OpenBerryField", ids);
    }
}

public sealed class EntityChromeTests
{
    private static string[] Chrome(GameVersion version)
        => EditorSession.FromSave(BlankSaveFile.Get(version)).VisiblePkmControls()
            .Split('\n', StringSplitOptions.RemoveEmptyEntries);

    [Fact]
    public void gen1_hides_pid_nature_and_met_tab()
    {
        var names = Chrome(GameVersion.RD);
        Assert.Contains("CB_Species", names);
        Assert.Contains("TB_Nickname", names);
        Assert.DoesNotContain("TB_PID", names);
        Assert.DoesNotContain("CB_Nature", names);
        Assert.DoesNotContain("Tab_Met", names);
        Assert.DoesNotContain("Tab_Cosmetic", names);
        Assert.DoesNotContain("Label_SPD", names);
    }

    [Fact]
    public void gen5_shows_pid_nsparkle_and_met()
    {
        var names = Chrome(GameVersion.B);
        Assert.Contains("TB_PID", names);
        Assert.Contains("CB_Nature", names);
        Assert.Contains("CHK_NSparkle", names);
        Assert.Contains("Tab_Met", names);
        Assert.Contains("Tab_Cosmetic", names);
        Assert.Contains("NUD_PokeStarFame", names);
        Assert.DoesNotContain("B_PlusRecord", names);
    }
}
