using System.Text;
using PKHeX.Core;
using static PKHeX.Core.Species;

namespace PKHeX.Editor;

/// <summary>
/// Artwork resource names matching Original PKHeX's <c>SpriteName</c> with an <c>a</c> prefix.
/// </summary>
public static class ArtworkSpriteName
{
    public static bool AllowShinySprite { get; set; }

    private const char Separator = '_';
    private const char Cosplay = 'c';
    private const char Shiny = 's';
    private const char GGStarter = 'p';

    public static string GetBallResourceName(byte ball) => $"_ball{ball}";

    public static string GetArtworkResourceName(ushort species, byte form, byte gender, uint formarg, EntityContext context, bool shiny)
        => 'a' + GetResourceStringSprite(species, form, gender, formarg, context, shiny);

    public static string GetClassicResourceName(ushort species, byte form, byte gender, uint formarg, EntityContext context, bool shiny)
        => 'b' + GetResourceStringSprite(species, form, gender, formarg, context, shiny);

    public static string GetSpeciesOnlyArtwork(ushort species) => $"a_{species}";

    public static string GetItemResourceName(int item) => $"aitem_{item}";

    public static string GetResourceStringSprite(ushort species, byte form, byte gender, uint formarg, EntityContext context, bool shiny)
    {
        if (SpeciesDefaultFormSprite.Contains(species))
            form = 0;

        if (species == (ushort)Xerneas && context == EntityContext.Gen9a)
            form = 1;

        var sb = new StringBuilder(12);
        sb.Append(Separator).Append(species);

        if (form != 0)
        {
            sb.Append(Separator).Append(form);

            if (species == (ushort)Pikachu)
            {
                if (context == EntityContext.Gen6)
                    sb.Append(Cosplay);
                else if (form == 8)
                    sb.Append(GGStarter);
            }
            else if (species == (ushort)Eevee && form == 1)
            {
                sb.Append(GGStarter);
            }
        }

        if (gender == 1 && SpeciesGenderedSprite.Contains(species))
            sb.Append('f');

        if (species == (ushort)Alcremie)
        {
            if (form == 0)
                sb.Append(Separator).Append(form);
            sb.Append(Separator).Append(formarg);
        }

        if (shiny && AllowShinySprite)
            sb.Append(Shiny);
        return sb.ToString();
    }

    private static ReadOnlySpan<ushort> SpeciesDefaultFormSprite =>
    [
        (ushort)Mothim,
        (ushort)Scatterbug,
        (ushort)Spewpa,
        (ushort)Rockruff,
        (ushort)Mimikyu,
        (ushort)Sinistea,
        (ushort)Polteageist,
        (ushort)Urshifu,
        (ushort)Dudunsparce,
        (ushort)Poltchageist,
        (ushort)Sinistcha,
    ];

    private static ReadOnlySpan<ushort> SpeciesGenderedSprite =>
    [
        (ushort)Hippopotas,
        (ushort)Hippowdon,
        (ushort)Unfezant,
        (ushort)Frillish,
        (ushort)Jellicent,
        (ushort)Pyroar,
    ];
}
