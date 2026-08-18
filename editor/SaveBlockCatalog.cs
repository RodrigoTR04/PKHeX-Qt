using PKHeX.Core;

namespace PKHeX.Editor;

internal static class SaveBlockCatalog
{
    public static IReadOnlyList<string> IdsFor(SaveFile sav)
    {
        var ids = new List<string>();
        if (HasMisc(sav))
            ids.Add("misc");
        if (sav is ISaveBlock6Main)
        {
            ids.Add("pokepuffs");
            ids.Add("opowers");
        }
        if (sav is SAV9SV or SAV9ZA)
            ids.Add("fashion");
        return ids;
    }

    public static IEnumerable<ISaveBlockPage> Create(SaveFile sav)
    {
        if (HasMisc(sav))
            yield return Misc(sav);
        if (sav is ISaveBlock6Main main)
        {
            yield return Pokepuffs(main);
            yield return OPowers(main);
        }
        if (sav is SAV9SV or SAV9ZA)
            yield return Fashion(sav);
    }

    private static bool HasMisc(SaveFile sav) => sav is SAV2 { Version: GameVersion.C } or SAV3 or SAV4 or SAV5 or SAV8BS;

    private static BlockToolPage Misc(SaveFile sav)
    {
        var fields = new List<BlockToolPage.Field>();
        var actions = new Dictionary<string, Action>(StringComparer.Ordinal);
        Action? flush = null;
        switch (sav)
        {
            case SAV2 s2:
                fields.Add(new BlockToolPage.Field
                {
                    Name = "gsBall",
                    Label = "GS Ball",
                    Value = s2.IsEnabledGSBallMobileEvent ? 1u : 0u,
                    Get = () => s2.IsEnabledGSBallMobileEvent ? 1u : 0u,
                    Set = _ => { },
                });
                actions["B_VirtualConsoleGSBall"] = s2.EnableGSBallMobileEvent;
                break;
            case SAV3 s3:
                fields.Add(Coin(() => s3.Coin, v => s3.Coin = v));
                break;
            case SAV4 s4:
                fields.Add(Coin(() => s4.Coin, v => s4.Coin = v));
                break;
            case SAV5 s5:
                var records = s5.Records;
                fields.Add(new BlockToolPage.Field
                {
                    Name = "record16",
                    Label = "Record 16",
                    Get = () => records.GetRecord16(0),
                    Set = value => records.SetRecord16(0, (ushort)value),
                });
                flush = records.EndAccess;
                break;
            case SAV8BS bs:
                var unlock = new EventUnlocker8b(bs);
                actions["B_Spiritomb"] = unlock.UnlockSpiritomb;
                break;
        }
        return new BlockToolPage("misc", LangForm(sav), fields, actions, flush);
    }

    private static BlockToolPage Pokepuffs(ISaveBlock6Main sav)
    {
        var puff = sav.Puff;
        var puffs = puff.GetPuffs();
        return new BlockToolPage("pokepuffs", "SAV_Pokepuff",
        [
            new BlockToolPage.Field
            {
                Name = "puff0",
                Label = "Puff 0",
                Value = puffs.Length > 0 ? puffs[0] : 0u,
                Get = () => puff.GetPuffs()[0],
                Set = value => puff.GetPuffs()[0] = (byte)value,
            },
        ], []);
    }

    private static BlockToolPage OPowers(ISaveBlock6Main sav)
    {
        var block = sav.OPower;
        return new BlockToolPage("opowers", "SAV_OPower",
        [
            new BlockToolPage.Field
            {
                Name = "points",
                Label = "Points",
                Value = block.Points,
                Get = () => block.Points,
                Set = value => block.Points = (byte)value,
            },
        ], new Dictionary<string, Action>(StringComparer.Ordinal)
        {
            ["B_GiveAll"] = block.UnlockAll,
            ["B_ClearAll"] = block.ClearAll,
        });
    }

    private static BlockToolPage Fashion(SaveFile sav)
    {
        var actions = new Dictionary<string, Action>(StringComparer.Ordinal);
        if (sav is SAV9SV sv)
            actions["B_SetAllOwned"] = () => PlayerFashionUnlock9.UnlockBase(sv.Blocks, sv.Gender);
        return new BlockToolPage("fashion", "SAV_Fashion9", [], actions);
    }

    private static BlockToolPage.Field Coin(Func<uint> get, Action<uint> set)
        => new()
        {
            Name = "coins",
            Label = "Coins",
            Value = get(),
            Get = get,
            Set = set,
        };

    private static string LangForm(SaveFile sav) => sav switch
    {
        SAV2 => "SAV_Misc2",
        SAV3 => "SAV_Misc3",
        SAV4 => "SAV_Misc4",
        SAV5 => "SAV_Misc5",
        SAV8BS => "SAV_Misc8b",
        _ => "SAV_Misc5",
    };
}
