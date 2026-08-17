using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class InventoryTests
{
    private const int Potion = 17;

    [Fact]
    public void cancel_leaves_the_save_unchanged()
    {
        var fixture = GoldenSave.Gen5BlankExport();
        var session = EditorSession.Load(fixture);
        session.OpenInventory();
        var pouch = session.Inventory.IndexOf(InventoryType.Medicine);
        session.Inventory.SetSlot(pouch, session.Inventory.RowCount(pouch) - 1, Potion, 5);
        session.CancelInventory();

        Assert.Equal(fixture, session.Export());
    }

    [Fact]
    public void gen5_save_compresses_empty_slots_and_writes_a_clean_item()
    {
        var fixture = GoldenSave.Gen5BlankExport();
        var session = EditorSession.Load(fixture);
        session.OpenInventory();
        var pouch = session.Inventory.IndexOf(InventoryType.Medicine);
        session.Inventory.SetSlot(pouch, session.Inventory.RowCount(pouch) - 1, Potion, 5);
        session.SaveInventory();

        Assert.Equal(ExpectedPackedMedicine(fixture, Potion, 5), session.Export());
    }

    [Fact]
    public void json_document_save_matches_the_editor_slot_write()
    {
        var fixture = GoldenSave.Gen5BlankExport();
        var session = EditorSession.Load(fixture);
        session.OpenInventory();
        var pouch = session.Inventory.IndexOf(InventoryType.Medicine);
        session.Inventory.SetSlot(pouch, session.Inventory.RowCount(pouch) - 1, Potion, 5);
        var json = session.Inventory.ToJson();
        session.CancelInventory();
        session.SaveInventoryDocument(json);

        Assert.Equal(ExpectedPackedMedicine(fixture, Potion, 5), session.Export());
    }

    [Fact]
    public void gen9_save_rebuilds_pouch_items_and_drops_padding()
    {
        var sav = DirtyGen9PotionPadding(0xA5A5A5A5u);
        Assert.Equal(0xA5A5A5A5u, ReadPotionPadding(sav));

        var expected = DirtyGen9PotionPadding(0xA5A5A5A5u);
        RebuildLikeWinForms(expected);

        var session = EditorSession.FromSave(sav);
        session.OpenInventory();
        session.SaveInventory();

        Assert.Equal(0u, ReadPotionPadding(sav));
        Assert.Equal(expected.Write().ToArray(), sav.Write().ToArray());
    }

    private static SaveFile DirtyGen9PotionPadding(uint padding)
    {
        var sav = GoldenSave.Gen9Blank();
        var bag = sav.Inventory;
        foreach (var item in bag.GetPouch(InventoryType.Medicine).Items)
        {
            if (item.Index == Potion && item is InventoryItem9 n9)
                n9.Padding = padding;
        }
        bag.CopyTo(sav);
        return sav;
    }

    private static uint ReadPotionPadding(SaveFile sav)
    {
        var item = sav.Inventory.GetPouch(InventoryType.Medicine).Items
            .First(i => i.Index == Potion);
        return ((InventoryItem9)item).Padding;
    }

    private static void RebuildLikeWinForms(SaveFile sav)
    {
        var bag = sav.Inventory;
        var item0 = bag.Pouches[0].Items[0];
        var hasNew = item0 is IItemNewFlag;
        Sanitize(sav, bag);
        foreach (var pouch in bag.Pouches)
        {
            var valid = pouch.GetAllItems();
            var ctr = 0;
            for (var i = 0; i < pouch.Items.Length; i++)
            {
                var source = pouch.Items[i];
                if (source.Index != 0 && !valid.Contains((ushort)source.Index))
                    source = pouch.GetEmpty();
                var itemId = source.Index;
                var count = source.Count;
                if (itemId <= 0 && !hasNew)
                    continue;
                if (!bag.IsQuantitySane(pouch.Type, itemId, ref count, hasNew))
                    continue;
                var item = pouch.GetEmpty(itemId, count);
                CopyFlags(source, item);
                pouch.Items[ctr++] = item;
            }
            for (var i = ctr; i < pouch.Items.Length; i++)
                pouch.Items[i] = pouch.GetEmpty();
        }
        bag.CopyTo(sav);
    }

    private static byte[] ExpectedPackedMedicine(byte[] fixture, int itemId, int count)
    {
        if (!SaveUtil.TryGetSaveFile(fixture, out var sav))
            throw new InvalidOperationException("Fixture was not a save.");
        var bag = sav.Inventory;
        Sanitize(sav, bag);
        var pouch = bag.GetPouch(InventoryType.Medicine);
        pouch.Items[0] = pouch.GetEmpty(itemId, count);
        for (var i = 1; i < pouch.Items.Length; i++)
            pouch.Items[i] = pouch.GetEmpty();
        bag.CopyTo(sav);
        return sav.Write().ToArray();
    }

    private static void Sanitize(SaveFile sav, PlayerBag bag)
    {
        var names = GameInfo.Strings.GetItemStrings(sav.Context, sav.Version);
        foreach (var pouch in bag.Pouches)
            pouch.Sanitize(names.Length - 1);
    }

    private static void CopyFlags(InventoryItem source, InventoryItem item)
    {
        if (item is IItemFreeSpace f && source is IItemFreeSpace fs)
            f.IsFreeSpace = fs.IsFreeSpace;
        if (item is IItemFreeSpaceIndex fi && source is IItemFreeSpaceIndex fsi)
            fi.FreeSpaceIndex = fsi.FreeSpaceIndex;
        if (item is IItemFavorite v && source is IItemFavorite fv)
            v.IsFavorite = fv.IsFavorite;
        if (item is IItemNewFlag n && source is IItemNewFlag ns)
            n.IsNew = ns.IsNew;
        if (item is IItemNewShopFlag shop && source is IItemNewShopFlag shops)
            shop.IsNewShop = shops.IsNewShop;
        if (item is IItemHeldFlag held && source is IItemHeldFlag helds)
            held.IsHeld = helds.IsHeld;
    }
}
