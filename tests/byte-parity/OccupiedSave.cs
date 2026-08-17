using PKHeX.Core;

namespace PKHeX.ByteParity.Tests;

internal static class OccupiedSave
{
    public static (byte[] Save, PKM Entity) WithBoxSlot(Action<PKM>? configure = null, int box = 0, int slot = 0)
    {
        if (!SaveUtil.TryGetSaveFile(GoldenSave.Gen5BlankExport(), out var sav))
            throw new InvalidOperationException("Could not load the Gen5 blank fixture.");

        var pk = sav.BlankPKM;
        pk.Species = (ushort)Species.Bulbasaur;
        pk.SetUnshiny();
        configure?.Invoke(pk);
        pk.RefreshChecksum();
        sav.SetBoxSlotAtIndex(pk, box, slot);
        return (sav.Write().ToArray(), sav.GetBoxSlotAtIndex(box, slot));
    }

    public static (byte[] Save, PKM Entity) WithPartySlot(Action<PKM>? configure = null, int slot = 0)
    {
        if (!SaveUtil.TryGetSaveFile(GoldenSave.Gen5BlankExport(), out var sav))
            throw new InvalidOperationException("Could not load the Gen5 blank fixture.");

        var pk = sav.BlankPKM;
        pk.Species = (ushort)Species.Bulbasaur;
        pk.SetUnshiny();
        configure?.Invoke(pk);
        pk.RefreshChecksum();
        sav.SetPartySlotAtIndex(pk, slot);
        return (sav.Write().ToArray(), sav.GetPartySlotAtIndex(slot));
    }
}
