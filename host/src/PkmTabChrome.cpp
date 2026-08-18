#include "PkmTabChrome.h"

#include "ComboChrome.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateEdit>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStringList>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
constexpr int kFormMaxWidth = 408;
constexpr int kControlHeight = 28;

int em(const QWidget *widget, int count)
{
    return count * widget->fontMetrics().horizontalAdvance(QLatin1Char('M'));
}

void pinControl(QWidget *widget, int width = 0)
{
    widget->setFixedHeight(kControlHeight);
    if (width > 0)
        widget->setFixedWidth(width);
    else
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QFormLayout *formOn(QWidget *tab)
{
    auto *outer = new QVBoxLayout(tab);
    outer->setContentsMargins(8, 8, 8, 8);
    outer->setSpacing(4);
    auto *host = new QWidget(tab);
    host->setMaximumWidth(kFormMaxWidth);
    auto *layout = new QFormLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(8);
    layout->setVerticalSpacing(4);
    layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    outer->addWidget(host, 0, Qt::AlignHCenter | Qt::AlignTop);
    outer->addStretch();
    return layout;
}

QLabel *lab(QWidget *parent, const char *name, const QString &text)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(QString::fromLatin1(name));
    return label;
}

QLineEdit *edit(QWidget *parent, const QString &name, int width = 0)
{
    auto *box = new QLineEdit(parent);
    box->setObjectName(name);
    pinControl(box, width);
    return box;
}

QComboBox *combo(QWidget *parent, const QString &name, int width = 0)
{
    auto *box = new QComboBox(parent);
    box->setObjectName(name);
    box->setEditable(false);
    pinControl(box, width);
    configureComboBox(box);
    return box;
}

QCheckBox *check(QWidget *parent, const char *name, const QString &text)
{
    auto *box = new QCheckBox(text, parent);
    box->setObjectName(QString::fromLatin1(name));
    return box;
}

