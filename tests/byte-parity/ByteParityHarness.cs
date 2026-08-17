using PKHeX.Editor;

namespace PKHeX.ByteParity.Tests;

/// <summary>
/// Runs an Editor session against a save: load, optional named operations, export.
/// Does not create a Host or Qt widgets.
/// </summary>
public static class ByteParityHarness
{
    public static byte[] Run(
        ReadOnlyMemory<byte> save,
        IReadOnlyList<EditorOperation> operations,
        Action<EditorSession>? register = null)
    {
        var session = EditorSession.Load(save);
        register?.Invoke(session);
        return session.Export(operations);
    }
}
