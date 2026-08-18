#pragma once

#include <QStringList>

class QWidget;
class QLabel;

void fillPkmTabs(QWidget *tabMain, QWidget *tabMet, QWidget *tabStats, QWidget *tabMoves, QWidget *tabCosmetic, QWidget *tabOt);
void applyInGameFont(QWidget *window);
void setLegalityIcon(QLabel *legal, bool valid);
void applyPkmChrome(QWidget *window, int format, const QStringList &visible);
