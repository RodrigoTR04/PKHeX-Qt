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
}
