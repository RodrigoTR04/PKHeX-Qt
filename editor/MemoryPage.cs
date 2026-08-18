using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

internal sealed class MemoryPage : IAccessoryPage
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly PKM _pk;
    private Document _doc;

    public MemoryPage(PKM pk)
    {
        _pk = pk;
        _doc = Load(pk);
    }

    public string Id => "memories";
    public string LangForm => "MemoryAmie";

    public string ToJson()
    {
        _doc.Page = Id;
        _doc.LangForm = LangForm;
        _doc.Kind = "memories";
        return JsonSerializer.Serialize(_doc, JsonOptions);
    }

    public void ApplyJson(string json)
    {
        var incoming = JsonSerializer.Deserialize<Document>(json, JsonOptions)
            ?? throw new InvalidDataException("Memory document was empty.");
        if (incoming.OtFriendship is { } otf)
            _doc.OtFriendship = otf;
        if (incoming.HtFriendship is { } htf)
            _doc.HtFriendship = htf;
        if (incoming.OtMemory is { } otm)
            _doc.OtMemory = otm;
        if (incoming.OtVariable is { } otv)
            _doc.OtVariable = otv;
        if (incoming.OtIntensity is { } oti)
            _doc.OtIntensity = oti;
        if (incoming.OtFeeling is { } otfe)
            _doc.OtFeeling = otfe;
        if (incoming.HtMemory is { } htm)
            _doc.HtMemory = htm;
        if (incoming.HtVariable is { } htv)
            _doc.HtVariable = htv;
        if (incoming.HtIntensity is { } hti)
            _doc.HtIntensity = hti;
        if (incoming.HtFeeling is { } htfe)
            _doc.HtFeeling = htfe;
        if (incoming.OtAffection is { } ota)
            _doc.OtAffection = ota;
        if (incoming.HtAffection is { } hta)
            _doc.HtAffection = hta;
        if (incoming.Fullness is { } full)
            _doc.Fullness = full;
        if (incoming.Enjoyment is { } enjoy)
            _doc.Enjoyment = enjoy;
        if (incoming.Sociability is { } soc)
            _doc.Sociability = soc;
    }

    public void Modify(string action)
    {
        if (action != "B_ClearAll")
            throw new InvalidOperationException($"Unknown memory action '{action}'.");
        _doc = Load(_pk);
        _doc.OtFriendship = _pk.OriginalTrainerFriendship;
        _doc.HtFriendship = _pk.HandlingTrainerFriendship;
        _doc.OtMemory = _doc.HtMemory = 0;
        _doc.OtVariable = _doc.HtVariable = 0;
        _doc.OtIntensity = _doc.HtIntensity = 0;
        _doc.OtFeeling = _doc.HtFeeling = 0;
    }

    public void Save(PKM pk)
    {
        pk.OriginalTrainerFriendship = _doc.OtFriendship ?? pk.OriginalTrainerFriendship;
        pk.HandlingTrainerFriendship = _doc.HtFriendship ?? pk.HandlingTrainerFriendship;
        if (pk is IAffection a)
        {
            if (_doc.OtAffection is { } ota)
                a.OriginalTrainerAffection = ota;
            if (_doc.HtAffection is { } hta)
                a.HandlingTrainerAffection = hta;
        }
        if (pk is IFullnessEnjoyment f)
        {
            if (_doc.Fullness is { } full)
                f.Fullness = full;
            if (_doc.Enjoyment is { } enjoy)
                f.Enjoyment = enjoy;
        }
        if (pk is ITrainerMemories m)
        {
            m.OriginalTrainerMemory = _doc.OtMemory ?? m.OriginalTrainerMemory;
            m.OriginalTrainerMemoryVariable = _doc.OtVariable ?? m.OriginalTrainerMemoryVariable;
            m.OriginalTrainerMemoryIntensity = _doc.OtIntensity ?? m.OriginalTrainerMemoryIntensity;
            m.OriginalTrainerMemoryFeeling = _doc.OtFeeling ?? m.OriginalTrainerMemoryFeeling;
            m.HandlingTrainerMemory = _doc.HtMemory ?? m.HandlingTrainerMemory;
            m.HandlingTrainerMemoryVariable = _doc.HtVariable ?? m.HandlingTrainerMemoryVariable;
            m.HandlingTrainerMemoryIntensity = _doc.HtIntensity ?? m.HandlingTrainerMemoryIntensity;
            m.HandlingTrainerMemoryFeeling = _doc.HtFeeling ?? m.HandlingTrainerMemoryFeeling;
        }
        if (pk is ISociability s && _doc.Sociability is { } soc)
            s.Sociability = soc;
        if (pk is IGeoTrack g && _doc.Geo is { Count: 10 })
        {
            g.Geo1_Country = _doc.Geo[0];
            g.Geo1_Region = _doc.Geo[1];
            g.Geo2_Country = _doc.Geo[2];
            g.Geo2_Region = _doc.Geo[3];
            g.Geo3_Country = _doc.Geo[4];
            g.Geo3_Region = _doc.Geo[5];
            g.Geo4_Country = _doc.Geo[6];
            g.Geo4_Region = _doc.Geo[7];
            g.Geo5_Country = _doc.Geo[8];
            g.Geo5_Region = _doc.Geo[9];
        }
    }

    private static Document Load(PKM pk)
    {
        var doc = new Document
        {
            OtFriendship = pk.OriginalTrainerFriendship,
            HtFriendship = pk.HandlingTrainerFriendship,
        };
        if (pk is ITrainerMemories m)
        {
            doc.OtMemory = m.OriginalTrainerMemory;
            doc.OtVariable = m.OriginalTrainerMemoryVariable;
            doc.OtIntensity = m.OriginalTrainerMemoryIntensity;
            doc.OtFeeling = m.OriginalTrainerMemoryFeeling;
            doc.HtMemory = m.HandlingTrainerMemory;
            doc.HtVariable = m.HandlingTrainerMemoryVariable;
            doc.HtIntensity = m.HandlingTrainerMemoryIntensity;
            doc.HtFeeling = m.HandlingTrainerMemoryFeeling;
        }
        if (pk is IAffection a)
        {
            doc.OtAffection = a.OriginalTrainerAffection;
            doc.HtAffection = a.HandlingTrainerAffection;
        }
        if (pk is IFullnessEnjoyment f)
        {
            doc.Fullness = f.Fullness;
            doc.Enjoyment = f.Enjoyment;
        }
        if (pk is ISociability s)
            doc.Sociability = s.Sociability;
        if (pk is IGeoTrack g)
        {
            doc.Geo =
            [
                g.Geo1_Country, g.Geo1_Region,
                g.Geo2_Country, g.Geo2_Region,
                g.Geo3_Country, g.Geo3_Region,
                g.Geo4_Country, g.Geo4_Region,
                g.Geo5_Country, g.Geo5_Region,
            ];
        }
        return doc;
    }

    private sealed class Document
    {
        public string Page { get; set; } = "memories";
        public string LangForm { get; set; } = "MemoryAmie";
        public string Kind { get; set; } = "memories";
        public byte? OtFriendship { get; set; }
        public byte? HtFriendship { get; set; }
        public byte? OtMemory { get; set; }
        public ushort? OtVariable { get; set; }
        public byte? OtIntensity { get; set; }
        public byte? OtFeeling { get; set; }
        public byte? HtMemory { get; set; }
        public ushort? HtVariable { get; set; }
        public byte? HtIntensity { get; set; }
        public byte? HtFeeling { get; set; }
        public byte? OtAffection { get; set; }
        public byte? HtAffection { get; set; }
        public byte? Fullness { get; set; }
        public byte? Enjoyment { get; set; }
        public uint? Sociability { get; set; }
        public List<byte>? Geo { get; set; }
    }
}
