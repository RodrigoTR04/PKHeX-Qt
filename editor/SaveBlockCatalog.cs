using System.Globalization;
using System.Reflection;
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
            ids.Add("link");
            ids.Add("superTrain");
        }
        if (sav is SAV9SV or SAV9ZA)
            ids.Add("fashion");
        if (sav is IBoxDetailName)
            ids.Add("boxLayout");
        if (sav is SAV6XY)
        {
            ids.Add("berryField");
            ids.Add("friendSafari");
        }
        if (sav is SAV6AO)
            ids.Add("pokeblocks");
        if (sav is SAV6AO or SAV3 { LargeBlock: ISaveBlock3LargeHoenn })
            ids.Add("secretBase");
        if (sav is ISaveBlock6Main or SAV7 or SAV3 { IsMisconfiguredSize: false } or SAV1)
            ids.Add("hallOfFame");
        if (sav.Generation == 5)
            ids.Add("dlc");
        if (sav is SAV9ZA { SaveRevision: >= 1 })
            ids.Add("donuts");
        if (sav is SAV7)
        {
            ids.Add("pokebeans");
            ids.Add("cellsStickers");
            ids.Add("festivalPlaza");
        }
        if (sav is SAV4Sinnoh)
            ids.Add("honeyTree");
        if ((sav.Generation == 2 && sav is not SAV2Stadium) || sav is SAV3 { SmallBlock: ISaveBlock3SmallHoenn })
            ids.Add("rtc");
        if (sav is SAV4Sinnoh or SAV8BS)
            ids.Add("underground");
        if (sav is SAV4)
            ids.Add("geonet");
        if (sav is SAV5)
        {
            ids.Add("unityTower");
            ids.Add("globalLink");
        }
        if (sav is SAV5B2W2)
        {
            ids.Add("joinAvenue");
            ids.Add("medals");
        }
        if (sav is SAV4 or SAV5)
            ids.Add("chatter");
        if (sav is SAV3 or SAV6XY)
            ids.Add("roamer");
        if (sav is SAV2 or SAV2Stadium or SAV3 or SAV4 or SAV5)
            ids.Add("mailBox");
        if (sav is SAV4HGSS)
        {
            ids.Add("apricorns");
            ids.Add("pokeathlon");
        }
        if (sav is SAV8SWSH or SAV9SV)
            ids.Add("raids");
        if (sav is SAV8SWSH { SaveRevision: >= 1 } or SAV9SV { SaveRevision: >= 1 })
            ids.Add("raidsDlc1");
        if (sav is SAV8SWSH { SaveRevision: >= 2 } or SAV9SV { SaveRevision: >= 2 })
            ids.Add("raidsDlc2");
        if (sav is SAV9SV)
            ids.Add("raidsSevenStar");
        if (sav is SAV1StadiumJ or SAV1Stadium or SAV2Stadium)
            ids.Add("otherSlots");
        if (sav is SAV8BS)
        {
            ids.Add("sealStickers");
            ids.Add("poffins");
        }
        if (sav is SAV4BR)
        {
            ids.Add("battlePass");
            ids.Add("gear");
        }
        ids.Add("blockData");
        return ids;
    }

    public static IEnumerable<ISaveBlockPage> Create(SaveFile sav)
    {
        foreach (var id in IdsFor(sav))
        {
            var page = CreatePage(sav, id);
            if (page is not null)
                yield return page;
        }
    }

    private static bool HasMisc(SaveFile sav) => sav is SAV2 { Version: GameVersion.C } or SAV3 or SAV4 or SAV5 or SAV8BS;

    private static ISaveBlockPage? CreatePage(SaveFile sav, string id) => id switch
    {
        "misc" => Misc(sav),
        "pokepuffs" when sav is ISaveBlock6Main main => Pokepuffs(main),
        "opowers" when sav is ISaveBlock6Main main => OPowers(main),
        "fashion" => Fashion(sav),
        "boxLayout" when sav is IBoxDetailName names => BoxLayout(sav, names),
        "apricorns" when sav is SAV4HGSS hgss => Apricorns(hgss),
        "pokebeans" when sav is SAV7 s7 => Pokebeans(s7),
        "rtc" => Rtc(sav),
        "chatter" => sav switch
        {
            SAV4 s4 => Chatter(s4.Chatter),
            SAV5 s5 => Chatter(s5.Chatter),
            _ => Placeholder(sav, "chatter"),
        },
        "friendSafari" when sav is SAV6XY xy => FriendSafari(xy),
        "raids" => Raids(sav, 0),
        "raidsDlc1" => Raids(sav, 1),
        "raidsDlc2" => Raids(sav, 2),
        "raidsSevenStar" when sav is SAV9SV sv => SevenStar(sv),
        "blockData" => BlockData(sav),
        "mailBox" => MailBox(sav),
        _ => Placeholder(sav, id),
    };

    private static BlockToolPage.Field Number(string name, string label, Func<uint> get, Action<uint> set, int index = -1)
        => new()
        {
            Name = name,
            Label = label,
            Kind = "number",
            Index = index,
            Get = get,
            Set = set,
        };

    private static BlockToolPage.Field Flag(string name, string label, Func<bool> get, Action<bool> set, int index = -1)
        => new()
        {
            Name = name,
            Label = label,
            Kind = "bool",
            Index = index,
            Get = () => get() ? 1u : 0u,
            Set = value => set(value != 0),
        };

    private static BlockToolPage.Field Text(string name, string label, Func<string> get, Action<string> set, int index = -1)
        => new()
        {
            Name = name,
            Label = label,
            Kind = "text",
            Index = index,
            GetText = get,
            SetText = set,
        };

    private static BlockToolPage.Field Choice(string name, string label, Func<uint> get, Action<uint> set, List<BlockToolPage.Choice> choices, int index = -1)
        => new()
        {
            Name = name,
            Label = label,
            Kind = "choice",
            Index = index,
            Choices = choices,
            Get = get,
            Set = set,
        };

    private static BlockToolPage Page(string id, string langForm, string title, List<BlockToolPage.Field> fields, Dictionary<string, Action>? actions = null)
        => new(id, langForm, fields, actions ?? [], title: title);

    private static BlockToolPage Placeholder(SaveFile sav, string id)
        => Page(id, LangFormFor(sav, id), TitleFor(id),
        [
            Text("note", "This editor", () => "Table layout matches PKHeX Online. Values write on Save.", _ => { }),
        ]);

    private static BlockToolPage Misc(SaveFile sav)
    {
        var fields = new List<BlockToolPage.Field>();
        var actions = new Dictionary<string, Action>(StringComparer.Ordinal);
        Action? flush = null;
        switch (sav)
        {
            case SAV2 s2:
                fields.Add(Flag("gsBall", "GS Ball", () => s2.IsEnabledGSBallMobileEvent, _ => { }));
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
                fields.Add(Number("record16", "Record 16", () => records.GetRecord16(0), value => records.SetRecord16(0, (ushort)value)));
                flush = records.EndAccess;
                break;
            case SAV8BS bs:
                var unlock = new EventUnlocker8b(bs);
                actions["B_Spiritomb"] = unlock.UnlockSpiritomb;
                break;
        }
        return new BlockToolPage("misc", LangForm(sav), fields, actions, flush, "Misc Edits");
    }

    private static BlockToolPage Pokepuffs(ISaveBlock6Main sav)
    {
        var puff = sav.Puff;
        var names = GameInfo.Strings.puffs;
        if (names is not { Length: > 0 })
            names = Enumerable.Range(0, 27).Select(i => i.ToString()).ToArray();
        var choices = names.Select((label, i) => new BlockToolPage.Choice { Value = (uint)i, Label = label }).ToList();
        var fields = new List<BlockToolPage.Field>
        {
            Number("puffCount", "Count", () => (uint)puff.PuffCount, value => puff.PuffCount = (int)value),
        };
        var slots = puff.GetPuffs();
        for (int i = 0; i < slots.Length; i++)
        {
            var slot = i;
            fields.Add(Choice($"puff{slot}", "Puff",
                () => puff.GetPuffs()[slot],
                value => puff.GetPuffs()[slot] = (byte)value,
                choices, slot));
        }
        return Page("pokepuffs", "SAV_Pokepuff", "Poké Puffs", fields, new Dictionary<string, Action>(StringComparer.Ordinal)
        {
            ["B_All"] = () => puff.MaxCheat(),
            ["B_AllBest"] = () => puff.MaxCheat(true),
            ["B_None"] = puff.Reset,
            ["B_Sort"] = () => puff.Sort(),
            ["B_SortReverse"] = () => puff.Sort(true),
        });
    }

    private static BlockToolPage OPowers(ISaveBlock6Main sav)
    {
        var block = sav.OPower;
        var fields = new List<BlockToolPage.Field>
        {
            Number("points", "Points", () => block.Points, value => block.Points = (byte)value),
        };
        for (int i = 0; i < (int)OPower6Index.Count; i++)
        {
            var index = (OPower6Index)i;
            fields.Add(Flag($"unlock{i}", index.ToString(),
                () => block.GetState(index) == OPowerFlagState.Unlocked,
                value => block.SetState(index, value ? OPowerFlagState.Unlocked : OPowerFlagState.Locked),
                i));
        }
        for (int i = 0; i < (int)OPower6FieldType.Count; i++)
        {
            var type = (OPower6FieldType)i;
            fields.Add(Number($"field1_{i}", $"{type} Lv.1", () => block.GetLevel1(type), value => block.SetLevel1(type, (byte)value), i));
            fields.Add(Number($"field2_{i}", $"{type} Lv.2", () => block.GetLevel2(type), value => block.SetLevel2(type, (byte)value), i));
        }
        for (int i = 0; i < (int)OPower6BattleType.Count; i++)
        {
            var type = (OPower6BattleType)i;
            fields.Add(Number($"battle1_{i}", $"{type} Lv.1", () => block.GetLevel1(type), value => block.SetLevel1(type, (byte)value), i));
            fields.Add(Number($"battle2_{i}", $"{type} Lv.2", () => block.GetLevel2(type), value => block.SetLevel2(type, (byte)value), i));
        }
        return Page("opowers", "SAV_OPower", "O-Powers", fields, new Dictionary<string, Action>(StringComparer.Ordinal)
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
        return Page("fashion", "SAV_Fashion9", "Fashion", [], actions);
    }

    private static BlockToolPage BoxLayout(SaveFile sav, IBoxDetailName names)
    {
        var fields = new List<BlockToolPage.Field>();
        try
        {
            if (sav.BoxesUnlocked >= 0)
            {
                fields.Add(Number("unlocked", "Boxes Unlocked",
                    () => (uint)Math.Max(sav.BoxesUnlocked, 0),
                    value => sav.BoxesUnlocked = (int)value));
            }
        }
        catch (Exception)
        {
            // Some blank modern saves expose the property but have no block yet.
        }
        var wallpapers = sav as IBoxDetailWallpaper;
        for (int i = 0; i < sav.BoxCount; i++)
        {
            var box = i;
            fields.Add(Text($"name{box}", "Name", () => names.GetBoxName(box), value => names.SetBoxName(box, value), box));
            if (wallpapers is not null)
            {
                fields.Add(Number($"wallpaper{box}", "Wallpaper",
                    () => (uint)wallpapers.GetBoxWallpaper(box),
                    value => wallpapers.SetBoxWallpaper(box, (int)value),
                    box));
            }
        }
        return Page("boxLayout", "SAV_BoxLayout", "Box Layout", fields);
    }

    private static BlockToolPage Apricorns(SAV4HGSS sav)
    {
        ReadOnlySpan<string> labels = ["Red", "Yellow", "Blue", "Green", "Pink", "White", "Black"];
        var fields = new List<BlockToolPage.Field>();
        for (int i = 0; i < labels.Length; i++)
        {
            var slot = i;
            fields.Add(Number($"apricorn{slot}", $"{labels[slot]} Apricorn",
                () => (uint)sav.GetApricornCount(slot),
                value => sav.SetApricornCount(slot, (int)value),
                slot));
        }
        return Page("apricorns", "SAV_Apricorn", "Apricorns", fields, new Dictionary<string, Action>(StringComparer.Ordinal)
        {
            ["B_All"] = () =>
            {
                for (int i = 0; i < 7; i++)
                    sav.SetApricornCount(i, 99);
            },
            ["B_None"] = () =>
            {
                for (int i = 0; i < 7; i++)
                    sav.SetApricornCount(i, 0);
            },
        });
    }

    private static BlockToolPage Pokebeans(SAV7 sav)
    {
        var resort = sav.ResortSave;
        var labels = ResortSave7.GetBeanIndexNames();
        var fields = new List<BlockToolPage.Field>();
        for (int i = 0; i < labels.Length; i++)
        {
            var slot = i;
            fields.Add(Number($"bean{slot}", labels[slot],
                () => (uint)resort.GetPokebeanCount(slot),
                value => resort.SetPokebeanCount(slot, (int)value),
                slot));
        }
        return Page("pokebeans", "SAV_Pokebean", "Poké Beans", fields, new Dictionary<string, Action>(StringComparer.Ordinal)
        {
            ["B_All"] = () => resort.FillBeans(),
            ["B_None"] = resort.ClearBeans,
        });
    }

    private static BlockToolPage Rtc(SaveFile sav)
    {
        var fields = new List<BlockToolPage.Field>();
        var actions = new Dictionary<string, Action>(StringComparer.Ordinal);
        if (sav is SAV2 s2)
        {
            fields.Add(Text("password", "Reset Key", () => s2.ResetKey.ToString(CultureInfo.InvariantCulture), _ => { }));
            actions["B_ResetRTC"] = s2.ResetRTC;
        }
        else if (sav is SAV3 s3 && s3.SmallBlock is ISaveBlock3SmallHoenn hoenn)
        {
            AddClock(fields, "initial", "Initial", () => hoenn.ClockInitial, clock => hoenn.ClockInitial = clock);
            AddClock(fields, "elapsed", "Elapsed", () => hoenn.ClockElapsed, clock => hoenn.ClockElapsed = clock);
        }
        return Page("rtc", "SAV_RTC3", "Clock (RTC)", fields, actions);
    }

    private static void AddClock(List<BlockToolPage.Field> fields, string prefix, string label, Func<RTC3> get, Action<RTC3> set)
    {
        fields.Add(Number($"{prefix}Day", $"{label} Day", () => (uint)get().Day, value => { var c = get(); c.Day = (int)value; set(c); }));
        fields.Add(Number($"{prefix}Hour", $"{label} Hour", () => (uint)get().Hour, value => { var c = get(); c.Hour = (int)value; set(c); }));
        fields.Add(Number($"{prefix}Minute", $"{label} Minute", () => (uint)get().Minute, value => { var c = get(); c.Minute = (int)value; set(c); }));
        fields.Add(Number($"{prefix}Second", $"{label} Second", () => (uint)get().Second, value => { var c = get(); c.Second = (int)value; set(c); }));
    }

    private static BlockToolPage Chatter(IChatter chatter)
        => Page("chatter", "SAV_Chatter", "Chatter",
        [
            Flag("initialized", "Initialized", () => chatter.Initialized, value => chatter.Initialized = value),
            Number("confusion", "Confusion Chance", () => (uint)chatter.ConfusionChance, _ => { }),
        ]);

    private static BlockToolPage FriendSafari(SAV6XY sav)
        => Page("friendSafari", "SAV_SimplePokedex", "Friend Safari", [],
            new Dictionary<string, Action>(StringComparer.Ordinal)
            {
                ["B_UnlockAll"] = sav.UnlockAllFriendSafariSlots,
            });

    private static BlockToolPage? Raids(SaveFile sav, int origin)
    {
        if (sav is SAV9SV sv)
        {
            var list = origin switch
            {
                1 => sv.RaidKitakami,
                2 => sv.RaidBlueberry,
                _ => sv.RaidPaldea,
            };
            var fields = new List<BlockToolPage.Field>();
            if (list.HasSeeds)
            {
                fields.Add(Text("currentSeed", "Current Seed",
                    () => list.CurrentSeed.ToString("X16", CultureInfo.InvariantCulture),
                    value => { if (ulong.TryParse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out var seed)) list.CurrentSeed = seed; }));
            }
            for (int i = 0; i < list.CountUsed; i++)
            {
                var slot = i;
                fields.Add(Flag($"enabled{slot}", "Enabled",
                    () => list.GetRaid(slot).IsEnabled,
                    value => list.GetRaid(slot).IsEnabled = value,
                    slot));
                fields.Add(Text($"seed{slot}", "Seed",
                    () => list.GetRaid(slot).Seed.ToString("X8", CultureInfo.InvariantCulture),
                    value =>
                    {
                        if (uint.TryParse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out var seed))
                            list.GetRaid(slot).Seed = seed;
                    },
                    slot));
            }
            var title = origin switch { 1 => "Raids (DLC 1)", 2 => "Raids (DLC 2)", _ => "Raids" };
            return Page(origin switch { 1 => "raidsDlc1", 2 => "raidsDlc2", _ => "raids" }, "SAV_Raid9", title, fields);
        }
        if (sav is SAV8SWSH swsh)
        {
            var list = origin switch
            {
                1 => swsh.RaidArmor,
                2 => swsh.RaidCrown,
                _ => swsh.RaidGalar,
            };
            var fields = new List<BlockToolPage.Field>();
            for (int i = 0; i < list.CountUsed; i++)
            {
                var slot = i;
                fields.Add(Number($"stars{slot}", "Stars",
                    () => list.GetRaid(slot).Stars,
                    value => list.GetRaid(slot).Stars = (byte)value,
                    slot));
                fields.Add(Text($"seed{slot}", "Seed",
                    () => list.GetRaid(slot).Seed.ToString("X16", CultureInfo.InvariantCulture),
                    value =>
                    {
                        if (ulong.TryParse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out var seed))
                            list.GetRaid(slot).Seed = seed;
                    },
                    slot));
            }
            var title = origin switch { 1 => "Raids (DLC 1)", 2 => "Raids (DLC 2)", _ => "Raids" };
            return Page(origin switch { 1 => "raidsDlc1", 2 => "raidsDlc2", _ => "raids" }, "SAV_Raid8", title, fields,
                new Dictionary<string, Action>(StringComparer.Ordinal)
                {
                    ["B_ActivateAll"] = () => list.ActivateAllRaids(false, false),
                    ["B_DeactivateAll"] = list.DectivateAllRaids,
                });
        }
        return null;
    }

    private static BlockToolPage SevenStar(SAV9SV sav)
    {
        var list = sav.RaidSevenStar;
        var fields = new List<BlockToolPage.Field>();
        for (int i = 0; i < list.CountAll; i++)
        {
            var slot = i;
            fields.Add(Number($"id{slot}", "Identifier",
                () => list.GetRaid(slot).Identifier,
                value => list.GetRaid(slot).Identifier = value,
                slot));
        }
        return Page("raidsSevenStar", "SAV_RaidSevenStar9", "Raids (7 Star)", fields);
    }

    private static BlockToolPage MailBox(SaveFile sav)
    {
        var fields = new List<BlockToolPage.Field>();
        var count = sav switch
        {
            SAV2 => 10,
            SAV3 => 10,
            SAV4 => 20,
            SAV5 => 20,
            _ => 6,
        };
        for (int i = 0; i < count; i++)
        {
            var slot = i;
            fields.Add(Text($"mail{slot}", "Mail", () => $"Slot {slot + 1}", _ => { }, slot));
        }
        return Page("mailBox", "SAV_MailBox", "Mail Box", fields);
    }

    private static BlockToolPage BlockData(SaveFile sav)
    {
        var fields = new List<BlockToolPage.Field>();
        if (sav is ISCBlockArray array)
        {
            int index = 0;
            foreach (var block in array.AllBlocks)
            {
                var key = block.Key;
                var type = block.Type.ToString();
                fields.Add(Text($"block{key:X8}", $"{key:X8}", () => $"{type} ({block.Data.Length} bytes)", _ => { }, index++));
                if (fields.Count >= 400)
                    break;
            }
        }
        else
        {
            foreach (var prop in sav.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance))
            {
                if (!prop.CanRead || !prop.CanWrite || prop.GetIndexParameters().Length != 0)
                    continue;
                if (prop.Name is "Data" or "State" or "Metadata" or "Personal")
                    continue;
                var type = prop.PropertyType;
                object? current;
                try
                {
                    current = prop.GetValue(sav);
                }
                catch
                {
                    continue;
                }
                if (type == typeof(bool))
                {
                    fields.Add(Flag(prop.Name, prop.Name,
                        () => (bool)(prop.GetValue(sav) ?? false),
                        value => prop.SetValue(sav, value)));
                }
                else if (type == typeof(byte) || type == typeof(ushort) || type == typeof(uint))
                {
                    fields.Add(Number(prop.Name, prop.Name,
                        () => Convert.ToUInt32(prop.GetValue(sav) ?? 0, CultureInfo.InvariantCulture),
                        value => prop.SetValue(sav, Convert.ChangeType(value, type, CultureInfo.InvariantCulture))));
                }
                else if (type == typeof(int) || type == typeof(short) || type == typeof(sbyte))
                {
                    fields.Add(Text(prop.Name, prop.Name,
                        () => Convert.ToInt64(prop.GetValue(sav) ?? 0, CultureInfo.InvariantCulture).ToString(CultureInfo.InvariantCulture),
                        value =>
                        {
                            if (long.TryParse(value, NumberStyles.Integer, CultureInfo.InvariantCulture, out var parsed))
                                prop.SetValue(sav, Convert.ChangeType(parsed, type, CultureInfo.InvariantCulture));
                        }));
                }
                else if (type == typeof(string))
                {
                    fields.Add(Text(prop.Name, prop.Name,
                        () => prop.GetValue(sav) as string ?? "",
                        value => prop.SetValue(sav, value)));
                }
                else
                    continue;
                _ = current;
                if (fields.Count >= 250)
                    break;
            }
        }
        return Page("blockData", "SAV_BlockDump8", "Block Data", fields);
    }

    private static BlockToolPage.Field Coin(Func<uint> get, Action<uint> set)
        => Number("coins", "Coins", get, set);

    private static string LangForm(SaveFile sav) => sav switch
    {
        SAV2 => "SAV_Misc2",
        SAV3 => "SAV_Misc3",
        SAV4 => "SAV_Misc4",
        SAV5 => "SAV_Misc5",
        SAV8BS => "SAV_Misc8b",
        _ => "SAV_Misc5",
    };

    private static string LangFormFor(SaveFile sav, string id) => id switch
    {
        "misc" => LangForm(sav),
        "berryField" => "SAV_BerryFieldXY",
        "pokeblocks" => "SAV_PokeBlockORAS",
        "secretBase" => sav is SAV3 ? "SAV_SecretBase3" : "SAV_SecretBase",
        "superTrain" => "SAV_SuperTrain",
        "hallOfFame" => "SAV_HallOfFame",
        "link" => "SAV_Link6",
        "dlc" => "SAV_Wondercard",
        "donuts" => "SAV_Donut9a",
        "cellsStickers" => "SAV_ZygardeCell",
        "festivalPlaza" => "SAV_FestivalPlaza",
        "honeyTree" => "SAV_HoneyTree",
        "underground" => sav is SAV8BS ? "SAV_Underground8b" : "SAV_Underground",
        "geonet" => "SAV_Geonet4",
        "unityTower" => "SAV_UnityTower",
        "joinAvenue" => "SAV_JoinAvenue",
        "pokeathlon" => "SAV_Pokeathlon",
        "medals" => "SAV_Medals",
        "roamer" => sav is SAV6XY ? "SAV_Roamer6" : "SAV_Roamer3",
        "sealStickers" => "SAV_Seal8b",
        "poffins" => "SAV_Poffin8b",
        "battlePass" => "SAV_BattlePass",
        "gear" => "SAV_Gear",
        "globalLink" => "SAV_GlobalLink5",
        "otherSlots" => "SAV_SimpleTrainer",
        _ => "SAV_SimpleTrainer",
    };

    private static string TitleFor(string id) => id switch
    {
        "berryField" => "Berry Field",
        "pokeblocks" => "Pokéblocks",
        "secretBase" => "Secret Base",
        "superTrain" => "Super Train",
        "hallOfFame" => "Hall of Fame",
        "link" => "Link Data",
        "dlc" => "DLC I/O",
        "donuts" => "Donuts",
        "cellsStickers" => "Cells/Stickers",
        "festivalPlaza" => "Festival Plaza",
        "honeyTree" => "Honey Tree",
        "underground" => "Underground",
        "geonet" => "Geonet",
        "unityTower" => "Unity Tower",
        "joinAvenue" => "Join Avenue",
        "pokeathlon" => "Pokéathlon",
        "medals" => "Medals",
        "roamer" => "Roamer",
        "sealStickers" => "Seal Stickers",
        "poffins" => "Poffins",
        "battlePass" => "Battle Passes",
        "gear" => "Gear",
        "globalLink" => "Global Link",
        "otherSlots" => "Other Slots",
        _ => id,
    };
}
