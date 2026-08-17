namespace PKHeX.Editor;

/// <summary>
/// Process-local editor used by the Host: open a save from a path, export to a path.
/// Command-line Open and the native Open dialog both call <see cref="OpenFromPath"/>.
/// </summary>
public sealed class EditorApp
{
    public EditorSession? Session { get; private set; }

    public void OpenFromPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        Session = EditorSession.OpenDropped(Session, path);
    }

    public void SaveToPath(string path)
    {
        ArgumentException.ThrowIfNullOrEmpty(path);
        if (Session is null)
            throw new InvalidOperationException("No save is open.");
        File.WriteAllBytes(path, Session.Export());
    }
}
