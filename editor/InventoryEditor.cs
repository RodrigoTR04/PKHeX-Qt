using System.Text.Json;
using System.Text.Json.Serialization;
using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Inventory subform: WinForms <c>SAV_Inventory</c> Save rebuilds clean items via
/// <see cref="InventoryPouch.GetEmpty"/>, compresses empty slots when the game has no New flag,
/// then <see cref="PlayerBag.CopyTo(SaveFile)"/>. HaX lists and quantities stay out.
/// </summary>
public sealed class InventoryEditor
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
    };

    private readonly SaveFile _origin;
    private readonly PlayerBag _bag;
    private readonly string[] _itemlist;
    private readonly InventoryRow[][] _rows;

    private InventoryEditor(SaveFile origin, PlayerBag bag, string[] itemlist, InventoryRow[][] rows)
    {
        _origin = origin;
        _bag = bag;
        _itemlist = itemlist;
        _rows = rows;
        ItemColumnReadOnly = origin is SAV9ZA or SAV9SV;
        var item0 = bag.Pouches.Count == 0 ? null : bag.Pouches[0].Items[0];
        HasFreeSpace = item0 is IItemFreeSpace;
        HasFreeSpaceIndex = item0 is IItemFreeSpaceIndex;
        HasFavorite = item0 is IItemFavorite;
        HasNew = item0 is IItemNewFlag;
        HasNewShop = item0 is IItemNewShopFlag;
        HasHeld = item0 is IItemHeldFlag;
    }

    public bool ItemColumnReadOnly { get; }
    public bool HasFreeSpace { get; }
    public bool HasFreeSpaceIndex { get; }
    public bool HasFavorite { get; }
    public bool HasNew { get; }
    public bool HasNewShop { get; }
    public bool HasHeld { get; }

    public static InventoryEditor Open(SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(sav);
        var bag = sav.Inventory;
        var itemlist = CopyItemNames(sav);
        var rows = new InventoryRow[bag.Pouches.Count][];
        var editor = new InventoryEditor(sav, bag, itemlist, rows);
        editor.LoadAllBags();
        return editor;
    }

    public int IndexOf(InventoryType type)
    {
        for (var i = 0; i < _bag.Pouches.Count; i++)
        {
            if (_bag.Pouches[i].Type == type)
                return i;
        }
        throw new ArgumentOutOfRangeException(nameof(type), type, "No pouch of that type.");
    }

    public int RowCount(int pouch) => _rows[pouch].Length;

    public void SetSlot(int pouch, int row, int itemId, int count)
    {
        var cells = _rows[pouch][row];
        if (!_bag.IsQuantitySane(_bag.Pouches[pouch].Type, itemId, ref count, HasNew))
        {
            cells.ItemId = 0;
            cells.Count = 0;
            return;
        }
        cells.ItemId = itemId;
        cells.Count = count;
    }

    public void Save()
    {
        SetBags();
        _bag.CopyTo(_origin);
        _origin.State.Edited = true;
    }

    public string ToJson() => JsonSerializer.Serialize(ToDocument(), JsonOptions);

    public void ApplyJson(string json)
    {
        ArgumentException.ThrowIfNullOrEmpty(json);
        var document = JsonSerializer.Deserialize<InventoryDocument>(json, JsonOptions)
            ?? throw new InvalidDataException("Inventory document was empty.");
        ApplyDocument(document);
    }

    public void Modify(string action)
    {
        ArgumentException.ThrowIfNullOrEmpty(action);
        var pouch = CurrentPouch;
        if (action.StartsWith("giveAll", StringComparison.Ordinal))
        {
            var items = _bag.Pouches[pouch].GetAllItems().ToArray();
            ModifyPouch(pouch, p => p.GiveAllItems(_bag, items, GiveCount));
            return;
        }
        if (action is "giveNone")
        {
            ModifyPouch(pouch, p => p.RemoveAll());
            return;
        }
        if (action is "giveModify")
        {
            ModifyPouch(pouch, p => p.ModifyAllCount(_bag, GiveCount));
            return;
        }

        ModifyPouch(pouch, action switch
        {
            "mnuSortName" => p => p.SortByName(_itemlist),
            "mnuSortNameReverse" => p => p.SortByName(_itemlist, reverse: true),
            "mnuSortCount" => p => p.SortByCount(),
            "mnuSortCountReverse" => p => p.SortByCount(reverse: true),
            "mnuSortIndex" => p => p.SortByIndex(),
            "mnuSortIndexReverse" => p => p.SortByIndex(reverse: true),
            _ => throw new InvalidOperationException($"Unknown inventory action '{action}'."),
        });
    }

    public int CurrentPouch { get; set; }
    public int GiveCount { get; set; } = 1;

    private void LoadAllBags()
    {
        for (var p = 0; p < _bag.Pouches.Count; p++)
        {
            var pouch = _bag.Pouches[p];
            pouch.Sanitize(_itemlist.Length - 1);
            var valid = pouch.GetAllItems();
            var rows = new InventoryRow[pouch.Items.Length];
            for (var i = 0; i < pouch.Items.Length; i++)
            {
                var item = pouch.Items[i];
                if (item.Index != 0 && !valid.Contains((ushort)item.Index))
                    item = pouch.Items[i] = pouch.GetEmpty();
                rows[i] = ReadRow(item);
            }
            _rows[p] = rows;
        }
        if (_bag.Pouches.Count != 0)
            GiveCount = Math.Max(1, _bag.Pouches[0].MaxCount - 4);
    }

    private void SetBags()
    {
        for (var p = 0; p < _bag.Pouches.Count; p++)
            SetBag(_rows[p], _bag.Pouches[p]);
    }

    private void SetBag(InventoryRow[] rows, InventoryPouch pouch)
    {
        var ctr = 0;
        foreach (var cells in rows)
        {
            var itemId = cells.ItemId;
            if (itemId <= 0 && !HasNew)
                continue;
            var count = cells.Count;
            if (!_bag.IsQuantitySane(pouch.Type, itemId, ref count, HasNew))
                continue;
            var item = pouch.GetEmpty(itemId, count);
            WriteFlags(cells, item);
            pouch.Items[ctr] = item;
            ctr++;
        }
        for (var i = ctr; i < pouch.Items.Length; i++)
            pouch.Items[i] = pouch.GetEmpty();
    }

    private void ModifyPouch(int pouch, Action<InventoryPouch> func)
    {
        var p = _bag.Pouches[pouch];
        SetBag(_rows[pouch], p);
        func(p);
        var valid = p.GetAllItems();
        for (var i = 0; i < p.Items.Length; i++)
        {
            var item = p.Items[i];
            if (item.Index != 0 && !valid.Contains((ushort)item.Index))
                item = p.Items[i] = p.GetEmpty();
            _rows[pouch][i] = ReadRow(item);
        }
    }

    private void ApplyDocument(InventoryDocument document)
    {
        CurrentPouch = document.CurrentPouch;
        if (document.GiveCount > 0)
            GiveCount = document.GiveCount;
        if (document.Pouches is null)
            return;
        for (var p = 0; p < document.Pouches.Count && p < _rows.Length; p++)
        {
            var src = document.Pouches[p].Rows;
            if (src is null)
                continue;
            var dest = _rows[p];
            for (var i = 0; i < src.Count && i < dest.Length; i++)
            {
                var row = src[i];
                dest[i].ItemId = row.ItemId;
                dest[i].Count = row.Count;
                dest[i].Favorite = row.Favorite;
                dest[i].IsNew = row.IsNew;
                dest[i].FreeSpace = row.FreeSpace;
                dest[i].FreeSpaceIndex = row.FreeSpaceIndex;
                dest[i].NewShop = row.NewShop;
                dest[i].Held = row.Held;
                if (!string.IsNullOrEmpty(row.Item))
                    dest[i].ItemId = IndexOfName(row.Item);
            }
        }
    }

    private InventoryDocument ToDocument()
    {
        var pouches = new List<InventoryPouchDto>(_bag.Pouches.Count);
        for (var p = 0; p < _bag.Pouches.Count; p++)
        {
            var pouch = _bag.Pouches[p];
            var disable = pouch.Type is InventoryType.PCItems or InventoryType.FreeSpace
                          && _origin is not SAV8LA;
            pouches.Add(new InventoryPouchDto
            {
                Type = pouch.Type.ToString(),
                MaxCount = pouch.MaxCount,
                GiveDisabled = disable,
                Choices = [.. GetStringsForPouch(pouch.GetAllItems())],
                Rows = _rows[p].Select(r => new InventoryRowDto
                {
                    ItemId = r.ItemId,
                    Item = NameOf(r.ItemId),
                    Count = r.Count,
                    Favorite = r.Favorite,
                    IsNew = r.IsNew,
                    FreeSpace = r.FreeSpace,
                    FreeSpaceIndex = r.FreeSpaceIndex,
                    NewShop = r.NewShop,
                    Held = r.Held,
                }).ToList(),
            });
        }
        return new InventoryDocument
        {
            ItemColumnReadOnly = ItemColumnReadOnly,
            HasFavorite = HasFavorite,
            HasNew = HasNew,
            HasFreeSpace = HasFreeSpace,
            HasFreeSpaceIndex = HasFreeSpaceIndex,
            HasNewShop = HasNewShop,
            HasHeld = HasHeld,
            CurrentPouch = CurrentPouch,
            GiveCount = GiveCount,
            Pouches = pouches,
        };
    }

    private InventoryRow ReadRow(InventoryItem item) => new()
    {
        ItemId = item.Index,
        Count = item.Count,
        FreeSpace = item is IItemFreeSpace f && f.IsFreeSpace,
        FreeSpaceIndex = item is IItemFreeSpaceIndex fi ? fi.FreeSpaceIndex : 0,
        Favorite = item is IItemFavorite v && v.IsFavorite,
        IsNew = item is IItemNewFlag n && n.IsNew,
        NewShop = item is IItemNewShopFlag ns && ns.IsNewShop,
        Held = item is IItemHeldFlag g && g.IsHeld,
    };

    private static void WriteFlags(InventoryRow cells, InventoryItem item)
    {
        if (item is IItemFreeSpace f)
            f.IsFreeSpace = cells.FreeSpace;
        if (item is IItemFreeSpaceIndex fi)
            fi.FreeSpaceIndex = cells.FreeSpaceIndex;
        if (item is IItemFavorite v)
            v.IsFavorite = cells.Favorite;
        if (item is IItemNewFlag n)
            n.IsNew = cells.IsNew;
        if (item is IItemNewShopFlag ns)
            ns.IsNewShop = cells.NewShop;
        if (item is IItemHeldFlag g)
            g.IsHeld = cells.Held;
    }

    private string[] GetStringsForPouch(ReadOnlySpan<ushort> items)
    {
        var result = new string[items.Length + 1];
        for (var i = 0; i < result.Length - 1; i++)
            result[i] = NameOf(items[i]);
        result[items.Length] = NameOf(0);
        Array.Sort(result);
        return result;
    }

    private string NameOf(int itemId)
    {
        if ((uint)itemId < (uint)_itemlist.Length)
            return _itemlist[itemId];
        return $"(Item #{itemId:000})";
    }

    private int IndexOfName(string name)
    {
        var index = Array.IndexOf(_itemlist, name);
        return index < 0 ? 0 : index;
    }

    private static string[] CopyItemNames(SaveFile sav)
    {
        var itemlist = GameInfo.Strings.GetItemStrings(sav.Context, sav.Version).ToArray();
        for (var i = 0; i < itemlist.Length; i++)
        {
            if (string.IsNullOrEmpty(itemlist[i]))
                itemlist[i] = $"(Item #{i:000})";
        }
        return itemlist;
    }

    private sealed class InventoryRow
    {
        public int ItemId;
        public int Count;
        public bool Favorite;
        public bool IsNew;
        public bool FreeSpace;
        public uint FreeSpaceIndex;
        public bool NewShop;
        public bool Held;
    }

    private sealed class InventoryDocument
    {
        public bool ItemColumnReadOnly { get; set; }
        public bool HasFavorite { get; set; }
        public bool HasNew { get; set; }
        public bool HasFreeSpace { get; set; }
        public bool HasFreeSpaceIndex { get; set; }
        public bool HasNewShop { get; set; }
        public bool HasHeld { get; set; }
        public int CurrentPouch { get; set; }
        public int GiveCount { get; set; }
        public List<InventoryPouchDto>? Pouches { get; set; }
    }

    private sealed class InventoryPouchDto
    {
        public string Type { get; set; } = "";
        public int MaxCount { get; set; }
        public bool GiveDisabled { get; set; }
        public List<string>? Choices { get; set; }
        public List<InventoryRowDto>? Rows { get; set; }
    }

    private sealed class InventoryRowDto
    {
        public int ItemId { get; set; }
        public string? Item { get; set; }
        public int Count { get; set; }
        public bool Favorite { get; set; }
        public bool IsNew { get; set; }
        public bool FreeSpace { get; set; }
        public uint FreeSpaceIndex { get; set; }
        public bool NewShop { get; set; }
        public bool Held { get; set; }
    }
}
