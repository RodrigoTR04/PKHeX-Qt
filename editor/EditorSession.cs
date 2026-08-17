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
    private int _originBox;
    private int _originSlot;

    private readonly ArtworkSpriteComposer _sprites = new();

    private EditorSession(SaveFile sav)
    {
        _sav = sav;
    }

    public static EditorSession Load(ReadOnlyMemory<byte> save)
    {
        var data = save.ToArray();
        if (!SaveUtil.TryGetSaveFile(data, out var sav))
            throw new InvalidDataException("Unrecognized save file.");
        return new EditorSession(sav);
    }

    public static EditorSession OpenDropped(EditorSession? current, string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var bytes = File.ReadAllBytes(path);
        if (SaveUtil.TryGetSaveFile(bytes, out _))
            return Load(bytes);
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
        if (_partyOrigin)
            _sav.SetPartySlotAtIndex(pk, _originSlot);
        else
            _sav.SetBoxSlotAtIndex(pk, _originBox, _originSlot);
        CurrentEntity = pk.Data.ToArray();
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
        _originBox = box;
        _originSlot = slot;
        CurrentEntity = pk.Data.ToArray();
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
        => WriteToSlot(RequireCurrent().Clone(), party, box, slot);

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
