using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// PKM-tab control names Original PKHeX shows for this entity
/// (<c>PKMEditor.ToggleInterface</c> / <c>StatEditor.ToggleInterface</c>).
/// </summary>
internal static class EntityChrome
{
    public static IReadOnlyList<string> VisibleNames(PKM pk, SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(pk);
        ArgumentNullException.ThrowIfNull(sav);
        var format = pk.Format;
        var names = new List<string>
        {
            "CB_Species", "Label_Species",
            "CHK_NicknamedFlag", "TB_Nickname",
            "TB_EXP", "Label_EXP", "TB_Level", "Label_CurLevel",
            "CB_Language", "Label_Language",
            "GB_CurrentMoves",
            "CB_Move1", "CB_Move2", "CB_Move3", "CB_Move4",
            "TB_PP1", "TB_PP2", "TB_PP3", "TB_PP4",
            "CB_PPUps1", "CB_PPUps2", "CB_PPUps3", "CB_PPUps4",
            "Label_CurPP", "Label_PPups",
            "GB_OT", "Label_OT", "TB_OT",
            "Label_TID", "TB_TID",
            "TB_BaseHP", "TB_IVHP", "TB_EVHP", "Stat_HP", "Label_HP",
            "TB_BaseATK", "TB_IVATK", "TB_EVATK", "Stat_ATK", "Label_ATK",
            "TB_BaseDEF", "TB_IVDEF", "TB_EVDEF", "Stat_DEF", "Label_DEF",
            "TB_BaseSPA", "TB_IVSPA", "TB_EVSPA", "Stat_SPA", "Label_SPA",
            "TB_BaseSPE", "TB_IVSPE", "TB_EVSPE", "Stat_SPE", "Label_SPE",
            "Label_Base", "Label_IVs", "Label_EVs", "Label_Stats",
            "BTN_RandomIVs", "BTN_RandomEVs",
            "TLP_StatGrid",
        };

        if (format >= 2)
        {
            names.AddRange(["UC_Gender", "PB_MarkShiny", "CHK_IsEgg", "Label_HeldItem", "CB_HeldItem"]);
            names.AddRange(["Label_Friendship", "TB_Friendship", "UC_OTGender"]);
            names.AddRange(["Label_MetLocation", "CB_MetLocation", "Label_MetLevel", "TB_MetLevel"]);
            names.AddRange(["Label_PKRS", "Label_PKRSdays", "CB_PKRSStrain", "CB_PKRSDays", "CHK_Infected", "CHK_Cured"]);
            names.Add("Tab_Met");
        }

        if (format >= 3)
        {
            names.AddRange(["Label_PID", "TB_PID", "BTN_RerollPID"]);
            names.AddRange(["Label_Nature", "CB_Nature", "Label_Ability", "CB_Ability"]);
            names.AddRange(["Label_Form", "CB_Form"]);
            names.AddRange(["Label_OriginGame", "CB_GameOrigin", "Label_Ball", "CB_Ball", "CHK_Fateful"]);
            names.AddRange(["Label_SID", "TB_SID"]);
            names.AddRange(["L_Characteristic"]);
            names.Add("Tab_Cosmetic");
        }

        if (format != 1)
            names.AddRange(["TB_BaseSPD", "TB_IVSPD", "TB_EVSPD", "Stat_SPD", "Label_SPD"]);

        if (format >= 4)
        {
            names.AddRange(["Label_MetDate", "CAL_MetDate", "CHK_AsEgg", "GB_EggConditions"]);
            names.AddRange(["Label_EggLocation", "CB_EggLocation", "Label_EggDate", "CAL_EggDate"]);
        }

        if (format is 4 or 5 or 6)
            names.AddRange(["Label_GroundTile", "CB_GroundTile"]);

        if (format == 5)
            names.AddRange(["CHK_NSparkle", "NUD_PokeStarFame"]);

        if (format <= 7 || pk is PB8)
            names.AddRange(["Label_HiddenPowerPrefix", "CB_HPType"]);
        if (format <= 5)
            names.Add("Label_HiddenPowerPower");

        if (pk is IContestStatsReadOnly)
        {
            names.AddRange([
                "Label_ContestStats",
                "Label_Cool", "TB_Cool",
                "Label_Beauty", "TB_Beauty",
                "Label_Cute", "TB_Cute",
                "Label_Smart", "TB_Smart",
                "Label_Tough", "TB_Tough",
                "Label_Sheen", "TB_Sheen",
            ]);
        }

        if (HaXHeldItemHidden(sav, pk))
        {
            names.Remove("Label_HeldItem");
            names.Remove("CB_HeldItem");
        }

        return names;
    }

    private static bool HaXHeldItemHidden(SaveFile sav, PKM pk)
        => sav is SAV7b or SAV8LA || pk is PB7 or PA8;
}