void fillMain(QWidget *tab)
{
    auto *layout = formOn(tab);

    auto *pidRow = new QWidget(tab);
    auto *pidLay = new QHBoxLayout(pidRow);
    pidLay->setContentsMargins(0, 0, 0, 0);
    pidLay->setSpacing(4);
    auto *shiny = check(pidRow, "PB_MarkShiny", QStringLiteral("★"));
    pidLay->addWidget(shiny);
    pidLay->addWidget(edit(pidRow, "TB_PID", em(tab, 8)));
    pidLay->addWidget(combo(pidRow, "UC_Gender", em(tab, 3)));
    auto *reroll = new QPushButton(QStringLiteral("Reroll"), pidRow);
    reroll->setObjectName(QStringLiteral("BTN_RerollPID"));
    pinControl(reroll);
    pidLay->addWidget(reroll);
    pidLay->addStretch();
    layout->addRow(lab(tab, "Label_PID", QStringLiteral("PID:")), pidRow);

    layout->addRow(lab(tab, "Label_Species", QStringLiteral("Species:")), combo(tab, "CB_Species"));
    layout->addRow(lab(tab, "Label_Form", QStringLiteral("Form:")), combo(tab, "CB_Form"));

    auto *nickRow = new QWidget(tab);
    auto *nickLay = new QHBoxLayout(nickRow);
    nickLay->setContentsMargins(0, 0, 0, 0);
    nickLay->setSpacing(4);
    nickLay->addWidget(edit(nickRow, "TB_Nickname"));
    auto *nickHidden = check(nickRow, "CHK_Nicknamed", QStringLiteral("Nickname:"));
    nickHidden->hide();
    layout->addRow(check(tab, "CHK_NicknamedFlag", QStringLiteral("Nickname:")), nickRow);

    auto *expRow = new QWidget(tab);
    auto *expLay = new QHBoxLayout(expRow);
    expLay->setContentsMargins(0, 0, 0, 0);
    expLay->setSpacing(6);
    expLay->addWidget(edit(expRow, "TB_EXP"));
    expLay->addWidget(lab(expRow, "Label_CurLevel", QStringLiteral("Level")));
    expLay->addWidget(edit(expRow, "TB_Level", em(tab, 4)));
    layout->addRow(lab(tab, "Label_EXP", QStringLiteral("Exp. Points:")), expRow);

    layout->addRow(lab(tab, "Label_Nature", QStringLiteral("Nature:")), combo(tab, "CB_Nature"));
    layout->addRow(lab(tab, "Label_HeldItem", QStringLiteral("Held Item:")), combo(tab, "CB_HeldItem"));
    layout->addRow(lab(tab, "Label_Ability", QStringLiteral("Ability:")), combo(tab, "CB_Ability"));
    layout->addRow(lab(tab, "Label_Language", QStringLiteral("Language:")), combo(tab, "CB_Language"));

    auto *flagRow = new QWidget(tab);
    auto *flagLay = new QHBoxLayout(flagRow);
    flagLay->setContentsMargins(0, 0, 0, 0);
    flagLay->setSpacing(8);
    flagLay->addWidget(check(flagRow, "CHK_IsEgg", QStringLiteral("Is Egg")));
    auto *infected = check(flagRow, "CHK_Infected", QStringLiteral("Infected"));
    auto *cured = check(flagRow, "CHK_Cured", QStringLiteral("Cured"));
    infected->setEnabled(false);
    cured->setEnabled(false);
    flagLay->addWidget(infected);
    flagLay->addWidget(cured);
    flagLay->addStretch();
    layout->addRow(QString(), flagRow);

    auto *pkrsRow = new QWidget(tab);
    auto *pkrsLay = new QHBoxLayout(pkrsRow);
    pkrsLay->setContentsMargins(0, 0, 0, 0);
    pkrsLay->setSpacing(6);
    pkrsLay->addWidget(lab(pkrsRow, "Label_PKRS", QStringLiteral("PkRs:")));
    pkrsLay->addWidget(combo(pkrsRow, "CB_PKRSStrain", em(tab, 5)));
    pkrsLay->addWidget(lab(pkrsRow, "Label_PKRSdays", QStringLiteral("d:")));
    pkrsLay->addWidget(combo(pkrsRow, "CB_PKRSDays", em(tab, 5)));
    pkrsLay->addStretch();
    layout->addRow(QString(), pkrsRow);

    layout->addRow(check(tab, "CHK_NSparkle", QStringLiteral("Active")));
}

void fillMet(QWidget *tab)
{
    auto *layout = formOn(tab);
    layout->addRow(lab(tab, "Label_OriginGame", QStringLiteral("Origin Game:")), combo(tab, "CB_GameOrigin"));
    layout->addRow(lab(tab, "Label_MetLocation", QStringLiteral("Met Location:")), combo(tab, "CB_MetLocation"));
    layout->addRow(lab(tab, "Label_Ball", QStringLiteral("Ball:")), combo(tab, "CB_Ball"));
    auto *metDate = new QDateEdit(tab);
    metDate->setObjectName(QStringLiteral("CAL_MetDate"));
    metDate->setCalendarPopup(true);
    metDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    pinControl(metDate);
    layout->addRow(lab(tab, "Label_MetDate", QStringLiteral("Met Date:")), metDate);

    auto *metRow = new QWidget(tab);
    auto *metLay = new QHBoxLayout(metRow);
    metLay->setContentsMargins(0, 0, 0, 0);
    metLay->setSpacing(8);
    metLay->addWidget(edit(metRow, "TB_MetLevel", em(tab, 4)));
    metLay->addWidget(check(metRow, "CHK_Fateful", QStringLiteral("Fateful Encounter")));
    metLay->addStretch();
    layout->addRow(lab(tab, "Label_MetLevel", QStringLiteral("Met Level:")), metRow);

    layout->addRow(lab(tab, "Label_GroundTile", QStringLiteral("Encountered On:")), combo(tab, "CB_GroundTile"));
    auto *asEgg = check(tab, "CHK_AsEgg", QStringLiteral("As Egg"));
    asEgg->setEnabled(false);
    layout->addRow(asEgg);
    auto *egg = new QGroupBox(QStringLiteral("Egg Met Conditions"), tab);
    egg->setObjectName(QStringLiteral("GB_EggConditions"));
    auto *eggForm = new QFormLayout(egg);
    eggForm->addRow(lab(egg, "Label_EggLocation", QStringLiteral("Location:")), combo(egg, "CB_EggLocation"));
    auto *eggDate = new QDateEdit(egg);
    eggDate->setObjectName(QStringLiteral("CAL_EggDate"));
    eggDate->setCalendarPopup(true);
    eggDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    pinControl(eggDate);
    eggForm->addRow(lab(egg, "Label_EggDate", QStringLiteral("Date:")), eggDate);
    layout->addRow(egg);
}

