#include "MainWindow.h"

#include "EditorBridge.h"
#include "LangCatalog.h"
#include "PkmTabChrome.h"
#include "SavToolChrome.h"
#include "SlotChrome.h"
#include "ui_MainWindow.h"

#include <QAbstractSpinBox>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDateEdit>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QUrl>
#include <QWidget>

MainWindow::MainWindow(EditorBridge &editor, QWidget *parent)
    : QMainWindow(parent)
    , _editor(editor)
    , _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);
    fillSavChrome();
    fillSlotChrome();
    fillPkmChrome();
    applyEnglishStrings();
    applyInGameFont(this);
    setWindowTitle(QStringLiteral("PKHeX Qt"));

    connect(_ui->Menu_Open, &QAction::triggered, this, &MainWindow::onMenuOpen);
    connect(_ui->Menu_ExportSAV, &QAction::triggered, this, &MainWindow::onMenuExportSav);
    connect(_ui->Menu_Exit, &QAction::triggered, this, &MainWindow::onMenuExit);
    connect(_ui->Menu_ShowdownImportPKM, &QAction::triggered, this, &MainWindow::onShowdownImport);
    connect(_ui->Menu_ShowdownExportPKM, &QAction::triggered, this, &MainWindow::onShowdownExportPkm);
    connect(_ui->Menu_ShowdownExportParty, &QAction::triggered, this, &MainWindow::onShowdownExportParty);
    connect(_ui->Menu_ShowdownExportCurrentBox, &QAction::triggered, this, &MainWindow::onShowdownExportBox);
    updateExportEnabled();
}

MainWindow::~MainWindow() = default;

bool MainWindow::openPath(const QString &path)
{
    const bool ok = _editor.openPath(path);
    if (!ok)
        QMessageBox::warning(this, windowTitle(), tr("Could not open that save."));
    updateExportEnabled();
    refreshStorage();
    if (ok)
    {
        StorageLayout layout;
        if (_editor.storageLayout(layout))
            _editor.selectSlot(QStringLiteral("box:%1:0").arg(layout.currentBox));
        refreshPkmEditor();
    }
    return ok;
}

bool MainWindow::savePath(const QString &path)
{
    const bool ok = _editor.savePath(path);
    if (!ok)
        QMessageBox::warning(this, windowTitle(), tr("Could not export that save."));
    return ok;
}

void MainWindow::onMenuOpen()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open"),
        QString(),
        tr("All Files (*)"));
    if (path.isEmpty())
        return;
    openPath(path);
}

void MainWindow::onMenuExportSav()
{
    if (!_editor.hasSession())
        return;

    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Export SAV"),
        QString(),
        tr("All Files (*)"));
    if (path.isEmpty())
        return;
    savePath(path);
}

void MainWindow::updateExportEnabled()
{
    const bool open = _editor.hasSession();
    _ui->Menu_ExportSAV->setEnabled(open);
    _ui->Menu_ShowdownImportPKM->setEnabled(open);
    _ui->Menu_ShowdownExportPKM->setEnabled(open);
    _ui->Menu_ShowdownExportParty->setEnabled(open);
    _ui->Menu_ShowdownExportCurrentBox->setEnabled(open);
}

void MainWindow::onMenuExit()
{
    close();
}

void MainWindow::fillSavChrome()
{
    fillSavToolButtons(findChild<QWidget *>(QStringLiteral("FLP_SAVtools")));
}

void MainWindow::fillSlotChrome()
{
    fillBoxAndPartyChrome(
        findChild<QWidget *>(QStringLiteral("Tab_Box")),
        findChild<QWidget *>(QStringLiteral("Tab_PartyBattle")));

    const auto slotLabels = findChildren<QLabel *>();
    for (auto *slot : slotLabels)
    {
        if (!slot->property("slotKey").toString().isEmpty())
            slot->installEventFilter(this);
    }

    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    auto *left = findChild<QPushButton *>(QStringLiteral("B_BoxLeft"));
    auto *right = findChild<QPushButton *>(QStringLiteral("B_BoxRight"));
    if (select != nullptr)
        connect(select, &QComboBox::currentIndexChanged, this, &MainWindow::onBoxSelected);
    if (left != nullptr)
        connect(left, &QPushButton::clicked, this, &MainWindow::onBoxLeft);
    if (right != nullptr)
        connect(right, &QPushButton::clicked, this, &MainWindow::onBoxRight);
}

