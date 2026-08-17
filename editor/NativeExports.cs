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
