#include "SaveBlockWindow.h"

#include "ComboChrome.h"
#include "LangCatalog.h"
#include "ui_SAV_SimpleTrainer.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLayoutItem>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtGlobal>
#include <QVariant>
#include <QWidget>

SaveBlockWindow::SaveBlockWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::SAV_SimpleTrainer>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _ui->CB_Gender->addItem(QStringLiteral("♂"), 0);
    _ui->CB_Gender->addItem(QStringLiteral("♀"), 1);
    _ui->CB_BattleStyle->addItem(QStringLiteral("Shift"), 0);
    _ui->CB_BattleStyle->addItem(QStringLiteral("Set"), 1);
    _ui->CB_SoundType->addItem(QStringLiteral("Mono"), 0);
    _ui->CB_SoundType->addItem(QStringLiteral("Stereo"), 1);
    for (int i = 0; i < 8; ++i)
        _ui->CB_TextSpeed->addItem(QString::number(i), i);
    connect(_ui->B_Save, &QPushButton::clicked, this, &QDialog::accept);
    connect(_ui->B_Cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(_ui->B_MaxCash, &QPushButton::clicked, this, [this] { emit modifyRequested(QStringLiteral("B_MaxCash")); });
    connect(_ui->B_MaxCoins, &QPushButton::clicked, this, [this] { emit modifyRequested(QStringLiteral("B_MaxCoins")); });
    const auto markMap = [this] { _mapEdited = true; };
    connect(_ui->NUD_M, qOverload<int>(&QSpinBox::valueChanged), this, markMap);
    connect(_ui->NUD_X, qOverload<int>(&QSpinBox::valueChanged), this, markMap);
    connect(_ui->NUD_Y, qOverload<int>(&QSpinBox::valueChanged), this, markMap);
    connect(_ui->NUD_Z, qOverload<int>(&QSpinBox::valueChanged), this, markMap);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("SAV_SimpleTrainer"));
    setPageKind(QStringLiteral("trainer"));
    configureComboBoxes(this);
}

SaveBlockWindow::~SaveBlockWindow() = default;

void SaveBlockWindow::loadDocument(const QString &json)
{
    const auto root = QJsonDocument::fromJson(json.toUtf8()).object();
    const QString page = root.value(QStringLiteral("page")).toString();
    const QString kind = root.value(QStringLiteral("kind")).toString();
    if (page == QLatin1String("flags") || kind == QLatin1String("flags"))
        fillFlags(root);
    else if (kind == QLatin1String("fields"))
        fillFields(root);
    else
        fillTrainer(root);
    setProperty("saveBlockRoot", QVariant::fromValue(root));
}

void SaveBlockWindow::setPageKind(const QString &kind)
{
    const bool trainer = kind == QLatin1String("trainer");
    const bool flags = kind == QLatin1String("flags");
    const bool fields = kind == QLatin1String("fields");
    _ui->GB_Trainer->setVisible(trainer);
    _ui->GB_Adventure->setVisible(trainer);
    _ui->GB_Badges->setVisible(trainer);
    _ui->GB_Options->setVisible(trainer);
    _ui->GB_Map->setVisible(trainer);
    _ui->B_MaxCash->setVisible(trainer);
    _ui->B_MaxCoins->setVisible(trainer);
    _ui->GB_Flags->setVisible(flags);
    _ui->GB_FlagStatus->setVisible(flags);
    _ui->GB_Constants->setVisible(flags);
    _ui->GB_BlockTools->setVisible(fields);
}

