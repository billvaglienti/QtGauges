#ifndef GRAPHICSVIEWNOSCROLL_H
#define GRAPHICSVIEWNOSCROLL_H

#include <QGraphicsView>

/*!
 * In order to prevent the GraphicsView from scrolling the scene in response to
 * the mouse inputs we override the view and "throw away" the scroll function
 * by overriding it and doing nothing.  If using the form editor to place your
 * GraphicsView on the form, then you can "promote" it right there to be a
 * GraphicsViewNoScroll.
 */
class QGraphicsViewNoScroll : public QGraphicsView
{
public:

    inline QGraphicsViewNoScroll(QWidget* parent)
        : QGraphicsView(parent)
    {}

    inline QGraphicsViewNoScroll(QGraphicsScene* scene, QWidget* parent)
        : QGraphicsView(scene, parent)
    {}

    inline void scrollContentsBy(int, int)
    {
        //don't do anything hah! this prevents user mouse scrolling
    }
};

#endif // GRAPHICSVIEWNOSCROLL_H
