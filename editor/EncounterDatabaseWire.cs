using System.Text.Json;
using System.Text.Json.Serialization;
using PKHeX.Core;
using static PKHeX.Core.MessageStrings;

namespace PKHeX.Editor;

internal static class EncounterDatabaseWire
{
    private static readonly JsonSerializerOptions Json = new()
    {
        PropertyNameCaseInsensitive = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        Converters = { new JsonStringEnumConverter() },
    };

    public static string Document(SaveFile sav, IReadOnlyList<EncounterDatabaseHit> hits)
        => JsonSerializer.Serialize(new
        {
            speciesChoices = SpeciesLines(sav),
            moveChoices = MoveLines(sav),
            versionChoices = VersionLines(sav),
            count = hits.Count,
            hits,
        }, Json);

    public static EncounterDatabaseQuery Parse(string json)
    {
        if (string.IsNullOrWhiteSpace(json))
            return new EncounterDatabaseQuery();
        var query = JsonSerializer.Deserialize<EncounterDatabaseQuery>(json, Json) ?? new EncounterDatabaseQuery();
        query.Types ??= Enum.GetValues<EncounterTypeGroup>();
        return query;
    }

    private static string SpeciesLines(SaveFile sav)
    {
        var source = new FilteredGameDataSource(sav, GameInfo.Sources).Source;
        var species = source.SpeciesDataSource.ToList();
        if (species.Count > 0)
            species[0] = new ComboItem(MsgAny, 0);
        return Lines(species);
    }

    private static string MoveLines(SaveFile sav)
    {
        var moves = new FilteredGameDataSource(sav, GameInfo.Sources).Moves.ToList();
        if (moves.Count > 0)
            moves[0] = new ComboItem(MsgAny, 0);
        return Lines(moves);
    }

    private static string VersionLines(SaveFile sav)
    {
        var versions = new FilteredGameDataSource(sav, GameInfo.Sources).Source.VersionDataSource.ToList();
        versions.Insert(0, new ComboItem(MsgAny, 0));
        if (versions.Count > 1)
            versions.RemoveAt(versions.Count - 1);
        return Lines(versions);
    }

    private static string Lines(IEnumerable<ComboItem> items)
        => string.Join('\n', items.Select(z => $"{z.Value}\t{z.Text}"));
}