void SaveBlockWindow::fillTrainer(const QJsonObject &root)
{
    setPageKind(QStringLiteral("trainer"));
    _mapEdited = false;
    _ui->TB_OTName->setText(root.value(QStringLiteral("ot")).toString());
    _ui->CB_Gender->setCurrentIndex(root.value(QStringLiteral("gender")).toInt());
    _ui->MT_TID->setValue(root.value(QStringLiteral("tid")).toInt());
    _ui->MT_SID->setValue(root.value(QStringLiteral("sid")).toInt());
    _ui->MT_Money->setMaximum(root.value(QStringLiteral("maxMoney")).toInt(9999999));
    _ui->MT_Money->setValue(root.value(QStringLiteral("money")).toInt());
    _ui->MT_Coins->setMaximum(root.value(QStringLiteral("maxCoins")).toInt(9999));
    _ui->MT_Coins->setValue(root.value(QStringLiteral("coins")).toInt());
    _ui->MT_Hours->setValue(root.value(QStringLiteral("hours")).toInt());
    _ui->MT_Minutes->setValue(root.value(QStringLiteral("minutes")).toInt());
    _ui->MT_Seconds->setValue(root.value(QStringLiteral("seconds")).toInt());
    _ui->MT_PikaFriend->setValue(root.value(QStringLiteral("pikaFriendship")).toInt());
    _ui->MT_PikaBeach->setValue(root.value(QStringLiteral("pikaBeach")).toInt());
    _ui->CB_Country->setValue(root.value(QStringLiteral("country")).toInt());
    _ui->CB_Region->setValue(root.value(QStringLiteral("region")).toInt());
    _ui->CHK_BattleEffects->setChecked(root.value(QStringLiteral("battleEffects")).toBool());
    _ui->CB_BattleStyle->setCurrentIndex(root.value(QStringLiteral("battleStyle")).toInt());
    _ui->CB_SoundType->setCurrentIndex(root.value(QStringLiteral("sound")).toInt());
    _ui->CB_TextSpeed->setCurrentIndex(root.value(QStringLiteral("textSpeed")).toInt());
    _ui->NUD_M->setValue(root.value(QStringLiteral("mapM")).toInt());
    _ui->NUD_X->setValue(root.value(QStringLiteral("mapX")).toInt());
    _ui->NUD_Y->setValue(root.value(QStringLiteral("mapY")).toInt());
    _ui->NUD_Z->setValue(root.value(QStringLiteral("mapZ")).toInt());
    _mapEdited = false;

    const int badges = root.value(QStringLiteral("badges")).toInt();
    const int badgeCount = root.value(QStringLiteral("badgeCount")).toInt(8);
    const char *names[] = {
        "CHK_1", "CHK_2", "CHK_3", "CHK_4", "CHK_5", "CHK_6", "CHK_7", "CHK_8",
        "CHK_H1", "CHK_H2", "CHK_H3", "CHK_H4", "CHK_H5", "CHK_H6", "CHK_H7", "CHK_H8",
    };
    for (int i = 0; i < 16; ++i)
    {
        auto *box = findChild<QCheckBox *>(QString::fromLatin1(names[i]));
        if (box == nullptr)
            continue;
        box->setVisible(i < badgeCount);
        box->setChecked((badges & (1 << i)) != 0);
    }

    const bool gender = root.value(QStringLiteral("hasGender")).toBool();
    const bool sid = root.value(QStringLiteral("hasSid")).toBool();
    const bool coins = root.value(QStringLiteral("hasCoins")).toBool();
    const bool country = root.value(QStringLiteral("hasCountry")).toBool();
    const bool map = root.value(QStringLiteral("hasMap")).toBool();
    const bool options = root.value(QStringLiteral("hasOptions")).toBool();
    const bool pika = root.value(QStringLiteral("hasPika")).toBool();
    _ui->CB_Gender->setVisible(gender);
    _ui->L_SID->setVisible(sid);
    _ui->MT_SID->setVisible(sid);
    _ui->L_Coins->setVisible(coins);
    _ui->MT_Coins->setVisible(coins);
    _ui->B_MaxCoins->setVisible(coins);
    _ui->L_Country->setVisible(country);
    _ui->CB_Country->setVisible(country);
    _ui->L_Region->setVisible(country);
    _ui->CB_Region->setVisible(country);
    _ui->GB_Map->setVisible(map);
    _ui->GB_Options->setVisible(options);
    _ui->GB_Badges->setVisible(badgeCount > 0);
    _ui->L_PikaFriend->setVisible(pika);
    _ui->MT_PikaFriend->setVisible(pika);
    _ui->L_PikaBeach->setVisible(pika);
    _ui->MT_PikaBeach->setVisible(pika);

    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, root.value(QStringLiteral("langForm")).toString(QStringLiteral("SAV_SimpleTrainer")));
}

