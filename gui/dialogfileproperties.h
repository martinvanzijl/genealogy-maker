#ifndef DIALOGFILEPROPERTIES_H
#define DIALOGFILEPROPERTIES_H

#include <QDialog>

class DiagramScene;

namespace Ui {
class DialogFileProperties;
}

class DialogFileProperties : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFileProperties(QWidget *parent = 0);
    ~DialogFileProperties();

    void setDiagram(DiagramScene * &scene);
    void setFile(const QString &filePath);

private slots:
    void on_pushButtonClose_clicked();

    void clear();

private:
    Ui::DialogFileProperties *ui;
};

#endif // DIALOGFILEPROPERTIES_H
