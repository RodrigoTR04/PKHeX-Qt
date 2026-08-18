using System.Text.Json;
using System.Text.Json.Serialization;
using PKHeX.Core;

namespace PKHeX.Editor;

internal sealed class BoxExportWire
{
    private static readonly JsonSerializerOptions Json = new()
    {
        PropertyNameCaseInsensitive = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        Converters = { new JsonStringEnumConverter() },
    };

    public string Namer { get; set; } = "Default";
    public string[] Namers { get; set; } = [];
    public BoxExportSettings Settings { get; set; } = new();

    public static string Prepare(UserConfig config)
        => JsonSerializer.Serialize(new BoxExportWire
        {
            Namer = string.IsNullOrEmpty(config.DefaultBoxExportNamer) ? "Default" : config.DefaultBoxExportNamer,
            Namers = [.. EntityFileNamer.AvailableNamers.Select(namer => namer.Name)],
            Settings = config.BoxExport,
        }, Json);

    public static BoxExportWire Parse(string json)
        => JsonSerializer.Deserialize<BoxExportWire>(json, Json) ?? new BoxExportWire();

    public void Apply(UserConfig config)
    {
        config.DefaultBoxExportNamer = string.IsNullOrEmpty(Namer) ? "Default" : Namer;
        config.BoxExport = Settings ?? new BoxExportSettings();
    }
}
