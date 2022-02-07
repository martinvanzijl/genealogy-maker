#ifndef DIALOGFIND_H
#define DIALOGFIND_H

#include <QDialog>

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

signals:
    void search(const QString &text);

private slots:
    void on_pushButtonClose_clicked();
    void on_pushButtonFind_clicked();

private:
    Ui::DialogFind *ui;

    QString m_hint;
};

#endif // DIALOGFIND_H
