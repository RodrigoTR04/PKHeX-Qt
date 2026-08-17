namespace PKHeX.Editor;

internal static class GlowEdges
{
    private const int PollutePixelColorIndex = 0;
    private const byte TransparencyThresholdHalf = 0x80;

    public static void Apply(Span<byte> data, byte blue, byte green, byte red, int width, int reach = 3, double amount = 0.0777)
    {
        for (int i = data.Length - 4; i >= 0; i -= 4)
            data[i + PollutePixelColorIndex] = 0;
        PollutePixels(data, width, reach, amount);
        CleanPollutedPixels(data, blue, green, red);
    }

    public static void SetAllUsedPixelsOpaque(Span<byte> data, byte threshold = TransparencyThresholdHalf)
    {
        for (int i = data.Length - 4; i >= 0; i -= 4)
        {
            if (data[i + 3] >= threshold)
                data[i + 3] = 0xFF;
        }
    }

    public static void RemovePixels(Span<byte> pixels, ReadOnlySpan<byte> original, byte threshold = TransparencyThresholdHalf)
    {
        for (int i = original.Length - 4; i >= 0; i -= 4)
        {
            if (original[i + 3] >= threshold)
            {
                pixels[i] = 0;
                pixels[i + 1] = 0;
                pixels[i + 2] = 0;
                pixels[i + 3] = 0;
            }
        }
    }

    private static void PollutePixels(Span<byte> data, int width, int reach, double amount, byte threshold = TransparencyThresholdHalf)
    {
        int stride = width * 4;
        int height = data.Length / stride;
        for (int i = data.Length - 4; i >= 0; i -= 4)
        {
            if (data[i + 3] < threshold)
                continue;

            int x = (i % stride) / 4;
            int y = i / stride;
            int left = Math.Max(0, x - reach);
            int right = Math.Min(width - 1, x + reach);
            int top = Math.Max(0, y - reach);
            int bottom = Math.Min(height - 1, y + reach);
            for (int ix = left; ix <= right; ix++)
            {
                for (int iy = top; iy <= bottom; iy++)
                {
                    var c = 4 * (ix + (iy * width));
                    ref var b = ref data[c + PollutePixelColorIndex];
                    b += (byte)(amount * (0xFF - b));
                }
            }
        }
    }

    private static void CleanPollutedPixels(Span<byte> data, byte blue, byte green, byte red)
    {
        for (int i = data.Length - 4; i >= 0; i -= 4)
        {
            var transparency = data[i + PollutePixelColorIndex];
            if (transparency == 0)
                continue;

            data[i + 0] = blue;
            data[i + 1] = green;
            data[i + 2] = red;
            data[i + 3] = transparency;
        }
    }
}
