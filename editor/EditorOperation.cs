namespace PKHeX.Editor;

/// <summary>
/// Named Editor-layer action, matching Original PKHeX control names when the operation is a WinForms command.
/// </summary>
public sealed record EditorOperation(string Name);
