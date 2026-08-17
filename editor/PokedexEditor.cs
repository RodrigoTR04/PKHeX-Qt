using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Generation-parameterized Pokédex session. WinForms Save extras live here:
/// Gen 5/6 write <c>InitialSpecies</c>; Gen 7 Seen None walks every species and form row, which Core <c>SeenNone</c> does not.
/// Gen 4 uses <c>ModifyAll</c>; BDSP Seen None is <c>SetAllSeen(false)</c>; blank SV is revision 2 (Kitakami form).
/// </summary>
public sealed class PokedexEditor
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly SaveFile _origin;
    private readonly byte[] _snapshot;
    private readonly IPokedexKind _kind;

    private PokedexEditor(SaveFile origin, byte[] snapshot, IPokedexKind kind)
    {
        _origin = origin;
        _snapshot = snapshot;
        _kind = kind;
        _kind.SelectDefault();
    }

    public static bool Supports(SaveFile sav) => sav is SAV4 or SAV5 or SAV6XY or SAV6AO or SAV7 or SAV7b
        or SAV8SWSH or SAV8BS or SAV8LA or SAV9SV or SAV9ZA;

    public static PokedexEditor Open(SaveFile sav)
    {
        ArgumentNullException.ThrowIfNull(sav);
        IPokedexKind kind = sav switch
        {
            SAV4 s4 => new Gen4Kind(s4),
            SAV5 s5 => new Gen5Kind(s5),
            SAV6XY xy => new Gen6Kind(xy, xy.Zukan, "SAV_PokedexXY"),
            SAV6AO ao => new Gen6Kind(ao, ao.Zukan, "SAV_PokedexORAS"),
            SAV7 s7 => new Gen7Kind(s7, s7.Zukan, "SAV_PokedexSM"),
            SAV7b b7 => new Gen7bKind(b7),
            SAV8SWSH swsh => new CoreZukanKind(swsh, (action, species) => BulkZukan(swsh.Zukan, action, species), "SAV_PokedexSWSH"),
            SAV8BS bs => new BdspKind(bs),
            SAV8LA la => new LegendsKind(la),
            SAV9SV sv => new CoreZukanKind(sv, (action, species) => BulkZukan(sv.Zukan, action, species),
                sv.SaveRevision == 0 ? "SAV_PokedexSV" : "SAV_PokedexSVKitakami"),
            SAV9ZA za => new CoreZukanKind(za, (action, species) => BulkZukan(za.Zukan, action, species), "SAV_Pokedex9a"),
            _ => throw new NotSupportedException("This save has no Pokédex shell adapter yet."),
        };
        return new PokedexEditor(sav, sav.Data.ToArray(), kind);
    }

    public void Discard() => _snapshot.CopyTo(_origin.Data);

    public void SelectSpecies(ushort species) => _kind.SelectSpecies(species);

    public void Bulk(string action)
    {
        ArgumentException.ThrowIfNullOrEmpty(action);
        _kind.Bulk(action);
    }

    public void Save()
    {
        _kind.FlushEntry();
        _kind.CommitSaveExtras();
        _origin.State.Edited = true;
    }

    public string ToJson() => JsonSerializer.Serialize(_kind.ToDocument(), JsonOptions);

    public void ApplyJson(string json)
    {
        ArgumentException.ThrowIfNullOrEmpty(json);
        var document = JsonSerializer.Deserialize<PokedexDocument>(json, JsonOptions)
            ?? throw new InvalidDataException("Pokédex document was empty.");
        _kind.ApplyDocument(document);
    }

    private interface IPokedexKind
    {
        void SelectDefault();
        void SelectSpecies(ushort species);
        void Bulk(string action);
        void FlushEntry();
        void CommitSaveExtras();
        PokedexDocument ToDocument();
        void ApplyDocument(PokedexDocument document);
    }

    private sealed class Gen5Kind : IPokedexKind
    {
        private readonly SAV5 _sav;
        private readonly Zukan5 _dex;
        private ushort _species = 1;

        public Gen5Kind(SAV5 sav)
        {
            _sav = sav;
            _dex = sav.Zukan;
        }

        public void SelectDefault()
        {
            var initial = _dex.InitialSpecies;
            _species = initial is 0 or > 649 ? (ushort)1 : initial;
        }

        public void SelectSpecies(ushort species)
        {
            if (species is 0 or > 649)
                return;
            _species = species;
        }

        public void Bulk(string action)
        {
            var language = (LanguageID)_sav.Language;
            switch (action)
            {
                case "mnuSeenNone":
                    _dex.SeenNone();
                    break;
                case "mnuSeenAll":
                    _dex.SeenAll(shinyToo: false);
                    break;
                case "mnuCaughtNone":
                    _dex.CaughtNone();
                    break;
                case "mnuCaughtAll":
                    _dex.CaughtAll(language, allLanguages: false);
                    break;
                case "mnuComplete":
                    _dex.SeenAll(shinyToo: false);
                    _dex.CaughtAll(language, allLanguages: false);
                    break;
                case "B_GiveAll":
                    _dex.GiveAll(_species, state: true, shinyToo: false, language, allLanguages: false);
                    break;
                default:
                    throw new InvalidOperationException($"Unknown Pokédex action '{action}'.");
            }
        }

        public void FlushEntry() { }

        public void CommitSaveExtras()
        {
            if (_species != 0)
                _dex.InitialSpecies = _species;
        }

        public PokedexDocument ToDocument() => SharedDocument(
            "SAV_Pokedex5",
            _sav.MaxSpeciesID,
            7,
            hasNationalDex: true,
            hasSpinda: true,
            nationalUnlocked: _dex.IsNationalDexUnlocked,
            nationalActive: _dex.IsNationalDexMode,
            spinda: _dex.Spinda.ToString("X8"),
            species: _species,
            caught: _dex.GetCaught(_species));

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0 and <= 649)
                _species = document.Species;
            _dex.IsNationalDexUnlocked = document.NationalUnlocked;
            _dex.IsNationalDexMode = document.NationalActive;
            if (!string.IsNullOrEmpty(document.Spinda)
                && uint.TryParse(document.Spinda, System.Globalization.NumberStyles.HexNumber, null, out var pid))
                _dex.Spinda = pid;
            ApplyCurrentFlags(document);
        }

        private void ApplyCurrentFlags(PokedexDocument document)
        {
            if (_species is 0 or > 649)
                return;
            _dex.SetCaught(_species, document.Caught);
            for (var i = 0; i < 4; i++)
            {
                _dex.SetSeen(_species, i, Flag(document.Seen, i));
                _dex.SetDisplayed(_species, i, Flag(document.Displayed, i));
            }
            if (_species <= 493 && document.Languages is not null)
            {
                for (var i = 0; i < 7 && i < document.Languages.Length; i++)
                    _dex.SetLanguageFlag(_species, i, document.Languages[i]);
            }
        }
    }

    private sealed class Gen7Kind : IPokedexKind
    {
        private readonly SAV7 _sav;
        private readonly Zukan7 _dex;
        private int _index;

        public Gen7Kind(SAV7 sav, Zukan7 dex, string langForm)
        {
            _sav = sav;
            _dex = dex;
            _langForm = langForm;
        }

        private readonly string _langForm;

        public void SelectDefault() => _index = 0;

        public void SelectSpecies(ushort species)
        {
            if (species is 0 || species > _sav.MaxSpeciesID)
                return;
            _index = species - 1;
        }

        public void Bulk(string action)
        {
            switch (action)
            {
                case "mnuSeenNone":
                    ClearAll(seenToo: true);
                    break;
                case "mnuCaughtNone":
                    ClearAll(seenToo: false);
                    break;
                case "mnuSeenAll":
                case "mnuCaughtAll":
                case "mnuComplete":
                case "B_GiveAll":
                    throw new InvalidOperationException($"Pokédex action '{action}' is not in the Gen 7 Seen None shell.");
                default:
                    throw new InvalidOperationException($"Unknown Pokédex action '{action}'.");
            }
            _index = 0;
        }

        public void FlushEntry() { }

        public void CommitSaveExtras() { }

        public PokedexDocument ToDocument()
        {
            var species = (ushort)(_index + 1);
            return SharedDocument(
                _langForm,
                _sav.MaxSpeciesID,
                9,
                hasNationalDex: false,
                hasSpinda: false,
                nationalUnlocked: false,
                nationalActive: false,
                spinda: "",
                species: species <= _sav.MaxSpeciesID ? species : _dex.GetBaseSpecies(_index),
                caught: species <= _sav.MaxSpeciesID && _dex.GetCaught(species));
        }

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0)
                SelectSpecies(document.Species);
        }

        private void ClearAll(bool seenToo)
        {
            var names = _dex.GetEntryNames(GameInfo.Strings.Species);
            for (var i = 0; i < names.Count; i++)
            {
                var id = (ushort)(i + 1);
                if (id <= _sav.MaxSpeciesID)
                {
                    _dex.SetCaught(id, false);
                    for (var lang = 0; lang < 9; lang++)
                        _dex.SetLanguageFlag(i, lang, false);
                }
                if (!seenToo)
                    continue;
                for (var region = 0; region < 4; region++)
                {
                    _dex.SetSeen(id, region, false);
                    _dex.SetDisplayed(i, region, false);
                }
            }
        }
    }

    private sealed class Gen4Kind : IPokedexKind
    {
        private readonly SAV4 _sav;
        private ushort _species = 1;

        public Gen4Kind(SAV4 sav) => _sav = sav;

        public void SelectDefault() => _species = 1;

        public void SelectSpecies(ushort species)
        {
            if (species is 0 or > 493)
                return;
            _species = species;
        }

        public void Bulk(string action)
        {
            var lang = Zukan4.GetGen4LanguageBitIndex(_sav.Language);
            var args = action switch
            {
                "mnuSeenNone" => Zukan4.SetDexArgs.None,
                "mnuSeenAll" => Zukan4.SetDexArgs.SeenAll,
                "mnuCaughtNone" => Zukan4.SetDexArgs.CaughtNone,
                "mnuCaughtAll" => Zukan4.SetDexArgs.CaughtAll,
                "mnuComplete" => Zukan4.SetDexArgs.Complete,
                "B_GiveAll" => Zukan4.SetDexArgs.Complete,
                _ => throw new InvalidOperationException($"Unknown Pokédex action '{action}'."),
            };
            for (ushort i = 1; i <= 493; i++)
                _sav.Dex.ModifyAll(i, args, lang);
        }

        public void FlushEntry() { }

        public void CommitSaveExtras() { }

        public PokedexDocument ToDocument() => SharedDocument(
            "SAV_Pokedex4",
            _sav.MaxSpeciesID,
            6,
            hasNationalDex: false,
            hasSpinda: false,
            nationalUnlocked: false,
            nationalActive: false,
            spinda: "",
            species: _species,
            caught: _sav.Dex.GetCaught(_species));

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0 and <= 493)
                _species = document.Species;
        }
    }

    private sealed class Gen6Kind : IPokedexKind
    {
        private readonly SaveFile _sav;
        private readonly Zukan6 _dex;
        private readonly string _langForm;
        private ushort _species = 1;

        public Gen6Kind(SaveFile sav, Zukan6 dex, string langForm)
        {
            _sav = sav;
            _dex = dex;
            _langForm = langForm;
        }

        public void SelectDefault()
        {
            var initial = _dex.InitialSpecies;
            _species = initial is 0 || initial > _sav.MaxSpeciesID ? (ushort)1 : initial;
        }

        public void SelectSpecies(ushort species)
        {
            if (species is 0 || species > _sav.MaxSpeciesID)
                return;
            _species = species;
        }

        public void Bulk(string action)
        {
            var language = (LanguageID)_sav.Language;
            switch (action)
            {
                case "mnuSeenNone":
                    _dex.SeenNone();
                    break;
                case "mnuSeenAll":
                    _dex.SeenAll(shinyToo: false);
                    break;
                case "mnuCaughtNone":
                    _dex.CaughtNone();
                    break;
                case "mnuCaughtAll":
                    _dex.CaughtAll(language, allLanguages: false);
                    break;
                case "mnuComplete":
                    _dex.SeenAll(shinyToo: false);
                    _dex.CaughtAll(language, allLanguages: false);
                    break;
                case "B_GiveAll":
                    _dex.GiveAll(_species, state: true, shinyToo: false, language, allLanguages: false);
                    break;
                default:
                    throw new InvalidOperationException($"Unknown Pokédex action '{action}'.");
            }
        }

        public void FlushEntry() { }

        public void CommitSaveExtras()
        {
            if (_species != 0)
                _dex.InitialSpecies = _species;
        }

        public PokedexDocument ToDocument() => SharedDocument(
            _langForm,
            _sav.MaxSpeciesID,
            7,
            hasNationalDex: true,
            hasSpinda: true,
            nationalUnlocked: _dex.IsNationalDexUnlocked,
            nationalActive: _dex.IsNationalDexMode,
            spinda: _dex.Spinda.ToString("X8"),
            species: _species,
            caught: _dex.GetCaught(_species));

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0 && document.Species <= _sav.MaxSpeciesID)
                _species = document.Species;
            _dex.IsNationalDexUnlocked = document.NationalUnlocked;
            _dex.IsNationalDexMode = document.NationalActive;
            if (!string.IsNullOrEmpty(document.Spinda)
                && uint.TryParse(document.Spinda, System.Globalization.NumberStyles.HexNumber, null, out var pid))
                _dex.Spinda = pid;
            if (_species is 0 || _species > _sav.MaxSpeciesID)
                return;
            _dex.SetCaught(_species, document.Caught);
            for (var i = 0; i < 4; i++)
            {
                _dex.SetSeen(_species, i, Flag(document.Seen, i));
                _dex.SetDisplayed(_species, i, Flag(document.Displayed, i));
            }
            for (var i = 0; i < 7; i++)
                _dex.SetLanguageFlag(_species, i, Flag(document.Languages, i));
        }
    }

    private sealed class Gen7bKind : IPokedexKind
    {
        private readonly SAV7b _sav;
        private readonly Zukan7b _dex;
        private int _index;

        public Gen7bKind(SAV7b sav)
        {
            _sav = sav;
            _dex = sav.Zukan;
        }

        public void SelectDefault() => _index = 0;

        public void SelectSpecies(ushort species)
        {
            if (species is 0 || species > _sav.MaxSpeciesID)
                return;
            _index = species - 1;
        }

        public void Bulk(string action)
        {
            switch (action)
            {
                case "mnuSeenNone":
                    ClearAll(seenToo: true);
                    ResetSizeRecords();
                    break;
                case "mnuCaughtNone":
                    ClearAll(seenToo: false);
                    break;
                default:
                    throw new InvalidOperationException($"Unknown Pokédex action '{action}'.");
            }
            _index = 0;
        }

        public void FlushEntry() { }

        public void CommitSaveExtras() { }

        public PokedexDocument ToDocument()
        {
            var species = (ushort)(_index + 1);
            return SharedDocument(
                "SAV_PokedexGG",
                _sav.MaxSpeciesID,
                9,
                hasNationalDex: false,
                hasSpinda: false,
                nationalUnlocked: false,
                nationalActive: false,
                spinda: "",
                species: species <= _sav.MaxSpeciesID ? species : _dex.GetBaseSpecies(_index),
                caught: species <= _sav.MaxSpeciesID && _dex.GetCaught(species));
        }

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0)
                SelectSpecies(document.Species);
        }

        private void ClearAll(bool seenToo)
        {
            var names = _dex.GetEntryNames(GameInfo.Strings.Species);
            for (var i = 0; i < names.Count; i++)
            {
                var id = (ushort)(i + 1);
                if (id <= _sav.MaxSpeciesID)
                {
                    _dex.SetCaught(id, false);
                    for (var lang = 0; lang < 9; lang++)
                        _dex.SetLanguageFlag(i, lang, false);
                }
                if (!seenToo)
                    continue;
                for (var region = 0; region < 4; region++)
                {
                    _dex.SetSeen(id, region, false);
                    _dex.SetDisplayed(i, region, false);
                }
            }
        }

        private void ResetSizeRecords()
        {
            for (ushort species = 1; species <= _sav.MaxSpeciesID; species++)
            {
                if (!Zukan7b.TryGetSizeEntryIndex(species, 0, out var index))
                    continue;
                foreach (DexSizeType group in Enum.GetValues<DexSizeType>())
                    _dex.SetSizeData(group, index, Zukan7b.DefaultEntryValueH, Zukan7b.DefaultEntryValueW);
            }
        }
    }

    private sealed class CoreZukanKind : IPokedexKind
    {
        private readonly SaveFile _sav;
        private readonly Action<string, ushort> _bulk;
        private readonly string _langForm;
        private ushort _species = 1;

        public CoreZukanKind(SaveFile sav, Action<string, ushort> bulk, string langForm)
        {
            _sav = sav;
            _bulk = bulk;
            _langForm = langForm;
        }

        public void SelectDefault() => _species = 1;

        public void SelectSpecies(ushort species)
        {
            if (species is 0 || species > _sav.MaxSpeciesID)
                return;
            _species = species;
        }

        public void Bulk(string action) => _bulk(action, _species);

        public void FlushEntry() { }

        public void CommitSaveExtras() { }

        public PokedexDocument ToDocument() => SharedDocument(
            _langForm,
            _sav.MaxSpeciesID,
            0,
            hasNationalDex: false,
            hasSpinda: false,
            nationalUnlocked: false,
            nationalActive: false,
            spinda: "",
            species: _species,
            caught: _sav.GetCaught(_species));

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0 && document.Species <= _sav.MaxSpeciesID)
                _species = document.Species;
        }
    }

    private sealed class BdspKind : IPokedexKind
    {
        private readonly SAV8BS _sav;
        private readonly Zukan8b _dex;
        private ushort _species = 1;

        public BdspKind(SAV8BS sav)
        {
            _sav = sav;
            _dex = sav.Zukan;
        }

        public void SelectDefault() => _species = 1;

        public void SelectSpecies(ushort species)
        {
            if (species is 0 or > 493)
                return;
            _species = species;
        }

        public void Bulk(string action)
        {
            switch (action)
            {
                case "mnuSeenNone":
                    _dex.SetAllSeen(false);
                    break;
                case "mnuSeenAll":
                    _dex.SetAllSeen();
                    break;
                case "mnuCaughtNone":
                    _dex.CaughtNone();
                    break;
                case "mnuCaughtAll":
                    _dex.CaughtAll();
                    break;
                case "mnuComplete":
                    _dex.CompleteDex();
                    break;
                default:
                    throw new InvalidOperationException($"Unknown Pokédex action '{action}'.");
            }
        }

        public void FlushEntry() { }

        public void CommitSaveExtras() { }

        public PokedexDocument ToDocument() => SharedDocument(
            "SAV_PokedexBDSP",
            _sav.MaxSpeciesID,
            0,
            hasNationalDex: true,
            hasSpinda: false,
            nationalUnlocked: _dex.HasNationalDex,
            nationalActive: false,
            spinda: "",
            species: _species,
            caught: _dex.GetCaught(_species));

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0 and <= 493)
                _species = document.Species;
            _dex.HasNationalDex = document.NationalUnlocked;
        }
    }

    private sealed class LegendsKind : IPokedexKind
    {
        private readonly SAV8LA _sav;
        private ushort _species = 1;

        public LegendsKind(SAV8LA sav) => _sav = sav;

        public void SelectDefault() => _species = 1;

        public void SelectSpecies(ushort species)
        {
            if (species is 0 || species > _sav.MaxSpeciesID)
                return;
            _species = species;
        }

        public void Bulk(string action) =>
            throw new InvalidOperationException("Legends: Arceus has no Seen none bulk in Original PKHeX.");

        public void FlushEntry() { }

        public void CommitSaveExtras() { }

        public PokedexDocument ToDocument() => SharedDocument(
            "SAV_PokedexLA",
            _sav.MaxSpeciesID,
            0,
            hasNationalDex: false,
            hasSpinda: false,
            nationalUnlocked: false,
            nationalActive: false,
            spinda: "",
            species: _species,
            caught: _sav.GetCaught(_species));

        public void ApplyDocument(PokedexDocument document)
        {
            if (document.Species is > 0 && document.Species <= _sav.MaxSpeciesID)
                _species = document.Species;
        }
    }

    private static void BulkZukan<T>(ZukanBase<T> dex, string action, ushort species)
        where T : SaveFile
    {
        switch (action)
        {
            case "mnuSeenNone":
                dex.SeenNone();
                break;
            case "mnuSeenAll":
                dex.SeenAll();
                break;
            case "mnuCaughtNone":
                dex.CaughtNone();
                break;
            case "mnuCaughtAll":
                dex.CaughtAll();
                break;
            case "mnuComplete":
                dex.CompleteDex();
                break;
            case "B_GiveAll":
                dex.SetDexEntryAll(species);
                break;
            default:
                throw new InvalidOperationException($"Unknown Pokédex action '{action}'.");
        }
    }

    private static PokedexDocument SharedDocument(
        string langForm,
        int maxSpecies,
        int languageCount,
        bool hasNationalDex,
        bool hasSpinda,
        bool nationalUnlocked,
        bool nationalActive,
        string spinda,
        ushort species,
        bool caught)
    {
        var names = new List<string>(maxSpecies);
        var speciesNames = GameInfo.Strings.Species;
        for (var i = 1; i <= maxSpecies; i++)
            names.Add($"{i:000} - {speciesNames[i]}");
        return new PokedexDocument
        {
            LangForm = langForm,
            MaxSpecies = maxSpecies,
            LanguageCount = languageCount,
            HasNationalDex = hasNationalDex,
            HasSpinda = hasSpinda,
            NationalUnlocked = nationalUnlocked,
            NationalActive = nationalActive,
            Spinda = spinda,
            Species = species,
            Caught = caught,
            SpeciesNames = names,
        };
    }

    private static bool Flag(bool[]? flags, int index) => flags is not null && index < flags.Length && flags[index];

    internal sealed class PokedexDocument
    {
        public string LangForm { get; set; } = "SAV_Pokedex5";
        public int MaxSpecies { get; set; }
        public int LanguageCount { get; set; }
        public bool HasNationalDex { get; set; }
        public bool HasSpinda { get; set; }
        public bool NationalUnlocked { get; set; }
        public bool NationalActive { get; set; }
        public string Spinda { get; set; } = "";
        public ushort Species { get; set; }
        public bool Caught { get; set; }
        public bool[]? Seen { get; set; }
        public bool[]? Displayed { get; set; }
        public bool[]? Languages { get; set; }
        public List<string>? SpeciesNames { get; set; }
    }
}