void MainWindow::onBoxSelected(int index)
{
    if (!_editor.hasSession() || index < 0)
        return;
    _editor.setCurrentBox(index);
    StorageLayout layout;
    if (!_editor.storageLayout(layout))
        return;
    refreshBoxSlots(layout);
}

void MainWindow::onBoxLeft()
{
    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    if (select == nullptr || select->count() == 0)
        return;
    const int next = (select->currentIndex() + select->count() - 1) % select->count();
    select->setCurrentIndex(next);
}

void MainWindow::onBoxRight()
{
    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    if (select == nullptr || select->count() == 0)
        return;
    const int next = (select->currentIndex() + 1) % select->count();
    select->setCurrentIndex(next);
}

void MainWindow::refreshStorage()
{
    if (!_editor.hasSession())
        return;

    StorageLayout layout;
    if (!_editor.storageLayout(layout))
        return;

    refreshBoxSelector(layout);
    refreshBoxSlots(layout);
    refreshPartySlots();
}

void MainWindow::refreshBoxSelector(const StorageLayout &layout)
{
    auto *select = findChild<QComboBox *>(QStringLiteral("CB_BoxSelect"));
    if (select == nullptr)
        return;

    const QSignalBlocker block(select);
    select->clear();
    for (int i = 0; i < layout.boxCount; ++i)
        select->addItem(tr("Box %1").arg(i + 1));
    if (layout.currentBox >= 0 && layout.currentBox < layout.boxCount)
        select->setCurrentIndex(layout.currentBox);
}

void MainWindow::refreshBoxSlots(const StorageLayout &layout)
{
    auto *grid = findChild<BoxPokeGrid *>(QStringLiteral("BoxPokeGrid"));
    if (grid == nullptr)
        return;

    grid->setWallpaperPng(_editor.slotPng(QStringLiteral("wallpaper:%1").arg(layout.currentBox)));

    int index = 0;
    for (int row = 0; row < 5; ++row)
    {
        for (int column = 0; column < 6; ++column)
        {
            const QString name = QStringLiteral("Pokémon Grid Row %1 Column %2")
                                     .arg(row, 2, 10, QLatin1Char('0'))
                                     .arg(column, 2, 10, QLatin1Char('0'));
            auto *slot = grid->findChild<QLabel *>(name);
            if (slot == nullptr)
                continue;
            const QString key = QStringLiteral("box:%1:%2").arg(layout.currentBox).arg(index);
            slot->setProperty("slotKey", key);
            applySlotPng(slot, _editor.slotPng(key));
            ++index;
        }
    }
}

void MainWindow::refreshPartySlots()
{
    auto *grid = findChild<QWidget *>(QStringLiteral("PartyPokeGrid"));
    if (grid == nullptr)
        return;

    int index = 0;
    for (int row = 0; row < 3; ++row)
    {
        for (int column = 0; column < 2; ++column)
        {
            const QString name = QStringLiteral("Pokémon Grid Row %1 Column %2")
                                     .arg(row, 2, 10, QLatin1Char('0'))
                                     .arg(column, 2, 10, QLatin1Char('0'));
            auto *slot = grid->findChild<QLabel *>(name);
            if (slot == nullptr)
                continue;
            const QString key = QStringLiteral("party:%1").arg(index);
            slot->setProperty("slotKey", key);
            applySlotPng(slot, _editor.slotPng(key));
            ++index;
        }
    }
}

void MainWindow::applyEnglishStrings()
{
    const QString path = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt"));
    LangCatalog catalog;
    if (!catalog.loadFromFile(path))
        return;
    catalog.apply(this, QStringLiteral("Main"));
}

