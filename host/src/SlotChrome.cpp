#include "SlotChrome.h"

#include "ComboChrome.h"

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QVBoxLayout>

class SlotLabel : public QLabel
{
public:
    explicit SlotLabel(QWidget *parent)
        : QLabel(parent)
    {
        setAlignment(Qt::AlignCenter);
        setScaledContents(false);
        setFrameStyle(QFrame::NoFrame);
        setLineWidth(0);
        setAutoFillBackground(false);
        setAttribute(Qt::WA_TranslucentBackground);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumSize(8, 8);
        setStyleSheet(QStringLiteral("background: transparent; border: none;"));
    }

    void setSourcePng(const QByteArray &png)
    {
        _source = QPixmap();
        if (!png.isEmpty())
            _source.loadFromData(png);
        relayoutPixmap();
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QLabel::resizeEvent(event);
        relayoutPixmap();
    }

private:
    void relayoutPixmap()
    {
        if (_source.isNull())
        {
            clear();
            return;
        }
        const qreal dpr = devicePixelRatioF();
        const QSize target = size() * dpr;
        if (target.width() < 1 || target.height() < 1)
            return;
        QPixmap scaled = _source.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        scaled.setDevicePixelRatio(dpr);
        setPixmap(scaled);
    }

    QPixmap _source;
};

namespace
{
QString slotName(int row, int column)
{
    return QStringLiteral("Pokémon Grid Row %1 Column %2")
        .arg(row, 2, 10, QLatin1Char('0'))
        .arg(column, 2, 10, QLatin1Char('0'));
}

QLabel *makeSlot(QWidget *parent, int row, int column)
{
    auto *slot = new SlotLabel(parent);
    slot->setObjectName(slotName(row, column));
    return slot;
}

void fillGrid(QWidget *grid, int columns, int rows, const QString &keyPrefix)
{
    auto *layout = new QGridLayout(grid);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    int index = 0;
    for (int row = 0; row < rows; ++row)
    {
        layout->setRowStretch(row, 1);
        for (int column = 0; column < columns; ++column)
        {
            layout->setColumnStretch(column, 1);
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
    layout->setSpacing(4);

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
    configureComboBox(select);

    auto *right = new QPushButton(header);
    right->setObjectName(QStringLiteral("B_BoxRight"));
    right->setFixedSize(32, 24);
    right->setText(QStringLiteral(">"));

    headerLayout->addWidget(left);
    headerLayout->addWidget(select, 1);
    headerLayout->addWidget(right);

    auto *grid = new BoxPokeGrid(box);
    grid->setObjectName(QStringLiteral("BoxPokeGrid"));
    grid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    fillGrid(grid, 6, 5, QStringLiteral("box:0:"));

    layout->addWidget(header, 0);
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
    grid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    fillGrid(grid, 2, 3, QStringLiteral("party:"));

    layout->addWidget(grid, 1);
    return party;
}
}

BoxPokeGrid::BoxPokeGrid(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
    if (auto *labeled = dynamic_cast<SlotLabel *>(slot))
    {
        labeled->setSourcePng(png);
        return;
    }
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
    QPixmap scaled = pm.scaled(slot->size() * dpr, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaled.setDevicePixelRatio(dpr);
    slot->setPixmap(scaled);
}

void fillBoxAndPartyChrome(QWidget *tabBox, QWidget *tabParty)
{
    if (tabBox != nullptr)
    {
        auto *layout = new QVBoxLayout(tabBox);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->addWidget(makeBoxEditor(tabBox), 1);
    }

    if (tabParty != nullptr)
    {
        auto *layout = new QVBoxLayout(tabParty);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->addWidget(makePartyEditor(tabParty), 1);
    }
}
