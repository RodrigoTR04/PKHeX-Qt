using System.Collections.Concurrent;
using PKHeX.Core;
using PKHeX.Core.Searching;

namespace PKHeX.Editor;

public sealed class PkmDatabaseOptions
{
    public string DatabasePath { get; set; } = string.Empty;
    public string BackupPath { get; set; } = string.Empty;
    public List<string> OtherBackupPaths { get; set; } = [];
    public bool SearchBackups { get; set; } = true;
    public bool SearchExtraSaves { get; set; } = true;
    public bool SearchExtraSavesDeep { get; set; } = true;
    public bool FilterUnavailableSpecies { get; set; } = true;
    public DatabaseSortMode InitialSortMode { get; set; }

    public static PkmDatabaseOptions FromConfig(UserConfig config)
    {
        ArgumentNullException.ThrowIfNull(config);
        return new PkmDatabaseOptions
        {
            DatabasePath = config.DatabaseDirectory,
            BackupPath = config.BackupDirectory,
            OtherBackupPaths = config.OtherBackupPaths,
            SearchBackups = config.SearchBackups,
            SearchExtraSaves = config.SearchExtraSaves,
            SearchExtraSavesDeep = config.SearchExtraSavesDeep,
            FilterUnavailableSpecies = config.FilterUnavailableSpecies,
            InitialSortMode = config.InitialSortMode,
        };
    }
}

public sealed class PkmDatabaseQuery
{
    public ushort Species { get; set; }
    public string Nickname { get; set; } = string.Empty;
    public bool SearchBoxes { get; set; } = true;
    public bool SearchDatabase { get; set; } = true;
    public bool SearchBackups { get; set; } = true;
    public bool? SearchLegal { get; set; }
    public bool? SearchShiny { get; set; }
    public bool? SearchEgg { get; set; }
    public string BatchInstructions { get; set; } = string.Empty;
}

public readonly record struct PkmDatabaseHit(int Index, ushort Species, string Identify, int Box, int Slot, string Kind);

internal sealed class PkmDatabase
{
    private readonly SaveFile _sav;
    private readonly string _databasePath;
    private List<SlotCache> _raw = [];
    private List<SlotCache> _results = [];

    public PkmDatabase(SaveFile sav, PkmDatabaseOptions options)
    {
        ArgumentNullException.ThrowIfNull(sav);
        ArgumentNullException.ThrowIfNull(options);
        _sav = sav;
        _databasePath = options.DatabasePath;
        _raw = LoadEntities(sav, options);
        _results = _raw;
    }

    public IReadOnlyList<PkmDatabaseHit> Search(PkmDatabaseQuery query)
    {
        ArgumentNullException.ThrowIfNull(query);
        IEnumerable<SlotCache> res = _raw;
        if (!query.SearchBoxes)
            res = res.Where(z => z.SAV != _sav);
        if (!query.SearchDatabase)
            res = res.Where(z => !IsIndividualFile(z));
        if (!query.SearchBackups)
            res = res.Where(z => !IsBackup(z));

        var settings = new SearchSettings
        {
            Species = query.Species,
            Nickname = query.Nickname,
            BatchInstructions = query.BatchInstructions ?? string.Empty,
            SearchLegal = query.SearchLegal,
            SearchShiny = query.SearchShiny,
            SearchEgg = query.SearchEgg,
        };
        _results = settings.Search(res).ToList();
        return Hits(_results);
    }

    public SlotCache Hit(int index)
    {
        if ((uint)index >= (uint)_results.Count)
            throw new ArgumentOutOfRangeException(nameof(index));
        return _results[index];
    }

    private bool IsBackup(SlotCache pk) => pk.SAV is not FakeSaveFile && pk.SAV != _sav;

    private bool IsIndividualFile(SlotCache pk)
        => pk.Source is SlotInfoFileSingle(var path)
           && !string.IsNullOrEmpty(_databasePath)
           && path.StartsWith(_databasePath + Path.DirectorySeparatorChar, StringComparison.Ordinal);

