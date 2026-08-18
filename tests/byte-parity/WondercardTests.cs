using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class WondercardTests
{
    [Fact]
    public void cancel_leaves_the_save_unchanged()
    {
        var fixture = GoldenSave.Gen5BlankExport();
        var session = EditorSession.Load(fixture);
        session.OpenWondercards();
        session.ImportWondercard(PotionPgf().Write().ToArray(), ".pgf");
        session.SetWondercardSlot(0);
        session.CancelWondercards();

        Assert.Equal(fixture, session.Export());
    }

    [Fact]
    public void gen5_import_and_save_matches_winforms_album_write()
    {
        var fixture = GoldenSave.Gen5BlankExport();
        if (!SaveUtil.TryGetSaveFile(fixture, out var expectedSav))
            throw new InvalidOperationException();
        var gift = PotionPgf();
        WriteAlbumLikeWinForms(expectedSav, gift, requestedIndex: 0);

        var session = EditorSession.Load(fixture);
        session.OpenWondercards();
        session.ImportWondercard(gift.Write().ToArray(), ".pgf");
        session.SetWondercardSlot(0);
        session.SaveWondercards();

        Assert.Equal(expectedSav.Write().ToArray(), session.Export());
    }

    [Fact]
    public void gen6_import_and_save_matches_winforms_album_write()
    {
        var sav = BlankSaveFile.Get(GameVersion.X);
        var before = sav.Write().ToArray();
        var gift = PotionWc6();
        var expectedSav = BlankSaveFile.Get(GameVersion.X);
        WriteAlbumLikeWinForms(expectedSav, gift, requestedIndex: 0);

        var session = EditorSession.FromSave(sav);
        session.OpenWondercards();
        session.ImportWondercard(gift.Write().ToArray(), ".wc6");
        session.SetWondercardSlot(0);
        session.SaveWondercards();

        Assert.Equal(expectedSav.Write().ToArray(), session.Export());
        Assert.NotEqual(before, session.Export());
    }

    [Fact]
    public void set_hijacks_to_the_first_empty_slot_of_the_same_type()
    {
        var fixture = ClearedGen5Album();
        if (!SaveUtil.TryGetSaveFile(fixture, out var expectedSav))
            throw new InvalidOperationException();
        var gift = PotionPgf();
        WriteAlbumLikeWinForms(expectedSav, gift, requestedIndex: 5);

        var session = EditorSession.Load(fixture);
        session.OpenWondercards();
        session.ImportWondercard(gift.Write().ToArray(), ".pgf");
        session.SetWondercardSlot(5);
        session.SaveWondercards();

        if (!SaveUtil.TryGetSaveFile(session.Export(), out var got))
            throw new InvalidOperationException();
        var cards = ((IMysteryGiftStorageProvider)got).MysteryGiftStorage;
        Assert.False(cards.GetMysteryGift(0).IsEmpty);
        Assert.True(cards.GetMysteryGift(5).IsEmpty);
        Assert.Equal(123, cards.GetMysteryGift(0).CardID);
        Assert.Equal(((IMysteryGiftStorageProvider)expectedSav).MysteryGiftStorage.GetMysteryGift(0).CardID, cards.GetMysteryGift(0).CardID);
    }

    private static byte[] ClearedGen5Album()
    {
        if (!SaveUtil.TryGetSaveFile(GoldenSave.Gen5BlankExport(), out var sav))
            throw new InvalidOperationException();
        var cards = ((IMysteryGiftStorageProvider)sav).MysteryGiftStorage;
        for (var i = 0; i < cards.GiftCountMax; i++)
        {
            var gift = cards.GetMysteryGift(i);
            gift.Clear();
            cards.SetMysteryGift(i, gift);
        }
        if (cards is IMysteryGiftFlags flags)
            flags.ClearReceivedFlags();
        if (cards is MysteryBlock5 s5)
            s5.EndAccess();
        return sav.Write().ToArray();
    }

    private static PGF PotionPgf()
    {
        return new PGF
        {
            IsItem = true,
            ItemID = 17,
            CardID = 123,
            Language = 2,
            CardTitle = "Potion",
        };
    }

    private static WC6 PotionWc6()
    {
        return new WC6
        {
            IsItem = true,
            ItemID = 17,
            Quantity = 1,
            CardID = 123,
            Language = 2,
            CardTitle = "Potion",
        };
    }

    private static void WriteAlbumLikeWinForms(SaveFile sav, DataMysteryGift gift, int requestedIndex)
    {
        var cards = ((IMysteryGiftStorageProvider)sav).MysteryGiftStorage;
        var album = LoadAlbum(sav, cards);
        if (!gift.IsCardCompatible(sav, out _))
            throw new InvalidOperationException("Gift is not compatible.");

        var index = requestedIndex;
        var lastUnfilled = LastUnfilledByType(gift, album);
        if (lastUnfilled > -1 && lastUnfilled < index)
            index = lastUnfilled;
        album[index] = gift.Clone();

        if (cards is IMysteryGiftFlags flags)
        {
            var received = new List<int>();
            for (var i = 1; i < flags.MysteryGiftReceivedFlagMax; i++)
            {
                if (flags.GetMysteryGiftReceivedFlag(i))
                    received.Add(i);
            }
            if ((uint)gift.CardID < flags.MysteryGiftReceivedFlagMax && !received.Contains(gift.CardID))
                received.Add(gift.CardID);
            flags.ClearReceivedFlags();
            foreach (var id in received)
                flags.SetMysteryGiftReceivedFlag(id, true);
        }

        if (cards is MysteryBlock4 s4)
        {
            s4.IsDeliveryManActive = album.Any(g => !g.IsEmpty);
            MysteryBlock4.UpdateSlotPGT(album, sav is SAV4HGSS);
            if (sav is SAV4HGSS hgss)
                hgss.LockCapsuleSlot = (PCD)album[^1];
        }
        for (var i = 0; i < cards.GiftCountMax; i++)
            cards.SetMysteryGift(i, album[i]);
        if (cards is MysteryBlock5 s5)
            s5.EndAccess();
    }

    private static DataMysteryGift[] LoadAlbum(SaveFile sav, IMysteryGiftStorage cards)
    {
        var count = cards.GiftCountMax;
        var size = sav is SAV4HGSS ? count + 1 : count;
        var result = new DataMysteryGift[size];
        for (var i = 0; i < count; i++)
            result[i] = cards.GetMysteryGift(i);
        if (sav is SAV4HGSS s4)
            result[^1] = s4.LockCapsuleSlot;
        return result;
    }

    private static int LastUnfilledByType(DataMysteryGift gift, ReadOnlySpan<DataMysteryGift> album)
    {
        for (var i = 0; i < album.Length; i++)
        {
            var exist = album[i];
            if (!exist.IsEmpty)
                continue;
            if (exist.Type != gift.Type)
                continue;
            return i;
        }
        return -1;
    }
}
