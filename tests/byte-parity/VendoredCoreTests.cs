using System.Security.Cryptography;
using Xunit;

namespace PKHeX.ByteParity.Tests;

public sealed class VendoredCoreTests
{
    [Fact]
    public void vendored_core_matches_oracle_core()
    {
        var root = RepoPaths.Root;
        var oracle = Path.Combine(root, "PKHeX-26.07.07", "PKHeX.Core");
        var vendored = Path.Combine(root, "core");

        var oracleFiles = Index(oracle);
        var vendoredFiles = Index(vendored);

        Assert.Equal(oracleFiles.Keys.OrderBy(k => k, StringComparer.Ordinal), vendoredFiles.Keys.OrderBy(k => k, StringComparer.Ordinal));

        foreach (var (relative, oracleHash) in oracleFiles)
            Assert.Equal(oracleHash, vendoredFiles[relative]);
    }

    private static Dictionary<string, string> Index(string directory)
    {
        var files = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (var path in Directory.EnumerateFiles(directory, "*", SearchOption.AllDirectories))
        {
            var relative = Path.GetRelativePath(directory, path).Replace('\\', '/');
            if (relative.StartsWith("bin/", StringComparison.Ordinal) || relative.StartsWith("obj/", StringComparison.Ordinal))
                continue;
            files[relative] = Convert.ToHexString(SHA256.HashData(File.ReadAllBytes(path)));
        }

        return files;
    }
}
