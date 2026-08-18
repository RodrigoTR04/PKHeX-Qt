#pragma once

#include <QStringList>

class QWidget;

void fillSavToolButtons(QWidget *flow);
void applySavToolVisibility(QWidget *window, const QStringList &visible);
