#pragma once

#include <QDialog>
#include <QString>
#include <memory>

namespace Ui
{
class SAV_Pokedex5;
}

class PokedexWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PokedexWindow(QWidget *parent = nullptr);
    ~PokedexWindow() override;

    void loadDocument(const QString &json);
    QString document() const;

signals:
    void modifyRequested(const QString &action);

private:
    void buildMenus();
    void applyCapabilities();

    std::unique_ptr<Ui::SAV_Pokedex5> _ui;
    QString _langForm;
};
