using System.Text.Json;
using PKHeX.Core;
using static PKHeX.Core.MessageStrings;

namespace PKHeX.Editor;

/// <summary>
/// WinForms <c>SAV_Wondercard</c>: clone the save, edit the album, Save copies back, Cancel drops the clone.
/// Empty-slot hijack, Gen4 PGT/PCD conversion, received flags, and Gen5 <c>EndAccess</c> stay here.
/// </summary>
public sealed class WondercardEditor
{
    private static readonly JsonSerializerOptions Json = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly SaveFile _origin;
    private readonly SaveFile _sav;
    private readonly IMysteryGiftStorage _cards;
    private readonly IMysteryGiftFlags? _flags;
    private readonly DataMysteryGift[] _album;
    private readonly List<int> _received = [];
    private DataMysteryGift? _viewed;

    private WondercardEditor(SaveFile origin, SaveFile sav)
    {
        _origin = origin;
        _sav = sav;
        _cards = Provider(sav);
        _album = LoadAlbum(sav, _cards);
        _flags = _cards as IMysteryGiftFlags;
        LoadReceivedFlags();
        _viewed = _album[0];
    }

    public static bool Supports(SaveFile sav)
        => sav is IMysteryGiftStorageProvider && sav.Generation is 4 or 5 or 6 or 7;

    public static WondercardEditor Open(SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(sav);
        if (!Supports(sav))
            throw new NotSupportedException("This save has no wondercard album.");
        return new WondercardEditor(sav, sav.Clone());
    }

    public void Import(byte[] data, string ext)
    {
        ArgumentNullException.ThrowIfNull(data);
        var gift = MysteryGift.GetMysteryGift(data, ext ?? string.Empty)
            ?? throw new InvalidDataException(MsgMysteryGiftInvalid);
        _viewed = gift;
    }

    public void ViewSlot(int index) => _viewed = Slot(index);

    public void SetSlot(int index)
    {
        if (_viewed is not { } gift)
            throw new InvalidOperationException(MsgMysteryGiftSlotNone);
        if (!gift.IsCardCompatible(_sav, out var msg))
            throw new InvalidDataException($"{MsgMysteryGiftSlotFail} {msg}");

        var lastUnfilled = LastUnfilledByType(gift, _album);
        if (lastUnfilled > -1 && lastUnfilled < index)
            index = lastUnfilled;
        if (gift is PCD { IsLockCapsule: true })
            index = 11;

        var other = Slot(index);
        if (gift is PCD { CanConvertToPGT: true } pcd && other is PGT)
            gift = pcd.Gift;
        else if (gift.Type != other.Type)
            throw new InvalidDataException($"{MsgMysteryGiftSlotFail} {gift.Type} != {other.Type}");
        else if (gift is PCD g && (g.IsLockCapsule != (index == 11)))
            throw new InvalidDataException(MsgMysteryGiftSlotFail);

        _album[index] = gift.Clone();
        _viewed = _album[index];
        AddReceived(gift.CardID);
    }

    public void DeleteSlot(int index)
    {
        Slot(index).Clear();
        var i = index;
        while (i < _album.Length - 1)
        {
            if (_album[i + 1].IsEmpty)
                break;
            if (_album[i + 1].Type != _album[i].Type)
                break;
            i++;
            (_album[i], _album[i - 1]) = (_album[i - 1], _album[i]);
        }
    }

    public void SetAllUsed(bool used)
    {
        foreach (var gift in _album)
            gift.GiftUsed = used;
    }

    public void RemoveReceived(int cardId) => _received.Remove(cardId);

    public void Save()
    {
        SaveReceivedFlags();
        SaveReceivedCards();
        _origin.CopyChangesFrom(_sav);
        _origin.State.Edited = true;
    }

    public string ToJson()
    {
        string summary;
        try
        {
            summary = _viewed is null ? string.Empty : string.Join('\n', _viewed.GetDescription());
        }
        catch
        {
            summary = MsgMysteryGiftParseTypeUnknown;
        }

        return JsonSerializer.Serialize(new
        {
            generation = _sav.Generation,
            showUsed = _album[0] is not WR7,
            viewedExt = _viewed is null ? string.Empty : "." + _viewed.Extension,
            viewedData = _viewed is null ? string.Empty : Convert.ToBase64String(_viewed.Write()),
            viewedSummary = summary,
            received = _received.Select(z => z.ToString("0000")).ToArray(),
            slots = _album.Select((g, i) => new
            {
                index = i,
                type = g.Type,
                empty = g.IsEmpty,
                cardId = g.CardID,
                used = g.GiftUsed,
            }).ToArray(),
        }, Json);
    }

