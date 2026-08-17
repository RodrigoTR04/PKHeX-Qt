using PKHeX.Core;
using static PKHeX.Core.GameVersion;

namespace PKHeX.Editor;

/// <summary>
/// Wallpaper resource names matching Original PKHeX's <c>WallpaperUtil.GetWallpaperResourceName</c>.
/// </summary>
public static class WallpaperResourceName
{
    public const string Default = "box_wp16xy";

    public static string Get(GameVersion version, int index)
    {
        index++;
        var suffix = GetResourceSuffix(version, index);
        var variant = version switch
        {
            SL when index is 20 => "_n",
            VL when index is 20 => "_u",
            _ => string.Empty,
        };

        return $"box_wp{index:00}{suffix}{variant}";
    }

    public static string GetForSave(SaveFile sav, int box)
    {
        if (sav is SAV9ZA)
            return "box_wp02bdsp";
        if (sav is SAV8LA)
            return "box_wp01bdsp";
        if (sav is not IBoxDetailWallpaper wp)
            return Default;

        return Get(sav.Version, wp.GetBoxWallpaper(box));
    }

    private static string GetResourceSuffix(GameVersion version, int index) => version.Context switch
    {
        EntityContext.Gen3 when version == E => "e",
        EntityContext.Gen3 when FRLG.Contains(version) && index > 12 => "frlg",
        EntityContext.Gen3 => "rs",

        EntityContext.Gen4 when index <= 16 => "dp",
        EntityContext.Gen4 when version == Pt => "pt",
        EntityContext.Gen4 when HGSS.Contains(version) => "hgss",

        EntityContext.Gen5 => B2W2.Contains(version) && index > 16 ? "b2w2" : "bw",
        EntityContext.Gen6 => ORAS.Contains(version) && index > 16 ? "ao" : "xy",
        EntityContext.Gen7 => "xy",
        EntityContext.Gen8b => "bdsp",
        EntityContext.Gen8 => "swsh",
        EntityContext.Gen9 => "sv",
        _ => string.Empty,
    };
}
