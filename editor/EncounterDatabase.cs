using System.Runtime.CompilerServices;
using PKHeX.Core;
using PKHeX.Core.Searching;

namespace PKHeX.Editor;

public sealed class EncounterDatabaseQuery
{
    public ushort Species { get; set; }
    public ushort Move1 { get; set; }
    public ushort Move2 { get; set; }
    public ushort Move3 { get; set; }
    public ushort Move4 { get; set; }
    public GameVersion Version { get; set; }
    public bool? SearchEgg { get; set; }
    public bool? SearchShiny { get; set; }
    public string BatchInstructions { get; set; } = string.Empty;
    public EncounterTypeGroup[] Types { get; set; } = Enum.GetValues<EncounterTypeGroup>();
    public bool ReturnNoneIfEmptySearch { get; set; } = true;
    public bool FilterUnavailableSpecies { get; set; } = true;
    public bool UseTabsAsCriteria { get; set; } = true;
    public bool UseTabsAsCriteriaAnySpecies { get; set; } = true;
}

public readonly record struct EncounterDatabaseHit(int Index, ushort Species, byte Form, string Fingerprint, string Summary);

internal sealed class EncounterDatabase
{
    private static readonly object PriorityGate = new();
    private List<IEncounterInfo> _results = [];

    public IReadOnlyList<EncounterDatabaseHit> Search(SaveFile sav, EncounterDatabaseQuery query)
    {
        ArgumentNullException.ThrowIfNull(sav);
        ArgumentNullException.ThrowIfNull(query);
        var settings = CreateSettings(sav, query);
        if (!IsSearchAllowed(query, settings))
        {
            _results = [];
            return Hits();
        }

        lock (PriorityGate)
        {
            var previous = EncounterMovesetGenerator.PriorityList;
            EncounterMovesetGenerator.PriorityList = query.Types.Length == 0
                ? Enum.GetValues<EncounterTypeGroup>()
                : query.Types;
            try
            {
                _results = SearchCore(sav, query, settings).ToList();
            }
            finally
            {
                EncounterMovesetGenerator.PriorityList = previous;
            }
        }
        return Hits();
    }

    public IEncounterInfo Hit(int index)
    {
        if ((uint)index >= (uint)_results.Count)
            throw new ArgumentOutOfRangeException(nameof(index));
        return _results[index];
    }

    public PKM Take(int index, SaveFile sav, PKM? editor, EncounterDatabaseQuery query)
    {
        var enc = Hit(index);
        var criteria = GetCriteria(enc, sav, editor, query);
        var temp = enc.ConvertToPKM(sav, criteria);
        var pk = EntityConverter.ConvertToType(temp, sav.PKMType, out _)
            ?? throw new InvalidDataException("Could not convert that encounter to this save.");
        sav.AdaptToSaveFile(pk);
        pk.RefreshChecksum();
        return pk;
    }

    public static string Fingerprint(IEncounterInfo enc)
        => $"{enc.GetType().Name}:{enc.Species}:{enc.Form}:{enc.Version}:{enc.LevelMin}:{enc.LevelMax}";

    private List<EncounterDatabaseHit> Hits()
    {
        var hits = new List<EncounterDatabaseHit>(_results.Count);
        for (var i = 0; i < _results.Count; i++)
        {
            var enc = _results[i];
            var summary = string.Join('\n', enc.GetTextLines());
            hits.Add(new EncounterDatabaseHit(i, enc.Species, enc.Form, Fingerprint(enc), summary));
        }
        return hits;
    }

    private static SearchSettings CreateSettings(SaveFile sav, EncounterDatabaseQuery query)
    {
        var settings = new SearchSettings
        {
            Context = sav.Context,
            Generation = sav.Generation,
            Species = query.Species,
            BatchInstructions = query.BatchInstructions ?? string.Empty,
            Version = query.Version,
            SearchEgg = query.SearchEgg,
            SearchShiny = query.SearchShiny,
        };
        settings.AddMove(query.Move1);
        settings.AddMove(query.Move2);
        settings.AddMove(query.Move3);
        settings.AddMove(query.Move4);
        return settings;
    }

    private static bool IsSearchAllowed(EncounterDatabaseQuery query, SearchSettings settings)
    {
        if (query.Types.Length == 0)
            return false;
        if (settings is { Species: 0, Moves.Count: 0 } && !query.ReturnNoneIfEmptySearch)
            return false;
        return true;
    }

