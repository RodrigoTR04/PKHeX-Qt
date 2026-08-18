#include "WondercardWindow.h"

#include "LangCatalog.h"
#include "ui_SAV_Wondercard.h"

#include <QAbstractItemView>
#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>

WondercardWindow::WondercardWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::SAV_Wondercard>())
{
    _ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _ui->FLP_Gifts->horizontalHeader()->setStretchLastSection(true);
    _ui->FLP_Gifts->verticalHeader()->setVisible(false);
    _ui->FLP_Gifts->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->FLP_Gifts->setSelectionMode(QAbstractItemView::SingleSelection);
    _ui->FLP_Gifts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->FLP_Gifts->setContextMenuPolicy(Qt::CustomContextMenu);
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("SAV_Wondercard"));

    auto *mnu = new QMenu(this);
    auto *view = mnu->addAction(QStringLiteral("View"));
    view->setObjectName(QStringLiteral("mnuView"));
    auto *set = mnu->addAction(QStringLiteral("Set"));
    set->setObjectName(QStringLiteral("mnuSet"));
    auto *del = mnu->addAction(QStringLiteral("Delete"));
    del->setObjectName(QStringLiteral("mnuDelete"));
    auto *flagDel = new QAction(QStringLiteral("Delete"), this);
    flagDel->setObjectName(QStringLiteral("flagDel"));
    _ui->LB_Received->addAction(flagDel);
    _ui->LB_Received->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(_ui->B_Save, &QPushButton::clicked, this, &QDialog::accept);
    connect(_ui->B_Cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(_ui->B_UsedAll, &QPushButton::clicked, this, [this] { emit modifyRequested(QStringLiteral("usedAll")); });
    connect(_ui->B_UnusedAll, &QPushButton::clicked, this, [this] { emit modifyRequested(QStringLiteral("unusedAll")); });
    connect(_ui->B_Import, &QPushButton::clicked, this, [this] {
        const QString path = QFileDialog::getOpenFileName(this, windowTitle(), QString(), tr("Mystery Gift (*)"));
        if (!path.isEmpty())
            emit importRequested(path);
    });
    connect(_ui->B_Output, &QPushButton::clicked, this, [this] {
        const QString path = QFileDialog::getSaveFileName(this, windowTitle(), QString(), tr("Mystery Gift (*)"));
        if (!path.isEmpty())
            emit exportRequested(path);
    });
    connect(_ui->FLP_Gifts, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem *item) {
        if (item == nullptr)
            return;
        emit modifyRequested(QStringLiteral("view:%1").arg(item->data(Qt::UserRole).toInt()));
    });
    connect(_ui->FLP_Gifts, &QWidget::customContextMenuRequested, this, [this, mnu, view, set, del](const QPoint &pos) {
        auto *item = _ui->FLP_Gifts->itemAt(pos);
        if (item == nullptr)
            return;
        const int index = item->data(Qt::UserRole).toInt();
        view->disconnect();
        set->disconnect();
        del->disconnect();
        connect(view, &QAction::triggered, this, [this, index] { emit modifyRequested(QStringLiteral("view:%1").arg(index)); });
        connect(set, &QAction::triggered, this, [this, index] { emit modifyRequested(QStringLiteral("set:%1").arg(index)); });
        connect(del, &QAction::triggered, this, [this, index] { emit modifyRequested(QStringLiteral("delete:%1").arg(index)); });
        mnu->exec(_ui->FLP_Gifts->viewport()->mapToGlobal(pos));
    });
    connect(flagDel, &QAction::triggered, this, [this] {
        auto *item = _ui->LB_Received->currentItem();
        if (item == nullptr)
            return;
        emit modifyRequested(QStringLiteral("flag:%1").arg(item->text().toInt()));
    });
}

WondercardWindow::~WondercardWindow() = default;

void WondercardWindow::loadDocument(const QString &json)
{
    const auto root = QJsonDocument::fromJson(json.toUtf8()).object();
    _ui->RTB->setPlainText(root.value(QStringLiteral("viewedSummary")).toString());
    _ui->B_UsedAll->setVisible(root.value(QStringLiteral("showUsed")).toBool());
    _ui->B_UnusedAll->setVisible(root.value(QStringLiteral("showUsed")).toBool());
    setProperty("viewedData", root.value(QStringLiteral("viewedData")).toString());
    setProperty("viewedExt", root.value(QStringLiteral("viewedExt")).toString());

    const auto album = root.value(QStringLiteral("slots")).toArray();
    auto *table = _ui->FLP_Gifts;
    table->setRowCount(album.size());
    for (int row = 0; row < album.size(); ++row)
    {
        const auto slot = album.at(row).toObject();
        const int index = slot.value(QStringLiteral("index")).toInt();
        auto *num = new QTableWidgetItem(QString::number(index + 1));
        auto *type = new QTableWidgetItem(slot.value(QStringLiteral("type")).toString());
        auto *card = new QTableWidgetItem(slot.value(QStringLiteral("empty")).toBool()
            ? QStringLiteral("(empty)")
            : QString::number(slot.value(QStringLiteral("cardId")).toInt()).rightJustified(4, QLatin1Char('0')));
        num->setData(Qt::UserRole, index);
        type->setData(Qt::UserRole, index);
        card->setData(Qt::UserRole, index);
        table->setItem(row, 0, num);
        table->setItem(row, 1, type);
        table->setItem(row, 2, card);
    }

    _ui->LB_Received->clear();
    const auto received = root.value(QStringLiteral("received")).toArray();
    for (const auto &item : received)
        _ui->LB_Received->addItem(item.toString());
}

QString WondercardWindow::document() const
{
    QJsonArray received;
    for (int i = 0; i < _ui->LB_Received->count(); ++i)
        received.append(_ui->LB_Received->item(i)->text());
    QJsonObject root{
        {QStringLiteral("viewedData"), property("viewedData").toString()},
        {QStringLiteral("viewedExt"), property("viewedExt").toString()},
        {QStringLiteral("received"), received},
    };
    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}
