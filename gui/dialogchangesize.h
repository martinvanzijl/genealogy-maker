#ifndef DIALOGCHANGESIZE_H
#define DIALOGCHANGESIZE_H

#include <QDialog>

namespace Ui {
class DialogChangeSize;
}

class DialogChangeSize : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChangeSize(QWidget *parent = 0);
    ~DialogChangeSize();

    void setCurrentSize(const QSizeF &size);
    int getNewWidth() const;
    int getNewHeight() const;

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonOK_clicked();

private:
    Ui::DialogChangeSize *ui;
};

#endif // DIALOGCHANGESIZE_H
