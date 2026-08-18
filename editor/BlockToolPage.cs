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
    private readonly string _title;
    private readonly List<Field> _fields;
    private readonly Dictionary<string, Action> _actions;
    private readonly Action _flush;
    private HashSet<string>? _applied;

    public BlockToolPage(string id, string langForm, List<Field> fields, Dictionary<string, Action> actions, Action? flush = null, string? title = null)
    {
        _id = id;
        _langForm = langForm;
        _title = title ?? langForm;
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
            Title = _title,
            Kind = "fields",
            Fields = [.. _fields.Select(z => z.ToDto())],
            Actions = [.. _actions.Keys],
        }, JsonOptions);
    }

    public void ApplyJson(string json)
    {
        var document = JsonSerializer.Deserialize<Document>(json, JsonOptions)
            ?? throw new InvalidDataException("Block document was empty.");
        if (document.Fields is null)
            return;
        _applied = new HashSet<string>(StringComparer.Ordinal);
        foreach (var incoming in document.Fields)
        {
            var field = _fields.Find(z => z.Name == incoming.Name);
            if (field is null)
                continue;
            field.Apply(incoming);
            _applied.Add(field.Name);
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
        {
            if (_applied is not null && !_applied.Contains(field.Name))
                continue;
            field.Commit();
        }
        _flush();
    }

    public void Discard() => _flush();

    internal sealed class Choice
    {
        public uint Value { get; set; }
        public string Label { get; set; } = "";
    }

    internal sealed class Field
    {
        public string Name { get; set; } = "";
        public string Label { get; set; } = "";
        public string Kind { get; set; } = "number";
        public uint Value { get; set; }
        public string Text { get; set; } = "";
        public int Index { get; set; } = -1;
        public List<Choice>? Choices { get; set; }
        public Func<uint> Get { get; init; } = () => 0;
        public Action<uint> Set { get; init; } = _ => { };
        public Func<string>? GetText { get; init; }
        public Action<string>? SetText { get; init; }

        public void Reload()
        {
            if (GetText is not null)
                Text = GetText();
            else
                Value = Get();
        }

        public void Commit()
        {
            if (SetText is not null)
                SetText(Text);
            else
                Set(Value);
        }

        public FieldDto ToDto() => new()
        {
            Name = Name,
            Label = Label,
            Kind = Kind,
            Value = Value,
            Text = Text,
            Index = Index,
            Choices = Choices,
        };

        public void Apply(FieldDto incoming)
        {
            Value = incoming.Value;
            Text = incoming.Text ?? Text;
        }
    }

    internal sealed class FieldDto
    {
        public string Name { get; set; } = "";
        public string Label { get; set; } = "";
        public string Kind { get; set; } = "number";
        public uint Value { get; set; }
        public string Text { get; set; } = "";
        public int Index { get; set; } = -1;
        public List<Choice>? Choices { get; set; }
    }

    private sealed class Document
    {
        public string Page { get; set; } = "";
        public string LangForm { get; set; } = "";
        public string Title { get; set; } = "";
        public string Kind { get; set; } = "fields";
        public List<FieldDto>? Fields { get; set; }
        public List<string>? Actions { get; set; }
    }
}
