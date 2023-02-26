#include "dialogfind.h"
#include "ui_dialogfind.h"

#include <QSettings>
#include <QTimer>

DialogFind::DialogFind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFind)
{
    ui->setupUi(this);

    // Store hint.
    m_hint = ui->labelStatus->text();

    // Load preferences.
    updateGuiFromPreferences();

    // Create timer.
    m_transparencyTimer = new QTimer(this);
    connect(m_transparencyTimer, SIGNAL(timeout()), this, SLOT(setFullOpacity()));
}

DialogFind::~DialogFind()
{
    delete ui;
}

void DialogFind::beforeShow()
{
    // Show hint.
    setStatus(m_hint);

    // Set focus.
    ui->lineEditText->clear();
    ui->lineEditText->setFocus();
}

void DialogFind::setStatus(const QString &text)
{
    ui->labelStatus->setText(text);
}

void DialogFind::updateGuiFromPreferences()
{
    // Load transparency setting.
    QSettings settings;

    bool value = settings.value("interface/findWindowTransparent", false).toBool();
//    if (value) {
//        setWindowOpacity(0.8);
//    }
//    else {
//        setWindowOpacity(1.0);
//    }

    m_transparentDuringHighlight = value;
}

void DialogFind::onFound()
{
    if (m_transparentDuringHighlight) {
        // Make partly transparent while highlighting found item.
        setWindowOpacity(0.8);
//            QTimer::singleShot(1000, this, SLOT(setFullOpacity()));
        m_transparencyTimer->start(1000);
    }
}

void DialogFind::on_pushButtonClose_clicked()
{
    close();
}

void DialogFind::on_pushButtonFind_clicked()
{
    auto text = ui->lineEditText->text();
    if (!text.isEmpty())
    {
        emit search(text);
    }
}

void DialogFind::setFullOpacity()
{
    setWindowOpacity(1.0);
}

void DialogFind::on_lineEditText_textChanged(const QString &newText)
{
    Q_UNUSED(newText);

    QString labelText = ui->labelStatus->text();

    if (labelText == "Person found." || labelText == "Person not found.") {
        ui->labelStatus->clear();
    }
}
