#pragma once

#include <QByteArray>
#include <QPixmap>
#include <QWidget>

class QLabel;

class BoxPokeGrid : public QWidget
{
    Q_OBJECT
public:
    explicit BoxPokeGrid(QWidget *parent = nullptr);
    void setWallpaperPng(const QByteArray &png);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap _wallpaper;
};

void fillBoxAndPartyChrome(QWidget *tabBox, QWidget *tabParty);
void applySlotPng(QLabel *slot, const QByteArray &png);
