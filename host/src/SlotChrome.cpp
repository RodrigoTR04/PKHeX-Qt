#include "SlotChrome.h"

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
constexpr int kSpriteW = 68;
constexpr int kSpriteH = 56;
constexpr int kBorder = 1;

QString slotName(int row, int column)
{
    return QStringLiteral("Pokémon Grid Row %1 Column %2")
        .arg(row, 2, 10, QLatin1Char('0'))
        .arg(column, 2, 10, QLatin1Char('0'));
}

QLabel *makeSlot(QWidget *parent, int row, int column)
{
    auto *slot = new QLabel(parent);
    slot->setObjectName(slotName(row, column));
    slot->setFixedSize(kSpriteW + 2 * kBorder, kSpriteH + 2 * kBorder);
    slot->setAlignment(Qt::AlignCenter);
    slot->setScaledContents(false);
    slot->setFrameStyle(QFrame::Box);
    slot->setLineWidth(kBorder);
    return slot;
}

void fillGrid(QWidget *grid, int columns, int rows, const QString &keyPrefix)
{
    auto *layout = new QGridLayout(grid);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(1);
    int index = 0;
    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            auto *slot = makeSlot(grid, row, column);
            slot->setProperty("slotKey", keyPrefix + QString::number(index));
            layout->addWidget(slot, row, column);
            ++index;
        }
    }
}

QWidget *makeBoxEditor(QWidget *tab)
{
    auto *box = new QWidget(tab);
    box->setObjectName(QStringLiteral("Box"));
    auto *layout = new QVBoxLayout(box);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    auto *header = new QWidget(box);
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    auto *left = new QPushButton(header);
    left->setObjectName(QStringLiteral("B_BoxLeft"));
    left->setFixedSize(32, 24);
    left->setText(QStringLiteral("<"));

    auto *select = new QComboBox(header);
    select->setObjectName(QStringLiteral("CB_BoxSelect"));
    select->setMinimumWidth(128);

    auto *right = new QPushButton(header);
    right->setObjectName(QStringLiteral("B_BoxRight"));
    right->setFixedSize(32, 24);
    right->setText(QStringLiteral(">"));

    headerLayout->addWidget(left);
    headerLayout->addWidget(select, 1);
    headerLayout->addWidget(right);

    auto *grid = new BoxPokeGrid(box);
    grid->setObjectName(QStringLiteral("BoxPokeGrid"));
    fillGrid(grid, 6, 5, QStringLiteral("box:0:"));

    layout->addWidget(header);
    layout->addWidget(grid, 1);
    return box;
}

QWidget *makePartyEditor(QWidget *tab)
{
    auto *party = new QWidget(tab);
    party->setObjectName(QStringLiteral("SL_Party"));
    auto *layout = new QVBoxLayout(party);
    layout->setContentsMargins(0, 0, 0, 0);

    auto *grid = new QWidget(party);
    grid->setObjectName(QStringLiteral("PartyPokeGrid"));
    fillGrid(grid, 2, 3, QStringLiteral("party:"));

    layout->addWidget(grid, 1);
    return party;
}
}

BoxPokeGrid::BoxPokeGrid(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
}

void BoxPokeGrid::setWallpaperPng(const QByteArray &png)
{
    _wallpaper = QPixmap();
    _wallpaper.loadFromData(png);
    update();
}

void BoxPokeGrid::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    if (!_wallpaper.isNull())
    {
        const qreal dpr = devicePixelRatioF();
        QPixmap scaled = _wallpaper.scaled(
            size() * dpr,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation);
        scaled.setDevicePixelRatio(dpr);
        painter.drawPixmap(rect(), scaled);
    }
    QWidget::paintEvent(event);
}

void applySlotPng(QLabel *slot, const QByteArray &png)
{
    if (slot == nullptr)
        return;
    if (png.isEmpty())
    {
        slot->clear();
        return;
    }

    QPixmap pm;
    if (!pm.loadFromData(png))
    {
        slot->clear();
        return;
    }

    const qreal dpr = slot->devicePixelRatioF();
    QPixmap scaled = pm.scaled(
        QSize(kSpriteW, kSpriteH) * dpr,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);
    scaled.setDevicePixelRatio(dpr);
    slot->setPixmap(scaled);
}

void fillBoxAndPartyChrome(QWidget *tabBox, QWidget *tabParty)
{
    if (tabBox != nullptr)
    {
        auto *layout = new QVBoxLayout(tabBox);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->addWidget(makeBoxEditor(tabBox));
    }

    if (tabParty != nullptr)
    {
        auto *layout = new QVBoxLayout(tabParty);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->addWidget(makePartyEditor(tabParty));
    }
}
