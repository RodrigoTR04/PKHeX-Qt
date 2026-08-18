using PKHeX.Core;

namespace PKHeX.Editor;

public interface ISaveBlockPage
{
    string Id { get; }
    string LangForm { get; }
    string ToJson();
    void ApplyJson(string json);
    void Modify(string action);
    void Save(SaveFile sav);
}
