using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Shared load/edit/Save/Cancel lifecycle for accessory PKM editors. The open format decides which pages exist.
/// </summary>
public sealed class EntityAccessoryEditor
{
    private readonly PKM _entity;
    private readonly byte[] _snapshot;
    private readonly Dictionary<string, IAccessoryPage> _pages = new(StringComparer.Ordinal);
    private string _active = "ribbons";

    private EntityAccessoryEditor(PKM entity)
    {
        _entity = entity;
        _snapshot = entity.Data.ToArray();
        Register(new RibbonPage(entity));
        if (entity.Format >= 6 && entity is not PB7)
            Register(new MemoryPage(entity));
        if (entity.Format is 6 or 7 && entity is not PB7 && entity is ISuperTrainRegimen st)
            Register(FlagListPage.Medals(st, entity));
        if (entity is ITechRecord tech)
            Register(FlagListPage.Tech(tech, entity));
        if (entity is IMoveShop8Mastery shop)
            Register(FlagListPage.Shop(shop, entity));
        if (entity is IPlusRecord plus && entity.PersonalInfo is IPermitPlus permit)
            Register(FlagListPage.Plus(plus, permit, entity));
    }

    public static EntityAccessoryEditor Open(PKM pk)
    {
        ArgumentNullException.ThrowIfNull(pk);
        return new EntityAccessoryEditor(pk);
    }

    public IReadOnlyCollection<string> PageIds => _pages.Keys;

    public static IReadOnlyList<string> PageIdsFor(PKM entity)
    {
        ArgumentNullException.ThrowIfNull(entity);
        var ids = new List<string> { "ribbons" };
        if (entity.Format >= 6 && entity is not PB7)
            ids.Add("memories");
        if (entity.Format is 6 or 7 && entity is not PB7 && entity is ISuperTrainRegimen)
            ids.Add("medals");
        if (entity is ITechRecord)
            ids.Add("tech");
        if (entity is IMoveShop8Mastery)
            ids.Add("shop");
        if (entity is IPlusRecord && entity.PersonalInfo is IPermitPlus)
            ids.Add("plus");
        return ids;
    }

    public void Register(IAccessoryPage page)
    {
        ArgumentNullException.ThrowIfNull(page);
        ArgumentException.ThrowIfNullOrEmpty(page.Id);
        _pages[page.Id] = page;
    }

    public void Select(string page)
    {
        ArgumentException.ThrowIfNullOrEmpty(page);
        if (!_pages.ContainsKey(page))
            throw new InvalidOperationException($"This entity has no '{page}' accessory page.");
        _active = page;
    }

    public void SetRibbon(string name, bool value) => Ribbons.SetFlag(name, value);

    public void SetAffixed(sbyte value) => Ribbons.SetAffixed(value);

    public void SetFlag(int index, bool value) => Flags.SetFlag(index, value);

    public void SetPair(int index, bool purchased, bool mastered) => Flags.SetPair(index, purchased, mastered);

    public void Modify(string action) => Active.Modify(action);

    public void ApplyJson(string json)
    {
        using var doc = JsonDocument.Parse(json);
        if (doc.RootElement.TryGetProperty("page", out var page) && page.GetString() is { Length: > 0 } id)
            Select(id);
        Active.ApplyJson(json);
    }

    public string ToJson() => Active.ToJson();

    public void Save()
    {
        Active.Save(_entity);
        _entity.RefreshChecksum();
    }

    public void Discard() => _snapshot.CopyTo(_entity.Data);

    private IAccessoryPage Active =>
        _pages.TryGetValue(_active, out var page)
            ? page
            : throw new InvalidOperationException($"Accessory page '{_active}' is not registered.");

    private RibbonPage Ribbons =>
        _pages.TryGetValue("ribbons", out var page) && page is RibbonPage ribbons
            ? ribbons
            : throw new InvalidOperationException("Ribbons page is not registered.");

    private FlagListPage Flags =>
        Active as FlagListPage ?? throw new InvalidOperationException("The active page is not a flag list.");
}
