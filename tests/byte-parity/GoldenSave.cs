using PKHeX.Core;

namespace PKHeX.ByteParity.Tests;

internal static class GoldenSave
{
    /// <summary>
    /// Blank Gen5 save written by Core, then loaded and written again so the bytes are a stable export fixture.
    /// </summary>
    public static byte[] Gen5BlankExport()
    {
        var written = BlankSaveFile.Get(GameVersion.B).Write().ToArray();
        if (!SaveUtil.TryGetSaveFile(written, out var loaded))
            throw new InvalidOperationException("Core did not recognize its own blank Gen5 export.");
        return loaded.Write().ToArray();
    }

    /// <summary>
    /// Blank Scarlet save. Encrypted export is not round-tripped through <see cref="SaveUtil.TryGetSaveFile"/>;
    /// tests keep the live <see cref="SaveFile"/> like the Editor does after New.
    /// </summary>
    public static SaveFile Gen9Blank() => BlankSaveFile.Get(GameVersion.SL);

    public static SaveFile Gen7Blank() => BlankSaveFile.Get(GameVersion.SN);
}
