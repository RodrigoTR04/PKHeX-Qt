using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Load a save, apply named operations, and export bytes. The Host is not required.
/// </summary>
public sealed class EditorSession
{
    private readonly SaveFile _sav;
    private readonly Dictionary<string, Action<SaveFile>> _operations = new(StringComparer.Ordinal);

    public byte[] CurrentEntity { get; private set; } = [];

    public int BoxCount => _sav.BoxCount;
    public int BoxSlotCount => _sav.BoxSlotCount;
    public int PartySlotCount => 6;
    public int CurrentBox
    {
        get => _sav.CurrentBox;
        set => _sav.CurrentBox = value;
    }

    private PKM? _current;
    private bool _partyOrigin;
    private bool _bound;
    private int _originBox;
    private int _originSlot;
    private byte[] _cleanEntity = [];

    private readonly ArtworkSpriteComposer _sprites = new();
    private InventoryEditor? _inventory;
    private PokedexEditor? _pokedex;
    private EntityAccessoryEditor? _accessory;

    private EditorSession(SaveFile sav)
    {
        _sav = sav;
        _sav.State.Edited = false;
    }

    public bool SaveEdited => _sav.State.Edited;
    public bool IsExportable => _sav.State.Exportable;
    public string? LoadedPath => string.IsNullOrEmpty(_sav.Metadata.FilePath) ? null : _sav.Metadata.FilePath;
    public string SuggestedBackupName => PathUtil.CleanFileName(_sav.Metadata.BAKName);

    public bool EntityUnsaved =>
        _cleanEntity.AsSpan().ContainsAnyExcept((byte)0)
        && !CurrentEntity.AsSpan().SequenceEqual(_cleanEntity);

    public bool NeedsClosePrompt => SaveEdited || EntityUnsaved;
    public bool NeedsOpenPrompt => SaveEdited;
    public bool NeedsExportPrompt(bool checkUnsavedEntity) => checkUnsavedEntity && EntityUnsaved;

    public static EditorSession Load(ReadOnlyMemory<byte> save)
    {
        var data = save.ToArray();
        if (!SaveUtil.TryGetSaveFile(data, out var sav))
            throw new InvalidDataException("Unrecognized save file.");
        return new EditorSession(sav);
    }

    public static EditorSession Blank(GameVersion version)
        => new(BlankSaveFile.Get(version));

    public static EditorSession FromSave(SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(sav);
        return new EditorSession(sav);
    }

    public static EditorSession OpenDropped(EditorSession? current, string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        if (SaveUtil.TryGetSaveFile(path, out var sav))
            return new EditorSession(sav);
        if (current is null)
            throw new InvalidDataException("Unrecognized save file.");
        current.ImportEntityFromPath(path);
        return current;
    }

    public void Register(string name, Action<SaveFile> operation)
    {
        ArgumentException.ThrowIfNullOrEmpty(name);
        ArgumentNullException.ThrowIfNull(operation);
        _operations[name] = operation;
    }

    public InventoryEditor Inventory =>
        _inventory ?? throw new InvalidOperationException("Inventory is not open.");

    public void OpenInventory() => _inventory = InventoryEditor.Open(_sav);

    public void SaveInventory()
    {
        Inventory.Save();
        _inventory = null;
    }

    public void CancelInventory() => _inventory = null;

    public string InventoryDocument() => InventoryEditor.Open(_sav).ToJson();

    public string InventoryModify(string action, string json)
    {
        ArgumentException.ThrowIfNullOrEmpty(action);
        var editor = InventoryEditor.Open(_sav);
        editor.ApplyJson(json);
        editor.Modify(action);
        return editor.ToJson();
    }

    public void SaveInventoryDocument(string json)
    {
        var editor = InventoryEditor.Open(_sav);
        editor.ApplyJson(json);
        editor.Save();
        _inventory = null;
    }

    public PokedexEditor Pokedex =>
        _pokedex ?? throw new InvalidOperationException("Pokédex is not open.");

    public bool HasPokedexEditor => PokedexEditor.Supports(_sav);

    public void OpenPokedex() => _pokedex = PokedexEditor.Open(_sav);

    public void SavePokedex()
    {
        Pokedex.Save();
        _pokedex = null;
    }

    public void CancelPokedex()
    {
        _pokedex?.Discard();
        _pokedex = null;
    }

    public string PokedexDocument()
    {
        OpenPokedex();
        return Pokedex.ToJson();
    }

