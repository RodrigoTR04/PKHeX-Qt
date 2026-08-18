using PKHeX.Core;
using static PKHeX.Core.MessageStrings;

namespace PKHeX.Editor;

internal static class SaveBatchEditor
{
    public readonly record struct Result(bool Ok, bool Wrote, string Message);

    public static Result Run(SaveFile sav, string scope, string instructions)
    {
        ArgumentNullException.ThrowIfNull(sav);
        ArgumentException.ThrowIfNullOrEmpty(scope);
        ArgumentNullException.ThrowIfNull(instructions);

        if (StringInstructionSet.HasEmptyLine(instructions))
            return new Result(false, false, MsgBEInstructionInvalid);

        var sets = StringInstructionSet.GetBatchSets(instructions);
        if (Array.Exists(sets, s => s.Filters.Any(z => string.IsNullOrWhiteSpace(z.PropertyValue))))
            return new Result(false, false, MsgBEFilterEmpty);
        if (Array.Exists(sets, z => z.Instructions.Count == 0))
            return new Result(false, false, MsgBEInstructionNone);

        var emptyVal = sets.SelectMany(s => s.Instructions.Where(z => string.IsNullOrWhiteSpace(z.PropertyValue))).ToArray();
        if (emptyVal.Length != 0)
        {
            var props = string.Join(", ", emptyVal.Select(z => z.PropertyName));
            return new Result(false, false, MsgBEPropertyEmpty + Environment.NewLine + props);
        }

        foreach (var set in sets)
        {
            EntityBatchEditor.ScreenStrings(set.Filters);
            EntityBatchEditor.ScreenStrings(set.Instructions);
        }

        var editor = new EntityBatchProcessor();
        if (scope.Equals("party", StringComparison.OrdinalIgnoreCase))
            ProcessSave(sav, editor, sets, boxes: false, party: true);
        else if (scope.Equals("boxes", StringComparison.OrdinalIgnoreCase))
            ProcessSave(sav, editor, sets, boxes: true, party: false);
        else
            return new Result(false, false, MsgBEInstructionNone);

        return new Result(true, true, editor.GetEditorResults(sets));
    }

    public static string PropertyList()
    {
        var names = EntityBatchEditor.Instance.Properties[0];
        return string.Join('\n', names);
    }

    private static void ProcessSave(SaveFile sav, EntityBatchProcessor editor, StringInstructionSet[] sets, bool boxes, bool party)
    {
        if (party)
        {
            var data = new List<SlotCache>(sav.PartyCount);
            SlotInfoLoader.AddPartyData(sav, data);
            foreach (var set in sets)
                ProcessSlots(sav, editor, data, set.Filters, set.Instructions);
            foreach (var slot in data)
                slot.Source.WriteTo(sav, slot.Entity, EntityImportSettings.None);
        }
        if (boxes)
        {
            var data = new List<SlotCache>(sav.SlotCount);
            SlotInfoLoader.AddBoxData(sav, data);
            foreach (var set in sets)
                ProcessSlots(sav, editor, data, set.Filters, set.Instructions);
            foreach (var slot in data)
                slot.Source.WriteTo(sav, slot.Entity, EntityImportSettings.None);
        }
    }

    private static void ProcessSlots(
        SaveFile sav,
        EntityBatchProcessor editor,
        IList<SlotCache> data,
        IReadOnlyList<StringInstruction> filters,
        IReadOnlyList<StringInstruction> instructions)
    {
        if (data.Count == 0)
            return;

        var filterMeta = filters.Where(f => BatchFilters.FilterMeta.Any(z => z.IsMatch(f.PropertyName))).ToArray();
        if (filterMeta.Length != 0)
            filters = filters.Except(filterMeta).ToArray();

        var max = sav.MaxSpeciesID;
        for (int i = 0; i < data.Count; i++)
        {
            var entry = data[i];
            var pk = entry.Entity;
            var spec = pk.Species;
            if (spec == 0 || spec > max)
                continue;
            if (entry.Source is SlotInfoBox info && sav.GetBoxSlotFlags(info.Box, info.Slot).IsOverwriteProtected())
                editor.AddSkipped();
            else if (!EntityBatchEditor.IsFilterMatchMeta(filterMeta, entry))
                editor.AddSkipped();
            else
                editor.Process(pk, filters, instructions);
        }
    }
}
