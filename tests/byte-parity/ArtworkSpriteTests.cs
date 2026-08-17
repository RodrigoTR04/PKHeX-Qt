using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class ArtworkSpriteTests
{
    [Fact]
    public void occupied_species_layer_uses_artwork_resource_not_classic()
    {
        var name = ArtworkSpriteName.GetArtworkResourceName(
            (ushort)Species.Bulbasaur, 0, 0, 0, EntityContext.Gen5, shiny: false);

        Assert.StartsWith("a_", name, StringComparison.Ordinal);
        Assert.False(name.StartsWith("b_", StringComparison.Ordinal));
        Assert.Equal("a_1", name);
    }

    [Fact]
    public void occupied_box_slot_png_pixels_match_the_oracle_artwork_file()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot();
        Assert.Equal((ushort)Species.Bulbasaur, entity.Species);
        Assert.Equal(0, entity.Ball);
        var session = EditorSession.Load(save);
        var composed = session.ComposeBoxSlotPng(0, 0);
        var expectedPath = Path.Combine(
            RepoPaths.Root,
            "PKHeX-26.07.07",
            "PKHeX.Drawing.PokeSprite",
            "Resources",
            "img",
            "Artwork Pokemon Sprites",
            "a_1.png");

        Assert.True(File.Exists(expectedPath), expectedPath);
        var expected = File.ReadAllBytes(expectedPath);
        Assert.Equal(
            ArtworkSpriteComposer.BgraPixels(expected, out var ew, out var eh),
            ArtworkSpriteComposer.BgraPixels(composed, out var cw, out var ch));
        Assert.Equal(ew, cw);
        Assert.Equal(eh, ch);
    }

    [Fact]
    public void shiny_overlay_changes_the_top_left_from_the_bare_artwork()
    {
        var (plainSave, _) = OccupiedSave.WithBoxSlot();
        var (shinySave, shinyEntity) = OccupiedSave.WithBoxSlot(pk => pk.SetShiny());
        if (!SaveUtil.TryGetSaveFile(shinySave, out var loaded) || !loaded.GetBoxSlotAtIndex(0, 0).IsShiny)
            throw new InvalidOperationException("Shiny PID was not stored in the save.");
        var plain = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(plainSave).ComposeBoxSlotPng(0, 0), out _, out _);
        var shiny = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(shinySave).ComposeBoxSlotPng(0, 0), out _, out _);

        Assert.NotEqual(plain, shiny);
    }

    [Fact]
    public void held_item_overlay_changes_pixels_from_the_bare_artwork()
    {
        var (plainSave, _) = OccupiedSave.WithBoxSlot();
        var (itemSave, _) = OccupiedSave.WithBoxSlot(pk => pk.HeldItem = 1);
        var plain = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(plainSave).ComposeBoxSlotPng(0, 0), out _, out _);
        var withItem = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(itemSave).ComposeBoxSlotPng(0, 0), out _, out _);

        Assert.NotEqual(plain, withItem);
    }

    [Fact]
    public void egg_overlay_changes_pixels_from_the_bare_artwork()
    {
        var (plainSave, _) = OccupiedSave.WithBoxSlot();
        var (eggSave, _) = OccupiedSave.WithBoxSlot(pk => pk.IsEgg = true);
        var plain = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(plainSave).ComposeBoxSlotPng(0, 0), out _, out _);
        var egg = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(eggSave).ComposeBoxSlotPng(0, 0), out _, out _);

        Assert.NotEqual(plain, egg);
    }

    [Fact]
    public void ball_overlay_changes_pixels_from_the_bare_artwork()
    {
        var (plainSave, _) = OccupiedSave.WithBoxSlot(pk => pk.Ball = 4);
        var (ballSave, _) = OccupiedSave.WithBoxSlot(pk => pk.Ball = 1);
        var poke = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(plainSave).ComposeBoxSlotPng(0, 0), out _, out _);
        var master = ArtworkSpriteComposer.BgraPixels(
            EditorSession.Load(ballSave).ComposeBoxSlotPng(0, 0), out _, out _);

        Assert.NotEqual(poke, master);
    }

    [Fact]
    public void occupied_party_slot_png_matches_the_same_box_slot_compose()
    {
        var (boxSave, _) = OccupiedSave.WithBoxSlot();
        var (partySave, _) = OccupiedSave.WithPartySlot();
        var box = EditorSession.Load(boxSave).ComposeBoxSlotPng(0, 0);
        var party = EditorSession.Load(partySave).ComposePartySlotPng(0);

        Assert.Equal(
            ArtworkSpriteComposer.BgraPixels(box, out _, out _),
            ArtworkSpriteComposer.BgraPixels(party, out _, out _));
    }
}
