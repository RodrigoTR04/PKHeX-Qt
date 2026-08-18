using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Shared load/edit/Save/Cancel lifecycle for accessory PKM editors. Ribbons is the first page; later editors register more.
/// </summary>
public sealed class EntityAccessoryEditor
{
    private readonly PKM _entity;
    private readonly byte[] _snapshot;
    private readonly Dictionary<string, IAccessoryPage> _pages = new(StringComparer.Ordinal);

    private EntityAccessoryEditor(PKM entity)
    {
        _entity = entity;
        _snapshot = entity.Data.ToArray();
        Register(new RibbonPage(entity));
    }

    public static EntityAccessoryEditor Open(PKM pk)
    {
        ArgumentNullException.ThrowIfNull(pk);
        return new EntityAccessoryEditor(pk);
    }

    public IReadOnlyCollection<string> PageIds => _pages.Keys;

    public void Register(IAccessoryPage page)
    {
        ArgumentNullException.ThrowIfNull(page);
        ArgumentException.ThrowIfNullOrEmpty(page.Id);
        _pages[page.Id] = page;
    }

    public void SetRibbon(string name, bool value) => Ribbons.SetFlag(name, value);

    public void SetAffixed(sbyte value) => Ribbons.SetAffixed(value);

    public void Modify(string action) => Ribbons.Modify(action);

    public void ApplyJson(string json) => Ribbons.ApplyJson(json);

    public string ToJson() => Ribbons.ToJson();

    public void Save()
    {
        foreach (var page in _pages.Values)
            page.Save(_entity);
        _entity.RefreshChecksum();
    }

    public void Discard() => _snapshot.CopyTo(_entity.Data);

    private RibbonPage Ribbons =>
        _pages.TryGetValue("ribbons", out var page) && page is RibbonPage ribbons
            ? ribbons
            : throw new InvalidOperationException("Ribbons page is not registered.");
}
