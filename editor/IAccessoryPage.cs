using PKHeX.Core;

namespace PKHeX.Editor;

public interface IAccessoryPage
{
    string Id { get; }
    string LangForm { get; }
    string ToJson();
    void ApplyJson(string json);
    void Modify(string action);
    void Save(PKM pk);
}
