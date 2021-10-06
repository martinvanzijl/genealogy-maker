#ifndef DIALOGHELP_H
#define DIALOGHELP_H

#include <QDialog>

namespace Ui {
class DialogHelp;
}

class DialogHelp : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHelp(QWidget *parent = 0);
    ~DialogHelp();

private slots:
    void on_pushButtonClose_clicked();

private:
    Ui::DialogHelp *ui;
    void loadHelpFile();
};

#endif // DIALOGHELP_H
