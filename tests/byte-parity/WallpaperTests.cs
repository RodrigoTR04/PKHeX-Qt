using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class WallpaperTests
{
    [Fact]
    public void gen5_box_wallpaper_pixels_match_the_oracle_bw_wallpaper_file()
    {
        if (!SaveUtil.TryGetSaveFile(GoldenSave.Gen5BlankExport(), out var sav) || sav is not IBoxDetailWallpaper wp)
            throw new InvalidOperationException("Gen5 blank is not a wallpaper save.");

        var resource = WallpaperResourceName.Get(sav.Version, wp.GetBoxWallpaper(0));
        var expectedPath = Directory.EnumerateFiles(
                Path.Combine(RepoPaths.Root, "PKHeX-26.07.07", "PKHeX.Drawing.Misc", "Resources", "img", "box"),
                resource + ".png",
                SearchOption.AllDirectories)
            .Single();

        var session = EditorSession.Load(GoldenSave.Gen5BlankExport());
        var composed = session.ComposeWallpaperPng(0);
        var expected = File.ReadAllBytes(expectedPath);

        Assert.Equal(
            ArtworkSpriteComposer.BgraPixels(expected, out var ew, out var eh),
            ArtworkSpriteComposer.BgraPixels(composed, out var cw, out var ch));
        Assert.Equal(ew, cw);
        Assert.Equal(eh, ch);
        Assert.StartsWith("box_wp", resource, StringComparison.Ordinal);
    }
}