namespace
{
const char *kComboFields[] = {
    "CB_Species", "CB_Form", "CB_Nature", "CB_HeldItem", "CB_Ability", "CB_Language",
    "CB_GameOrigin", "CB_MetLocation", "CB_Ball", "CB_EggLocation", "CB_GroundTile",
    "CB_Move1", "CB_Move2", "CB_Move3", "CB_Move4", "CB_HPType",
    "CB_PPUps1", "CB_PPUps2", "CB_PPUps3", "CB_PPUps4", "UC_Gender", "UC_OTGender",
    "CB_PKRSStrain", "CB_PKRSDays",
};

const char *kEditFields[] = {
    "TB_PID", "TB_Nickname", "TB_EXP", "TB_Level", "TB_MetLevel", "TB_OT", "TB_TID", "TB_SID",
    "TB_Friendship", "TB_IVHP", "TB_IVATK", "TB_IVDEF", "TB_IVSPA", "TB_IVSPD", "TB_IVSPE",
    "TB_EVHP", "TB_EVATK", "TB_EVDEF", "TB_EVSPA", "TB_EVSPD", "TB_EVSPE",
    "TB_PP1", "TB_PP2", "TB_PP3", "TB_PP4", "TB_Cool", "TB_Beauty", "TB_Cute", "TB_Smart",
    "TB_Tough", "TB_Sheen", "TB_BaseHP", "TB_BaseATK", "TB_BaseDEF", "TB_BaseSPA", "TB_BaseSPD",
    "TB_BaseSPE", "Stat_HP", "Stat_ATK", "Stat_DEF", "Stat_SPA", "Stat_SPD", "Stat_SPE",
};

const char *kCheckFields[] = {
    "CHK_NicknamedFlag", "CHK_IsEgg", "CHK_Infected", "CHK_Cured", "CHK_NSparkle",
    "CHK_Fateful", "CHK_AsEgg", "PB_MarkShiny",
};

const char *kWritableChecks[] = {
    "CHK_NicknamedFlag", "CHK_IsEgg", "CHK_NSparkle", "CHK_Fateful", "PB_MarkShiny",
};
}

void MainWindow::fillPkmChrome()
{
    fillPkmTabs(
        findChild<QWidget *>(QStringLiteral("Tab_Main")),
        findChild<QWidget *>(QStringLiteral("Tab_Met")),
        findChild<QWidget *>(QStringLiteral("Tab_Stats")),
        findChild<QWidget *>(QStringLiteral("Tab_Moves")),
        findChild<QWidget *>(QStringLiteral("Tab_Cosmetic")),
        findChild<QWidget *>(QStringLiteral("Tab_OTMisc")));
    bindPkmFields();
    if (auto *legal = findChild<QLabel *>(QStringLiteral("PB_Legal")))
    {
        legal->installEventFilter(this);
        legal->setProperty("legalityIcon", true);
    }
}

void MainWindow::bindPkmFields()
{
    for (const char *name : kEditFields)
    {
        if (auto *box = findChild<QLineEdit *>(QString::fromLatin1(name)))
            connect(box, &QLineEdit::editingFinished, this, &MainWindow::onPkmFieldEdited);
    }
    for (const char *name : kComboFields)
    {
        if (auto *box = findChild<QComboBox *>(QString::fromLatin1(name)))
            connect(box, &QComboBox::currentIndexChanged, this, &MainWindow::onPkmFieldEdited);
    }
    for (const char *name : kWritableChecks)
    {
        if (auto *box = findChild<QCheckBox *>(QString::fromLatin1(name)))
            connect(box, &QCheckBox::toggled, this, &MainWindow::onPkmFieldEdited);
    }
    for (auto *date : findChildren<QDateEdit *>())
        connect(date, &QDateEdit::dateChanged, this, &MainWindow::onPkmFieldEdited);
    if (auto *fame = findChild<QSpinBox *>(QStringLiteral("NUD_PokeStarFame")))
        connect(fame, &QSpinBox::valueChanged, this, &MainWindow::onPkmFieldEdited);
}

void MainWindow::onLegalityClicked()
{
    if (!_editor.hasSession())
        return;
    const QString report = _editor.legalityReport(false);
    QMessageBox box(this);
    box.setWindowTitle(tr("Legality Report"));
    box.setText(_editor.legalityValid() ? tr("Legal") : tr("Invalid"));
    box.setInformativeText(report);
    box.exec();
}