void SaveBlockWindow::fillFlags(const QJsonObject &root)
{
    setPageKind(QStringLiteral("flags"));
    auto *flags = _ui->TLP_Flags;
    flags->horizontalHeader()->setVisible(false);
    flags->verticalHeader()->setVisible(false);
    flags->setShowGrid(false);
    flags->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    const auto flagRows = root.value(QStringLiteral("flags")).toArray();
    flags->setRowCount(flagRows.size());
    for (int i = 0; i < flagRows.size(); ++i)
    {
        const auto row = flagRows.at(i).toObject();
        auto *check = new QCheckBox(flags);
        check->setChecked(row.value(QStringLiteral("value")).toBool());
        flags->setCellWidget(i, 0, check);
        auto *label = new QTableWidgetItem(row.value(QStringLiteral("name")).toString());
        label->setFlags(label->flags() & ~Qt::ItemIsEditable);
        label->setData(Qt::UserRole, row.value(QStringLiteral("index")).toInt());
        flags->setItem(i, 1, label);
    }
    auto *work = _ui->TLP_Const;
    work->horizontalHeader()->setVisible(false);
    work->verticalHeader()->setVisible(false);
    work->setShowGrid(false);
    work->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    const auto workRows = root.value(QStringLiteral("work")).toArray();
    work->setRowCount(workRows.size());
    for (int i = 0; i < workRows.size(); ++i)
    {
        const auto row = workRows.at(i).toObject();
        auto *spin = new QSpinBox(work);
        spin->setMaximum(65535);
        spin->setValue(row.value(QStringLiteral("value")).toInt());
        work->setCellWidget(i, 0, spin);
        auto *label = new QTableWidgetItem(row.value(QStringLiteral("name")).toString());
        label->setFlags(label->flags() & ~Qt::ItemIsEditable);
        label->setData(Qt::UserRole, row.value(QStringLiteral("index")).toInt());
        work->setItem(i, 1, label);
    }
    _ui->NUD_Flag->setMaximum(qMax(0, root.value(QStringLiteral("flagCount")).toInt() - 1));
    _ui->NUD_Flag->setValue(root.value(QStringLiteral("customFlag")).toInt());
    _ui->c_CustomFlag->setChecked(root.value(QStringLiteral("customFlagValue")).toBool());
    _ui->CB_Stats->setMaximum(qMax(0, root.value(QStringLiteral("workCount")).toInt() - 1));
    _ui->CB_Stats->setValue(root.value(QStringLiteral("customWork")).toInt());
    _ui->MT_Stat->setValue(root.value(QStringLiteral("customWorkValue")).toInt());
    _ui->GB_Constants->setVisible(root.value(QStringLiteral("workCount")).toInt() > 0);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, root.value(QStringLiteral("langForm")).toString(QStringLiteral("SAV_EventFlags")));
}

