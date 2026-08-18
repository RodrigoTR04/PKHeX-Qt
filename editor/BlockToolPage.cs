using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

internal sealed class BlockToolPage : ISaveBlockPage
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly string _id;
    private readonly string _langForm;
    private readonly List<Field> _fields;
    private readonly Dictionary<string, Action> _actions;
    private readonly Action _flush;

    public BlockToolPage(string id, string langForm, List<Field> fields, Dictionary<string, Action> actions, Action? flush = null)
    {
        _id = id;
        _langForm = langForm;
        _fields = fields;
        _actions = actions;
        _flush = flush ?? (() => { });
    }

    public string Id => _id;
    public string LangForm => _langForm;

    public string ToJson()
    {
        foreach (var field in _fields)
            field.Reload();
        return JsonSerializer.Serialize(new Document
        {
            Page = _id,
            LangForm = _langForm,
            Kind = "fields",
            Fields = [.. _fields.Select(z => new FieldDto { Name = z.Name, Label = z.Label, Value = z.Value })],
            Actions = [.. _actions.Keys],
        }, JsonOptions);
    }

    public void ApplyJson(string json)
    {
        var document = JsonSerializer.Deserialize<Document>(json, JsonOptions)
            ?? throw new InvalidDataException("Block document was empty.");
        if (document.Fields is null)
            return;
        foreach (var incoming in document.Fields)
        {
            var field = _fields.Find(z => z.Name == incoming.Name);
            if (field is not null)
                field.Value = incoming.Value;
        }
    }

    public void Modify(string action)
    {
        if (!_actions.TryGetValue(action, out var run))
            throw new InvalidOperationException($"Unknown block action '{action}'.");
        run();
        foreach (var field in _fields)
            field.Reload();
    }

    public void Save(SaveFile sav)
    {
        _ = sav;
        foreach (var field in _fields)
            field.Commit();
        _flush();
    }

    public void Discard() => _flush();

    internal sealed class Field
    {
        public string Name { get; set; } = "";
        public string Label { get; set; } = "";
        public uint Value { get; set; }
        public required Action<uint> Set { get; init; }
        public required Func<uint> Get { get; init; }
        public void Reload() => Value = Get();
        public void Commit() => Set(Value);
    }

    private sealed class FieldDto
    {
        public string Name { get; set; } = "";
        public string Label { get; set; } = "";
        public uint Value { get; set; }
    }

    private sealed class Document
    {
        public string Page { get; set; } = "";
        public string LangForm { get; set; } = "";
        public string Kind { get; set; } = "fields";
        public List<FieldDto>? Fields { get; set; }
        public List<string>? Actions { get; set; }
    }
}