void MainWindow::onPkmFieldEdited()
{
    if (_pkmBusy || !_editor.hasSession())
        return;
    auto *obj = sender();
    if (obj == nullptr)
        return;
    const QString name = obj->objectName();
    if (name.isEmpty())
        return;

    QString value;
    if (auto *box = qobject_cast<QLineEdit *>(obj))
        value = box->text();
    else if (auto *combo = qobject_cast<QComboBox *>(obj))
        value = combo->currentData().isValid() ? combo->currentData().toString() : combo->currentText();
    else if (auto *check = qobject_cast<QCheckBox *>(obj))
        value = check->isChecked() ? QStringLiteral("1") : QStringLiteral("0");
    else if (auto *date = qobject_cast<QDateEdit *>(obj))
        value = date->date().toString(QStringLiteral("yyyy-MM-dd"));
    else if (auto *spin = qobject_cast<QSpinBox *>(obj))
        value = QString::number(spin->value());
    else
        return;

    writeField(name, value);
}

void MainWindow::writeField(const QString &name, const QString &value)
{
    if (!_editor.setField(name, value))
        return;
    _editor.commitCurrent();
    refreshPkmEditor();
}

void MainWindow::refreshPkmEditor()
{
    if (!_editor.hasSession())
        return;
    _pkmBusy = true;
    for (const char *name : kComboFields)
        fillComboChoices(QString::fromLatin1(name));
    for (const char *name : kComboFields)
        applyFieldValue(QString::fromLatin1(name), _editor.getField(QString::fromLatin1(name)));
    for (const char *name : kEditFields)
        applyFieldValue(QString::fromLatin1(name), _editor.getField(QString::fromLatin1(name)));
    for (const char *name : kCheckFields)
        applyFieldValue(QString::fromLatin1(name), _editor.getField(QString::fromLatin1(name)));
    applyFieldValue(QStringLiteral("CAL_MetDate"), _editor.getField(QStringLiteral("CAL_MetDate")));
    applyFieldValue(QStringLiteral("CAL_EggDate"), _editor.getField(QStringLiteral("CAL_EggDate")));
    applyFieldValue(QStringLiteral("NUD_PokeStarFame"), _editor.getField(QStringLiteral("NUD_PokeStarFame")));
    applyFieldValue(QStringLiteral("L_Characteristic"), _editor.getField(QStringLiteral("L_Characteristic")));
    applyFieldValue(QStringLiteral("Label_HiddenPowerPower"), _editor.getField(QStringLiteral("Label_HiddenPowerPower")));
    setLegalityIcon(findChild<QLabel *>(QStringLiteral("PB_Legal")), _editor.legalityValid());
    _pkmBusy = false;
}

void MainWindow::fillComboChoices(const QString &name)
{
    auto *box = findChild<QComboBox *>(name);
    if (box == nullptr)
        return;
    const QSignalBlocker block(box);
    box->clear();
    if (name == QLatin1String("UC_Gender") || name == QLatin1String("UC_OTGender"))
    {
        box->addItem(QStringLiteral("♂"), 0);
        box->addItem(QStringLiteral("♀"), 1);
        return;
    }
    const auto lines = _editor.fieldChoices(name).split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const auto &line : lines)
    {
        const auto parts = line.split(QLatin1Char('\t'));
        if (parts.size() < 2)
            continue;
        box->addItem(parts[1], parts[0].toInt());
    }
}

