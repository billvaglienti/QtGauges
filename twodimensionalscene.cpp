#include "twodimensionalscene.h"
#include <math.h>
#include <QGraphicsTextItem>

TwoDimensionalScene::TwoDimensionalScene()
{
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the rect center
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForCenter(const QRectF rect) const
{
    return QPointF(-rect.width()/2, -rect.height()/2);
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the
 * center of the top side.
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForTopCenter(const QRectF rect) const
{
    return QPointF(-rect.width()/2, 0);
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the top
 * left corner.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForTopLeft(void) const
{
    return QPointF(0, 0);
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the
 * top right corner.
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForTopRight(const QRectF rect) const
{
    return QPointF(-rect.width(), 0);
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the
 * center of the bottom side.
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForBottomCenter(const QRectF rect) const
{
    return QPointF(-rect.width()/2, -rect.height());
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the bottom
 * left corner.
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForBottomLeft(const QRectF rect) const
{
    return QPointF(0, -rect.height());
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the
 * bottom right corner.
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForBottomRight(const QRectF rect) const
{
    return QPointF(-rect.width(), -rect.height());
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the center
 * of the left side.
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForLeftCenter(const QRectF rect) const
{
    return QPointF(0, -rect.height()/2);
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the center
 * of the right side.
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF TwoDimensionalScene::rectOffsetForRightCenter(const QRectF rect) const
{
    return QPointF(-rect.width(), -rect.height()/2);
}

void TwoDimensionalScene::setupView(QGraphicsView* view)
{
    // Make the view background transparent
    view->setStyleSheet("background: transparent");

    // turn on anti-aliasing
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    // No dragging
    view->setDragMode(QGraphicsView::NoDrag);

    // Set the size of our view
    size = view->rect();

    // No items in the scene
    myScene.clear();

    // Fix the size and location of the scene to match the view
    // myScene.setSceneRect(0, 0, size.width(), size.height());

    // In case the developer forgets on the form editor...
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Set this scene in the view
    view->setScene(&myScene);

}// setupView


/*!
 * Draw the gridlines used for a two dimensional plot
 * \param xOrg is the origin x coordinate for the lower left corner
 * \param yOrg is the origin y coordinate for the lower left corner
 * \param xScale is the scale factor between x and pixels
 * \param xMin is the minimum value on the x axis
 * \param xMax is the maximum value on the x axis
 * \param xTicks is the number of vertical grid lines
 * \param yScale is the scale factor between x and pixels
 * \param yMin is the minimum value on the y axis
 * \param yMax is the maximum value on the y axis
 * \param yTicks is the number of horiztonal grid lines
 * \param color is the color of the grid lines
 */
void TwoDimensionalScene::drawGrid(double xOrg, double yOrg, double xScale, double xMin, double xMax, uint32_t xTicks, double yScale, double yMin, double yMax, uint32_t yTicks, QColor color)
{
    uint32_t i;
    double X, Y;

    // Draw grid lines
    QPainterPath path;
    X = xOrg + xScale*(xMax - xMin);
    for(i = 0; i <= yTicks; i++)
    {
        Y = yOrg + i*yScale*(yMax - yMin)/(yTicks);
        path.moveTo(xOrg, Y);
        path.lineTo(X,    Y);

    }// for all horizontal grid lines

    Y = yOrg + yScale*(yMax - yMin);
    for(i = 0; i <= xTicks; i++)
    {
        X = xOrg + i*xScale*(xMax - xMin)/(xTicks);
        path.moveTo(X, yOrg);
        path.lineTo(X, Y);

    }// for all vertical grid lines

    // Grid lines are in grey
    myScene.addPath(path, color);

}// TwoDimensionalScene::drawGrid


/*!
 * Draw the horizontal axis of the plot
 * \param xOrg is the x pixel position of the origin
 * \param yOrg is the y pixel position of the origin
 * \param dXScale is scale between x pixels and x values
 * \param dXMin is the minimum x value of the axis
 * \param dXMax is the maximum x value of the axis
 * \param XTicks is the number of tick marks of the axis
 * \param label is the axis text label
 * \param precision is the number of decimal places to use for the tick mark labels
 * \param color is the color to drawy in
 * \param high should be true if yOrg is at the top of the grid, else yOrg at the bottom
 */
void TwoDimensionalScene::DrawHorizontalAxis(double xOrg, double yOrg, double dXScale, double dXMin, double dXMax, uint32_t XTicks, const QString& label, int precision, QColor color, bool high)
{
    double X;
    uint32_t i;
    double dTickSpace;
    QPointF pos;
    QPainterPath path;
    double tickLength = fabs((dXMax-dXMin)*.01*dXScale);

    // The label, centered above the axis
    pos.setX(xOrg + dXScale*(dXMax - dXMin)/2.0);
    pos.setY(yOrg);
    QGraphicsTextItem* item = myScene.addText(label);
    item->setDefaultTextColor(color);

    // Label is above or below the line according to if this is a high or low line
    if(high)
        item->setPos(pos + rectOffsetForTopCenter(item->boundingRect()));
    else
        item->setPos(pos + rectOffsetForBottomCenter(item->boundingRect()));

    // X-axis start point
    pos.setX(xOrg);
    pos.setY(yOrg);
    path.moveTo(pos);

    // X-axis end point
    pos.setX(xOrg + dXScale*(dXMax-dXMin));
    path.lineTo(pos);

    // Space between major tick marks
    dTickSpace = (dXMax - dXMin)/XTicks;

    // Now draw the X axis tick marks and labels
    for(i = 0; i <= XTicks; i++)
    {
        X = xOrg + dXScale*dTickSpace*i;

        // Tick mark text
        pos.setX(X);
        pos.setY(yOrg);
        item = myScene.addText(QString("%1").arg(dXMin + dTickSpace*i, 0, 'f', precision));
        item->setDefaultTextColor(color);

        // tick mark
        path.moveTo(X,yOrg);

        // Label is above or below the line according to if this is a high or low line
        if(high)
        {
            item->setPos(pos + rectOffsetForBottomCenter(item->boundingRect()));
            path.lineTo(X,yOrg+tickLength);
        }
        else
        {

            item->setPos(pos + rectOffsetForTopCenter(item->boundingRect()));
            path.lineTo(X,yOrg-tickLength);
        }

    }// for all the ticks to be drawn
    myScene.addPath(path, QPen(color));

}// TwoDimensionalScene::DrawHorizontalAxis


/*!
 * Draw a vertical axis of the plot
 * \param xOrg is the x pixel position of the origin
 * \param yOrg is the y pixel position of the origin
 * \param dXScale is scale between x pixels and x values
 * \param dXMin is the minimum x value of the axis
 * \param dXMax is the maximum x value of the axis
 * \param XTicks is the number of tick marks of the axis
 * \param label is the axis text label
 * \param precision is the number of decimal places to use for the tick mark labels
 * \param color is the color to drawy in
 * \param left should be true if xOrg is at the left of the grid, else xOrg at the right
 * \param firstPoint should be true to draw the first (lowest) tick mark and its lable, else it is not drawn
 */
void TwoDimensionalScene::DrawVerticalAxis(double xOrg, double yOrg, double dYScale, double dYMin, double dYMax, uint32_t YTicks, const QString & label, int precision, QColor color, bool left, bool firstPoint)
{
    QStringList labels;
    QList<QColor> colors;

    labels.append(label);
    colors.append(color);
    DrawVerticalAxis(xOrg, yOrg, dYScale, dYMin, dYMax, YTicks, labels, precision, colors, left, firstPoint);
}

/*!
 * Draw a vertical axis of the plot
 * \param xOrg is the x pixel position of the origin
 * \param yOrg is the y pixel position of the origin
 * \param dXScale is scale between x pixels and x values
 * \param dXMin is the minimum x value of the axis
 * \param dXMax is the maximum x value of the axis
 * \param XTicks is the number of tick marks of the axis
 * \param labels is the list of axis labels
 * \param precision is the number of decimal places to use for the tick mark labels
 * \param colors is the list of axis label colors.  The first color is used for the tick mark labels
 * \param left should be true if xOrg is at the left of the grid, else xOrg at the right
 * \param firstPoint should be true to draw the first (lowest) tick mark and its lable, else it is not drawn
 */
void TwoDimensionalScene::DrawVerticalAxis(double xOrg, double yOrg, double dYScale, double dYMin, double dYMax, uint32_t YTicks, const QStringList & labels, int precision, QList<QColor> colors, bool left, bool firstPoint)
{
    double Y;
    int32_t i;
    double dTickSpace;
    QPainterPath path;
    QGraphicsTextItem* item;
    QRectF rect;
    QList<QGraphicsTextItem*> itemList;
    QPointF pos;
    double tickLength = fabs((dYMax-dYMin)*.01*dYScale);
    double center = yOrg + dYScale*(dYMax-dYMin)/2.0;

    // The label centered vertically
    pos.setY(center);

    // Adjus left or right based on tick length
    if(left)
        pos.setX(xOrg + tickLength);
    else
        pos.setX(xOrg - tickLength);

    for(i = 0; i < labels.size(); i++)
    {
        // The label
        item = myScene.addText(labels[i]);

        // Set the text color which could be different for each sucessive label
        if(i < colors.size())
            item->setDefaultTextColor(colors[i]);

        // Adjust position for rect center
        rect = item->boundingRect();
        if(left)
            item->setPos(pos + rectOffsetForLeftCenter(rect));
        else
            item->setPos(pos + rectOffsetForRightCenter(rect));

        // Keep our list of items
        itemList.push_back(item);

        // Adjust vertical position for next label output, place below the previous one.
        //   This screws up the vertical centering, but we'll fix that below
        pos.setY(pos.y() + rect.height());

    }// for all labels

    // Now vertically center the list of items
    if(itemList.size() > 1)
    {
        // This item is physically on top of the list
        item = itemList.first();
        QPointF top = item->pos();

        // The "pos" is the bottom left corner of the bounding rectangle.  We need to account for the height as well
        top.setY(top.y() - item->boundingRect().height());

        // This item is physically on the bottom of the list
        item = itemList.last();
        QPointF bottom = item->pos();

        double average = (top.y() + bottom.y())/2.0;

        // we want to shift all the items up so that top and bottom are centered around the vertical center
        double shift = average - center;

        // Shift all items
        for(i = 0; i < itemList.size(); i++)
        {
            item = itemList[i];
            pos = item->pos();
            pos.setY(pos.y() + shift);
            item->setPos(pos);

        }// for all items

    }// If more than one text item


    // Compute the 'average' color
    QColor color;
    if(colors.size() > 1)
    {
        int32_t red = 0, green = 0, blue = 0;
        for(i = 0; i < colors.size(); i++)
        {
            red += colors[i].red();
            green += colors[i].green();
            blue += colors[i].blue();
        }
        red /= colors.size();
        green /= colors.size();
        blue /= colors.size();

        color = QColor(red, green, blue);
    }
    else
        color = colors[0];

    // Y-axis start point
    path.moveTo(xOrg,yOrg);

    // Y-axis end point
    Y = yOrg + dYScale*(dYMax-dYMin);
    path.lineTo(xOrg,Y);

    // Space between major ticks amrks
    dTickSpace = (dYMax - dYMin)/YTicks;

    //  Now draw the Y axis tick marks and lables
    if(firstPoint)
        i = 0;
    else
        i = 1;

    for(; i <= (int32_t)YTicks; i++)
    {
        Y = yOrg + dYScale*dTickSpace*i;

        // Tick mark text
        pos.setX(xOrg);
        pos.setY(Y);
        item = myScene.addText(QString("%1").arg(dYMin + dTickSpace*i, 0, 'f', precision));
        item->setDefaultTextColor(color);

        path.moveTo(xOrg,Y);

        if(left)
        {
            item->setPos(pos + rectOffsetForRightCenter(item->boundingRect()));
            path.lineTo(xOrg + tickLength, Y);
        }
        else
        {
            item->setPos(pos + rectOffsetForLeftCenter(item->boundingRect()));
            path.lineTo(xOrg - tickLength, Y);
        }

    }// for all ticks to be drawn
    myScene.addPath(path, QPen(color));

}// TwoDimensionalScene::DrawVerticalAxis


/*!
 * Compute tick mark information for an axis given range of data
 * \param minValue is the minimum value in the axis data set
 * \param maxValue is the maximum value in the axis data set
 * \param numTicks is the suggested number of tick marks to include, and will be updated with the correct number
 * \param startValue receives the starting value for the axis
 * \param endValue receives the ending value for the axis
 * \return the number of decimal digits after the point needed to display the full resolution of the tick mark spacing
 */
int32_t TwoDimensionalScene::tickMarks(double minValue, double maxValue, uint32_t* numTicks, double* startValue, double* endValue)
{
    // First get fixed tick mark result
    int32_t place = tickMarks(minValue, maxValue, *numTicks, startValue, endValue);
    double spacing = (*endValue - *startValue)/(*numTicks);

    // Reduce number of tick marks as needed
    uint32_t i = 0;

    if(minValue < maxValue)
    {
        while((i*spacing + *startValue) < maxValue)
            i++;
    }
    else
    {
        while((i*spacing + *startValue) > maxValue)
            i++;
    }

    *endValue = i*spacing + *startValue;
    *numTicks = i;

    return place;
}


/*!
 * Compute tick mark information for an axis given range of data
 * \param minValue is the minimum value in the axis data set
 * \param maxValue is the maximum value in the axis data set
 * \param numTicks is the number of tick marks to include
 * \param startValue receives the starting value for the axis
 * \param endValue receives the ending value for the axis
 * \return the number of decimal digits after the point needed to display the full resolution of the tick mark spacing
 */
int32_t TwoDimensionalScene::tickMarks(double minValue, double maxValue, uint32_t numTicks, double* startValue, double* endValue)
{
    // Find the spacing from the range
    double spacing;

    int32_t place = tickMarkSpacing(numTicks, maxValue - minValue, &spacing);

    // We can only tolerate so much spacing
    if(spacing < 0.000001)
    {
        spacing = 0.1;
        place = 1;
    }

    (*startValue) = minValue - fmod(minValue, spacing);

    /*
    // Find the beginning point
    if(minValue < 0.0)
    {
        *startValue = 0.0;
        while(*startValue > minValue)
            *startValue -= spacing; // back down till we are less than first value

    }// if the start is less than zero
    else
    {
        *startValue = 0.0;
        while(*startValue <= minValue)
            *startValue += spacing; // creep up till we are more than first value

        *startValue -= spacing; // start  just before first value

    }// if the start is zero or greater than zero
    */

    // Reduce number of tick marks as needed
    *endValue = numTicks*spacing + *startValue;

    return place;

}// void TwoDimensionalScene::tickMarks


/*!
 * Given a maximum value to be plotted on a scale, determine how much each tick
 * mark should cover to get reasonable tick mark values in the decimal number system
 * \param NumTicks is the number of tick marks on the scale
 * \param dMaxValue is the maximum value to be plotted
 * \param spacing receives the best tick mark size of the scale
 * \return the number of decimal digits after the point needed to display the full resolution of the tick mark spacing
 */
int32_t TwoDimensionalScene::tickMarkSpacing(uint32_t NumTicks, double dMaxValue, double* spacing)
{
    int32_t iPlace = 0;

    if((dMaxValue <= 0.0) || (NumTicks <= 0))
    {
        (*spacing) = 1.0;
        return iPlace;
    }

    dMaxValue = dMaxValue/NumTicks;

    // Test case .003
    // iPlace = 0
    // 0.03, iPlace = -1
    // 0.30, iPlace = -2
    // 3.00, iPlace = -3
    // return 4x10^-3 = 0.004

    // Test case 3000
    // iPlace = 0
    // 300.0, iPlace = 1
    // 30.00, iPlace = 2
    // 3.000, iPlace = 3
    // return 4x10^3 = 4000

    // Test case 3
    // iPlace = 0
    // return 4x10^0 = 4.000


    while(dMaxValue > 10.0)
    {
        dMaxValue = dMaxValue/10.0;
        iPlace++;
    }

    while(dMaxValue < 1.0)
    {
        dMaxValue = dMaxValue*10.0;
        iPlace--;
    }

    if(dMaxValue > 5.001)
    {
        *spacing = 10.0*pow(10.0, iPlace); // 10 x 10^Place

        // In this instance we need one less decimal for display resolution
        iPlace++;
    }
    else if(dMaxValue > 4.001)
        *spacing = 5.0*pow(10.0, iPlace);  // 5 x 10^Place
    else if(dMaxValue > 2.001)
        *spacing = 4.0*pow(10.0, iPlace);  // 4 x 10^Place
    else if(dMaxValue > 1.001)
        *spacing = 2.0*pow(10.0, iPlace);  // 2 x 10^Place
    else
        *spacing = pow(10.0, iPlace);  // 1 x 10^Place

    // Return the number of digits needed for display
    (iPlace > 0) ? iPlace = 0 : iPlace = -iPlace;

    return iPlace;

}// TwoDimensionalScene::dTickMarkSpacing