QWidget *statRow(QWidget *parent, const char *rowName, const char *labelName, const QString &label,
    const char *base, const char *iv, const char *ev, const char *stat)
{
    auto *row = new QWidget(parent);
    row->setObjectName(QString::fromLatin1(rowName));
    auto *lay = new QHBoxLayout(row);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(4);
    auto *name = lab(row, labelName, label);
    name->setMinimumWidth(em(parent, 4));
    lay->addWidget(name);
    auto *b = edit(row, base, em(parent, 4));
    b->setReadOnly(true);
    lay->addWidget(b);
    lay->addWidget(edit(row, iv, em(parent, 4)));
    lay->addWidget(edit(row, ev, em(parent, 4)));
    auto *s = edit(row, stat, em(parent, 4));
    s->setReadOnly(true);
    lay->addWidget(s);
    return row;
}

void fillStats(QWidget *tab)
{
    auto *outer = new QVBoxLayout(tab);
    outer->setContentsMargins(8, 8, 8, 8);
    auto *host = new QWidget(tab);
    host->setMaximumWidth(kFormMaxWidth);
    auto *layout = new QVBoxLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    auto *gridHost = new QWidget(host);
    gridHost->setObjectName(QStringLiteral("TLP_StatGrid"));
    auto *grid = new QVBoxLayout(gridHost);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(2);
    auto *header = new QWidget(gridHost);
    auto *headLay = new QHBoxLayout(header);
    headLay->setContentsMargins(0, 0, 0, 0);
    headLay->setSpacing(4);
    auto *pad = new QLabel(header);
    pad->setMinimumWidth(em(tab, 4));
    headLay->addWidget(pad);
    headLay->addWidget(lab(header, "Label_Base", QStringLiteral("Base")));
    headLay->addWidget(lab(header, "Label_IVs", QStringLiteral("IVs")));
    headLay->addWidget(lab(header, "Label_EVs", QStringLiteral("EVs")));
    headLay->addWidget(lab(header, "Label_Stats", QStringLiteral("Stats")));
    grid->addWidget(header);
    grid->addWidget(statRow(gridHost, "StatRow_HP", "Label_HP", QStringLiteral("HP:"), "TB_BaseHP", "TB_IVHP", "TB_EVHP", "Stat_HP"));
    grid->addWidget(statRow(gridHost, "StatRow_ATK", "Label_ATK", QStringLiteral("Atk:"), "TB_BaseATK", "TB_IVATK", "TB_EVATK", "Stat_ATK"));
    grid->addWidget(statRow(gridHost, "StatRow_DEF", "Label_DEF", QStringLiteral("Def:"), "TB_BaseDEF", "TB_IVDEF", "TB_EVDEF", "Stat_DEF"));
    grid->addWidget(statRow(gridHost, "StatRow_SPA", "Label_SPA", QStringLiteral("SpA:"), "TB_BaseSPA", "TB_IVSPA", "TB_EVSPA", "Stat_SPA"));
    grid->addWidget(statRow(gridHost, "StatRow_SPD", "Label_SPD", QStringLiteral("SpD:"), "TB_BaseSPD", "TB_IVSPD", "TB_EVSPD", "Stat_SPD"));
    grid->addWidget(statRow(gridHost, "StatRow_SPE", "Label_SPE", QStringLiteral("Spe:"), "TB_BaseSPE", "TB_IVSPE", "TB_EVSPE", "Stat_SPE"));
    layout->addWidget(gridHost);

    auto *hp = new QWidget(host);
    auto *hpLay = new QHBoxLayout(hp);
    hpLay->setContentsMargins(0, 0, 0, 0);
    hpLay->addWidget(lab(hp, "Label_HiddenPowerPrefix", QStringLiteral("Hidden Power Type:")));
    hpLay->addWidget(combo(hp, "CB_HPType"));
    hpLay->addWidget(lab(hp, "Label_HiddenPowerPower", QStringLiteral("60")));
    layout->addWidget(hp);
    layout->addWidget(lab(host, "L_Characteristic", QStringLiteral("Characteristic:")));
    auto *randI = new QPushButton(QStringLiteral("Randomize IVs"), host);
    randI->setObjectName(QStringLiteral("BTN_RandomIVs"));
    auto *randE = new QPushButton(QStringLiteral("Randomize EVs"), host);
    randE->setObjectName(QStringLiteral("BTN_RandomEVs"));
    auto *btns = new QHBoxLayout();
    btns->addWidget(randI);
    btns->addWidget(randE);
    layout->addLayout(btns);
    outer->addWidget(host, 0, Qt::AlignHCenter | Qt::AlignTop);
    outer->addStretch();
}

