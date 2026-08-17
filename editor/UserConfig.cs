using System.Text.Json;
using System.Text.Json.Serialization;

namespace PKHeX.Editor;

/// <summary>
/// PKHeX Qt settings. Lives in the OS per-user config directory, not next to the binary and not in Original PKHeX's file.
/// </summary>
public sealed class UserConfig
{
    public const string FileName = "config.json";

    [JsonIgnore]
    public string Root { get; set; } = DefaultRoot();

    public bool BackupEnabled { get; set; } = true;
    public bool AskedCreateBackupFolder { get; set; }
    public bool CheckUnsavedEntityOnExport { get; set; } = true;

    [JsonIgnore]
    public string FilePath => Path.Combine(Root, FileName);

    [JsonIgnore]
    public string BackupDirectory => Path.Combine(Root, "bak");

    public static string DefaultRoot()
        => Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "PKHeX-Qt");

    public static UserConfig Load(string? root = null)
    {
        var config = new UserConfig { Root = root ?? DefaultRoot() };
        if (!File.Exists(config.FilePath))
            return config;
        var loaded = JsonSerializer.Deserialize<UserConfig>(File.ReadAllText(config.FilePath));
        if (loaded is null)
            return config;
        loaded.Root = config.Root;
        return loaded;
    }

    public void Save()
    {
        Directory.CreateDirectory(Root);
        File.WriteAllText(FilePath, JsonSerializer.Serialize(this, new JsonSerializerOptions { WriteIndented = true }));
    }
}
