using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Which SAV-tab buttons Original PKHeX shows for this save
/// (<c>SAVEditor.ToggleViewSubEditors</c> / <c>ToggleViewMisc</c>).
/// </summary>
internal static class SavToolCatalog
{
    public static IReadOnlyList<string> VisibleButtons(SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(sav);
        if (sav is BulkStorage)
            return [];

        var ids = new List<string>
        {
            "B_VerifyCHK",
            "B_VerifySaveEntities",
            "B_Blocks",
        };
        if (sav.HasBox)
            ids.Add("B_SaveBoxBin");
        if (sav.Metadata.FilePath is not null)
            ids.Add("Menu_ExportBAK");

        if (sav.HasParty || sav is SAV7b)
            ids.Add("B_OpenTrainerInfo");
        if ((sav.HasParty && sav is not SAV4BR) || sav is SAV7b)
            ids.Add("B_OpenItemPouch");
        if (sav is IBoxDetailName)
            ids.Add("B_OpenBoxLayout");
        if (sav is IMysteryGiftStorageProvider)
            ids.Add("B_OpenWondercards");
        if (sav is ISaveBlock6Main)
        {
            ids.Add("B_OpenOPowers");
            ids.Add("B_OpenLinkInfo");
            ids.Add("B_OpenPokepuffs");
            ids.Add("B_OpenSuperTraining");
            ids.Add("B_OUTPasserby");
            ids.Add("B_JPEG");
        }
        if (sav is IEventFlag37 or IEventFlagProvider37 or SAV1 or SAV2 or SAV8BS or SAV7b or SAV9ZA)
            ids.Add("B_OpenEventFlags");
        if (sav.HasPokeDex)
            ids.Add("B_OpenPokedex");
        if (sav is SAV6XY)
        {
            ids.Add("B_OpenBerryField");
            ids.Add("B_OpenFriendSafari");
        }
        if (sav is SAV6AO)
            ids.Add("B_OpenPokeblocks");
        if (sav is SAV6AO or SAV3 { LargeBlock: ISaveBlock3LargeHoenn })
            ids.Add("B_OpenSecretBase");
        if (sav is ISaveBlock6Main or SAV7 or SAV3 { IsMisconfiguredSize: false } or SAV1)
            ids.Add("B_OpenHallofFame");
        if (sav.Generation == 5)
            ids.Add("B_DLC");
        if (sav is SAV9ZA { SaveRevision: >= 1 })
            ids.Add("B_Donuts");
        if (sav is SAV7)
        {
            ids.Add("B_OpenPokeBeans");
            ids.Add("B_CellsStickers");
            ids.Add("B_FestivalPlaza");
        }
        if (sav is SAV2 { Version: GameVersion.C } or SAV3 or SAV4 or SAV5 or SAV8BS)
            ids.Add("B_OpenMiscEditor");
        if (sav is SAV4Sinnoh)
            ids.Add("B_OpenHoneyTreeEditor");
        if ((sav.Generation == 2 && sav is not SAV2Stadium) || sav is SAV3 { SmallBlock: ISaveBlock3SmallHoenn })
            ids.Add("B_OpenRTCEditor");
        if (sav is SAV4Sinnoh or SAV8BS)
            ids.Add("B_OpenUGSEditor");
        if (sav is SAV4)
            ids.Add("B_OpenGeonetEditor");
        if (sav is SAV5)
        {
            ids.Add("B_OpenUnityTowerEditor");
            ids.Add("B_OpenGlobalLink");
        }
        if (sav is SAV5B2W2)
        {
            ids.Add("B_OpenJoinAvenueEditor");
            ids.Add("B_OpenMedalsEditor");
        }
        if (sav is SAV4 or SAV5)
            ids.Add("B_OpenChatterEditor");
        if (sav is SAV3 or SAV6XY)
            ids.Add("B_Roamer");
        if (sav is SAV2 or SAV2Stadium or SAV3 or SAV4 or SAV5)
            ids.Add("B_MailBox");
        if (sav is SAV4HGSS)
        {
            ids.Add("B_OpenApricorn");
            ids.Add("B_OpenPokeathlon");
        }
        if (sav is SAV8SWSH or SAV9SV)
            ids.Add("B_Raids");
        if (sav is SAV8SWSH { SaveRevision: >= 1 } or SAV9SV { SaveRevision: >= 1 })
            ids.Add("B_RaidsDLC1");
        if (sav is SAV8SWSH { SaveRevision: >= 2 } or SAV9SV { SaveRevision: >= 2 })
            ids.Add("B_RaidsDLC2");
        if (sav is SAV9SV)
            ids.Add("B_RaidsSevenStar");
        if (sav is SAV1StadiumJ or SAV1Stadium or SAV2Stadium)
            ids.Add("B_OtherSlots");
        if (sav is SAV8BS)
        {
            ids.Add("B_OpenSealStickers");
            ids.Add("B_Poffins");
        }
        if (sav is SAV4BR)
        {
            ids.Add("B_OpenBattlePass");
            ids.Add("B_OpenGear");
        }
        if (sav is SAV9SV or SAV9ZA)
            ids.Add("B_OpenFashion");
        if (sav is SAV4)
            ids.Add("B_ConvertKorean");
        return ids;
    }
}
