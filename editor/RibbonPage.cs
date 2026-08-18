using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

internal sealed class RibbonPage : IAccessoryPage
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly PKM _pk;
    private readonly List<RibbonInfo> _ribbons;
    private sbyte _affixed = AffixedRibbon.None;

    public RibbonPage(PKM pk)
    {
        _pk = pk;
        _ribbons = RibbonInfo.GetRibbonInfo(pk);
        if (pk is IRibbonSetAffixed affixed)
            _affixed = affixed.AffixedRibbon;
    }

    public string Id => "ribbons";
    public string LangForm => "RibbonEditor";

    public void SetFlag(string name, bool value)
    {
        var rib = Find(name);
        if (rib.Type is not RibbonValueType.Boolean)
            throw new InvalidOperationException($"{name} is not a flag ribbon.");
        rib.HasRibbon = value;
    }

    public void SetAffixed(sbyte value) => _affixed = value;

    public void Modify(string action)
    {
        switch (action)
        {
            case "B_All":
                foreach (var rib in _ribbons)
                {
                    if (rib.Type is RibbonValueType.Boolean)
                        rib.HasRibbon = true;
                    else
                        rib.RibbonCount = (byte)MaxCount(rib);
                }
                break;
            case "B_None":
                _affixed = AffixedRibbon.None;
                foreach (var rib in _ribbons)
                {
                    rib.HasRibbon = false;
                    rib.RibbonCount = 0;
                }
                break;
            case "B_AllLegal":
                RibbonApplicator.RemoveAllValidRibbons(_pk);
                RibbonApplicator.SetAllValidRibbons(_pk);
                ReloadFromEntity();
                break;
            case "B_NoneLegal":
                RibbonApplicator.RemoveAllValidRibbons(_pk);
                if (_pk is IRibbonSetAffixed affixed)
                    affixed.AffixedRibbon = AffixedRibbon.None;
                ReloadFromEntity();
                break;
            default:
                throw new InvalidOperationException($"Unknown ribbon action '{action}'.");
        }
    }

    public string ToJson()
    {
        var names = GameInfo.Strings.Ribbons;
        var rows = _ribbons.Select(rib => new RibbonRow
        {
            Name = rib.Name,
            Label = names.GetName(rib.Name),
            Type = rib.Type is RibbonValueType.Boolean ? "bool" : "byte",
            HasRibbon = rib.HasRibbon,
            Count = rib.RibbonCount,
            MaxCount = rib.Type is RibbonValueType.Byte ? MaxCount(rib) : 0,
        }).ToList();
        var document = new RibbonDocument
        {
            Page = Id,
            LangForm = LangForm,
            HasAffixed = _pk is IRibbonSetAffixed,
            Affixed = _affixed,
            Ribbons = rows,
            AffixedChoices = AffixedChoices(),
        };
        return JsonSerializer.Serialize(document, JsonOptions);
    }

    public void ApplyJson(string json)
    {
        ArgumentException.ThrowIfNullOrEmpty(json);
        var document = JsonSerializer.Deserialize<RibbonDocument>(json, JsonOptions)
            ?? throw new InvalidDataException("Ribbon document was empty.");
        _affixed = (sbyte)document.Affixed;
        if (document.Ribbons is null)
            return;
        foreach (var row in document.Ribbons)
        {
            var rib = Find(row.Name);
            if (rib.Type is RibbonValueType.Boolean)
                rib.HasRibbon = row.HasRibbon;
            else
                rib.RibbonCount = row.Count;
        }
    }

    public void Save(PKM pk)
    {
        foreach (var rib in _ribbons)
            ReflectUtil.SetValue(pk, rib.Name, rib.Type is RibbonValueType.Boolean ? rib.HasRibbon : rib.RibbonCount);
        if (pk is IRibbonSetAffixed affixed)
            affixed.AffixedRibbon = _affixed;
    }

    private RibbonInfo Find(string name)
        => _ribbons.Find(z => z.Name == name)
           ?? throw new InvalidOperationException($"Ribbon '{name}' is not on this entity.");

    private int MaxCount(RibbonInfo rib)
    {
        var max = rib.MaxCount;
        if (max == 8 && rib.Name is nameof(IRibbonSetMemory6.RibbonCountMemoryBattle) && _pk.Format >= 9)
            max = 7;
        return max;
    }

    private void ReloadFromEntity()
    {
        _ribbons.Clear();
        _ribbons.AddRange(RibbonInfo.GetRibbonInfo(_pk));
        if (_pk is IRibbonSetAffixed affixed)
            _affixed = affixed.AffixedRibbon;
    }

    private List<AffixedChoice> AffixedChoices()
    {
        if (_pk is not IRibbonSetAffixed)
            return [];
        var none = GameInfo.Strings.Move[0];
        var names = GameInfo.Strings.Ribbons;
        var list = new List<AffixedChoice>(AffixedRibbon.Max + 2)
        {
            new() { Text = none, Value = AffixedRibbon.None },
        };
        for (var i = 0; i <= AffixedRibbon.Max; i++)
        {
            var name = names.GetName($"Ribbon{(RibbonIndex)i}");
            list.Add(new AffixedChoice { Text = name, Value = i });
        }
        list.Sort((a, b) =>
        {
            if (a.Value == AffixedRibbon.None)
                return -1;
            if (b.Value == AffixedRibbon.None)
                return 1;
            return string.Compare(a.Text, b.Text, StringComparison.Ordinal);
        });
        return list;
    }

    private sealed class RibbonDocument
    {
        public string Page { get; set; } = "ribbons";
        public string LangForm { get; set; } = "RibbonEditor";
        public bool HasAffixed { get; set; }
        public int Affixed { get; set; } = AffixedRibbon.None;
        public List<AffixedChoice>? AffixedChoices { get; set; }
        public List<RibbonRow>? Ribbons { get; set; }
    }

    private sealed class RibbonRow
    {
        public string Name { get; set; } = "";
        public string Label { get; set; } = "";
        public string Type { get; set; } = "bool";
        public bool HasRibbon { get; set; }
        public byte Count { get; set; }
        public int MaxCount { get; set; }
    }

    private sealed class AffixedChoice
    {
        public string Text { get; set; } = "";
        public int Value { get; set; }
    }
}