    private static IEnumerable<IEncounterInfo> SearchCore(SaveFile sav, EncounterDatabaseQuery query, SearchSettings settings)
    {
        var pk = sav.BlankPKM;
        var moves = settings.Moves.ToArray();
        var versions = settings.GetVersions(sav);
        var species = settings.Species == 0 ? FullRange(sav.MaxSpeciesID) : [settings.Species];
        IEnumerable<IEncounterInfo> results = GetAllSpeciesFormEncounters(species, sav.Personal, versions, moves, pk, sav, query.FilterUnavailableSpecies);
        if (settings.SearchEgg is not null)
            results = results.Where(z => z.IsEgg == settings.SearchEgg);
        if (settings.SearchShiny is not null)
            results = results.Where(z => z.IsShiny == settings.SearchShiny);

        results = results.Distinct(new ReferenceComparer());

        if (query.FilterUnavailableSpecies)
        {
            var filter = EntityPresenceFilters.GetFilterGeneric<IEncounterInfo>(sav.Context);
            if (filter is not null)
                results = results.Where(filter);
        }

        ReadOnlySpan<char> batchText = settings.BatchInstructions;
        if (batchText.Length != 0 && !StringInstructionSet.HasEmptyLine(batchText))
        {
            var filters = StringInstruction.GetFilters(batchText);
            EntityBatchEditor.ScreenStrings(filters);
            results = results.Where(enc => BatchEditingUtil.IsFilterMatch(filters, enc));
        }

        return results;
    }

    private static IEnumerable<ushort> FullRange(int max)
    {
        for (ushort i = 1; i <= max; i++)
            yield return i;
    }

    private static IEnumerable<IEncounterInfo> GetAllSpeciesFormEncounters(
        IEnumerable<ushort> species,
        IPersonalTable pt,
        ReadOnlyMemory<GameVersion> versions,
        ReadOnlyMemory<ushort> moves,
        PKM pk,
        SaveFile sav,
        bool filterUnavailable)
    {
        foreach (var s in species)
        {
            var pi = pt.GetFormEntry(s, 0);
            var fc = pi.FormCount;
            if (fc == 0 && !filterUnavailable)
            {
                pi = PersonalTable.USUM.GetFormEntry(s, 0);
                fc = pi.FormCount;
            }
            for (byte f = 0; f < fc; f++)
            {
                if (FormInfo.IsBattleOnlyForm(s, f, pk.Format))
                    continue;
                pk.Species = s;
                pk.Form = f;
                pk.SetGender(pk.GetSaneGender());
                EncounterMovesetGenerator.OptimizeCriteria(pk, sav);
                foreach (var enc in EncounterMovesetGenerator.GenerateEncounters(pk, moves, versions))
                    yield return enc;
            }
        }
    }

    private static EncounterCriteria GetCriteria(IEncounterTemplate enc, SaveFile sav, PKM? editor, EncounterDatabaseQuery query)
    {
        if (!query.UseTabsAsCriteria || editor is null)
            return EncounterCriteria.Unrestricted;

        var tree = EvolutionTree.GetEvolutionTree(editor.Context);
        var isInChain = tree.IsSpeciesDerivedFrom(editor.Species, editor.Form, enc.Species, enc.Form);
        if (!query.UseTabsAsCriteriaAnySpecies && !isInChain)
            return EncounterCriteria.Unrestricted;

        var set = new ShowdownSet(editor);
        var mutations = EncounterMutationUtil.GetSuggested(editor.Context, set.Level);
        var criteria = EncounterCriteria.GetCriteria(set, editor.PersonalInfo, mutations);
        if (editor.Context.IsHyperTrainingAvailable(100))
            criteria = criteria.ReviseIVsHyperTrainAvailable();
        if (!isInChain || EntityGender.IsSingleGender(enc.Species))
            criteria = criteria with { Gender = Gender.Random };
        if (!criteria.Mutations.CanGetAbility(enc.Ability, criteria.Ability))
            criteria = criteria with { Ability = AbilityPermission.Any12H };
        return criteria;
    }

    private sealed class ReferenceComparer : IEqualityComparer<IEncounterInfo>
    {
        public bool Equals(IEncounterInfo? x, IEncounterInfo? y) => ReferenceEquals(x, y);
        public int GetHashCode(IEncounterInfo obj) => RuntimeHelpers.GetHashCode(obj);
    }
}
