using System.Globalization;
using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Maps Original PKHeX PKM-tab control names to the current entity.
/// </summary>
internal static class EntityFields
{
    public static string Get(PKM pk, SaveFile sav, string name)
    {
        return name switch
        {
            "TB_Nickname" => pk.Nickname,
            "CHK_NicknamedFlag" => Flag(pk.IsNicknamed),
            "CHK_Nicknamed" => Flag(pk.IsNicknamed),
            "CB_Species" => pk.Species.ToString(CultureInfo.InvariantCulture),
            "CB_Form" => pk.Form.ToString(CultureInfo.InvariantCulture),
            "TB_EXP" => pk.EXP.ToString(CultureInfo.InvariantCulture),
            "TB_Level" => pk.CurrentLevel.ToString(CultureInfo.InvariantCulture),
            "CB_Nature" => ((int)pk.Nature).ToString(CultureInfo.InvariantCulture),
            "CB_HeldItem" => pk.HeldItem.ToString(CultureInfo.InvariantCulture),
            "CB_Ability" => pk.Ability.ToString(CultureInfo.InvariantCulture),
            "CB_Language" => pk.Language.ToString(CultureInfo.InvariantCulture),
            "CHK_IsEgg" => Flag(pk.IsEgg),
            "CHK_Infected" => Flag(pk.IsPokerusInfected),
            "CHK_Cured" => Flag(pk.IsPokerusCured),
            "CB_PKRSStrain" => pk.PokerusStrain.ToString(CultureInfo.InvariantCulture),
            "CB_PKRSDays" => pk.PokerusDays.ToString(CultureInfo.InvariantCulture),
            "CHK_NSparkle" => Flag(pk is PK5 pk5n && pk5n.NSparkle),
            "TB_PID" => pk.PID.ToString("X8", CultureInfo.InvariantCulture),
            "UC_Gender" => pk.Gender.ToString(CultureInfo.InvariantCulture),
            "PB_MarkShiny" => Flag(pk.IsShiny),
            "CB_GameOrigin" => ((int)pk.Version).ToString(CultureInfo.InvariantCulture),
            "CB_MetLocation" => pk.MetLocation.ToString(CultureInfo.InvariantCulture),
            "CB_Ball" => pk.Ball.ToString(CultureInfo.InvariantCulture),
            "TB_MetLevel" => pk.MetLevel.ToString(CultureInfo.InvariantCulture),
            "CHK_Fateful" => Flag(pk.FatefulEncounter),
            "CAL_MetDate" => FormatDate(pk.MetDate),
            "CHK_AsEgg" => Flag(EncounterStateUtil.IsMetAsEgg(pk)),
            "CB_EggLocation" => pk.EggLocation.ToString(CultureInfo.InvariantCulture),
            "CAL_EggDate" => FormatDate(pk.EggMetDate),
            "CB_GroundTile" => pk is IGroundTile gt ? ((int)gt.GroundTile).ToString(CultureInfo.InvariantCulture) : "0",
            "TB_OT" => pk.OriginalTrainerName,
            "UC_OTGender" => pk.OriginalTrainerGender.ToString(CultureInfo.InvariantCulture),
            "TB_TID" => pk.TID16.ToString(CultureInfo.InvariantCulture),
            "TB_SID" => pk.SID16.ToString(CultureInfo.InvariantCulture),
            "TB_Friendship" => pk.CurrentFriendship.ToString(CultureInfo.InvariantCulture),
            "TB_IVHP" => pk.IV_HP.ToString(CultureInfo.InvariantCulture),
            "TB_IVATK" => pk.IV_ATK.ToString(CultureInfo.InvariantCulture),
            "TB_IVDEF" => pk.IV_DEF.ToString(CultureInfo.InvariantCulture),
            "TB_IVSPA" => pk.IV_SPA.ToString(CultureInfo.InvariantCulture),
            "TB_IVSPD" => pk.IV_SPD.ToString(CultureInfo.InvariantCulture),
            "TB_IVSPE" => pk.IV_SPE.ToString(CultureInfo.InvariantCulture),
            "TB_IVTotal" => pk.IVTotal.ToString(CultureInfo.InvariantCulture),
            "TB_EVHP" => pk.EV_HP.ToString(CultureInfo.InvariantCulture),
            "TB_EVATK" => pk.EV_ATK.ToString(CultureInfo.InvariantCulture),
            "TB_EVDEF" => pk.EV_DEF.ToString(CultureInfo.InvariantCulture),
            "TB_EVSPA" => pk.EV_SPA.ToString(CultureInfo.InvariantCulture),
            "TB_EVSPD" => pk.EV_SPD.ToString(CultureInfo.InvariantCulture),
            "TB_EVSPE" => pk.EV_SPE.ToString(CultureInfo.InvariantCulture),
            "TB_EVTotal" => pk.EVTotal.ToString(CultureInfo.InvariantCulture),
            "TB_BaseHP" => pk.PersonalInfo.HP.ToString(CultureInfo.InvariantCulture),
            "TB_BaseATK" => pk.PersonalInfo.ATK.ToString(CultureInfo.InvariantCulture),
            "TB_BaseDEF" => pk.PersonalInfo.DEF.ToString(CultureInfo.InvariantCulture),
            "TB_BaseSPA" => pk.PersonalInfo.SPA.ToString(CultureInfo.InvariantCulture),
            "TB_BaseSPD" => pk.PersonalInfo.SPD.ToString(CultureInfo.InvariantCulture),
            "TB_BaseSPE" => pk.PersonalInfo.SPE.ToString(CultureInfo.InvariantCulture),
            "Stat_HP" => pk.Stat_HPMax.ToString(CultureInfo.InvariantCulture),
            "Stat_ATK" => pk.Stat_ATK.ToString(CultureInfo.InvariantCulture),
            "Stat_DEF" => pk.Stat_DEF.ToString(CultureInfo.InvariantCulture),
            "Stat_SPA" => pk.Stat_SPA.ToString(CultureInfo.InvariantCulture),
            "Stat_SPD" => pk.Stat_SPD.ToString(CultureInfo.InvariantCulture),
            "Stat_SPE" => pk.Stat_SPE.ToString(CultureInfo.InvariantCulture),
            "CB_HPType" => HpType(pk),
            "Label_HiddenPowerPower" => pk.HPPower.ToString(CultureInfo.InvariantCulture),
            "L_Characteristic" => pk.Characteristic.ToString(CultureInfo.InvariantCulture),
            "CB_Move1" => pk.Move1.ToString(CultureInfo.InvariantCulture),
            "CB_Move2" => pk.Move2.ToString(CultureInfo.InvariantCulture),
            "CB_Move3" => pk.Move3.ToString(CultureInfo.InvariantCulture),
            "CB_Move4" => pk.Move4.ToString(CultureInfo.InvariantCulture),
            "TB_PP1" => pk.Move1_PP.ToString(CultureInfo.InvariantCulture),
            "TB_PP2" => pk.Move2_PP.ToString(CultureInfo.InvariantCulture),
            "TB_PP3" => pk.Move3_PP.ToString(CultureInfo.InvariantCulture),
            "TB_PP4" => pk.Move4_PP.ToString(CultureInfo.InvariantCulture),
            "CB_PPUps1" => pk.Move1_PPUps.ToString(CultureInfo.InvariantCulture),
            "CB_PPUps2" => pk.Move2_PPUps.ToString(CultureInfo.InvariantCulture),
            "CB_PPUps3" => pk.Move3_PPUps.ToString(CultureInfo.InvariantCulture),
            "CB_PPUps4" => pk.Move4_PPUps.ToString(CultureInfo.InvariantCulture),
            "TB_Cool" => Contest(pk)?.ContestCool.ToString(CultureInfo.InvariantCulture) ?? "0",
            "TB_Beauty" => Contest(pk)?.ContestBeauty.ToString(CultureInfo.InvariantCulture) ?? "0",
            "TB_Cute" => Contest(pk)?.ContestCute.ToString(CultureInfo.InvariantCulture) ?? "0",
            "TB_Smart" => Contest(pk)?.ContestSmart.ToString(CultureInfo.InvariantCulture) ?? "0",
            "TB_Tough" => Contest(pk)?.ContestTough.ToString(CultureInfo.InvariantCulture) ?? "0",
            "TB_Sheen" => Contest(pk)?.ContestSheen.ToString(CultureInfo.InvariantCulture) ?? "0",
            "NUD_PokeStarFame" => pk is PK5 star ? star.PokeStarFame.ToString(CultureInfo.InvariantCulture) : "0",
            _ => throw new InvalidOperationException($"Unknown field '{name}'."),
        };
    }

