using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

internal sealed class EventFlagsPage : ISaveBlockPage
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly bool[] _flags;
    private readonly ulong[] _work;
    private readonly Action<ulong[]> _commitWork;
    private readonly Action _save;
    private readonly EventLabelCollection _labels;
    private readonly string _version;

    private EventFlagsPage(bool[] flags, ulong[] work, Action<ulong[]> commitWork, Action save, EventLabelCollection labels, string version)
    {
        _flags = flags;
        _work = work;
        _commitWork = commitWork;
        _save = save;
        _labels = labels;
        _version = version;
    }

    public string Id => "flags";
    public string LangForm => "SAV_EventFlags";

    public static bool Supports(SaveFile sav) => TryCreate(sav) is not null;

    public static EventFlagsPage? TryCreate(SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(sav);
        try
        {
            if (sav is SAV2 s2)
            {
                var ws = new EventWorkspace<SAV2, byte>(s2, s2.Version);
                return new EventFlagsPage(
                    ws.Flags,
                    Array.ConvertAll(ws.Values, z => (ulong)z),
                    work =>
                    {
                        for (var i = 0; i < ws.Values.Length; i++)
                            ws.Values[i] = (byte)work[i];
                    },
                    ws.Save,
                    ws.Labels,
                    s2.Version.ToString());
            }

            IEventFlag37? block = sav as IEventFlag37 ?? (sav as IEventFlagProvider37)?.EventWork;
            if (block is null)
                return null;
            var editor = new EventWorkspace<IEventFlag37, ushort>(block, sav.Version);
            return new EventFlagsPage(
                editor.Flags,
                Array.ConvertAll(editor.Values, z => (ulong)z),
                work =>
                {
                    for (var i = 0; i < editor.Values.Length; i++)
                        editor.Values[i] = (ushort)work[i];
                },
                editor.Save,
                editor.Labels,
                sav.Version.ToString());
        }
        catch (ArgumentOutOfRangeException)
        {
            return null;
        }
    }

    public string ToJson()
    {
        var flags = new List<FlagRow>(_labels.Flag.Count);
        foreach (var label in _labels.Flag)
        {
            if ((uint)label.Index >= (uint)_flags.Length)
                continue;
            flags.Add(new FlagRow { Index = label.Index, Name = label.Name, Type = label.Type.ToString(), Value = _flags[label.Index] });
        }
        var work = new List<WorkRow>(_labels.Work.Count);
        foreach (var label in _labels.Work)
        {
            if ((uint)label.Index >= (uint)_work.Length)
                continue;
            work.Add(new WorkRow { Index = label.Index, Name = label.Name, Type = label.Type.ToString(), Value = _work[label.Index] });
        }
        return JsonSerializer.Serialize(new Document
        {
            Page = Id,
            LangForm = LangForm,
            Kind = "flags",
            Version = _version,
            FlagCount = _flags.Length,
            WorkCount = _work.Length,
            CustomFlag = 0,
            CustomFlagValue = _flags.Length > 0 && _flags[0],
            CustomWork = 0,
            CustomWorkValue = _work.Length > 0 ? _work[0] : 0,
            Flags = flags,
            Work = work,
        }, JsonOptions);
    }

    public void ApplyJson(string json)
    {
        var document = JsonSerializer.Deserialize<Document>(json, JsonOptions)
            ?? throw new InvalidDataException("Flag document was empty.");
        if (document.CustomFlag is { } index && (uint)index < (uint)_flags.Length && document.CustomFlagValue is { } custom)
            _flags[index] = custom;
        if (document.CustomWork is { } workIndex && (uint)workIndex < (uint)_work.Length && document.CustomWorkValue is { } workValue)
            _work[workIndex] = workValue;
        if (document.SetFlags is not null)
        {
            foreach (var row in document.SetFlags)
            {
                if ((uint)row.Index < (uint)_flags.Length)
                    _flags[row.Index] = row.Value;
            }
        }
        if (document.Flags is not null)
        {
            foreach (var row in document.Flags)
            {
                if ((uint)row.Index < (uint)_flags.Length)
                    _flags[row.Index] = row.Value;
            }
        }
        if (document.SetWork is not null)
        {
            foreach (var row in document.SetWork)
            {
                if ((uint)row.Index < (uint)_work.Length)
                    _work[row.Index] = row.Value;
            }
        }
        if (document.Work is not null)
        {
            foreach (var row in document.Work)
            {
                if ((uint)row.Index < (uint)_work.Length)
                    _work[row.Index] = row.Value;
            }
        }
    }

    public void Modify(string action)
        => throw new InvalidOperationException($"Unknown flag action '{action}'.");

    public void Discard() { }

    public void Save(SaveFile sav)
    {
        _ = sav;
        _commitWork(_work);
        _save();
    }

    private sealed class Document
    {
        public string Page { get; set; } = "flags";
        public string LangForm { get; set; } = "SAV_EventFlags";
        public string Kind { get; set; } = "flags";
        public string Version { get; set; } = "";
        public int FlagCount { get; set; }
        public int WorkCount { get; set; }
        public int? CustomFlag { get; set; }
        public bool? CustomFlagValue { get; set; }
        public int? CustomWork { get; set; }
        public ulong? CustomWorkValue { get; set; }
        public List<FlagRow>? Flags { get; set; }
        public List<WorkRow>? Work { get; set; }
        public List<FlagRow>? SetFlags { get; set; }
        public List<WorkRow>? SetWork { get; set; }
    }

    private sealed class FlagRow
    {
        public int Index { get; set; }
        public string Name { get; set; } = "";
        public string Type { get; set; } = "";
        public bool Value { get; set; }
    }

    private sealed class WorkRow
    {
        public int Index { get; set; }
        public string Name { get; set; } = "";
        public string Type { get; set; } = "";
        public ulong Value { get; set; }
    }
}