void fillMoves(QWidget *tab)
{
    auto *outer = new QVBoxLayout(tab);
    outer->setContentsMargins(8, 8, 8, 8);
    auto *host = new QWidget(tab);
    host->setMaximumWidth(kFormMaxWidth);
    auto *layout = new QVBoxLayout(host);
    layout->setContentsMargins(0, 0, 0, 0);
    auto *gb = new QGroupBox(QStringLiteral("Current Moves"), host);
    gb->setObjectName(QStringLiteral("GB_CurrentMoves"));
    auto *grid = new QGridLayout(gb);
    grid->addWidget(lab(gb, "Label_CurPP", QStringLiteral("PP")), 0, 1);
    grid->addWidget(lab(gb, "Label_PPups", QStringLiteral("Ups")), 0, 2);
    for (int i = 1; i <= 4; ++i)
    {
        grid->addWidget(combo(gb, QStringLiteral("CB_Move%1").arg(i)), i, 0);
        grid->addWidget(edit(gb, QStringLiteral("TB_PP%1").arg(i), em(tab, 4)), i, 1);
        grid->addWidget(combo(gb, QStringLiteral("CB_PPUps%1").arg(i), em(tab, 4)), i, 2);
    }
    layout->addWidget(gb);
    auto *flags = new QWidget(host);
    flags->setObjectName(QStringLiteral("FLP_MoveFlags"));
    auto *flagLay = new QHBoxLayout(flags);
    flagLay->setContentsMargins(0, 0, 0, 0);
    auto *relearn = new QPushButton(QStringLiteral("Relearn Flags"), flags);
    relearn->setObjectName(QStringLiteral("B_RelearnFlags"));
    auto *shop = new QPushButton(QStringLiteral("Move Shop"), flags);
    shop->setObjectName(QStringLiteral("B_MoveShop"));
    auto *plus = new QPushButton(QStringLiteral("Plus Flags"), flags);
    plus->setObjectName(QStringLiteral("B_PlusRecord"));
    flagLay->addWidget(relearn);
    flagLay->addWidget(shop);
    flagLay->addWidget(plus);
    flagLay->addStretch();
    layout->addWidget(flags);
    outer->addWidget(host, 0, Qt::AlignHCenter | Qt::AlignTop);
    outer->addStretch();
}

