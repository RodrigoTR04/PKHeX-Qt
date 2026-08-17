using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class SlotSelectTests
{
    [Fact]
    public void selecting_a_box_slot_sets_current_entity_to_that_slots_bytes()
    {
        var (save, entity) = OccupiedSave.WithBoxSlot(box: 0, slot: 3);
        var session = EditorSession.Load(save);

        session.SelectBoxSlot(0, 3);

        Assert.Equal(entity.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void selecting_a_party_slot_sets_current_entity_to_that_slots_bytes()
    {
        var (save, entity) = OccupiedSave.WithPartySlot(slot: 1);
        var session = EditorSession.Load(save);

        session.SelectPartySlot(1);

        Assert.Equal(entity.Data.ToArray(), session.CurrentEntity);
    }

    [Fact]
    public void selecting_a_slot_does_not_change_exported_save_bytes()
    {
        var (save, _) = OccupiedSave.WithBoxSlot();
        var session = EditorSession.Load(save);
        var before = session.Export();

        session.SelectBoxSlot(0, 0);

        Assert.Equal(before, session.Export());
    }
}
