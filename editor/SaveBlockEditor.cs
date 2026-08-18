using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Shared load/edit/Save/Cancel lifecycle for save-block editors. Simple trainer is the first page.
/// </summary>
public sealed class SaveBlockEditor
{
    private readonly SaveFile _origin;
    private readonly byte[] _snapshot;
    private readonly Dictionary<string, ISaveBlockPage> _pages = new(StringComparer.Ordinal);
    private string _active = "trainer";

    private SaveBlockEditor(SaveFile origin)
    {
        _origin = origin;
        _snapshot = origin.Data.ToArray();
        if (SimpleTrainerPage.Supports(origin))
            Register(new SimpleTrainerPage(origin));
    }

    public static bool Supports(SaveFile sav) => SimpleTrainerPage.Supports(sav);

    public static SaveBlockEditor Open(SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(sav);
        return new SaveBlockEditor(sav);
    }

    public IReadOnlyCollection<string> PageIds => _pages.Keys;

    public void Register(ISaveBlockPage page)
    {
        ArgumentNullException.ThrowIfNull(page);
        ArgumentException.ThrowIfNullOrEmpty(page.Id);
        _pages[page.Id] = page;
    }

    public void Select(string page)
    {
        ArgumentException.ThrowIfNullOrEmpty(page);
        if (!_pages.ContainsKey(page))
            throw new InvalidOperationException($"This save has no '{page}' block page.");
        _active = page;
    }

    public void ApplyJson(string json)
    {
        using var doc = JsonDocument.Parse(json);
        if (doc.RootElement.TryGetProperty("page", out var page) && page.GetString() is { Length: > 0 } id)
            Select(id);
        Active.ApplyJson(json);
    }

    public string ToJson() => Active.ToJson();

    public void Modify(string action) => Active.Modify(action);

    public void Save()
    {
        Active.Save(_origin);
        _origin.State.Edited = true;
    }

    public void Discard() => _snapshot.CopyTo(_origin.Data);

    private ISaveBlockPage Active =>
        _pages.TryGetValue(_active, out var page)
            ? page
            : throw new InvalidOperationException($"Save-block page '{_active}' is not registered.");
}
