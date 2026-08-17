namespace PKHeX.ByteParity.Tests;

internal static class RepoPaths
{
    public static string Root
    {
        get
        {
            var dir = new DirectoryInfo(AppContext.BaseDirectory);
            while (dir is not null)
            {
                var oracleCore = Path.Combine(dir.FullName, "PKHeX-26.07.07", "PKHeX.Core");
                var vendoredCore = Path.Combine(dir.FullName, "core");
                if (Directory.Exists(oracleCore) && Directory.Exists(vendoredCore))
                    return dir.FullName;
                dir = dir.Parent;
            }

            throw new InvalidOperationException("Could not find the repository root from the test output directory.");
        }
    }
}
