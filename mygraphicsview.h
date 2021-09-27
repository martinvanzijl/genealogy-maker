#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>

class MyGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    MyGraphicsView(QGraphicsScene* scene, QWidget* owner = nullptr);

public slots:
    void onMouseReleased();

private:
    bool eventFilter(QObject* object, QEvent* event) override;
};

#endif // MYGRAPHICSVIEW_H
