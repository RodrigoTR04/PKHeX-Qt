using System.Runtime.InteropServices;
using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Entry points the C++ Host loads through hostfxr. One session per process; two processes are two instances.
/// </summary>
public static class NativeExports
{
    internal static readonly EditorApp App = new();

    public static int OpenPath(IntPtr arg, int size)
    {
        try
        {
            App.OpenFromPath(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int SavePath(IntPtr arg, int size)
    {
        try
        {
            App.SaveToPath(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int HasSession(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        return App.Session is null ? 0 : 1;
    }

    public static int NeedsClosePrompt(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        return App.NeedsClosePrompt ? 1 : 0;
    }

    public static int NeedsOpenPrompt(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        return App.NeedsOpenPrompt ? 1 : 0;
    }

    public static int NeedsExportPrompt(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        return App.NeedsExportPrompt ? 1 : 0;
    }

    public static int PathIsSave(IntPtr arg, int size)
    {
        try
        {
            return App.PathIsSave(ReadUtf8(arg, size)) ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int SaveUserConfig(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            App.SaveConfig();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int ExportBackup(IntPtr arg, int size)
    {
        try
        {
            App.ExportBackup(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int SaveEntityPath(IntPtr arg, int size)
    {
        try
        {
            App.SaveEntityToPath(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PrepareBackupName(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = App.SuggestedBackupName;
            return EncodingLength(_preparedText);
        }
        catch
        {
            return -1;
        }
    }

    public static int ApplyStartup(IntPtr arg, int size)
    {
        try
        {
            var text = arg == IntPtr.Zero || size <= 0
                ? string.Empty
                : ReadUtf8AllowEmpty(arg, size);
            var args = string.IsNullOrEmpty(text)
                ? Array.Empty<string>()
                : text.Split('\n', StringSplitOptions.RemoveEmptyEntries);
            App.TryStartup(args);
            return App.Session is null ? 1 : 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int TakeBackupPrompt(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        return App.TakeBackupPrompt() ? 1 : 0;
    }

    public static int CreateBackupFolder(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            App.CreateBackupFolder();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int IsExportable(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        return App.IsExportable ? 1 : 0;
    }

    public static int PrepareBackupPath(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        _preparedText = App.Config.BackupDirectory;
        return EncodingLength(_preparedText);
    }

    public static int SelectSlot(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            ParseSlotKey(ReadUtf8(arg, size), out var party, out var box, out var slot);
            if (party)
                session.SelectPartySlot(slot);
            else
                session.SelectBoxSlot(box, slot);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int SetCurrentBox(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            if (!int.TryParse(ReadUtf8(arg, size), out var box))
                return 1;
            session.CurrentBox = box;
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int GetStorageLayout(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            if (arg == IntPtr.Zero || size < 16)
                return 1;
            Span<int> values = stackalloc int[4];
            values[0] = session.BoxCount;
            values[1] = session.BoxSlotCount;
            values[2] = session.PartySlotCount;
            values[3] = session.CurrentBox;
            Marshal.Copy(values.ToArray(), 0, arg, 4);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PrepareSlotPng(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            var key = ReadUtf8(arg, size);
            _preparedPng = key.StartsWith("wallpaper:", StringComparison.Ordinal)
                ? session.ComposeWallpaperPng(int.Parse(key["wallpaper:".Length..], System.Globalization.CultureInfo.InvariantCulture))
                : ComposeSlot(session, key);
            return _preparedPng.Length;
        }
        catch
        {
            _preparedPng = null;
            return -1;
        }
    }

    public static int CopyPreparedPng(IntPtr arg, int size)
    {
        if (_preparedPng is null || arg == IntPtr.Zero || size < _preparedPng.Length)
            return 1;
        Marshal.Copy(_preparedPng, 0, arg, _preparedPng.Length);
        return 0;
    }

    public static int GetField(IntPtr arg, int size)
    {
        try
        {
            _preparedText = RequireSession().GetField(ReadUtf8(arg, size));
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SetField(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('=');
            if (split <= 0)
                return 1;
            RequireSession().SetField(raw[..split], raw[(split + 1)..]);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int CommitCurrent(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            RequireSession().CommitCurrent();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int LegalityValid(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            return RequireSession().LegalityValid ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int PrepareLegalityReport(IntPtr arg, int size)
    {
        try
        {
            var verbose = ReadUtf8(arg, size) == "1";
            _preparedText = RequireSession().LegalityReport(verbose);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int GetChoices(IntPtr arg, int size)
    {
        try
        {
            _preparedText = RequireSession().FieldChoices(ReadUtf8(arg, size));
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int CopyPreparedText(IntPtr arg, int size)
    {
        if (_preparedText is null || arg == IntPtr.Zero)
            return 1;
        var utf8 = System.Text.Encoding.UTF8.GetBytes(_preparedText);
        if (size < utf8.Length)
            return 1;
        Marshal.Copy(utf8, 0, arg, utf8.Length);
        return 0;
    }

    public static int PreviewShowdown(IntPtr arg, int size)
    {
        try
        {
            _preparedText = RequireSession().PreviewShowdown(ReadUtf8(arg, size));
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int ImportShowdown(IntPtr arg, int size)
    {
        try
        {
            RequireSession().ImportShowdown(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PrepareShowdownExport(IntPtr arg, int size)
    {
        try
        {
            var scope = ReadUtf8(arg, size);
            var session = RequireSession();
            _preparedText = scope switch
            {
                "party" => session.ExportPartyShowdown(),
                "box" => session.ExportCurrentBoxShowdown(),
                _ => session.ExportShowdown(),
            };
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int PrepareEntityCopy(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedPng = RequireSession().ExportEntity();
            return _preparedPng.Length;
        }
        catch
        {
            _preparedPng = null;
            return -1;
        }
    }

    public static int PrepareEntityFileName(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = RequireSession().EntityFileName;
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int ImportEntity(IntPtr arg, int size)
    {
        try
        {
            if (arg == IntPtr.Zero || size <= 0)
                return 1;
            var data = new byte[size];
            Marshal.Copy(arg, data, 0, size);
            RequireSession().ImportEntity(data);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int SlotPreview(IntPtr arg, int size)
    {
        try
        {
            ParseSlotKey(ReadUtf8(arg, size), out var party, out var box, out var slot);
            _preparedText = RequireSession().SlotPreview(party, box, slot);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SlotCryPath(IntPtr arg, int size)
    {
        try
        {
            ParseSlotKey(ReadUtf8(arg, size), out var party, out var box, out var slot);
            _preparedText = App.SlotCryPath(party, box, slot);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int QrHasBoxSlotCopies(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            return RequireSession().QrHasBoxSlotCopies ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int PrepareQrMessage(IntPtr arg, int size)
    {
        try
        {
            ParseQrPlacement(ReadUtf8AllowEmpty(arg, size), out var box, out var slot, out var copies);
            _preparedText = RequireSession().ExportQrMessage(box, slot, copies);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int PrepareQrPng(IntPtr arg, int size)
    {
        try
        {
            ParseQrPlacement(ReadUtf8AllowEmpty(arg, size), out var box, out var slot, out var copies);
            _preparedPng = RequireSession().ExportQrPng(box, slot, copies);
            return _preparedPng.Length;
        }
        catch
        {
            _preparedPng = null;
            return -1;
        }
    }

    public static int ImportQrMessage(IntPtr arg, int size)
    {
        try
        {
            RequireSession().ImportQrMessage(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int ImportQrPng(IntPtr arg, int size)
    {
        try
        {
            if (arg == IntPtr.Zero || size <= 0)
                return 1;
            var data = new byte[size];
            Marshal.Copy(arg, data, 0, size);
            RequireSession().ImportQrPng(data);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int WriteCurrentToSlot(IntPtr arg, int size)
    {
        try
        {
            ParseSlotKey(ReadUtf8(arg, size), out var party, out var box, out var slot);
            RequireSession().WriteCurrentToSlot(party, box, slot);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int DeleteSlot(IntPtr arg, int size)
    {
        try
        {
            ParseSlotKey(ReadUtf8(arg, size), out var party, out var box, out var slot);
            RequireSession().DeleteSlot(party, box, slot);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int SwapSlots(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('|');
            if (split <= 0)
                return 1;
            ParseSlotKey(raw[..split], out var partyA, out var boxA, out var slotA);
            ParseSlotKey(raw[(split + 1)..], out var partyB, out var boxB, out var slotB);
            RequireSession().SwapSlots(partyA, boxA, slotA, partyB, boxB, slotB);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int DropOnSlot(IntPtr arg, int size)
    {
        try
        {
            if (arg == IntPtr.Zero || size <= 0)
                return 1;
            var payload = new byte[size];
            Marshal.Copy(arg, payload, 0, size);
            var zero = Array.IndexOf(payload, (byte)0);
            if (zero <= 0 || zero >= payload.Length - 1)
                return 1;
            var key = System.Text.Encoding.UTF8.GetString(payload, 0, zero);
            ParseSlotKey(key, out var party, out var box, out var slot);
            RequireSession().DropEntityOnSlot(party, box, slot, payload.AsMemory(zero + 1));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PrepareInventory(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = RequireSession().InventoryDocument();
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int InventoryModify(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('\n');
            if (split <= 0)
                return -1;
            _preparedText = RequireSession().InventoryModify(raw[..split], raw[(split + 1)..]);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SaveInventory(IntPtr arg, int size)
    {
        try
        {
            RequireSession().SaveInventoryDocument(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int HasPokedex(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            return RequireSession().HasPokedexEditor ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int PreparePokedex(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = RequireSession().PokedexDocument();
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int PokedexModify(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('\n');
            if (split <= 0)
                return -1;
            _preparedText = RequireSession().PokedexModify(raw[..split], raw[(split + 1)..]);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SavePokedex(IntPtr arg, int size)
    {
        try
        {
            RequireSession().SavePokedexDocument(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int CancelPokedex(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            RequireSession().CancelPokedex();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int HasAccessory(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            return RequireSession().HasAccessoryEditor ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int PrepareAccessory(IntPtr arg, int size)
    {
        try
        {
            var page = size > 0 ? ReadUtf8(arg, size) : "ribbons";
            if (string.IsNullOrWhiteSpace(page) || page == "-")
                page = "ribbons";
            _preparedText = RequireSession().AccessoryDocument(page);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int PrepareAccessoryPages(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = RequireSession().AccessoryPages();
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int AccessoryModify(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('\n');
            if (split <= 0)
                return -1;
            _preparedText = RequireSession().AccessoryModify(raw[..split], raw[(split + 1)..]);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SaveAccessory(IntPtr arg, int size)
    {
        try
        {
            RequireSession().SaveAccessoryDocument(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int CancelAccessory(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            RequireSession().CancelAccessory();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int HasSaveBlock(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            return RequireSession().HasSaveBlockEditor ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int PrepareSaveBlockPages(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = RequireSession().SaveBlockPages();
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int PrepareSaveBlock(IntPtr arg, int size)
    {
        try
        {
            var page = size > 0 ? ReadUtf8(arg, size) : "trainer";
            if (string.IsNullOrWhiteSpace(page) || page == "-")
                page = "trainer";
            _preparedText = RequireSession().SaveBlockDocument(page);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SaveBlockModify(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('\n');
            if (split <= 0)
                return -1;
            _preparedText = RequireSession().SaveBlockModify(raw[..split], raw[(split + 1)..]);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SaveSaveBlock(IntPtr arg, int size)
    {
        try
        {
            RequireSession().SaveSaveBlockDocument(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int CancelSaveBlock(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            RequireSession().CancelSaveBlock();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PrepareBatchProperties(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = SaveBatchEditor.PropertyList();
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int RunBatch(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('\n');
            if (split <= 0)
                return -1;
            var result = RequireSession().RunBatch(raw[..split], raw[(split + 1)..]);
            _preparedText = (result.Ok ? "ok\n" : "err\n") + result.Message;
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int HasBox(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            return RequireSession().HasBox ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int PrepareBoxExport(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = BoxExportWire.Prepare(App.Config);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int ExportBoxes(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('\n');
            if (split <= 0)
                return -1;
            var dest = raw[..split];
            var wire = BoxExportWire.Parse(raw[(split + 1)..]);
            wire.Apply(App.Config);
            var settings = wire.Settings ?? BoxExportSettings.Default;
            var count = RequireSession().ExportBoxes(dest, settings, wire.Namer);
            if (count < 0)
                _preparedText = "err\n" + MessageStrings.MsgSaveBoxExportInvalid;
            else if (settings.Notify == BoxExportNofify.Silent)
                _preparedText = "ok\n";
            else
                _preparedText = "ok\n" + string.Format(MessageStrings.MsgSaveBoxExportPathCount, count) + "\n" + dest;
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SaveBoxExportSettings(IntPtr arg, int size)
    {
        try
        {
            BoxExportWire.Parse(ReadUtf8(arg, size)).Apply(App.Config);
            App.SaveConfig();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PreparePkmDatabase(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            var session = RequireSession();
            session.OpenPkmDatabase(PkmDatabaseOptions.FromConfig(App.Config));
            var hits = session.SearchPkmDatabase(new PkmDatabaseQuery());
            _preparedText = PkmDatabaseWire.Document(session, hits);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SearchPkmDatabase(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            var hits = session.SearchPkmDatabase(PkmDatabaseWire.Parse(ReadUtf8AllowEmpty(arg, size)));
            _preparedText = PkmDatabaseWire.Document(session, hits);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int LoadPkmDatabaseHit(IntPtr arg, int size)
    {
        try
        {
            if (!int.TryParse(ReadUtf8(arg, size), out var index))
                return 1;
            RequireSession().LoadPkmDatabaseHit(index);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    private static byte[]? _preparedPng;
    private static string? _preparedText;

    private static int EncodingLength(string text) => System.Text.Encoding.UTF8.GetByteCount(text);

    private static EditorSession RequireSession()
        => App.Session ?? throw new InvalidOperationException("No save is open.");

    private static byte[] ComposeSlot(EditorSession session, string key)
    {
        ParseSlotKey(key, out var party, out var box, out var slot);
        return party ? session.ComposePartySlotPng(slot) : session.ComposeBoxSlotPng(box, slot);
    }

    private static void ParseSlotKey(string key, out bool party, out int box, out int slot)
    {
        var parts = key.Split(':');
        if (parts.Length == 2 && parts[0] == "party" && int.TryParse(parts[1], out slot))
        {
            party = true;
            box = 0;
            return;
        }

        if (parts.Length == 3 && parts[0] == "box"
            && int.TryParse(parts[1], out box)
            && int.TryParse(parts[2], out slot))
        {
            party = false;
            return;
        }

        throw new FormatException("Slot key was not box:box:slot or party:slot.");
    }

    private static void ParseQrPlacement(string spec, out int box, out int slot, out int copies)
    {
        box = 0;
        slot = 0;
        copies = 1;
        if (string.IsNullOrWhiteSpace(spec))
            return;
        var parts = spec.Split(',');
        if (parts.Length < 3)
            return;
        _ = int.TryParse(parts[0], out box);
        _ = int.TryParse(parts[1], out slot);
        _ = int.TryParse(parts[2], out copies);
        if (copies < 1)
            copies = 1;
    }

    private static string ReadUtf8AllowEmpty(IntPtr arg, int size)
    {
        if (arg == IntPtr.Zero || size <= 0)
            return string.Empty;
        return (Marshal.PtrToStringUTF8(arg, size) ?? string.Empty).TrimEnd('\0');
    }

    private static string ReadUtf8(IntPtr arg, int size)
    {
        if (arg == IntPtr.Zero)
            throw new ArgumentNullException(nameof(arg));
        var path = size > 0
            ? Marshal.PtrToStringUTF8(arg, size)
            : Marshal.PtrToStringUTF8(arg);
        if (string.IsNullOrEmpty(path))
            throw new InvalidOperationException("Path was empty.");
        return path.TrimEnd('\0');
    }
}
