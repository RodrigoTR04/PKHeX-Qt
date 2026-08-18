using System.Text.Json;
using PKHeX.Core;

namespace PKHeX.Editor;

internal sealed class SimpleTrainerPage : ISaveBlockPage
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        PropertyNameCaseInsensitive = true,
    };

    private readonly SaveFile _sav;
    private Document _doc;

    public SimpleTrainerPage(SaveFile sav)
    {
        _sav = sav;
        _doc = Load(sav);
    }

    public string Id => "trainer";
    public string LangForm => "SAV_SimpleTrainer";

    public static bool Supports(SaveFile sav)
    {
        if (sav is SAV6 or SAV7 or SAV7b or SAV8SWSH or SAV8BS or SAV8LA or SAV9SV or SAV9ZA or SAV4BR)
            return false;
        return sav.HasParty;
    }

    public string ToJson()
    {
        _doc.Page = Id;
        _doc.LangForm = LangForm;
        return JsonSerializer.Serialize(_doc, JsonOptions);
    }

    public void ApplyJson(string json)
    {
        var incoming = JsonSerializer.Deserialize<Document>(json, JsonOptions)
            ?? throw new InvalidDataException("Trainer document was empty.");
        if (incoming.Ot is not null)
            _doc.Ot = incoming.Ot;
        if (incoming.Gender is { } gender)
            _doc.Gender = gender;
        if (incoming.Tid is { } tid)
            _doc.Tid = tid;
        if (incoming.Sid is { } sid)
            _doc.Sid = sid;
        if (incoming.Money is { } money)
            _doc.Money = money;
        if (incoming.Coins is { } coins)
            _doc.Coins = coins;
        if (incoming.Hours is { } hours)
            _doc.Hours = hours;
        if (incoming.Minutes is { } minutes)
            _doc.Minutes = minutes;
        if (incoming.Seconds is { } seconds)
            _doc.Seconds = seconds;
        if (incoming.Badges is { } badges)
            _doc.Badges = badges;
        if (incoming.Country is { } country)
            _doc.Country = country;
        if (incoming.Region is { } region)
            _doc.Region = region;
        if (incoming.PikaFriendship is { } pf)
            _doc.PikaFriendship = pf;
        if (incoming.PikaBeach is { } pb)
            _doc.PikaBeach = pb;
        if (incoming.BattleEffects is { } be)
            _doc.BattleEffects = be;
        if (incoming.BattleStyle is { } bs)
            _doc.BattleStyle = bs;
        if (incoming.Sound is { } sound)
            _doc.Sound = sound;
        if (incoming.TextSpeed is { } speed)
            _doc.TextSpeed = speed;
        if (incoming.SecondsToStart is { } start)
            _doc.SecondsToStart = start;
        if (incoming.SecondsToFame is { } fame)
            _doc.SecondsToFame = fame;
        if (incoming.MapUpdated is { } map)
            _doc.MapUpdated = map;
        if (incoming.MapM is { } m)
            _doc.MapM = m;
        if (incoming.MapX is { } x)
            _doc.MapX = x;
        if (incoming.MapY is { } y)
            _doc.MapY = y;
        if (incoming.MapZ is { } z)
            _doc.MapZ = z;
        if (incoming.HasGender is { } hg)
            _doc.HasGender = hg;
        if (incoming.HasSid is { } hs)
            _doc.HasSid = hs;
        if (incoming.HasCoins is { } hc)
            _doc.HasCoins = hc;
        if (incoming.HasCountry is { } hco)
            _doc.HasCountry = hco;
        if (incoming.HasMap is { } hm)
            _doc.HasMap = hm;
        if (incoming.HasAdventureDates is { } ha)
            _doc.HasAdventureDates = ha;
        if (incoming.HasOptions is { } ho)
            _doc.HasOptions = ho;
        if (incoming.HasPika is { } hp)
            _doc.HasPika = hp;
        if (incoming.BadgeCount is { } bc)
            _doc.BadgeCount = bc;
        if (incoming.MaxMoney is { } mm)
            _doc.MaxMoney = mm;
        if (incoming.MaxCoins is { } mc)
            _doc.MaxCoins = mc;
    }

    public void Modify(string action)
    {
        if (action == "B_MaxCash")
            _doc.Money = (uint)_sav.MaxMoney;
        else if (action == "B_MaxCoins")
            _doc.Coins = (uint)_sav.MaxCoins;
        else
            throw new InvalidOperationException($"Unknown trainer action '{action}'.");
    }

    public void Save(SaveFile sav)
    {
        if (_doc.Ot is { } ot && sav.OT != ot)
            sav.OT = ot;
        sav.Gender = _doc.Gender ?? sav.Gender;
        sav.TID16 = (ushort)(_doc.Tid ?? sav.TID16);
        sav.SID16 = (ushort)(_doc.Sid ?? sav.SID16);
        sav.Money = _doc.Money ?? sav.Money;
        sav.PlayedHours = (ushort)(_doc.Hours ?? sav.PlayedHours);
        sav.PlayedMinutes = (ushort)((_doc.Minutes ?? sav.PlayedMinutes) % 60);
        sav.PlayedSeconds = (ushort)((_doc.Seconds ?? sav.PlayedSeconds) % 60);

        var badgeval = _doc.Badges ?? 0;
        if (sav is SAV1 sav1)
        {
            sav1.Coin = (ushort)Math.Min(_doc.Coins ?? sav1.Coin, sav.MaxCoins);
            sav1.Badges = badgeval & 0xFF;
            sav1.PikaFriendship = (byte)Math.Min(255u, _doc.PikaFriendship ?? sav1.PikaFriendship);
            sav1.PikaBeachScore = (ushort)Math.Min(9999u, _doc.PikaBeach ?? sav1.PikaBeachScore);
            sav1.BattleEffects = _doc.BattleEffects ?? sav1.BattleEffects;
            sav1.BattleStyleSwitch = (_doc.BattleStyle ?? (sav1.BattleStyleSwitch ? 0 : 1)) == 0;
            sav1.Sound = _doc.Sound ?? sav1.Sound;
            sav1.TextSpeed = _doc.TextSpeed ?? sav1.TextSpeed;
        }

        if (sav is SAV2 sav2)
        {
            sav2.Coin = (ushort)Math.Min(_doc.Coins ?? sav2.Coin, sav.MaxCoins);
            sav2.Badges = badgeval & 0xFFFF;
            sav2.BattleEffects = _doc.BattleEffects ?? sav2.BattleEffects;
            sav2.BattleStyleSwitch = (_doc.BattleStyle ?? (sav2.BattleStyleSwitch ? 0 : 1)) == 0;
            sav2.Sound = (_doc.Sound ?? (sav2.Sound > 0 ? 1 : 0)) > 0 ? 2 : 0;
            sav2.TextSpeed = _doc.TextSpeed ?? sav2.TextSpeed;
        }

        if (sav is SAV3 sav3)
        {
            var small = sav3.SmallBlock;
            sav3.Badges = badgeval & 0xFF;
            small.OptionBattleStyle = (_doc.BattleStyle ?? (small.OptionBattleStyle ? 1 : 0)) == 1;
            small.OptionSound = (_doc.Sound ?? (small.OptionSound ? 1 : 0)) == 1;
            small.TextSpeed = _doc.TextSpeed ?? small.TextSpeed;
            small.OptionBattleScene = !(_doc.BattleEffects ?? !small.OptionBattleScene);
        }

        if (sav is SAV4 sav4)
        {
            if (_doc.MapUpdated == true)
            {
                sav4.M = _doc.MapM ?? sav4.M;
                sav4.X = _doc.MapX ?? sav4.X;
                sav4.Z = _doc.MapZ ?? sav4.Z;
                sav4.Y = _doc.MapY ?? sav4.Y;
            }
            sav4.Badges = (byte)badgeval;
            if (sav4 is SAV4HGSS hgss)
                hgss.Badges16 = badgeval >> 8;
            sav4.Country = (byte)(_doc.Country ?? sav4.Country);
            sav4.Region = (byte)(_doc.Region ?? sav4.Region);
        }
        else if (sav is SAV5 s5)
        {
            if (_doc.MapUpdated == true)
            {
                var pd = s5.PlayerPosition;
                pd.M = _doc.MapM ?? pd.M;
                pd.X = _doc.MapX ?? pd.X;
                pd.Z = _doc.MapZ ?? pd.Z;
                pd.Y = _doc.MapY ?? pd.Y;
            }
            s5.Misc.Badges = badgeval & 0xFF;
            s5.BattleSubway.BP = (ushort)Math.Min(_doc.Coins ?? (uint)s5.BattleSubway.BP, sav.MaxCoins);
            s5.Country = (byte)(_doc.Country ?? s5.Country);
            s5.Region = (byte)(_doc.Region ?? s5.Region);
        }

        sav.SecondsToStart = _doc.SecondsToStart ?? sav.SecondsToStart;
        sav.SecondsToFame = _doc.SecondsToFame ?? sav.SecondsToFame;
    }

    private static Document Load(SaveFile sav)
    {
        var doc = new Document
        {
            Ot = sav.OT,
            Gender = sav.Gender,
            Tid = sav.TID16,
            Sid = sav.SID16,
            Money = sav.Money,
            Hours = sav.PlayedHours,
            Minutes = sav.PlayedMinutes,
            Seconds = sav.PlayedSeconds,
            SecondsToStart = sav.SecondsToStart,
            SecondsToFame = sav.SecondsToFame,
            HasGender = sav.Generation > 1,
            HasSid = sav.Generation > 2,
            HasCoins = sav.Generation < 3 || sav is SAV5,
            HasCountry = sav.Generation > 3,
            HasMap = sav is SAV4 or SAV5,
            HasAdventureDates = sav is not (SAV1 or SAV2 or SAV3),
            HasOptions = sav is SAV1 or SAV2 or SAV3,
            HasPika = sav is SAV1 s1 && s1.Version.Contains(GameVersion.YW),
            BadgeCount = sav is SAV2 or SAV4HGSS ? 16 : sav is SAV3Colosseum or SAV3XD ? 0 : 8,
            MaxMoney = (uint)sav.MaxMoney,
            MaxCoins = (uint)sav.MaxCoins,
        };

        if (sav is SAV1 sav1)
        {
            doc.Coins = sav1.Coin;
            doc.Badges = sav1.Badges;
            doc.PikaFriendship = sav1.PikaFriendship;
            doc.PikaBeach = sav1.PikaBeachScore;
            doc.BattleEffects = sav1.BattleEffects;
            doc.BattleStyle = sav1.BattleStyleSwitch ? 0 : 1;
            doc.Sound = sav1.Sound;
            doc.TextSpeed = sav1.TextSpeed;
        }
        else if (sav is SAV2 sav2)
        {
            doc.Coins = sav2.Coin;
            doc.Badges = sav2.Badges;
            doc.BattleEffects = sav2.BattleEffects;
            doc.BattleStyle = sav2.BattleStyleSwitch ? 0 : 1;
            doc.Sound = sav2.Sound > 0 ? 1 : 0;
            doc.TextSpeed = sav2.TextSpeed;
        }
        else if (sav is SAV3 sav3)
        {
            var small = sav3.SmallBlock;
            doc.Badges = sav3.Badges;
            doc.BattleStyle = small.OptionBattleStyle ? 1 : 0;
            doc.Sound = small.OptionSound ? 1 : 0;
            doc.TextSpeed = small.TextSpeed;
            doc.BattleEffects = !small.OptionBattleScene;
        }
        else if (sav is SAV4 sav4)
        {
            doc.Badges = sav4.Badges | (sav4 is SAV4HGSS hgss ? hgss.Badges16 << 8 : 0);
            doc.Country = sav4.Country;
            doc.Region = sav4.Region;
            doc.MapM = sav4.M;
            doc.MapX = sav4.X;
            doc.MapY = sav4.Y;
            doc.MapZ = sav4.Z;
        }
        else if (sav is SAV5 s5)
        {
            doc.Coins = (uint)s5.BattleSubway.BP;
            doc.Badges = s5.Misc.Badges;
            doc.Country = s5.Country;
            doc.Region = s5.Region;
            var pd = s5.PlayerPosition;
            doc.MapM = pd.M;
            doc.MapX = pd.X;
            doc.MapY = pd.Y;
            doc.MapZ = pd.Z;
        }

        return doc;
    }

    private sealed class Document
    {
        public string Page { get; set; } = "trainer";
        public string LangForm { get; set; } = "SAV_SimpleTrainer";
        public string? Ot { get; set; }
        public byte? Gender { get; set; }
        public uint? Tid { get; set; }
        public uint? Sid { get; set; }
        public uint? Money { get; set; }
        public uint? Coins { get; set; }
        public int? Hours { get; set; }
        public int? Minutes { get; set; }
        public int? Seconds { get; set; }
        public int? Badges { get; set; }
        public int? Country { get; set; }
        public int? Region { get; set; }
        public uint? PikaFriendship { get; set; }
        public uint? PikaBeach { get; set; }
        public bool? BattleEffects { get; set; }
        public int? BattleStyle { get; set; }
        public int? Sound { get; set; }
        public int? TextSpeed { get; set; }
        public uint? SecondsToStart { get; set; }
        public uint? SecondsToFame { get; set; }
        public bool? MapUpdated { get; set; }
        public int? MapM { get; set; }
        public int? MapX { get; set; }
        public int? MapY { get; set; }
        public int? MapZ { get; set; }
        public bool? HasGender { get; set; }
        public bool? HasSid { get; set; }
        public bool? HasCoins { get; set; }
        public bool? HasCountry { get; set; }
        public bool? HasMap { get; set; }
        public bool? HasAdventureDates { get; set; }
        public bool? HasOptions { get; set; }
        public bool? HasPika { get; set; }
        public int? BadgeCount { get; set; }
        public uint? MaxMoney { get; set; }
        public uint? MaxCoins { get; set; }
    }
}