    public static void Set(PKM pk, SaveFile sav, string name, string value)
    {
        _ = sav;
        switch (name)
        {
            case "TB_Nickname": pk.Nickname = value; break;
            case "CHK_NicknamedFlag":
            case "CHK_Nicknamed": pk.IsNicknamed = IsFlag(value); break;
            case "CB_Species": pk.Species = (ushort)Int(value); break;
            case "CB_Form": pk.Form = (byte)Int(value); break;
            case "TB_EXP": pk.EXP = (uint)Int(value); break;
            case "TB_Level": pk.CurrentLevel = (byte)Math.Clamp(Int(value), 1, 100); break;
            case "CB_Nature": pk.Nature = (Nature)Int(value); break;
            case "CB_HeldItem": pk.HeldItem = Int(value); break;
            case "CB_Ability": pk.Ability = Int(value); break;
            case "CB_Language": pk.Language = (byte)Int(value); break;
            case "CHK_IsEgg": pk.IsEgg = IsFlag(value); break;
            case "CB_PKRSStrain": pk.PokerusStrain = Int(value); break;
            case "CB_PKRSDays": pk.PokerusDays = Int(value); break;
            case "CHK_NSparkle" when pk is PK5 pk5n: pk5n.NSparkle = IsFlag(value); break;
            case "TB_PID": pk.PID = ParseHex(value); break;
            case "UC_Gender": pk.Gender = (byte)Int(value); break;
            case "PB_MarkShiny":
                if (IsFlag(value)) pk.SetShiny();
                else pk.SetUnshiny();
                break;
            case "CB_GameOrigin": pk.Version = (GameVersion)Int(value); break;
            case "CB_MetLocation": pk.MetLocation = (ushort)Int(value); break;
            case "CB_Ball": pk.Ball = (byte)Int(value); break;
            case "TB_MetLevel": pk.MetLevel = (byte)Int(value); break;
            case "CHK_Fateful": pk.FatefulEncounter = IsFlag(value); break;
            case "CAL_MetDate": pk.MetDate = ParseDate(value); break;
            case "CB_EggLocation": pk.EggLocation = (ushort)Int(value); break;
            case "CAL_EggDate": pk.EggMetDate = ParseDate(value); break;
            case "CB_GroundTile" when pk is IGroundTile gt: gt.GroundTile = (GroundTileType)Int(value); break;
            case "TB_OT": pk.OriginalTrainerName = value; break;
            case "UC_OTGender": pk.OriginalTrainerGender = (byte)Int(value); break;
            case "TB_TID": pk.TID16 = (ushort)Int(value); break;
            case "TB_SID": pk.SID16 = (ushort)Int(value); break;
            case "TB_Friendship": pk.CurrentFriendship = (byte)Int(value); break;
            case "TB_IVHP": pk.IV_HP = Int(value); break;
            case "TB_IVATK": pk.IV_ATK = Int(value); break;
            case "TB_IVDEF": pk.IV_DEF = Int(value); break;
            case "TB_IVSPA": pk.IV_SPA = Int(value); break;
            case "TB_IVSPD": pk.IV_SPD = Int(value); break;
            case "TB_IVSPE": pk.IV_SPE = Int(value); break;
            case "TB_EVHP": pk.EV_HP = Int(value); break;
            case "TB_EVATK": pk.EV_ATK = Int(value); break;
            case "TB_EVDEF": pk.EV_DEF = Int(value); break;
            case "TB_EVSPA": pk.EV_SPA = Int(value); break;
            case "TB_EVSPD": pk.EV_SPD = Int(value); break;
            case "TB_EVSPE": pk.EV_SPE = Int(value); break;
            case "CB_Move1": pk.Move1 = (ushort)Int(value); break;
            case "CB_Move2": pk.Move2 = (ushort)Int(value); break;
            case "CB_Move3": pk.Move3 = (ushort)Int(value); break;
            case "CB_Move4": pk.Move4 = (ushort)Int(value); break;
            case "TB_PP1": pk.Move1_PP = (byte)Int(value); break;
            case "TB_PP2": pk.Move2_PP = (byte)Int(value); break;
            case "TB_PP3": pk.Move3_PP = (byte)Int(value); break;
            case "TB_PP4": pk.Move4_PP = (byte)Int(value); break;
            case "CB_PPUps1": pk.Move1_PPUps = Int(value); break;
            case "CB_PPUps2": pk.Move2_PPUps = Int(value); break;
            case "CB_PPUps3": pk.Move3_PPUps = Int(value); break;
            case "CB_PPUps4": pk.Move4_PPUps = Int(value); break;
            case "TB_Cool" when Contest(pk) is { } c: c.ContestCool = (byte)Int(value); break;
            case "TB_Beauty" when Contest(pk) is { } c: c.ContestBeauty = (byte)Int(value); break;
            case "TB_Cute" when Contest(pk) is { } c: c.ContestCute = (byte)Int(value); break;
            case "TB_Smart" when Contest(pk) is { } c: c.ContestSmart = (byte)Int(value); break;
            case "TB_Tough" when Contest(pk) is { } c: c.ContestTough = (byte)Int(value); break;
            case "TB_Sheen" when Contest(pk) is { } c: c.ContestSheen = (byte)Int(value); break;
            case "NUD_PokeStarFame" when pk is PK5 star: star.PokeStarFame = (byte)Int(value); break;
            default: throw new InvalidOperationException($"Unknown or read-only field '{name}'.");
        }
    }

