#include "PkmTabChrome.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateEdit>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
QFormLayout *form(QWidget *tab)
{
    auto *layout = new QFormLayout(tab);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);
    return layout;
}

QLabel *lab(QWidget *parent, const char *name, const QString &text)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(QString::fromLatin1(name));
    return label;
}

QLineEdit *edit(QWidget *parent, const QString &name)
{
    auto *box = new QLineEdit(parent);
    box->setObjectName(name);
    return box;
}

QComboBox *combo(QWidget *parent, const QString &name)
{
    auto *box = new QComboBox(parent);
    box->setObjectName(name);
    box->setEditable(false);
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
    auto *layout = form(tab);
    auto *pidRow = new QWidget(tab);
    auto *pidLay = new QHBoxLayout(pidRow);
    pidLay->setContentsMargins(0, 0, 0, 0);
    pidLay->addWidget(edit(pidRow, "TB_PID"));
    pidLay->addWidget(combo(pidRow, "UC_Gender"));
    auto *shiny = check(pidRow, "PB_MarkShiny", QStringLiteral("★"));
    pidLay->addWidget(shiny);
    auto *reroll = new QPushButton(QStringLiteral("Reroll"), pidRow);
    reroll->setObjectName(QStringLiteral("BTN_RerollPID"));
    pidLay->addWidget(reroll);
    layout->addRow(lab(tab, "Label_PID", QStringLiteral("PID:")), pidRow);

    layout->addRow(lab(tab, "Label_Species", QStringLiteral("Species:")), combo(tab, "CB_Species"));
    layout->addRow(lab(tab, "Label_Form", QStringLiteral("Form:")), combo(tab, "CB_Form"));
    layout->addRow(check(tab, "CHK_NicknamedFlag", QStringLiteral("Nickname:")), edit(tab, "TB_Nickname"));
    auto *nickLabel = check(tab, "CHK_Nicknamed", QStringLiteral("Nickname:"));
    nickLabel->hide();
    layout->addRow(lab(tab, "Label_EXP", QStringLiteral("Exp. Points:")), edit(tab, "TB_EXP"));
    layout->addRow(lab(tab, "Label_CurLevel", QStringLiteral("Level:")), edit(tab, "TB_Level"));
    layout->addRow(lab(tab, "Label_Nature", QStringLiteral("Nature:")), combo(tab, "CB_Nature"));
    layout->addRow(lab(tab, "Label_HeldItem", QStringLiteral("Held Item:")), combo(tab, "CB_HeldItem"));
    layout->addRow(lab(tab, "Label_Ability", QStringLiteral("Ability:")), combo(tab, "CB_Ability"));
    layout->addRow(lab(tab, "Label_Language", QStringLiteral("Language:")), combo(tab, "CB_Language"));
    layout->addRow(check(tab, "CHK_IsEgg", QStringLiteral("Is Egg")));
    auto *infected = check(tab, "CHK_Infected", QStringLiteral("Infected"));
    auto *cured = check(tab, "CHK_Cured", QStringLiteral("Cured"));
    infected->setEnabled(false);
    cured->setEnabled(false);
    layout->addRow(infected);
    layout->addRow(cured);
    layout->addRow(lab(tab, "Label_PKRS", QStringLiteral("PkRs:")), combo(tab, "CB_PKRSStrain"));
    layout->addRow(lab(tab, "Label_PKRSdays", QStringLiteral("d:")), combo(tab, "CB_PKRSDays"));
    layout->addRow(check(tab, "CHK_NSparkle", QStringLiteral("Active")));
}

