#ifndef TWODIMENSIONALSCENE_H
#define TWODIMENSIONALSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <QStringList>
#include <QList>
#include <stdint.h>

class TwoDimensionalScene
{
public:
    TwoDimensionalScene();

    //! Setup the view to contain our scene
    virtual void setupView(QGraphicsView* view);

    //! Clear the display
    void clear(void) {myScene.clear();}

    //! Set the size of the scene from the view rect.
    void setSize(QRect rect) {size = rect;}

protected:

    //! location is with respect to the rect center
    QPointF rectOffsetForCenter(const QRectF rect) const;

    //! location is with respect to the center of the top side.
    QPointF rectOffsetForTopCenter(const QRectF rect) const;

    //! location is with respect to the top left corner.
    QPointF rectOffsetForTopLeft(void) const;

    //! location is with respect to the top right corner.
    QPointF rectOffsetForTopRight(const QRectF rect) const;

    //! location is with respect to the center of the bottom side.
    QPointF rectOffsetForBottomCenter(const QRectF rect) const;

    //! location is with respect to the bottom left corner
    QPointF rectOffsetForBottomLeft(const QRectF rect) const;

    //! location is with respect to the bottom right tcorner.
    QPointF rectOffsetForBottomRight(const QRectF rect) const;

    //! the location is with respect to the center of the left side
    QPointF rectOffsetForLeftCenter(const QRectF rect) const;

    //! the location is with respect to the cente of the right side.
    QPointF rectOffsetForRightCenter(const QRectF rect) const;

    //! Draw the gridlines used for a two dimensional plot
    void drawGrid(double xOrg, double yOrg, double xScale, double xMin, double xMax, uint32_t xTicks, double yScale, double yMin, double yMax, uint32_t yTicks, QColor color = Qt::gray);

    //! Draw the X axis of the plot
    void DrawHorizontalAxis(double xOrg, double yOrg, double dXScale, double dXMin, double dXMax, uint32_t XTicks, const QString& label, int precision, QColor color, bool high = false);

    //! Draw a vertical axis of the plot
    void DrawVerticalAxis(double xOrg, double yOrg, double dYScale, double dYMin, double dYMax, uint32_t YTicks, const QString& label, int precision, QColor color, bool left = true, bool firstPoint = false);

    //! Draw a vertical axis of the plot, with multiple labels
    void DrawVerticalAxis(double xOrg, double yOrg, double dYScale, double dYMin, double dYMax, uint32_t YTicks, const QStringList & labels, int precision, QList<QColor> colors, bool left = true, bool firstPoint = false);

    //! Compute tick mark information for an axis given range of data, adjustable tick count
    static int32_t tickMarks(double minValue, double maxValue, uint32_t* numTicks, double* startValue, double* endValue);

    //! Compute tick mark information for an axis given range of data
    static int32_t tickMarks(double minValue, double maxValue, uint32_t numTicks, double* startValue, double* endValue);

    //! Compute tick mark sapcing
    static int32_t tickMarkSpacing(uint32_t NumTicks, double dMaxValue, double* spacing);

protected:
    QGraphicsScene myScene;     //!< The scene we render into
    QRect size;                 //!< The size of the view/scene
};

#endif // TWODIMENSIONALSCENE_H