void SaveBlockWindow::clearBlockActions()
{
    auto *layout = qobject_cast<QHBoxLayout *>(_ui->P_BlockActions->layout());
    if (layout == nullptr)
        return;
    while (layout->count() > 0)
    {
        QLayoutItem *item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
}

void SaveBlockWindow::fillFields(const QJsonObject &root)
{
    setPageKind(QStringLiteral("fields"));
    const QString title = root.value(QStringLiteral("title")).toString();
    if (!title.isEmpty())
    {
        setWindowTitle(title);
        _ui->GB_BlockTools->setTitle(title);
    }
    setMinimumSize(520, 560);
    auto *table = _ui->TLP_Fields;
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({QStringLiteral("#"), QStringLiteral("Label"), QStringLiteral("Value")});
    table->horizontalHeader()->setVisible(true);
    table->verticalHeader()->setVisible(false);
    table->setShowGrid(true);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    const auto rows = root.value(QStringLiteral("fields")).toArray();
    table->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i)
    {
        const auto row = rows.at(i).toObject();
        const QString kind = row.value(QStringLiteral("kind")).toString(QStringLiteral("number"));
        const int index = row.value(QStringLiteral("index")).toInt(-1);
        auto *indexItem = new QTableWidgetItem(index >= 0 ? QString::number(index) : QString());
        indexItem->setFlags(indexItem->flags() & ~Qt::ItemIsEditable);
        table->setItem(i, 0, indexItem);

        auto *label = new QTableWidgetItem(row.value(QStringLiteral("label")).toString());
        label->setFlags(label->flags() & ~Qt::ItemIsEditable);
        label->setData(Qt::UserRole, row.value(QStringLiteral("name")).toString());
        label->setData(Qt::UserRole + 1, kind);
        table->setItem(i, 1, label);

        if (kind == QLatin1String("bool"))
        {
            auto *check = new QCheckBox(table);
            check->setChecked(row.value(QStringLiteral("value")).toInt() != 0);
            table->setCellWidget(i, 2, check);
        }
        else if (kind == QLatin1String("text"))
        {
            auto *edit = new QLineEdit(table);
            edit->setText(row.value(QStringLiteral("text")).toString());
            table->setCellWidget(i, 2, edit);
        }
        else if (kind == QLatin1String("choice"))
        {
            auto *combo = new QComboBox(table);
            configureComboBox(combo);
            const auto choices = row.value(QStringLiteral("choices")).toArray();
            for (const auto &entry : choices)
            {
                const auto choice = entry.toObject();
                combo->addItem(choice.value(QStringLiteral("label")).toString(),
                    choice.value(QStringLiteral("value")).toInt());
            }
            const int at = combo->findData(row.value(QStringLiteral("value")).toInt());
            if (at >= 0)
                combo->setCurrentIndex(at);
            table->setCellWidget(i, 2, combo);
        }
        else
        {
            auto *spin = new QSpinBox(table);
            spin->setMaximum(999999999);
            spin->setValue(row.value(QStringLiteral("value")).toInt());
            table->setCellWidget(i, 2, spin);
        }
    }
    clearBlockActions();
    auto *actions = qobject_cast<QHBoxLayout *>(_ui->P_BlockActions->layout());
    const auto names = root.value(QStringLiteral("actions")).toArray();
    for (const auto &entry : names)
    {
        const QString name = entry.toString();
        auto *button = new QPushButton(_ui->P_BlockActions);
        button->setObjectName(name);
        button->setText(name);
        connect(button, &QPushButton::clicked, this, [this, name] { emit modifyRequested(name); });
        actions->addWidget(button);
    }
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, root.value(QStringLiteral("langForm")).toString());
    if (!title.isEmpty())
    {
        setWindowTitle(title);
        _ui->GB_BlockTools->setTitle(title);
    }
}

