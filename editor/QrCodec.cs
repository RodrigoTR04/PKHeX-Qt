using PKHeX.Core;
using QRCoder;
using SkiaSharp;
using ZXing;
using ZXing.Common;

namespace PKHeX.Editor;

/// <summary>
/// Portable QR encode/decode. Payload text comes from Core's <see cref="QRMessageUtil"/>; images are PNG bytes, not System.Drawing.
/// </summary>
public static class QrCodec
{
    public const int PixelsPerModule = 4;
    private const int QuietModules = 4;

    public static string Message(PKM pk, int box = 0, int slot = 0, int copies = 1)
    {
        ArgumentNullException.ThrowIfNull(pk);
        if (pk is PK7 pk7)
            return QRMessageUtil.GetMessage(pk7, box, slot, copies);
        return QRMessageUtil.GetMessage(pk);
    }

    public static byte[] EncodePng(string message)
    {
        ArgumentException.ThrowIfNullOrEmpty(message);
        using var data = QRCodeGenerator.GenerateQrCode(message, QRCodeGenerator.ECCLevel.Q);
        var matrix = data.ModuleMatrix;
        var modules = matrix.Count;
        var dim = (modules + QuietModules * 2) * PixelsPerModule;
        using var bitmap = new SKBitmap(dim, dim, SKColorType.Rgba8888, SKAlphaType.Opaque);
        bitmap.Erase(SKColors.White);
        var dark = new SKColor(0, 0, 0, 255);
        for (var y = 0; y < modules; y++)
        {
            var row = matrix[y];
            for (var x = 0; x < modules; x++)
            {
                if (!row[x])
                    continue;
                var left = (x + QuietModules) * PixelsPerModule;
                var top = (y + QuietModules) * PixelsPerModule;
                for (var dy = 0; dy < PixelsPerModule; dy++)
                {
                    for (var dx = 0; dx < PixelsPerModule; dx++)
                        bitmap.SetPixel(left + dx, top + dy, dark);
                }
            }
        }

        using var image = SKImage.FromBitmap(bitmap);
        using var png = image.Encode(SKEncodedImageFormat.Png, 100);
        return png.ToArray();
    }

    public static string DecodePng(ReadOnlyMemory<byte> png)
    {
        if (png.Length == 0)
            throw new InvalidDataException("QR image was empty.");
        using var bitmap = SKBitmap.Decode(png.ToArray())
            ?? throw new InvalidDataException("QR image could not be read.");
        var gray = ToGray(bitmap);
        var source = new RGBLuminanceSource(gray, bitmap.Width, bitmap.Height, RGBLuminanceSource.BitmapFormat.Gray8);
        var reader = new BarcodeReaderGeneric
        {
            AutoRotate = true,
            Options = new DecodingOptions
            {
                PossibleFormats = [BarcodeFormat.QR_CODE],
                TryHarder = true,
                CharacterSet = "ISO-8859-1",
            },
        };
        var result = reader.Decode(source)
            ?? throw new InvalidDataException("Reader could not find QR data in the image.");
        return result.Text;
    }

    private static byte[] ToGray(SKBitmap bitmap)
    {
        var gray = new byte[bitmap.Width * bitmap.Height];
        var i = 0;
        for (var y = 0; y < bitmap.Height; y++)
        {
            for (var x = 0; x < bitmap.Width; x++)
            {
                var c = bitmap.GetPixel(x, y);
                gray[i++] = (byte)((c.Red + c.Green + c.Blue) / 3);
            }
        }
        return gray;
    }
}
