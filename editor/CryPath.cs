using PKHeX.Core;

namespace PKHeX.Editor;

/// <summary>
/// Cry file names matching Original PKHeX's <c>CryPlayer</c>. Playback is the Host's job.
/// </summary>
public static class CryPath
{
    public static string FileName(ushort species, byte form, EntityContext context)
    {
        if (species == (ushort)Species.Urshifu && form == 1)
            return "892-1";

        var res = ArtworkSpriteName.GetResourceStringSprite(species, form, 0, 0, context, shiny: false);
        return res.Replace('_', '-')[1..];
    }

    public static string ExistingWav(ushort species, byte form, EntityContext context, string cryFolder)
    {
        if (species == 0 || string.IsNullOrWhiteSpace(cryFolder))
            return string.Empty;

        var named = Path.Combine(cryFolder, $"{FileName(species, form, context)}.wav");
        if (File.Exists(named))
            return named;

        var fallback = Path.Combine(cryFolder, $"{species}.wav");
        return File.Exists(fallback) ? fallback : string.Empty;
    }
}
