using System.Text.Json;
using System.Text.Json.Serialization;
using PKHeX.Core;

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
    public SaveFileLoadSetting AutoLoadSaveOnStartup { get; set; } = SaveFileLoadSetting.RecentBackup;
    public GameVersion DefaultSaveVersion { get; set; } = Latest.Version;
    public List<string> RecentlyLoaded { get; set; } = [];
    public List<string> OtherBackupPaths { get; set; } = [];
    public uint RecentlyLoadedMaxCount { get; set; } = 10;
    public string TemplatePath { get; set; } = "template";
    public string SoundPath { get; set; } = "sounds";
    public bool HoverSlotPlayCry { get; set; } = true;
    public BoxExportSettings BoxExport { get; set; } = new();
    public string DefaultBoxExportNamer { get; set; } = "Default";
    public string DatabasePath { get; set; } = "pkmdb";
    public bool SearchBackups { get; set; } = true;
    public bool SearchExtraSaves { get; set; } = true;
    public bool SearchExtraSavesDeep { get; set; } = true;
    public bool FilterUnavailableSpecies { get; set; } = true;
    public DatabaseSortMode InitialSortMode { get; set; }

    [JsonIgnore]
    public string FilePath => Path.Combine(Root, FileName);

    [JsonIgnore]
    public string DatabaseDirectory => Resolve(DatabasePath);

    [JsonIgnore]
    public string BackupDirectory => Path.Combine(Root, "bak");

    [JsonIgnore]
    public string TemplateDirectory => Resolve(TemplatePath);

    [JsonIgnore]
    public string SoundDirectory => Resolve(SoundPath);

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
        if (loaded.RecentlyLoadedMaxCount is < 1 or > 1000)
            loaded.RecentlyLoadedMaxCount = 10;
        return loaded;
    }

    public void Save()
    {
        Directory.CreateDirectory(Root);
        File.WriteAllText(FilePath, JsonSerializer.Serialize(this, new JsonSerializerOptions { WriteIndented = true }));
    }

    public void RememberLoaded(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        var recent = RecentlyLoaded;
        if (!recent.Remove(path) && recent.Count >= RecentlyLoadedMaxCount)
            recent.RemoveAt(recent.Count - 1);
        recent.Insert(0, path);
    }

    private string Resolve(string path)
    {
        if (string.IsNullOrWhiteSpace(path))
            return Root;
        return Path.IsPathRooted(path) ? path : Path.Combine(Root, path);
    }
}
