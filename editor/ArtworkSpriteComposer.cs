using PKHeX.Core;
using SkiaSharp;

namespace PKHeX.Editor;

/// <summary>
/// Composites Artwork sprites the way Original PKHeX's artwork builder does, then encodes PNG for the Host.
/// </summary>
public sealed class ArtworkSpriteComposer
{
    public const int Width = 68;
    public const int Height = 56;
    private const int ItemShiftX = 2;
    private const int ItemShiftY = 2;
    private const int ItemMaxSize = 32;
    private const int EggItemShiftX = 18;
    private const int EggItemShiftY = 1;
    private const int SlotTeamShiftX = Width - 19;
    private const double UnknownFormTransparency = 0.5;
    private const double ShinyTransparency = 0.7;
    private const double EggUnderLayerTransparency = 0.33;

    public static bool ShowEggSpriteAsItem { get; set; } = true;
    public static bool ShowEncounterBall { get; set; } = true;

    private readonly IReadOnlyList<string> _spriteDirs;
    private readonly IReadOnlyList<string> _wallpaperDirs;
    private GameVersion _version;

    public ArtworkSpriteComposer()
        : this(AssetLocator.SpriteDirectories(), AssetLocator.WallpaperDirectories())
    {
    }

    public ArtworkSpriteComposer(IReadOnlyList<string> spriteDirectories, IReadOnlyList<string> wallpaperDirectories)
    {
        _spriteDirs = spriteDirectories;
        _wallpaperDirs = wallpaperDirectories;
    }

    public static byte[] BgraPixels(ReadOnlySpan<byte> png, out int width, out int height)
    {
        using var bitmap = SKBitmap.Decode(png.ToArray())
            ?? throw new InvalidDataException("Could not decode PNG.");
        using var bgra = CopyBgra(bitmap);
        width = bgra.Width;
        height = bgra.Height;
        return CopyPixels(bgra);
    }

    public byte[] Compose(PKM pk, SaveFile sav)
    {
        Initialize(sav);
        if (pk.Species == 0)
            return FileBytes("b_0");

        var formarg = pk is IFormArgument f ? f.FormArgument : 0;
        var shiny = ShinyExtensions.GetType(pk);
        if (IsBareArtwork(pk, shiny))
        {
            var name = ArtworkSpriteName.GetArtworkResourceName(
                pk.Species, pk.Form, pk.Gender, formarg, pk.Context, shiny.IsShiny());
            var raw = FileBytesOrNull(name);
            if (raw is not null)
                return raw;
        }
        using var sprite = BuildSprite(pk.Species, pk.Form, pk.Gender, formarg, pk.SpriteItem, pk.IsEgg, shiny, pk.Context, pk.Ball);

        if (pk is IShadowCapture { IsShadow: true })
        {
            using var withShadow = ApplyShadow(pk, sprite, shiny);
            return Encode(withShadow);
        }

        if (pk is IGigantamaxReadOnly { CanGigantamax: true })
        {
            using var dyna = LoadRequired("dyna");
            using var layered = Layer(sprite, dyna, (sprite.Width - dyna.Width) / 2, 0);
            return Encode(layered);
        }

        if (pk is IAlphaReadOnly { IsAlpha: true })
        {
            using var alpha = LoadRequired("alpha_alt");
            using var layered = Layer(sprite, alpha, SlotTeamShiftX, 0);
            return Encode(layered);
        }

        return Encode(sprite);
    }

    public byte[] ComposeWallpaper(SaveFile sav, int box)
    {
        var name = WallpaperResourceName.GetForSave(sav, box);
        var path = AssetLocator.FindPng(name, _wallpaperDirs)
            ?? AssetLocator.FindPng(WallpaperResourceName.Default, _wallpaperDirs)
            ?? throw new FileNotFoundException($"Wallpaper '{name}' was not found.");
        return File.ReadAllBytes(path);
    }

    private void Initialize(SaveFile sav)
    {
        _version = sav.Version;
        if (sav.Generation != 3)
            return;
        if (_version == GameVersion.FRLG)
            _version = ReferenceEquals(sav.Personal, PersonalTable.FR) ? GameVersion.FR : GameVersion.LG;
    }