void fillCosmetic(QWidget *tab)
{
    auto *layout = formOn(tab);
    layout->addRow(lab(tab, "Label_ContestStats", QStringLiteral("Contest Stats")));
    layout->addRow(lab(tab, "Label_Cool", QStringLiteral("Cool")), edit(tab, "TB_Cool", em(tab, 4)));
    layout->addRow(lab(tab, "Label_Beauty", QStringLiteral("Beauty")), edit(tab, "TB_Beauty", em(tab, 4)));
    layout->addRow(lab(tab, "Label_Cute", QStringLiteral("Cute")), edit(tab, "TB_Cute", em(tab, 4)));
    layout->addRow(lab(tab, "Label_Smart", QStringLiteral("Smart")), edit(tab, "TB_Smart", em(tab, 4)));
    layout->addRow(lab(tab, "Label_Tough", QStringLiteral("Tough")), edit(tab, "TB_Tough", em(tab, 4)));
    layout->addRow(lab(tab, "Label_Sheen", QStringLiteral("Sheen")), edit(tab, "TB_Sheen", em(tab, 4)));
    auto *fame = new QSpinBox(tab);
    fame->setObjectName(QStringLiteral("NUD_PokeStarFame"));
    fame->setRange(0, 255);
    pinControl(fame, em(tab, 5));
    layout->addRow(QStringLiteral("Pokéstar Fame"), fame);
}

void fillOt(QWidget *tab)
{
    auto *layout = formOn(tab);
    auto *otBox = new QGroupBox(QStringLiteral("Trainer Information"), tab);
    otBox->setObjectName(QStringLiteral("GB_OT"));
    auto *otForm = new QFormLayout(otBox);
    otForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    auto *otRow = new QWidget(otBox);
    auto *otLay = new QHBoxLayout(otRow);
    otLay->setContentsMargins(0, 0, 0, 0);
    otLay->addWidget(edit(otRow, "TB_OT", em(tab, 7)));
    otLay->addWidget(combo(otRow, "UC_OTGender", em(tab, 3)));
    otLay->addStretch();
    otForm->addRow(lab(otBox, "Label_OT", QStringLiteral("OT:")), otRow);
    auto *idRow = new QWidget(otBox);
    auto *idLay = new QHBoxLayout(idRow);
    idLay->setContentsMargins(0, 0, 0, 0);
    idLay->addWidget(lab(idRow, "Label_TID", QStringLiteral("TID:")));
    idLay->addWidget(edit(idRow, "TB_TID", em(tab, 7)));
    idLay->addWidget(lab(idRow, "Label_SID", QStringLiteral("SID:")));
    idLay->addWidget(edit(idRow, "TB_SID", em(tab, 7)));
    idLay->addStretch();
    otForm->addRow(idRow);
    layout->addRow(otBox);
    layout->addRow(lab(tab, "Label_Friendship", QStringLiteral("Friendship:")), edit(tab, "TB_Friendship", em(tab, 7)));
    auto *editors = new QWidget(tab);
    editors->setObjectName(QStringLiteral("FLP_PKMEditors"));
    auto *editorLay = new QHBoxLayout(editors);
    editorLay->setContentsMargins(0, 0, 0, 0);
    auto *ribbons = new QPushButton(QStringLiteral("Ribbons"), editors);
    ribbons->setObjectName(QStringLiteral("BTN_Ribbons"));
    auto *medals = new QPushButton(QStringLiteral("Medals"), editors);
    medals->setObjectName(QStringLiteral("BTN_Medals"));
    auto *history = new QPushButton(QStringLiteral("Memories"), editors);
    history->setObjectName(QStringLiteral("BTN_History"));
    editorLay->addWidget(ribbons);
    editorLay->addWidget(medals);
    editorLay->addWidget(history);
    editorLay->addStretch();
    layout->addRow(editors);
}

