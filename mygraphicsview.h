#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>

class DiagramScene;

class MyGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    MyGraphicsView(DiagramScene *scene, QWidget* owner = nullptr);

public slots:
    void onMouseReleased();

private:
    bool eventFilter(QObject* object, QEvent* event) override;
    void wheelEvent(QWheelEvent *event) override;

    DiagramScene *m_diagramScene;

signals:
    void mouseWheelZoomed();
};

#endif // MYGRAPHICSVIEW_H