void fillMet(QWidget *tab)
{
    auto *layout = form(tab);
    layout->addRow(lab(tab, "Label_OriginGame", QStringLiteral("Origin Game:")), combo(tab, "CB_GameOrigin"));
    layout->addRow(lab(tab, "Label_MetLocation", QStringLiteral("Met Location:")), combo(tab, "CB_MetLocation"));
    layout->addRow(lab(tab, "Label_Ball", QStringLiteral("Ball:")), combo(tab, "CB_Ball"));
    auto *metDate = new QDateEdit(tab);
    metDate->setObjectName(QStringLiteral("CAL_MetDate"));
    metDate->setCalendarPopup(true);
    metDate->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    layout->addRow(lab(tab, "Label_MetDate", QStringLiteral("Met Date:")), metDate);
    layout->addRow(lab(tab, "Label_MetLevel", QStringLiteral("Met Level:")), edit(tab, "TB_MetLevel"));
    layout->addRow(check(tab, "CHK_Fateful", QStringLiteral("Fateful Encounter")));
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
    eggForm->addRow(lab(egg, "Label_EggDate", QStringLiteral("Date:")), eggDate);
    layout->addRow(egg);
}

void addStatRow(QGridLayout *grid, int row, const char *labelName, const QString &label, const char *base, const char *iv, const char *ev, const char *stat)
{
    grid->addWidget(lab(grid->parentWidget(), labelName, label), row, 0);
    auto *b = edit(grid->parentWidget(), base);
    b->setReadOnly(true);
    grid->addWidget(b, row, 1);
    grid->addWidget(edit(grid->parentWidget(), iv), row, 2);
    grid->addWidget(edit(grid->parentWidget(), ev), row, 3);
    auto *s = edit(grid->parentWidget(), stat);
    s->setReadOnly(true);
    grid->addWidget(s, row, 4);
}

void fillStats(QWidget *tab)
{
    auto *layout = new QVBoxLayout(tab);
    auto *gridHost = new QWidget(tab);
    gridHost->setObjectName(QStringLiteral("TLP_StatGrid"));
    auto *grid = new QGridLayout(gridHost);
    grid->addWidget(lab(gridHost, "Label_Base", QStringLiteral("Base")), 0, 1);
    grid->addWidget(lab(gridHost, "Label_IVs", QStringLiteral("IVs")), 0, 2);
    grid->addWidget(lab(gridHost, "Label_EVs", QStringLiteral("EVs")), 0, 3);
    grid->addWidget(lab(gridHost, "Label_Stats", QStringLiteral("Stats")), 0, 4);
    addStatRow(grid, 1, "Label_HP", QStringLiteral("HP:"), "TB_BaseHP", "TB_IVHP", "TB_EVHP", "Stat_HP");
    addStatRow(grid, 2, "Label_ATK", QStringLiteral("Atk:"), "TB_BaseATK", "TB_IVATK", "TB_EVATK", "Stat_ATK");
    addStatRow(grid, 3, "Label_DEF", QStringLiteral("Def:"), "TB_BaseDEF", "TB_IVDEF", "TB_EVDEF", "Stat_DEF");
    addStatRow(grid, 4, "Label_SPA", QStringLiteral("SpA:"), "TB_BaseSPA", "TB_IVSPA", "TB_EVSPA", "Stat_SPA");
    addStatRow(grid, 5, "Label_SPD", QStringLiteral("SpD:"), "TB_BaseSPD", "TB_IVSPD", "TB_EVSPD", "Stat_SPD");
    addStatRow(grid, 6, "Label_SPE", QStringLiteral("Spe:"), "TB_BaseSPE", "TB_IVSPE", "TB_EVSPE", "Stat_SPE");
    layout->addWidget(gridHost);
    auto *hp = new QWidget(tab);
    auto *hpLay = new QHBoxLayout(hp);
    hpLay->addWidget(lab(hp, "Label_HiddenPowerPrefix", QStringLiteral("Hidden Power Type:")));
    hpLay->addWidget(combo(hp, "CB_HPType"));
    hpLay->addWidget(lab(hp, "Label_HiddenPowerPower", QStringLiteral("60")));
    layout->addWidget(hp);
    layout->addWidget(lab(tab, "L_Characteristic", QStringLiteral("Characteristic:")));
    auto *randI = new QPushButton(QStringLiteral("Randomize IVs"), tab);
    randI->setObjectName(QStringLiteral("BTN_RandomIVs"));
    auto *randE = new QPushButton(QStringLiteral("Randomize EVs"), tab);
    randE->setObjectName(QStringLiteral("BTN_RandomEVs"));
    auto *btns = new QHBoxLayout();
    btns->addWidget(randI);
    btns->addWidget(randE);
    layout->addLayout(btns);
    layout->addStretch();
}