void hideEmptyFormRows(QWidget *tab)
{
    if (tab == nullptr)
        return;
    const auto forms = tab->findChildren<QFormLayout *>();
    for (auto *form : forms)
    {
        for (int row = 0; row < form->rowCount(); ++row)
        {
            auto *field = form->itemAt(row, QFormLayout::FieldRole);
            auto *label = form->itemAt(row, QFormLayout::LabelRole);
            bool any = false;
            if (label != nullptr && label->widget() != nullptr && label->widget()->isVisibleTo(tab))
                any = true;
            if (field != nullptr && field->widget() != nullptr)
            {
                auto *container = field->widget();
                if (container->findChildren<QWidget *>().isEmpty())
                    any = any || container->isVisibleTo(tab);
                for (auto *child : container->findChildren<QWidget *>())
                {
                    if (child->isVisibleTo(tab))
                        any = true;
                }
            }
            form->setRowVisible(row, any);
        }
    }
}

void applyStatOrder(QWidget *window, int format)
{
    auto *spa = window->findChild<QWidget *>(QStringLiteral("StatRow_SPA"));
    auto *spd = window->findChild<QWidget *>(QStringLiteral("StatRow_SPD"));
    auto *spe = window->findChild<QWidget *>(QStringLiteral("StatRow_SPE"));
    auto *grid = window->findChild<QWidget *>(QStringLiteral("TLP_StatGrid"));
    if (spa == nullptr || spd == nullptr || spe == nullptr || grid == nullptr)
        return;
    auto *layout = qobject_cast<QVBoxLayout *>(grid->layout());
    if (layout == nullptr)
        return;
    if (format == 1)
    {
        if (auto *label = window->findChild<QLabel *>(QStringLiteral("Label_SPA")))
            label->setText(QStringLiteral("Special:"));
        layout->removeWidget(spe);
        const int spaIndex = layout->indexOf(spa);
        layout->insertWidget(spaIndex, spe);
        if (auto *iv = window->findChild<QLineEdit *>(QStringLiteral("TB_IVHP")))
            iv->setEnabled(false);
    }
    else
    {
        if (auto *label = window->findChild<QLabel *>(QStringLiteral("Label_SPA")))
            label->setText(QStringLiteral("SpA:"));
        layout->removeWidget(spe);
        layout->addWidget(spe);
        if (auto *iv = window->findChild<QLineEdit *>(QStringLiteral("TB_IVHP")))
            iv->setEnabled(true);
    }
}
}

void fillPkmTabs(QWidget *tabMain, QWidget *tabMet, QWidget *tabStats, QWidget *tabMoves, QWidget *tabCosmetic, QWidget *tabOt)
{
    if (tabMain != nullptr)
        fillMain(tabMain);
    if (tabMet != nullptr)
        fillMet(tabMet);
    if (tabStats != nullptr)
        fillStats(tabStats);
    if (tabMoves != nullptr)
        fillMoves(tabMoves);
    if (tabCosmetic != nullptr)
        fillCosmetic(tabCosmetic);
    if (tabOt != nullptr)
        fillOt(tabOt);
}

void applyInGameFont(QWidget *window)
{
    if (window == nullptr)
        return;
    const QString path = QDir(QCoreApplication::applicationDirPath())
                             .filePath(QStringLiteral("fonts/PGLDings-NormalRegular.ttf"));
    const int id = QFontDatabase::addApplicationFont(path);
    QString family = QStringLiteral("PGLDings");
    if (id != -1)
    {
        const auto families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty())
            family = families.front();
    }
    QFont font(family, 13);
    for (const char *name : {"TB_Nickname", "TB_OT"})
    {
        if (auto *box = window->findChild<QLineEdit *>(QString::fromLatin1(name)))
            box->setFont(font);
    }
}

void setLegalityIcon(QLabel *legal, bool valid)
{
    if (legal == nullptr)
        return;
    const QString file = valid ? QStringLiteral("valid.png") : QStringLiteral("warn.png");
    const QString path = QDir(QCoreApplication::applicationDirPath())
                             .filePath(QStringLiteral("assets/sprites/") + file);
    QPixmap pm(path);
    if (pm.isNull())
    {
        legal->setText(valid ? QStringLiteral("✓") : QStringLiteral("!"));
        return;
    }
    legal->setPixmap(pm);
}