    private SKBitmap BuildSprite(ushort species, byte form, byte gender, uint formarg, int heldItem, bool isEgg, Shiny shiny, EntityContext context, byte ball)
    {
        if (context == EntityContext.Gen3 && species == (int)Species.Deoxys)
            form = GetDeoxysForm(_version);
        else if (context == EntityContext.Gen4 && species == (int)Species.Arceus)
            form = GetArceusForm4(form);

        var baseImage = GetBaseImage(species, form, gender, formarg, shiny.IsShiny(), context);
        if (isEgg)
            baseImage = LayerOverImageEgg(baseImage, species, heldItem != 0);
        if (heldItem > 0)
            baseImage = LayerOverImageItem(baseImage, heldItem, context);
        if (ShowEncounterBall && ball != 0)
            baseImage = LayerOverImageBall(baseImage, ball);
        if (shiny.IsShiny())
        {
            if (shiny == Shiny.AlwaysSquare && !context.IsSquareShinyDifferentiated)
                shiny = Shiny.Always;
            baseImage = LayerOverImageShiny(baseImage, shiny);
        }

        return baseImage;
    }

    private SKBitmap GetBaseImage(ushort species, byte form, byte gender, uint formarg, bool shiny, EntityContext context)
    {
        if (FormInfo.IsTotemForm(species, form, context))
        {
            var totem = GetBaseImageTotem(species, form, gender, formarg, shiny, context);
            if (totem is not null)
                return totem;
        }

        var img = GetBaseImageDefault(species, form, gender, formarg, shiny, context);
        return img ?? GetBaseImageFallback(species, form, gender, formarg, shiny, context);
    }

    private SKBitmap? GetBaseImageTotem(ushort species, byte form, byte gender, uint formarg, bool shiny, EntityContext context)
    {
        var baseform = FormInfo.GetTotemBaseForm(species, form);
        var b = GetBaseImageDefault(species, baseform, gender, formarg, shiny, context);
        if (b is null)
            return null;

        using var glow = GlowHollow(b, 0, 165, 255);
        var layered = Layer(b, glow, 0, 0);
        b.Dispose();
        return layered;
    }

    private SKBitmap? GetBaseImageDefault(ushort species, byte form, byte gender, uint formarg, bool shiny, EntityContext context)
    {
        var file = ArtworkSpriteName.GetArtworkResourceName(species, form, gender, formarg, context, shiny);
        var resource = LoadOptional(file);
        if (resource is null)
        {
            file = ArtworkSpriteName.GetClassicResourceName(species, form, gender, formarg, context, shiny);
            resource = LoadOptional(file);
        }

        return resource;
    }

    private SKBitmap GetBaseImageFallback(ushort species, byte form, byte gender, uint formarg, bool shiny, EntityContext context)
    {
        if (shiny)
        {
            var img = GetBaseImageDefault(species, form, gender, formarg, false, context);
            if (img is not null)
                return img;
        }

        var baseImage = LoadOptional(ArtworkSpriteName.GetSpeciesOnlyArtwork(species));
        using var unknown = LoadRequired("b_unknown");
        if (baseImage is null)
            return unknown.Copy();
        var layered = Layer(baseImage, unknown, 0, 0, UnknownFormTransparency);
        baseImage.Dispose();
        return layered;
    }

    private SKBitmap LayerOverImageItem(SKBitmap baseImage, int item, EntityContext context)
    {
        using var itemimg = GetItemSprite(item, context);
        int x = baseImage.Width - itemimg.Width - ((ItemMaxSize - itemimg.Width) / 4) - ItemShiftX;
        int y = baseImage.Height - itemimg.Height - ItemShiftY;
        var layered = Layer(baseImage, itemimg, x, y);
        baseImage.Dispose();
        return layered;
    }

    private SKBitmap GetItemSprite(int item, EntityContext context)
    {
        var lump = HeldItemLumpUtil.GetIsLump(item, context);
        return lump switch
        {
            HeldItemLumpImage.TechnicalMachine => LoadRequired("aitem_tm"),
            HeldItemLumpImage.TechnicalRecord => LoadRequired("bitem_tr"),
            _ => LoadOptional(ArtworkSpriteName.GetItemResourceName(item)) ?? LoadRequired("bitem_unk"),
        };
    }

