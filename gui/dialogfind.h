#ifndef DIALOGFIND_H
#define DIALOGFIND_H

#include <QDialog>

class QTimer;

namespace Ui {
class DialogFind;
}

class DialogFind : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFind(QWidget *parent = 0);
    ~DialogFind();

    void beforeShow();
    void setStatus(const QString &text);
    void updateGuiFromPreferences();

signals:
    void search(const QString &text);

public slots:
    void onFound();

private slots:
    void on_pushButtonClose_clicked();
    void on_pushButtonFind_clicked();
    void setFullOpacity();

    void on_lineEditText_textChanged(const QString &newText);

private:
    Ui::DialogFind *ui;

    QString m_hint;
    bool m_transparentDuringHighlight;
    QTimer *m_transparencyTimer;
};

#endif // DIALOGFIND_H