QJsonObject SaveBlockWindow::collectDocument() const
{
    QJsonObject root = property("saveBlockRoot").toJsonObject();
    if (root.value(QStringLiteral("page")).toString() == QLatin1String("flags"))
    {
        QJsonArray flagRows;
        for (int i = 0; i < _ui->TLP_Flags->rowCount(); ++i)
        {
            const auto *label = _ui->TLP_Flags->item(i, 1);
            if (label == nullptr)
                continue;
            QJsonObject row;
            row.insert(QStringLiteral("index"), label->data(Qt::UserRole).toInt());
            auto *check = qobject_cast<QCheckBox *>(_ui->TLP_Flags->cellWidget(i, 0));
            row.insert(QStringLiteral("value"), check != nullptr && check->isChecked());
            flagRows.append(row);
        }
        QJsonArray workRows;
        for (int i = 0; i < _ui->TLP_Const->rowCount(); ++i)
        {
            const auto *label = _ui->TLP_Const->item(i, 1);
            if (label == nullptr)
                continue;
            QJsonObject row;
            row.insert(QStringLiteral("index"), label->data(Qt::UserRole).toInt());
            auto *spin = qobject_cast<QSpinBox *>(_ui->TLP_Const->cellWidget(i, 0));
            row.insert(QStringLiteral("value"), spin == nullptr ? 0 : spin->value());
            workRows.append(row);
        }
        root.insert(QStringLiteral("flags"), flagRows);
        root.insert(QStringLiteral("work"), workRows);
        root.insert(QStringLiteral("customFlag"), _ui->NUD_Flag->value());
        root.insert(QStringLiteral("customFlagValue"), _ui->c_CustomFlag->isChecked());
        root.insert(QStringLiteral("customWork"), _ui->CB_Stats->value());
        root.insert(QStringLiteral("customWorkValue"), _ui->MT_Stat->value());
        return root;
    }
    if (root.value(QStringLiteral("kind")).toString() == QLatin1String("fields"))
    {
        QJsonArray fields;
        for (int i = 0; i < _ui->TLP_Fields->rowCount(); ++i)
        {
            const auto *label = _ui->TLP_Fields->item(i, 1);
            if (label == nullptr)
                continue;
            QJsonObject row;
            const QString kind = label->data(Qt::UserRole + 1).toString();
            row.insert(QStringLiteral("name"), label->data(Qt::UserRole).toString());
            row.insert(QStringLiteral("label"), label->text());
            row.insert(QStringLiteral("kind"), kind);
            const auto *indexItem = _ui->TLP_Fields->item(i, 0);
            if (indexItem != nullptr && !indexItem->text().isEmpty())
                row.insert(QStringLiteral("index"), indexItem->text().toInt());
            auto *widget = _ui->TLP_Fields->cellWidget(i, 2);
            if (auto *check = qobject_cast<QCheckBox *>(widget))
                row.insert(QStringLiteral("value"), check->isChecked() ? 1 : 0);
            else if (auto *edit = qobject_cast<QLineEdit *>(widget))
                row.insert(QStringLiteral("text"), edit->text());
            else if (auto *combo = qobject_cast<QComboBox *>(widget))
                row.insert(QStringLiteral("value"), combo->currentData().toInt());
            else if (auto *spin = qobject_cast<QSpinBox *>(widget))
                row.insert(QStringLiteral("value"), spin->value());
            fields.append(row);
        }
        root.insert(QStringLiteral("fields"), fields);
        return root;
    }
    root.insert(QStringLiteral("ot"), _ui->TB_OTName->text());
    root.insert(QStringLiteral("gender"), _ui->CB_Gender->currentIndex());
    root.insert(QStringLiteral("tid"), _ui->MT_TID->value());
    root.insert(QStringLiteral("sid"), _ui->MT_SID->value());
    root.insert(QStringLiteral("money"), _ui->MT_Money->value());
    root.insert(QStringLiteral("coins"), _ui->MT_Coins->value());
    root.insert(QStringLiteral("hours"), _ui->MT_Hours->value());
    root.insert(QStringLiteral("minutes"), _ui->MT_Minutes->value());
    root.insert(QStringLiteral("seconds"), _ui->MT_Seconds->value());
    root.insert(QStringLiteral("pikaFriendship"), _ui->MT_PikaFriend->value());
    root.insert(QStringLiteral("pikaBeach"), _ui->MT_PikaBeach->value());
    root.insert(QStringLiteral("country"), _ui->CB_Country->value());
    root.insert(QStringLiteral("region"), _ui->CB_Region->value());
    root.insert(QStringLiteral("battleEffects"), _ui->CHK_BattleEffects->isChecked());
    root.insert(QStringLiteral("battleStyle"), _ui->CB_BattleStyle->currentIndex());
    root.insert(QStringLiteral("sound"), _ui->CB_SoundType->currentIndex());
    root.insert(QStringLiteral("textSpeed"), _ui->CB_TextSpeed->currentIndex());
    root.insert(QStringLiteral("mapM"), _ui->NUD_M->value());
    root.insert(QStringLiteral("mapX"), _ui->NUD_X->value());
    root.insert(QStringLiteral("mapY"), _ui->NUD_Y->value());
    root.insert(QStringLiteral("mapZ"), _ui->NUD_Z->value());
    root.insert(QStringLiteral("mapUpdated"), _mapEdited);
    const char *names[] = {
        "CHK_1", "CHK_2", "CHK_3", "CHK_4", "CHK_5", "CHK_6", "CHK_7", "CHK_8",
        "CHK_H1", "CHK_H2", "CHK_H3", "CHK_H4", "CHK_H5", "CHK_H6", "CHK_H7", "CHK_H8",
    };
    int badges = 0;
    for (int i = 0; i < 16; ++i)
    {
        auto *box = findChild<QCheckBox *>(QString::fromLatin1(names[i]));
        if (box != nullptr && box->isVisible() && box->isChecked())
            badges |= 1 << i;
    }
    root.insert(QStringLiteral("badges"), badges);
    return root;
}

QString SaveBlockWindow::document() const
{
    return QString::fromUtf8(QJsonDocument(collectDocument()).toJson(QJsonDocument::Compact));
}