    public static string Choices(PKM pk, SaveFile sav, string name)
    {
        var src = new FilteredGameDataSource(sav, GameInfo.Sources);
        return name switch
        {
            "CB_Species" => Lines(src.Species),
            "CB_HeldItem" => Lines(src.Items),
            "CB_Nature" => Lines(src.Natures),
            "CB_Ability" => Lines(src.Abilities),
            "CB_Language" => Lines(src.Languages),
            "CB_Ball" => Lines(src.Balls),
            "CB_GameOrigin" => Lines(src.Games),
            "CB_MetLocation" => Lines(GameInfo.GetLocationList(pk.Version, pk.Context, egg: false)),
            "CB_EggLocation" => Lines(GameInfo.GetLocationList(pk.Version, pk.Context, egg: true)),
            "CB_GroundTile" => Lines(src.G4GroundTiles),
            "CB_Move1" or "CB_Move2" or "CB_Move3" or "CB_Move4" => Lines(src.Moves),
            "CB_Form" => FormLines(pk),
            "CB_HPType" => HpTypes(),
            "CB_PPUps1" or "CB_PPUps2" or "CB_PPUps3" or "CB_PPUps4" => "0\t0\n1\t1\n2\t2\n3\t3",
            "CB_PKRSStrain" => string.Join('\n', Enumerable.Range(0, 16).Select(i => $"{i}\t{i}")),
            "CB_PKRSDays" => "0\t0\n1\t1\n2\t2\n3\t3\n4\t4",
            _ => string.Empty,
        };
    }

