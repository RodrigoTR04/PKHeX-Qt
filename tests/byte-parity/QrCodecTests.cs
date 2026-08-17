using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class QrCodecTests
{
    [Fact]
    public void export_message_matches_core_for_the_current_entity()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot(pk =>
        {
            pk.Move1 = (ushort)Move.Tackle;
            pk.Nickname = "QRTEST";
            pk.IsNicknamed = true;
        });
        var expected = QRMessageUtil.GetMessage(entity);

        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);

        Assert.Equal(expected, session.ExportQrMessage());
        Assert.StartsWith("null/#", expected, StringComparison.Ordinal);
    }

    [Fact]
    public void import_message_loads_the_same_entity_core_would()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot(pk =>
        {
            pk.Move1 = (ushort)Move.Tackle;
            pk.Nickname = "QRIN";
            pk.IsNicknamed = true;
        });
        var message = QRMessageUtil.GetMessage(entity);
        var blank = OccupiedSave.WithBoxSlot(pk => pk.Species = (ushort)Species.Charmander).Save;

        var session = EditorSession.Load(blank);
        session.SelectBoxSlot(0, 0);
        session.ImportQrMessage(message);

        Assert.Equal(entity.Species.ToString(), session.GetField("CB_Species"));
        Assert.Equal("QRIN", session.GetField("TB_Nickname"));
    }

    [Fact]
    public void png_round_trip_preserves_the_core_payload()
    {
        var (_, entity) = OccupiedSave.WithBoxSlot(pk => pk.Move1 = (ushort)Move.Tackle);
        var message = QRMessageUtil.GetMessage(entity);
        var png = QrCodec.EncodePng(message);

        Assert.Equal([0x89, 0x50, 0x4E, 0x47], png.AsSpan(0, 4).ToArray());
        Assert.Equal(message, QrCodec.DecodePng(png));
    }

    [Fact]
    public void import_png_loads_the_encoded_entity()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot(pk =>
        {
            pk.Nickname = "PNGQR";
            pk.IsNicknamed = true;
        });
        var session = EditorSession.Load(save);
        session.SelectBoxSlot(0, 0);
        var png = session.ExportQrPng();

        var other = EditorSession.Load(OccupiedSave.WithBoxSlot().Save);
        other.SelectBoxSlot(0, 0);
        other.ImportQrPng(png);

        Assert.Equal("PNGQR", other.GetField("TB_Nickname"));
        Assert.Equal(entity.Species.ToString(), other.GetField("CB_Species"));
    }
}