    public void Modify(string action, string json)
    {
        ArgumentException.ThrowIfNullOrEmpty(action);
        if (!string.IsNullOrWhiteSpace(json))
            ApplyPayload(json);

        if (action is "usedAll")
        {
            SetAllUsed(true);
            return;
        }
        if (action is "unusedAll")
        {
            SetAllUsed(false);
            return;
        }
        if (action is "import")
            return;

        var split = action.IndexOf(':');
        if (split <= 0 || !int.TryParse(action[(split + 1)..], out var index))
            throw new InvalidOperationException($"Unknown wondercard action '{action}'.");
        var verb = action[..split];
        switch (verb)
        {
            case "view":
                ViewSlot(index);
                break;
            case "set":
                SetSlot(index);
                break;
            case "delete":
                DeleteSlot(index);
                break;
            case "flag":
                RemoveReceived(index);
                break;
            default:
                throw new InvalidOperationException($"Unknown wondercard action '{action}'.");
        }
    }

    private void ApplyPayload(string json)
    {
        using var doc = JsonDocument.Parse(json);
        var root = doc.RootElement;
        if (root.TryGetProperty("viewedData", out var dataEl) && root.TryGetProperty("viewedExt", out var extEl))
        {
            var data = dataEl.GetString();
            var ext = extEl.GetString();
            if (!string.IsNullOrEmpty(data) && !string.IsNullOrEmpty(ext))
                Import(Convert.FromBase64String(data), ext);
        }
        if (root.TryGetProperty("received", out var received) && received.ValueKind == JsonValueKind.Array)
        {
            _received.Clear();
            foreach (var item in received.EnumerateArray())
            {
                var text = item.GetString();
                if (text is not null && int.TryParse(text, out var id))
                    _received.Add(id);
            }
        }
    }

    private DataMysteryGift Slot(int index)
    {
        if ((uint)index >= (uint)_album.Length)
            throw new ArgumentOutOfRangeException(nameof(index));
        return _album[index];
    }

    private void AddReceived(int cardId)
    {
        if (_flags is null || (uint)cardId >= _flags.MysteryGiftReceivedFlagMax)
            return;
        if (!_received.Contains(cardId))
            _received.Add(cardId);
    }

    private void LoadReceivedFlags()
    {
        if (_flags is null)
            return;
        for (var i = 1; i < _flags.MysteryGiftReceivedFlagMax; i++)
        {
            if (_flags.GetMysteryGiftReceivedFlag(i))
                _received.Add(i);
        }
    }

    private void SaveReceivedFlags()
    {
        if (_flags is null)
            return;
        _flags.ClearReceivedFlags();
        foreach (var index in _received)
            _flags.SetMysteryGiftReceivedFlag(index, true);
    }

    private void SaveReceivedCards()
    {
        if (_cards is MysteryBlock4 s4)
        {
            s4.IsDeliveryManActive = _album.Any(g => !g.IsEmpty);
            MysteryBlock4.UpdateSlotPGT(_album, _sav is SAV4HGSS);
            if (_sav is SAV4HGSS hgss)
                hgss.LockCapsuleSlot = (PCD)_album[^1];
        }
        for (var i = 0; i < _cards.GiftCountMax; i++)
            _cards.SetMysteryGift(i, _album[i]);
        if (_cards is MysteryBlock5 s5)
            s5.EndAccess();
    }

    private static IMysteryGiftStorage Provider(SaveFile sav)
    {
        if (sav is IMysteryGiftStorageProvider provider)
            return provider.MysteryGiftStorage;
        throw new ArgumentException("Save file does not support Mystery Gifts.", nameof(sav));
    }

    private static DataMysteryGift[] LoadAlbum(SaveFile sav, IMysteryGiftStorage cards)
    {
        var count = cards.GiftCountMax;
        var size = sav is SAV4HGSS ? count + 1 : count;
        var result = new DataMysteryGift[size];
        for (var i = 0; i < count; i++)
            result[i] = cards.GetMysteryGift(i);
        if (sav is SAV4HGSS s4)
            result[^1] = s4.LockCapsuleSlot;
        return result;
    }

    private static int LastUnfilledByType(DataMysteryGift gift, ReadOnlySpan<DataMysteryGift> album)
    {
        for (var i = 0; i < album.Length; i++)
        {
            var exist = album[i];
            if (!exist.IsEmpty)
                continue;
            if (exist.Type != gift.Type)
                continue;
            return i;
        }
        return -1;
    }
}