    public string PokedexModify(string action, string json)
    {
        ArgumentException.ThrowIfNullOrEmpty(action);
        Pokedex.ApplyJson(json);
        Pokedex.Bulk(action);
        return Pokedex.ToJson();
    }

    public void SavePokedexDocument(string json)
    {
        Pokedex.ApplyJson(json);
        SavePokedex();
    }

    public bool HasAccessoryEditor => _current is { Format: >= 3 };

    public EntityAccessoryEditor Accessory =>
        _accessory ?? throw new InvalidOperationException("Accessory editor is not open.");

    public void OpenAccessory() => _accessory = EntityAccessoryEditor.Open(RequireCurrent());

    public void SaveAccessory()
    {
        Accessory.Save();
        CurrentEntity = RequireCurrent().Data.ToArray();
        _accessory = null;
    }

    public void CancelAccessory()
    {
        _accessory?.Discard();
        if (_current is not null)
            CurrentEntity = _current.Data.ToArray();
        _accessory = null;
    }

    public string AccessoryDocument() => AccessoryDocument("ribbons");

    public string AccessoryDocument(string page)
    {
        OpenAccessory();
        Accessory.Select(page);
        return Accessory.ToJson();
    }

    public string AccessoryPages() => string.Join('\n', EntityAccessoryEditor.PageIdsFor(RequireCurrent()));

    public string AccessoryModify(string action, string json)
    {
        ArgumentException.ThrowIfNullOrEmpty(action);
        Accessory.ApplyJson(json);
        Accessory.Modify(action);
        return Accessory.ToJson();
    }

    public void SaveAccessoryDocument(string json)
    {
        Accessory.ApplyJson(json);
        SaveAccessory();
    }

    public byte[] Export() => Export([]);

    public byte[] Export(IReadOnlyList<EditorOperation> operations)
    {
        ArgumentNullException.ThrowIfNull(operations);
        foreach (var operation in operations)
            Apply(operation);
        return _sav.Write().ToArray();
    }

    private void Apply(EditorOperation operation)
    {
        ArgumentNullException.ThrowIfNull(operation);
        if (!_operations.TryGetValue(operation.Name, out var apply))
            throw new InvalidOperationException($"No operation registered as '{operation.Name}'.");
        apply(_sav);
    }

    public void SelectBoxSlot(int box, int slot)
    {
        var pk = _sav.GetBoxSlotAtIndex(box, slot);
        LoadCurrent(pk, party: false, box, slot);
    }

    public void SelectPartySlot(int slot)
    {
        var pk = _sav.GetPartySlotAtIndex(slot);
        LoadCurrent(pk, party: true, 0, slot);
    }

    public string GetField(string name)
    {
        ArgumentException.ThrowIfNullOrEmpty(name);
        return EntityFields.Get(RequireCurrent(), _sav, name);
    }

    public void SetField(string name, string value)
    {
        ArgumentException.ThrowIfNullOrEmpty(name);
        ArgumentNullException.ThrowIfNull(value);
        var pk = RequireCurrent();
        EntityFields.Set(pk, _sav, name, value);
        CurrentEntity = pk.Data.ToArray();
    }

    public string FieldChoices(string name)
    {
        ArgumentException.ThrowIfNullOrEmpty(name);
        return EntityFields.Choices(RequireCurrent(), _sav, name);
    }

    public void CommitCurrent()
    {
        var pk = RequireCurrent();
        pk.FixMoves();
        pk.RefreshChecksum();
        CurrentEntity = pk.Data.ToArray();
        if (!_bound)
            return;
        if (_partyOrigin)
            _sav.SetPartySlotAtIndex(pk, _originSlot);
        else
            _sav.SetBoxSlotAtIndex(pk, _originBox, _originSlot);
        _sav.State.Edited = true;
        MarkEntityClean();
    }

    public void MarkClean()
    {
        _sav.State.Edited = false;
        if (_current is not null)
            MarkEntityClean();
    }