    private static List<PkmDatabaseHit> Hits(IReadOnlyList<SlotCache> results)
    {
        var hits = new List<PkmDatabaseHit>(results.Count);
        for (var i = 0; i < results.Count; i++)
        {
            var entry = results[i];
            var box = -1;
            var slot = -1;
            var kind = "other";
            switch (entry.Source)
            {
                case SlotInfoBox b:
                    box = b.Box;
                    slot = b.Slot;
                    kind = "box";
                    break;
                case SlotInfoParty p:
                    slot = p.Slot;
                    kind = "party";
                    break;
                case SlotInfoFileSingle:
                    kind = "file";
                    break;
                default:
                    kind = entry.SAV is FakeSaveFile ? "file" : "backup";
                    break;
            }
            hits.Add(new PkmDatabaseHit(i, entry.Entity.Species, entry.Identify(), box, slot, kind));
        }
        return hits;
    }

    private static List<SlotCache> LoadEntities(SaveFile sav, PkmDatabaseOptions options)
    {
        var otherPaths = new List<(string Path, bool IgnoreBackups)>();
        if (options.SearchExtraSaves)
        {
            foreach (var path in options.OtherBackupPaths.Where(Directory.Exists))
                otherPaths.Add((path, true));
        }
        if (options.SearchBackups && Directory.Exists(options.BackupPath))
            otherPaths.Add((options.BackupPath, false));

        var dbTemp = new ConcurrentBag<SlotCache>();
        var extensions = new HashSet<string>(EntityFileExtension.GetExtensionsAll().Select(z => $".{z}"));
        if (Directory.Exists(options.DatabasePath))
        {
            var files = Directory.EnumerateFiles(options.DatabasePath, "*", SearchOption.AllDirectories);
            Parallel.ForEach(files, file => SlotInfoLoader.AddFromLocalFile(file, dbTemp, sav, extensions));
        }

        foreach (var folder in otherPaths)
        {
            if (!SaveUtil.GetSavesFromFolder(folder.Path, options.SearchExtraSavesDeep, CancellationToken.None, out var paths, folder.IgnoreBackups))
                continue;
            Parallel.ForEach(paths, file => TryAddFromSavePath(dbTemp, file));
        }

        SlotInfoLoader.AddFromSaveFile(sav, dbTemp);
        var result = new List<SlotCache>(dbTemp);
        result.RemoveAll(z => !z.IsDataValid());

        if (options.FilterUnavailableSpecies)
        {
            var filter = EntityPresenceFilters.GetFilterEntity(sav.Context);
            if (filter is not null)
                result.RemoveAll(z => !filter(z.Entity));
        }

        if (options.InitialSortMode is DatabaseSortMode.SlotIdentity)
            result.Sort();
        else if (options.InitialSortMode is DatabaseSortMode.SpeciesForm)
            result.Sort((first, second) => first.CompareToSpeciesForm(second));

        foreach (var entry in result)
            entry.Entity.ForcePartyData();
        return result;
    }

    private static void TryAddFromSavePath(ConcurrentBag<SlotCache> dbTemp, string file)
    {
        if (SaveUtil.TryGetSaveFile(file, out var sav))
        {
            SlotInfoLoader.AddFromSaveFile(sav, dbTemp);
            return;
        }

        if (FileUtil.TryGetMemoryCard(file, out var mc))
            TryAddFromMemoryCard(dbTemp, mc, file);
    }

    private static void TryAddFromMemoryCard(ConcurrentBag<SlotCache> dbTemp, SAV3GCMemoryCard mc, string file)
    {
        var state = mc.GetMemoryCardState();
        if (state == MemoryCardSaveStatus.Invalid)
            return;
        if (mc.HasCOLO)
            TryAdd(dbTemp, mc, file, SaveFileType.Colosseum);
        if (mc.HasXD)
            TryAdd(dbTemp, mc, file, SaveFileType.XD);
        if (mc.HasRSBOX)
            TryAdd(dbTemp, mc, file, SaveFileType.RSBox);

        static void TryAdd(ConcurrentBag<SlotCache> bag, SAV3GCMemoryCard card, string path, SaveFileType game)
        {
            card.SelectSaveGame(game);
            if (!SaveUtil.TryGetSaveFile(card, out var sav))
                return;
            sav.Metadata.SetExtraInfo(path);
            SlotInfoLoader.AddFromSaveFile(sav, bag);
        }
    }
}