void fillMoves(QWidget *tab)
{
    auto *layout = new QVBoxLayout(tab);
    auto *gb = new QGroupBox(QStringLiteral("Current Moves"), tab);
    gb->setObjectName(QStringLiteral("GB_CurrentMoves"));
    auto *grid = new QGridLayout(gb);
    grid->addWidget(lab(gb, "Label_CurPP", QStringLiteral("PP")), 0, 1);
    grid->addWidget(lab(gb, "Label_PPups", QStringLiteral("Ups")), 0, 2);
    for (int i = 1; i <= 4; ++i)
    {
        grid->addWidget(combo(gb, QStringLiteral("CB_Move%1").arg(i)), i, 0);
        grid->addWidget(edit(gb, QStringLiteral("TB_PP%1").arg(i)), i, 1);
        grid->addWidget(combo(gb, QStringLiteral("CB_PPUps%1").arg(i)), i, 2);
    }
    layout->addWidget(gb);
    layout->addStretch();
}

void fillCosmetic(QWidget *tab)
{
    auto *layout = form(tab);
    layout->addRow(lab(tab, "Label_ContestStats", QStringLiteral("Contest Stats")));
    layout->addRow(lab(tab, "Label_Cool", QStringLiteral("Cool")), edit(tab, "TB_Cool"));
    layout->addRow(lab(tab, "Label_Beauty", QStringLiteral("Beauty")), edit(tab, "TB_Beauty"));
    layout->addRow(lab(tab, "Label_Cute", QStringLiteral("Cute")), edit(tab, "TB_Cute"));
    layout->addRow(lab(tab, "Label_Smart", QStringLiteral("Smart")), edit(tab, "TB_Smart"));
    layout->addRow(lab(tab, "Label_Tough", QStringLiteral("Tough")), edit(tab, "TB_Tough"));
    layout->addRow(lab(tab, "Label_Sheen", QStringLiteral("Sheen")), edit(tab, "TB_Sheen"));
    auto *fame = new QSpinBox(tab);
    fame->setObjectName(QStringLiteral("NUD_PokeStarFame"));
    fame->setRange(0, 255);
    layout->addRow(QStringLiteral("Pokéstar Fame"), fame);
}

void fillOt(QWidget *tab)
{
    auto *layout = form(tab);
    auto *otBox = new QGroupBox(QStringLiteral("Trainer Information"), tab);
    otBox->setObjectName(QStringLiteral("GB_OT"));
    auto *otForm = new QFormLayout(otBox);
    auto *otRow = new QWidget(otBox);
    auto *otLay = new QHBoxLayout(otRow);
    otLay->setContentsMargins(0, 0, 0, 0);
    otLay->addWidget(edit(otRow, "TB_OT"));
    otLay->addWidget(combo(otRow, "UC_OTGender"));
    otForm->addRow(lab(otBox, "Label_OT", QStringLiteral("OT:")), otRow);
    otForm->addRow(lab(otBox, "Label_TID", QStringLiteral("TID:")), edit(otBox, "TB_TID"));
    otForm->addRow(lab(otBox, "Label_SID", QStringLiteral("SID:")), edit(otBox, "TB_SID"));
    layout->addRow(otBox);
    layout->addRow(lab(tab, "Label_Friendship", QStringLiteral("Friendship:")), edit(tab, "TB_Friendship"));
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
