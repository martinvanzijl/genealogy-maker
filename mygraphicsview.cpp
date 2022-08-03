#include "mygraphicsview.h"

// -----------------------------------------------------------------------------
// Based on https://gist.github.com/Legor/a00760b6d7af32c01357fb7ff76ad86a
// -----------------------------------------------------------------------------

#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

#include "diagramscene.h"

MyGraphicsView::MyGraphicsView(DiagramScene *scene, QWidget *owner) :
    QGraphicsView(scene, owner)
{
    installEventFilter(this);

    m_diagramScene = scene;
    m_minScale = 0.10;  // 10%
    m_maxScale = 2.50;  // 250%
}

void MyGraphicsView::onMouseReleased()
{
    // This is a workaround, since the event filter does not catch
    // mouse-release events. Instead we use a signal from the
    // scene object.
    setDragMode(QGraphicsView::DragMode::NoDrag);
}

bool MyGraphicsView::eventFilter(QObject *object, QEvent *event) {

    Q_UNUSED(object);

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

        // Use the middle mouse button to scroll
        if (mouse_event->button() == Qt::MiddleButton)
        {
            // Temporarily enable dragging mode
            setDragMode(QGraphicsView::DragMode::ScrollHandDrag);

            // Emit a left mouse click (the default button for the drag mode)
            QMouseEvent* pressEvent = new QMouseEvent(QEvent::GraphicsSceneMousePress,
                                                      mouse_event->pos(), Qt::MouseButton::LeftButton,
                                                      Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
            mousePressEvent(pressEvent);
        }
        else if (mouse_event->button() == Qt::LeftButton) {
            // Use the middle mouse button for rectangle-select.
            if (!m_diagramScene->isDrawingArrow()) {
                setDragMode(QGraphicsView::DragMode::RubberBandDrag);

                // Emit a left mouse click again, now that the mode is set.
                QMouseEvent* pressEvent = new QMouseEvent(QEvent::GraphicsSceneMousePress,
                                                          mouse_event->pos(), Qt::MouseButton::LeftButton,
                                                          Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
                mousePressEvent(pressEvent);
            }
        }
    }

    // Let the default handler take effect.
    return false;
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    // Use Ctrl+Mouse Wheel to zoom.
    if (event->modifiers() & Qt::ControlModifier) {
        // zoom
        const ViewportAnchor anchor = transformationAnchor();
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        int angle = event->angleDelta().y();
        qreal factor;
        if (angle > 0) {
            factor = 1.1;
        } else {
            factor = 0.9;
        }
        zoomBy(factor);
        setTransformationAnchor(anchor);
        emit mouseWheelZoomed();
    } else {
        // normal
        QGraphicsView::wheelEvent(event);
    }
}

void MyGraphicsView::zoomBy(qreal factor)
{
    // Ensure zoom limits are adhered to.
    double currentScale = matrix().m11();
    double newScale = currentScale * factor;

    if (newScale > m_maxScale)
    {
        factor = m_maxScale / currentScale;
    }
    else if (newScale < m_minScale)
    {
        factor = m_minScale / currentScale;
    }

    // Scale the view.
    scale(factor, factor);
}
