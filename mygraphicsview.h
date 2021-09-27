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
    void wheelEvent(QWheelEvent *event) override;

signals:
    void mouseWheelZoomed();
};

#endif // MYGRAPHICSVIEW_H
