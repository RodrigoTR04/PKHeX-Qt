#include "BatchWindow.h"

#include "LangCatalog.h"
#include "ComboChrome.h"
#include "ui_BatchEditor.h"

#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>

BatchWindow::BatchWindow(QWidget *parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::BatchEditor>())
{
    _ui->setupUi(this);
    configureComboBoxes(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    _ui->TB_Folder->setVisible(false);
    connect(_ui->B_Go, &QPushButton::clicked, this, &BatchWindow::runRequested);
    connect(_ui->B_Add, &QPushButton::clicked, this, [this] {
        if (_ui->CB_Property->currentText().isEmpty())
            return;
        appendInstruction(QLatin1Char('.') + _ui->CB_Property->currentText() + QLatin1Char('='));
    });
    connect(_ui->RB_Path, &QRadioButton::clicked, this, [this] {
        const QString path = QFileDialog::getExistingDirectory(this, tr("Folder"));
        if (path.isEmpty())
        {
            _ui->RB_Boxes->setChecked(true);
            return;
        }
        _ui->TB_Folder->setText(path);
        _ui->TB_Folder->setVisible(true);
    });
    connect(_ui->RB_Boxes, &QRadioButton::clicked, this, [this] { _ui->TB_Folder->setVisible(false); });
    connect(_ui->RB_Party, &QRadioButton::clicked, this, [this] { _ui->TB_Folder->setVisible(false); });
    LangCatalog catalog;
    catalog.loadFromFile(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("lang/lang_en.txt")));
    catalog.apply(this, QStringLiteral("BatchEditor"));
}

BatchWindow::~BatchWindow() = default;

void BatchWindow::setProperties(const QString &names)
{
    _ui->CB_Property->clear();
    const auto lines = names.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    _ui->CB_Property->addItems(lines);
}

QString BatchWindow::instructions() const
{
    return _ui->RTB_Instructions->toPlainText();
}

QString BatchWindow::scope() const
{
    if (_ui->RB_Party->isChecked())
        return QStringLiteral("party");
    if (_ui->RB_Path->isChecked())
        return QStringLiteral("folder");
    return QStringLiteral("boxes");
}

void BatchWindow::appendInstruction(const QString &line)
{
    auto *box = _ui->RTB_Instructions;
    QString text = box->toPlainText();
    if (!text.isEmpty() && !text.endsWith(QLatin1Char('\n')))
        text.append(QLatin1Char('\n'));
    text.append(line);
    box->setPlainText(text);
}