    private static string FormLines(PKM pk)
    {
        var strings = GameInfo.Strings;
        var list = FormConverter.GetFormList(pk.Species, strings.Types, strings.forms, GameInfo.GenderSymbolUnicode, pk.Context);
        var lines = new List<string>(list.Length);
        for (int i = 0; i < list.Length; i++)
            lines.Add($"{i}\t{list[i]}");
        return string.Join('\n', lines);
    }

    private static string HpTypes()
    {
        var types = GameInfo.Strings.types;
        var lines = new List<string>();
        for (int i = 0; i < 16; i++)
        {
            if (i < types.Length)
                lines.Add($"{i}\t{types[i]}");
        }
        return string.Join('\n', lines);
    }

    private static string Lines(IEnumerable<ComboItem> items)
        => string.Join('\n', items.Select(i => $"{i.Value}\t{i.Text}"));

    private static string HpType(PKM pk)
    {
        Span<int> ivs = stackalloc int[6];
        pk.GetIVs(ivs);
        return HiddenPower.GetType(ivs, pk.Context).ToString(CultureInfo.InvariantCulture);
    }

    private static IContestStats? Contest(PKM pk) => pk as IContestStats;

    private static string Flag(bool value) => value ? "1" : "0";
    private static bool IsFlag(string value) => value is "1" or "true" or "True";
    private static int Int(string value) => int.Parse(value, CultureInfo.InvariantCulture);
    private static uint ParseHex(string value) => uint.Parse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture);

    private static string FormatDate(DateOnly? date)
        => date?.ToString("yyyy-MM-dd", CultureInfo.InvariantCulture) ?? string.Empty;

    private static DateOnly? ParseDate(string value)
        => DateOnly.TryParse(value, CultureInfo.InvariantCulture, out var date) ? date : null;
}