    private SKBitmap LayerOverImageShiny(SKBitmap baseImage, Shiny shiny)
    {
        var rareName = shiny is Shiny.AlwaysSquare ? "rare_icon_alt_2" : "rare_icon_alt";
        using var rare = LoadRequired(rareName);
        var layered = Layer(baseImage, rare, 0, 0, ShinyTransparency);
        baseImage.Dispose();
        return layered;
    }

    private SKBitmap LayerOverImageEgg(SKBitmap baseImage, ushort species, bool hasItem)
    {
        if (ShowEggSpriteAsItem && !hasItem)
            return LayerOverImageEggAsItem(baseImage, species);
        return LayerOverImageEggTransparentSpecies(baseImage, species);
    }

    private SKBitmap LayerOverImageEggTransparentSpecies(SKBitmap baseImage, ushort species)
    {
        ChangeOpacity(baseImage, EggUnderLayerTransparency);
        using var egg = GetEggSprite(species);
        var layered = Layer(baseImage, egg, 0, 0);
        baseImage.Dispose();
        return layered;
    }

    private SKBitmap LayerOverImageEggAsItem(SKBitmap baseImage, ushort species)
    {
        using var egg = GetEggSprite(species);
        var layered = Layer(baseImage, egg, EggItemShiftX, EggItemShiftY);
        baseImage.Dispose();
        return layered;
    }

    private SKBitmap LayerOverImageBall(SKBitmap baseImage, byte ball)
    {
        using var ballSprite = LoadOptional(ArtworkSpriteName.GetBallResourceName(ball)) ?? LoadRequired("_ball4");
        var layered = Layer(baseImage, ballSprite, 0, baseImage.Height - ballSprite.Height);
        baseImage.Dispose();
        return layered;
    }

    private SKBitmap GetEggSprite(ushort species)
        => species == (int)Species.Manaphy ? LoadRequired("a_490_e") : LoadRequired("a_egg");

    private SKBitmap ApplyShadow(PKM pk, SKBitmap img, Shiny shiny)
    {
        const ushort lugia = (int)Species.Lugia;
        SKBitmap sprite = img;
        var ownSprite = false;
        if (pk.Species is lugia)
        {
            using var shadowBase = LoadRequired("b_249x");
            sprite = BuildSpriteFromBase(shadowBase, lugia, pk.SpriteItem, pk.IsEgg, shiny, pk.Context, pk.Ball);
            ownSprite = true;
        }

        try
        {
            using var glow = GlowHollow(sprite, 75, 0, 130);
            return Layer(glow, sprite, 0, 0);
        }
        finally
        {
            if (ownSprite)
                sprite.Dispose();
        }
    }

    private SKBitmap BuildSpriteFromBase(SKBitmap baseSprite, ushort species, int heldItem, bool isEgg, Shiny shiny, EntityContext context, byte ball)
    {
        var copy = baseSprite.Copy();
        if (isEgg)
            copy = LayerOverImageEgg(copy, species, heldItem != 0);
        if (heldItem > 0)
            copy = LayerOverImageItem(copy, heldItem, context);
        if (ShowEncounterBall && ball != 0)
            copy = LayerOverImageBall(copy, ball);
        if (shiny.IsShiny())
            copy = LayerOverImageShiny(copy, shiny);
        return copy;
    }

    private static SKBitmap GlowHollow(SKBitmap baseSprite, byte blue, byte green, byte red)
    {
        var glow = CopyBgra(baseSprite);
        var pixels = CopyPixels(glow);
        var original = CopyPixels(baseSprite);
        GlowEdges.SetAllUsedPixelsOpaque(pixels);
        GlowEdges.Apply(pixels, blue, green, red, glow.Width);
        GlowEdges.RemovePixels(pixels, original);
        WritePixels(glow, pixels);
        return glow;
    }

