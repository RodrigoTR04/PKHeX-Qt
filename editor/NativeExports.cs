using System.Runtime.InteropServices;

namespace PKHeX.Editor;

/// <summary>
/// Entry points the C++ Host loads through hostfxr. One session per process; two processes are two instances.
/// </summary>
public static class NativeExports
{
    internal static readonly EditorApp App = new();

    public static int OpenPath(IntPtr arg, int size)
    {
        try
        {
            App.OpenFromPath(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int SavePath(IntPtr arg, int size)
    {
        try
        {
            App.SaveToPath(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int HasSession(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        return App.Session is null ? 0 : 1;
    }

    public static int SelectSlot(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            ParseSlotKey(ReadUtf8(arg, size), out var party, out var box, out var slot);
            if (party)
                session.SelectPartySlot(slot);
            else
                session.SelectBoxSlot(box, slot);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int SetCurrentBox(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            if (!int.TryParse(ReadUtf8(arg, size), out var box))
                return 1;
            session.CurrentBox = box;
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int GetStorageLayout(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            if (arg == IntPtr.Zero || size < 16)
                return 1;
            Span<int> values = stackalloc int[4];
            values[0] = session.BoxCount;
            values[1] = session.BoxSlotCount;
            values[2] = session.PartySlotCount;
            values[3] = session.CurrentBox;
            Marshal.Copy(values.ToArray(), 0, arg, 4);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PrepareSlotPng(IntPtr arg, int size)
    {
        try
        {
            var session = RequireSession();
            var key = ReadUtf8(arg, size);
            _preparedPng = key.StartsWith("wallpaper:", StringComparison.Ordinal)
                ? session.ComposeWallpaperPng(int.Parse(key["wallpaper:".Length..], System.Globalization.CultureInfo.InvariantCulture))
                : ComposeSlot(session, key);
            return _preparedPng.Length;
        }
        catch
        {
            _preparedPng = null;
            return -1;
        }
    }

    public static int CopyPreparedPng(IntPtr arg, int size)
    {
        if (_preparedPng is null || arg == IntPtr.Zero || size < _preparedPng.Length)
            return 1;
        Marshal.Copy(_preparedPng, 0, arg, _preparedPng.Length);
        return 0;
    }

    public static int GetField(IntPtr arg, int size)
    {
        try
        {
            _preparedText = RequireSession().GetField(ReadUtf8(arg, size));
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int SetField(IntPtr arg, int size)
    {
        try
        {
            var raw = ReadUtf8(arg, size);
            var split = raw.IndexOf('=');
            if (split <= 0)
                return 1;
            RequireSession().SetField(raw[..split], raw[(split + 1)..]);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int CommitCurrent(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            RequireSession().CommitCurrent();
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int LegalityValid(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            return RequireSession().LegalityValid ? 1 : 0;
        }
        catch
        {
            return 0;
        }
    }

    public static int PrepareLegalityReport(IntPtr arg, int size)
    {
        try
        {
            var verbose = ReadUtf8(arg, size) == "1";
            _preparedText = RequireSession().LegalityReport(verbose);
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int GetChoices(IntPtr arg, int size)
    {
        try
        {
            _preparedText = RequireSession().FieldChoices(ReadUtf8(arg, size));
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int CopyPreparedText(IntPtr arg, int size)
    {
        if (_preparedText is null || arg == IntPtr.Zero)
            return 1;
        var utf8 = System.Text.Encoding.UTF8.GetBytes(_preparedText);
        if (size < utf8.Length)
            return 1;
        Marshal.Copy(utf8, 0, arg, utf8.Length);
        return 0;
    }

    public static int PreviewShowdown(IntPtr arg, int size)
    {
        try
        {
            _preparedText = RequireSession().PreviewShowdown(ReadUtf8(arg, size));
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int ImportShowdown(IntPtr arg, int size)
    {
        try
        {
            RequireSession().ImportShowdown(ReadUtf8(arg, size));
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int PrepareShowdownExport(IntPtr arg, int size)
    {
        try
        {
            var scope = ReadUtf8(arg, size);
            var session = RequireSession();
            _preparedText = scope switch
            {
                "party" => session.ExportPartyShowdown(),
                "box" => session.ExportCurrentBoxShowdown(),
                _ => session.ExportShowdown(),
            };
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int PrepareEntityCopy(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedPng = RequireSession().ExportEntity();
            return _preparedPng.Length;
        }
        catch
        {
            _preparedPng = null;
            return -1;
        }
    }

    public static int PrepareEntityFileName(IntPtr arg, int size)
    {
        _ = arg;
        _ = size;
        try
        {
            _preparedText = RequireSession().EntityFileName;
            return EncodingLength(_preparedText);
        }
        catch
        {
            _preparedText = null;
            return -1;
        }
    }

    public static int ImportEntity(IntPtr arg, int size)
    {
        try
        {
            if (arg == IntPtr.Zero || size <= 0)
                return 1;
            var data = new byte[size];
            Marshal.Copy(arg, data, 0, size);
            RequireSession().ImportEntity(data);
            return 0;
        }
        catch
        {
            return 1;
        }
    }

    private static byte[]? _preparedPng;
    private static string? _preparedText;

    private static int EncodingLength(string text) => System.Text.Encoding.UTF8.GetByteCount(text);

    private static EditorSession RequireSession()
        => App.Session ?? throw new InvalidOperationException("No save is open.");

    private static byte[] ComposeSlot(EditorSession session, string key)
    {
        ParseSlotKey(key, out var party, out var box, out var slot);
        return party ? session.ComposePartySlotPng(slot) : session.ComposeBoxSlotPng(box, slot);
    }

    private static void ParseSlotKey(string key, out bool party, out int box, out int slot)
    {
        var parts = key.Split(':');
        if (parts.Length == 2 && parts[0] == "party" && int.TryParse(parts[1], out slot))
        {
            party = true;
            box = 0;
            return;
        }

        if (parts.Length == 3 && parts[0] == "box"
            && int.TryParse(parts[1], out box)
            && int.TryParse(parts[2], out slot))
        {
            party = false;
            return;
        }

        throw new FormatException("Slot key was not box:box:slot or party:slot.");
    }

    private static string ReadUtf8(IntPtr arg, int size)
    {
        if (arg == IntPtr.Zero)
            throw new ArgumentNullException(nameof(arg));
        var path = size > 0
            ? Marshal.PtrToStringUTF8(arg, size)
            : Marshal.PtrToStringUTF8(arg);
        if (string.IsNullOrEmpty(path))
            throw new InvalidOperationException("Path was empty.");
        return path.TrimEnd('\0');
    }
}