    public void SetLoadedPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        _sav.Metadata.SetExtraInfo(path);
    }

    public string BackupFileName(string destDir)
    {
        ArgumentException.ThrowIfNullOrEmpty(destDir);
        return _sav.Metadata.GetBackupFileName(destDir);
    }

    public bool TryCopyOpenBackup(string destDir)
    {
        ArgumentException.ThrowIfNullOrEmpty(destDir);
        if (!_sav.State.Exportable)
            return false;
        var dest = _sav.Metadata.GetBackupFileName(destDir);
        if (File.Exists(dest))
            return false;
        var src = _sav.Metadata.FilePath;
        if (string.IsNullOrEmpty(src) || !File.Exists(src))
            return false;
        File.Copy(src, dest, overwrite: true);
        return true;
    }

    public bool LegalityValid
    {
        get
        {
            var la = AnalyzeCurrent();
            return la is { Valid: true };
        }
    }

    public string LegalityReport(bool verbose = false)
    {
        var la = AnalyzeCurrent() ?? throw new InvalidOperationException("No entity is selected.");
        return la.Report("en", verbose);
    }

    private void LoadCurrent(PKM pk, bool party, int box, int slot)
    {
        _current = pk;
        _partyOrigin = party;
        _bound = true;
        _originBox = box;
        _originSlot = slot;
        MarkEntityClean();
    }

    public void LoadTemplate(string templatePath)
    {
        var pk = _sav.LoadTemplate(templatePath);
        if (pk.Data.SequenceEqual(_sav.BlankPKM.Data))
            EntityTemplates.TemplateFields(pk, _sav);
        _current = pk;
        _bound = false;
        _partyOrigin = false;
        MarkEntityClean();
    }

    private void MarkEntityClean()
    {
        CurrentEntity = RequireCurrent().Data.ToArray();
        _cleanEntity = CurrentEntity.ToArray();
    }

    private PKM RequireCurrent()
        => _current ?? throw new InvalidOperationException("No entity is selected.");

    private LegalityAnalysis? AnalyzeCurrent()
    {
        if (_current is null || _current.Species == 0)
            return null;
        return new LegalityAnalysis(_current, _sav.Personal);
    }

    public byte[] ComposeBoxSlotPng(int box, int slot)
        => _sprites.Compose(_sav.GetBoxSlotAtIndex(box, slot), _sav);

    public byte[] ComposePartySlotPng(int slot)
        => _sprites.Compose(_sav.GetPartySlotAtIndex(slot), _sav);

    public byte[] ComposeWallpaperPng(int box)
        => _sprites.ComposeWallpaper(_sav, box);

    public string ExportShowdown()
        => ShowdownParsing.GetShowdownText(RequireCurrent());

    public string ExportPartyShowdown()
        => ShowdownParsing.GetShowdownSets(_sav.PartyData, Environment.NewLine + Environment.NewLine);

    public string ExportCurrentBoxShowdown()
        => ShowdownParsing.GetShowdownSets(_sav.GetBoxData(CurrentBox), Environment.NewLine + Environment.NewLine);

    public string PreviewShowdown(string text)
    {
        ArgumentNullException.ThrowIfNull(text);
        var set = FirstSet(text);
        return set.Species == 0 ? string.Empty : set.GetText(BattleTemplateExportSettings.Showdown);
    }

    public void ImportShowdown(string text)
    {
        ArgumentException.ThrowIfNullOrEmpty(text);
        var set = FirstSet(text);
        if (set.Species == 0)
            throw new InvalidDataException("Set data not found.");
        var pk = RequireCurrent();
        pk.ApplySetDetails(set);
        CurrentEntity = pk.Data.ToArray();
    }

    public byte[] ExportEntity()
    {
        var pk = RequireCurrent();
        pk.RefreshChecksum();
        var data = new byte[pk.SIZE_PARTY];
        pk.WriteDecryptedDataParty(data);
        return data;
    }

    public string EntityFileName => RequireCurrent().FileName;

    public void ImportEntity(ReadOnlyMemory<byte> data, string extension = "")
        => ImportConverted(ConvertIncoming(data, extension));

    private static ShowdownSet FirstSet(string text)
        => BattleTemplateTeams.TryGetSets(text).FirstOrDefault() ?? new ShowdownSet(string.Empty);

    public void DropEntityOnSlot(bool party, int box, int slot, ReadOnlyMemory<byte> data, string extension = "")
        => WriteToSlot(ConvertIncoming(data, extension), party, box, slot);

    public void WriteCurrentToSlot(bool party, int box, int slot)
    {
        WriteToSlot(RequireCurrent().Clone(), party, box, slot);
        MarkEntityClean();
    }

    public void DeleteSlot(bool party, int box, int slot)
        => WriteToSlot(_sav.BlankPKM, party, box, slot);

    public void SwapSlots(bool partyA, int boxA, int slotA, bool partyB, int boxB, int slotB)
    {
        var a = ReadSlot(partyA, boxA, slotA).Clone();
        var b = ReadSlot(partyB, boxB, slotB).Clone();
        WriteToSlot(b, partyA, boxA, slotA);
        WriteToSlot(a, partyB, boxB, slotB);
    }

    public string SlotPreview(bool party, int box, int slot)
    {
        var pk = ReadSlot(party, box, slot);
        if (pk.Species == 0)
            return string.Empty;
        return ShowdownParsing.GetLocalizedPreviewText(pk, BattleTemplateExportSettings.Showdown);
    }

    public bool QrHasBoxSlotCopies => RequireCurrent() is PK7;

    public string ExportQrMessage(int box = 0, int slot = 0, int copies = 1)
    {
        var pk = RequireCurrent();
        pk.RefreshChecksum();
        return QrCodec.Message(pk, box, slot, copies);
    }

    public byte[] ExportQrPng(int box = 0, int slot = 0, int copies = 1)
        => QrCodec.EncodePng(ExportQrMessage(box, slot, copies));

    public void ImportQrMessage(string message)
    {
        ArgumentException.ThrowIfNullOrEmpty(message);
        if (TryPayloadBytes(message, out var payload))
        {
            ImportConverted(ImportQrPayload(payload));
            return;
        }

        var pk = QRMessageUtil.GetPKM(message, _sav.Context)
            ?? throw new InvalidDataException("Decoded data not a valid PKM/Gift.");
        ImportConverted(pk);
    }

    private static bool TryPayloadBytes(string message, out byte[] payload)
    {
        payload = [];
        if (!message.StartsWith("null/#", StringComparison.Ordinal) && !message.StartsWith("http", StringComparison.Ordinal))
            return false;
        var hash = message.IndexOf('#');
        if (hash < 0 || hash == message.Length - 1)
            return false;
        payload = Convert.FromBase64String(message[(hash + 1)..]);
        return payload.Length > 0;
    }

    private PKM ImportQrPayload(byte[] payload)
    {
        if (payload.Length == _sav.SIZE_STORED)
            return _sav.GetStoredSlot(payload);
        if (payload.Length == _sav.SIZE_PARTY)
            return _sav.GetPartySlot(payload);
        return ConvertIncoming(payload, $".{RequireCurrent().Extension}");
    }

    public void ImportQrPng(ReadOnlyMemory<byte> png)
        => ImportQrMessage(QrCodec.DecodePng(png));

    public string SlotCryPath(string cryFolder, bool party, int box, int slot)
    {
        var pk = ReadSlot(party, box, slot);
        return CryPath.ExistingWav(pk.Species, pk.Form, pk.Context, cryFolder);
    }

    public void ImportEntityFromPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var pk = FileUtil.GetSingleFromPath(path, _sav)
            ?? throw new InvalidDataException("Unrecognized Pokémon file.");
        ImportConverted(pk);
    }

    private PKM ReadSlot(bool party, int box, int slot)
        => party ? _sav.GetPartySlotAtIndex(slot) : _sav.GetBoxSlotAtIndex(box, slot);

    private void WriteToSlot(PKM pk, bool party, int box, int slot)
    {
        pk.FixMoves();
        pk.RefreshChecksum();
        if (party)
            _sav.SetPartySlotAtIndex(pk, slot);
        else
            _sav.SetBoxSlotAtIndex(pk, box, slot);
        _sav.State.Edited = true;
    }

    private PKM ConvertIncoming(ReadOnlyMemory<byte> data, string extension)
    {
        if (data.Length == 0)
            throw new InvalidDataException("Entity data was empty.");
        var ext = string.IsNullOrEmpty(extension) ? $".{(_current?.Extension ?? "pk5")}" : extension;
        if (!FileUtil.TryGetPKM(data.ToArray(), out var pk, ext, _sav))
        {
            pk = EntityFormat.GetFromBytes(data.ToArray());
            if (pk is null)
                throw new InvalidDataException("Unrecognized Pokémon file.");
        }
        return ConvertToSave(pk);
    }

    private void ImportConverted(PKM pk)
        => LoadCurrent(ConvertToSave(pk), _partyOrigin, _originBox, _originSlot);

    private PKM ConvertToSave(PKM pk)
    {
        var converted = EntityConverter.ConvertToType(pk, _sav.PKMType, out _)
            ?? throw new InvalidDataException("Could not convert that Pokémon to this save.");
        if (ReferenceEquals(pk, converted))
            _sav.AdaptToSaveFile(converted);
        return converted;
    }
}