void applyPkmChrome(QWidget *window, int format, const QStringList &visible)
{
    if (window == nullptr)
        return;
    const bool showAll = visible.size() == 1 && visible.front() == QLatin1String("*");
    static constexpr const char *kChrome[] = {
        "Label_PID", "TB_PID", "BTN_RerollPID", "UC_Gender", "PB_MarkShiny",
        "Label_Species", "CB_Species", "Label_Form", "CB_Form",
        "CHK_NicknamedFlag", "TB_Nickname",
        "Label_EXP", "TB_EXP", "Label_CurLevel", "TB_Level",
        "Label_Nature", "CB_Nature", "Label_HeldItem", "CB_HeldItem",
        "Label_Ability", "CB_Ability", "Label_Language", "CB_Language",
        "CHK_IsEgg", "CHK_Infected", "CHK_Cured", "CHK_NSparkle",
        "Label_PKRS", "CB_PKRSStrain", "Label_PKRSdays", "CB_PKRSDays",
        "Label_OriginGame", "CB_GameOrigin", "Label_MetLocation", "CB_MetLocation",
        "Label_Ball", "CB_Ball", "Label_MetDate", "CAL_MetDate",
        "Label_MetLevel", "TB_MetLevel", "CHK_Fateful",
        "Label_GroundTile", "CB_GroundTile", "CHK_AsEgg", "GB_EggConditions",
        "Label_EggLocation", "CB_EggLocation", "Label_EggDate", "CAL_EggDate",
        "Label_ContestStats", "Label_Cool", "TB_Cool", "Label_Beauty", "TB_Beauty",
        "Label_Cute", "TB_Cute", "Label_Smart", "TB_Smart", "Label_Tough", "TB_Tough",
        "Label_Sheen", "TB_Sheen", "NUD_PokeStarFame",
        "Label_OT", "TB_OT", "UC_OTGender", "Label_TID", "TB_TID", "Label_SID", "TB_SID",
        "Label_Friendship", "TB_Friendship",
        "Label_HiddenPowerPrefix", "CB_HPType", "Label_HiddenPowerPower", "L_Characteristic",
        "StatRow_SPD", "Label_SPD", "TB_BaseSPD", "TB_IVSPD", "TB_EVSPD", "Stat_SPD",
    };
    for (const char *name : kChrome)
    {
        if (auto *widget = window->findChild<QWidget *>(QString::fromLatin1(name)))
            widget->setVisible(showAll || visible.contains(QString::fromLatin1(name)));
    }
    if (auto *tabs = window->findChild<QTabWidget *>(QStringLiteral("PKME_Tabs")))
    {
        if (auto *met = window->findChild<QWidget *>(QStringLiteral("Tab_Met")))
        {
            const int index = tabs->indexOf(met);
            if (index >= 0)
                tabs->setTabVisible(index, showAll || visible.contains(QStringLiteral("Tab_Met")));
        }
        if (auto *cosmetic = window->findChild<QWidget *>(QStringLiteral("Tab_Cosmetic")))
        {
            const int index = tabs->indexOf(cosmetic);
            if (index >= 0)
                tabs->setTabVisible(index, showAll || visible.contains(QStringLiteral("Tab_Cosmetic")));
        }
    }
    if (auto *smart = window->findChild<QLabel *>(QStringLiteral("Label_Smart")))
        smart->setText(format <= 5 ? QStringLiteral("Smart") : QStringLiteral("Clever"));
    applyStatOrder(window, format);
    hideEmptyFormRows(window->findChild<QWidget *>(QStringLiteral("Tab_Main")));
    hideEmptyFormRows(window->findChild<QWidget *>(QStringLiteral("Tab_Met")));
    hideEmptyFormRows(window->findChild<QWidget *>(QStringLiteral("Tab_Cosmetic")));
    hideEmptyFormRows(window->findChild<QWidget *>(QStringLiteral("Tab_OTMisc")));
}
