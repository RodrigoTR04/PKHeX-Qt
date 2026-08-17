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
}
