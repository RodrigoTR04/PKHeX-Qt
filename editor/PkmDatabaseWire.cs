using System.Text.Json;
using System.Text.Json.Serialization;

namespace PKHeX.Editor;

internal static class PkmDatabaseWire
{
    private static readonly JsonSerializerOptions Json = new()
    {
        PropertyNameCaseInsensitive = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        Converters = { new JsonStringEnumConverter() },
    };

    public static string Document(EditorSession session, IReadOnlyList<PkmDatabaseHit> hits)
    {
        string species;
        try
        {
            species = session.FieldChoices("CB_Species");
        }
        catch (InvalidOperationException)
        {
            species = string.Empty;
        }

        return JsonSerializer.Serialize(new
        {
            speciesChoices = species,
            count = hits.Count,
            hits,
        }, Json);
    }

    public static PkmDatabaseQuery Parse(string json)
    {
        if (string.IsNullOrWhiteSpace(json))
            return new PkmDatabaseQuery();
        return JsonSerializer.Deserialize<PkmDatabaseQuery>(json, Json) ?? new PkmDatabaseQuery();
    }
}
