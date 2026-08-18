using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

internal sealed class FlagListPage : IAccessoryPage
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly string _id;
    private readonly string _langForm;
    private readonly bool _pair;
    private readonly List<Row> _rows;
    private readonly Action<int, bool, bool> _set;
    private readonly Action<string, PKM> _modify;
    private readonly PKM _pk;
    private bool _secretUnlocked;
    private bool _secretComplete;
    private byte _bag;
    private byte _hits;
    private readonly bool _hasBag;

    private FlagListPage(
        string id,
        string langForm,
        bool pair,
        List<Row> rows,
        Action<int, bool, bool> set,
        Action<string, PKM> modify,
        PKM pk,
        bool hasBag = false,
        bool secretUnlocked = false,
        bool secretComplete = false,
        byte bag = 0,
        byte hits = 0)
    {
        _id = id;
        _langForm = langForm;
        _pair = pair;
        _rows = rows;
        _set = set;
        _modify = modify;
        _pk = pk;
        _hasBag = hasBag;
        _secretUnlocked = secretUnlocked;
        _secretComplete = secretComplete;
        _bag = bag;
        _hits = hits;
    }

    public string Id => _id;
    public string LangForm => _langForm;

    public static FlagListPage Tech(ITechRecord record, PKM pk)
    {
        var names = GameInfo.Strings.Move;
        var indexes = record.Permit.RecordPermitIndexes;
        var rows = new List<Row>(indexes.Length);
        for (var i = 0; i < indexes.Length; i++)
            rows.Add(new Row { Index = i, Label = names[indexes[i]], Flag0 = record.GetMoveRecordFlag(i) });
        return new FlagListPage("tech", "TechRecordEditor", false, rows,
            (i, a, _) => record.SetMoveRecordFlag(i, a),
            (action, entity) =>
            {
                if (action == "B_None")
                    record.ClearRecordFlags();
                else if (action == "B_All")
                    record.SetRecordFlags(entity, TechnicalRecordApplicatorOption.LegalAll);
                else
                    throw new InvalidOperationException($"Unknown tech action '{action}'.");
                for (var i = 0; i < rows.Count; i++)
                    rows[i].Flag0 = record.GetMoveRecordFlag(i);
            }, pk);
    }

    public static FlagListPage Shop(IMoveShop8Mastery shop, PKM pk)
    {
        var names = GameInfo.Strings.Move;
        var indexes = shop.Permit.RecordPermitIndexes;
        var rows = new List<Row>(indexes.Length);
        for (var i = 0; i < indexes.Length; i++)
        {
            rows.Add(new Row
            {
                Index = i,
                Label = names[indexes[i]],
                Flag0 = shop.GetPurchasedRecordFlag(i),
                Flag1 = shop.GetMasteredRecordFlag(i),
            });
        }
        return new FlagListPage("shop", "MoveShopEditor", true, rows,
            (i, purchased, mastered) =>
            {
                shop.SetPurchasedRecordFlag(i, purchased);
                shop.SetMasteredRecordFlag(i, mastered);
            },
            (action, entity) =>
            {
                if (action == "B_None")
                    shop.ClearMoveShopFlags();
                else if (action == "B_All")
                    shop.SetMoveShopFlags(entity);
                else
                    throw new InvalidOperationException($"Unknown shop action '{action}'.");
                for (var i = 0; i < rows.Count; i++)
                {
                    rows[i].Flag0 = shop.GetPurchasedRecordFlag(i);
                    rows[i].Flag1 = shop.GetMasteredRecordFlag(i);
                }
            }, pk);
    }

    public static FlagListPage Plus(IPlusRecord plus, IPermitPlus permit, PKM pk)
    {
        var names = GameInfo.Strings.Move;
        var indexes = permit.PlusMoveIndexes;
        var rows = new List<Row>(indexes.Length);
        for (var i = 0; i < indexes.Length; i++)
            rows.Add(new Row { Index = i, Label = names[indexes[i]], Flag0 = plus.GetMovePlusFlag(i) });
        return new FlagListPage("plus", "PlusRecordEditor", false, rows,
            (i, a, _) => plus.SetMovePlusFlag(i, a),
            (action, entity) =>
            {
                if (action == "B_None")
                    plus.ClearPlusFlags(permit.PlusCountTotal);
                else if (action == "B_All")
                    plus.SetPlusFlags(entity, permit, PlusRecordApplicatorOption.LegalCurrent);
                else
                    throw new InvalidOperationException($"Unknown plus action '{action}'.");
                for (var i = 0; i < rows.Count; i++)
                    rows[i].Flag0 = plus.GetMovePlusFlag(i);
            }, pk);
    }

    public static FlagListPage Medals(ISuperTrainRegimen st, PKM pk)
    {
        var rows = new List<Row>(SuperTrainRegimenExtensions.CountRegimen + SuperTrainRegimenExtensions.CountRegimenDistribution);
        for (var i = 0; i < SuperTrainRegimenExtensions.CountRegimen; i++)
            rows.Add(new Row { Index = i, Label = SuperTrainRegimenExtensions.GetRegimenName(i), Flag0 = st.GetRegimenState(i), Group = "regular" });
        for (var i = 0; i < SuperTrainRegimenExtensions.CountRegimenDistribution; i++)
            rows.Add(new Row { Index = 100 + i, Label = SuperTrainRegimenExtensions.GetRegimenNameDistribution(i), Flag0 = st.GetRegimenStateDistribution(i), Group = "distribution" });
        var pk6 = pk as PK6;
        return new FlagListPage("medals", "SuperTrainingEditor", false, rows,
            (i, value, _) =>
            {
                if (i >= 100)
                    st.SetRegimenStateDistribution(i - 100, value);
                else
                    st.SetRegimenState(i, value);
            },
            (action, _) =>
            {
                if (action is not ("B_All" or "B_None"))
                    throw new InvalidOperationException($"Unknown medal action '{action}'.");
            },
            pk,
            hasBag: pk6 is not null,
            secretUnlocked: st.SecretSuperTrainingUnlocked,
            secretComplete: st.SuperTrainSupremelyTrained,
            bag: pk6?.TrainingBag ?? 0,
            hits: pk6?.TrainingBagHits ?? 0);
    }

    public void SetFlag(int index, bool value)
    {
        var row = _rows.Find(z => z.Index == index) ?? throw new InvalidOperationException($"No flag {index}.");
        row.Flag0 = value;
    }

    public void SetPair(int index, bool purchased, bool mastered)
    {
        var row = _rows.Find(z => z.Index == index) ?? throw new InvalidOperationException($"No flag {index}.");
        row.Flag0 = purchased;
        row.Flag1 = mastered;
    }

    public string ToJson()
    {
        var document = new Document
        {
            Page = _id,
            LangForm = _langForm,
            Kind = _pair ? "pair" : "flags",
            HasBag = _hasBag,
            SecretUnlocked = _secretUnlocked,
            SecretComplete = _secretComplete,
            Bag = _bag,
            Hits = _hits,
            Rows = _rows,
        };
        return JsonSerializer.Serialize(document, JsonOptions);
    }

    public void ApplyJson(string json)
    {
        var document = JsonSerializer.Deserialize<Document>(json, JsonOptions)
            ?? throw new InvalidDataException("Flag document was empty.");
        if (document.Rows is null)
            return;
        foreach (var incoming in document.Rows)
        {
            var row = _rows.Find(z => z.Index == incoming.Index);
            if (row is null)
                continue;
            row.Flag0 = incoming.Flag0;
            row.Flag1 = incoming.Flag1;
        }
        if (document.SecretUnlocked is { } unlocked)
            _secretUnlocked = unlocked;
        if (document.SecretComplete is { } complete)
            _secretComplete = complete;
        if (document.Bag is { } bag)
            _bag = bag;
        if (document.Hits is { } hits)
            _hits = hits;
    }

    public void Modify(string action)
    {
        _modify(action, _pk);
        if (_id != "medals")
            return;
        var all = action == "B_All";
        if (_hasBag)
        {
            _secretUnlocked = all;
            _secretComplete = all;
        }
        foreach (var row in _rows)
            row.Flag0 = all && (row.Group != "distribution" || _secretUnlocked);
    }

    public void Save(PKM pk)
    {
        foreach (var row in _rows)
            _set(row.Index, row.Flag0, row.Flag1);
        if (pk is PK6 pk6 && _hasBag)
        {
            pk6.SecretSuperTrainingUnlocked = _secretUnlocked;
            pk6.SuperTrainSupremelyTrained = _secretComplete;
            pk6.TrainingBag = _bag;
            pk6.TrainingBagHits = _hits;
        }
        else if (pk is ISuperTrainRegimen st)
        {
            st.SecretSuperTrainingUnlocked &= _secretUnlocked;
            st.SuperTrainSupremelyTrained &= _secretComplete;
        }
    }

    private sealed class Document
    {
        public string Page { get; set; } = "";
        public string LangForm { get; set; } = "";
        public string Kind { get; set; } = "flags";
        public bool HasBag { get; set; }
        public bool? SecretUnlocked { get; set; }
        public bool? SecretComplete { get; set; }
        public byte? Bag { get; set; }
        public byte? Hits { get; set; }
        public List<Row>? Rows { get; set; }
    }

    private sealed class Row
    {
        public int Index { get; set; }
        public string Label { get; set; } = "";
        public string Group { get; set; } = "";
        public bool Flag0 { get; set; }
        public bool Flag1 { get; set; }
    }
}