void MainWindow::applyFieldValue(const QString &name, const QString &value)
{
    if (auto *box = findChild<QLineEdit *>(name))
    {
        const QSignalBlocker block(box);
        box->setText(value);
        return;
    }
    if (auto *combo = findChild<QComboBox *>(name))
    {
        const QSignalBlocker block(combo);
        const int idx = combo->findData(value.toInt());
        if (idx >= 0)
            combo->setCurrentIndex(idx);
        return;
    }
    if (auto *check = findChild<QCheckBox *>(name))
    {
        const QSignalBlocker block(check);
        check->setChecked(value == QLatin1String("1"));
        return;
    }
    if (auto *date = findChild<QDateEdit *>(name))
    {
        const QSignalBlocker block(date);
        const auto parsed = QDate::fromString(value, QStringLiteral("yyyy-MM-dd"));
        if (parsed.isValid())
            date->setDate(parsed);
        return;
    }
    if (auto *spin = findChild<QSpinBox *>(name))
    {
        const QSignalBlocker block(spin);
        spin->setValue(value.toInt());
        return;
    }
    if (auto *label = findChild<QLabel *>(name))
        label->setText(value);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton)
        {
            if (watched->property("legalityIcon").toBool())
            {
                onLegalityClicked();
                return true;
            }
            const QString key = watched->property("slotKey").toString();
            if (!key.isEmpty())
            {
                _editor.selectSlot(key);
                refreshPkmEditor();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

namespace
{
constexpr auto kEntityMime = "application/x-pkhex-pokemon";
}

void MainWindow::onShowdownImport()
{
    if (!_editor.hasSession())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr || clip->text().isEmpty())
    {
        QMessageBox::warning(this, windowTitle(), tr("Set data not found in clipboard."));
        return;
    }
    const QString text = clip->text();
    const QString preview = _editor.previewShowdown(text);
    if (preview.isEmpty())
    {
        QMessageBox::warning(this, windowTitle(), tr("Set data not found in clipboard."));
        return;
    }
    const auto answer = QMessageBox::question(this, windowTitle(), tr("Import this set?\n\n%1").arg(preview));
    if (answer != QMessageBox::Yes)
        return;
    if (!_editor.importShowdown(text))
    {
        QMessageBox::warning(this, windowTitle(), tr("Set data not found in clipboard."));
        return;
    }
    refreshPkmEditor();
}

void MainWindow::onShowdownExportPkm()
{
    copyShowdown(QStringLiteral("pkm"), tr("Exported Showdown Set to Clipboard:"));
}

void MainWindow::onShowdownExportParty()
{
    copyShowdown(QStringLiteral("party"), tr("Showdown Team (Party) set to Clipboard."));
}

void MainWindow::onShowdownExportBox()
{
    copyShowdown(QStringLiteral("box"), tr("Showdown Sets copied to Clipboard."));
}

void MainWindow::copyShowdown(const QString &scope, const QString &success)
{
    if (!_editor.hasSession())
        return;
    const QString text = _editor.exportShowdown(scope);
    if (text.trimmed().isEmpty())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr)
        return;
    clip->setText(text);
    if (scope == QLatin1String("pkm"))
        QMessageBox::information(this, windowTitle(), success + QLatin1Char('\n') + text);
    else
        QMessageBox::information(this, windowTitle(), success);
}

void MainWindow::copyEntityToClipboard()
{
    if (!_editor.hasSession())
        return;
    const QByteArray data = _editor.exportEntity();
    if (data.isEmpty())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr)
        return;
    auto *mime = new QMimeData();
    mime->setData(QString::fromLatin1(kEntityMime), data);
    const QString name = _editor.entityFileName();
    if (!name.isEmpty())
    {
        const QString path = QDir::temp().filePath(name);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.write(data);
            file.close();
            mime->setUrls({QUrl::fromLocalFile(path)});
        }
    }
    clip->setMimeData(mime);
}

void MainWindow::pasteEntityFromClipboard()
{
    if (!_editor.hasSession())
        return;
    auto *clip = QGuiApplication::clipboard();
    if (clip == nullptr)
        return;
    const QMimeData *mime = clip->mimeData();
    if (mime == nullptr)
        return;
    QByteArray data;
    if (mime->hasFormat(QString::fromLatin1(kEntityMime)))
        data = mime->data(QString::fromLatin1(kEntityMime));
    else if (mime->hasUrls())
    {
        for (const auto &url : mime->urls())
        {
            const QString path = url.toLocalFile();
            if (path.isEmpty())
                continue;
            QFile file(path);
            if (file.open(QIODevice::ReadOnly))
            {
                data = file.readAll();
                break;
            }
        }
    }
    if (data.isEmpty() || !_editor.importEntity(data))
        return;
    refreshPkmEditor();
}

bool MainWindow::textWidgetHasFocus() const
{
    auto *focus = focusWidget();
    return qobject_cast<QLineEdit *>(focus) != nullptr
        || qobject_cast<QAbstractSpinBox *>(focus) != nullptr;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!textWidgetHasFocus())
    {
        if (event->matches(QKeySequence::Copy))
        {
            copyEntityToClipboard();
            return;
        }
        if (event->matches(QKeySequence::Paste))
        {
            pasteEntityFromClipboard();
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}