    private static bool IsBareArtwork(PKM pk, Shiny shiny)
    {
        if (pk.IsEgg || pk.SpriteItem > 0 || pk.Ball != 0 || shiny.IsShiny())
            return false;
        if (pk is IShadowCapture { IsShadow: true })
            return false;
        if (pk is IGigantamaxReadOnly { CanGigantamax: true })
            return false;
        if (pk is IAlphaReadOnly { IsAlpha: true })
            return false;
        return !FormInfo.IsTotemForm(pk.Species, pk.Form, pk.Context);
    }

    private byte[] FileBytes(string resourceName)
        => FileBytesOrNull(resourceName)
           ?? throw new FileNotFoundException($"Sprite '{resourceName}' was not found.");

    private byte[]? FileBytesOrNull(string resourceName)
    {
        var path = AssetLocator.FindPng(resourceName, _spriteDirs);
        return path is null ? null : File.ReadAllBytes(path);
    }

    private SKBitmap LoadRequired(string resourceName)
        => LoadOptional(resourceName)
           ?? throw new FileNotFoundException($"Sprite '{resourceName}' was not found.");

    private SKBitmap? LoadOptional(string resourceName)
    {
        var path = AssetLocator.FindPng(resourceName, _spriteDirs);
        if (path is null)
            return null;
        var bitmap = SKBitmap.Decode(path);
        if (bitmap is null)
            return null;
        return CopyBgra(bitmap, disposeSource: true);
    }

    private static byte[] Encode(SKBitmap bitmap)
    {
        using var image = SKImage.FromBitmap(bitmap);
        using var data = image.Encode(SKEncodedImageFormat.Png, 100);
        return data.ToArray();
    }

    private static SKBitmap Layer(SKBitmap baseLayer, SKBitmap overLayer, int x, int y, double opacity = 1)
    {
        var dest = CopyBgra(baseLayer);
        using var overlay = opacity is > 0.01 and < 1
            ? WithOpacity(overLayer, opacity)
            : CopyBgra(overLayer);
        using var canvas = new SKCanvas(dest);
        using var image = SKImage.FromBitmap(overlay);
        canvas.DrawImage(image, x, y);
        canvas.Flush();
        dest.NotifyPixelsChanged();
        return dest;
    }

    private static SKBitmap WithOpacity(SKBitmap src, double opacity)
    {
        var copy = CopyBgra(src);
        ChangeOpacity(copy, opacity);
        return copy;
    }

    private static void ChangeOpacity(SKBitmap bmp, double trans)
    {
        if (trans is <= 0.01 or > 1)
            return;
        var data = CopyPixels(bmp);
        for (int i = data.Length - 4; i >= 0; i -= 4)
            data[i + 3] = (byte)(data[i + 3] * trans);
        WritePixels(bmp, data);
    }

    private static SKBitmap CopyBgra(SKBitmap bitmap, bool disposeSource = false)
    {
        SKBitmap copy;
        if (bitmap.ColorType == SKColorType.Bgra8888)
            copy = bitmap.Copy() ?? throw new InvalidOperationException("Could not copy sprite bitmap.");
        else
            copy = bitmap.Copy(SKColorType.Bgra8888) ?? bitmap.Copy() ?? throw new InvalidOperationException("Could not copy sprite bitmap.");

        if (disposeSource)
            bitmap.Dispose();
        return copy;
    }

    private static byte[] CopyPixels(SKBitmap bitmap)
    {
        var dest = new byte[bitmap.ByteCount];
        System.Runtime.InteropServices.Marshal.Copy(bitmap.GetPixels(), dest, 0, dest.Length);
        return dest;
    }

    private static void WritePixels(SKBitmap bitmap, byte[] data)
    {
        System.Runtime.InteropServices.Marshal.Copy(data, 0, bitmap.GetPixels(), Math.Min(data.Length, bitmap.ByteCount));
    }

    private static byte GetDeoxysForm(GameVersion version) => version switch
    {
        GameVersion.FR => 1,
        GameVersion.LG => 2,
        GameVersion.E => 3,
        _ => 0,
    };

    private static byte GetArceusForm4(byte form) => form switch
    {
        > 9 => --form,
        9 => byte.MaxValue,
        _ => form,
    };
}
