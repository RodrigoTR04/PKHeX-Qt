using PKHeX.Core;
using PKHeX.Editor;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class LoadExportTests
{
    [Fact]
    public void golden_save_empty_operations_matches_fixture_byte_for_byte()
    {
        var fixture = GoldenSave.Gen5BlankExport();

        var exported = ByteParityHarness.Run(fixture, []);

        Assert.Equal(fixture, exported);
    }

    [Fact]
    public void registered_operations_do_not_change_load_export_when_list_is_empty()
    {
        var fixture = GoldenSave.Gen5BlankExport();

        var exported = ByteParityHarness.Run(fixture, [], session =>
        {
            session.Register("Menu_ExportSAV", static _ => { });
            session.Register("touch_first_byte", static sav => sav.Data[0] ^= 0xFF);
        });

        Assert.Equal(fixture, exported);
    }

    [Fact]
    public void registered_named_operation_runs_when_listed()
    {
        var fixture = GoldenSave.Gen5BlankExport();

        var exported = ByteParityHarness.Run(fixture, [new EditorOperation("touch_first_byte")], session =>
        {
            session.Register("touch_first_byte", static sav => sav.Data[0] ^= 0xFF);
        });

        Assert.NotEqual(fixture, exported);
        Assert.Equal(fixture[0] ^ 0xFF, exported[0]);
    }

    [Fact]
    public void harness_does_not_load_qt_assemblies()
    {
        _ = ByteParityHarness.Run(GoldenSave.Gen5BlankExport(), []);

        Assert.DoesNotContain(
            AppDomain.CurrentDomain.GetAssemblies(),
            assembly =>
            {
                var name = assembly.GetName().Name;
                return name is not null && name.Contains("Qt", StringComparison.OrdinalIgnoreCase);
            });
    }
}
