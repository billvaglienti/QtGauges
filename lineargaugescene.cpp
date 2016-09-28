#include "lineargaugescene.h"
#include <QGraphicsTextItem>
#include <limits>

LinearGaugeScene::LinearGaugeScene(void) :
    drawFrom(std::numeric_limits<double>::max()),
    horizontal(false),
    ticksRightOrBottom(false),
    size(),
    rectItem(0),
    readingTextItem(0)
{
}


/*!
 * Set the size of the scene from the view rect.
 * \param rect is the size of the view.
 */
void LinearGaugeScene::setSize(QRect rect)
{
    size = rect;

    size.setHeight(rect.height());
    size.setWidth(rect.width());

    dirty = true;
}


/*!
 * Set the starting point of the drawing rectangle, usually the rectangle draws
 * from the scaleStart, but it can draw from any starting location going towards
 * the current reading.  This is useful for cases where a deviation from center
 * is desired.
 * \param from is the location on the scale where the rectangle should draw from
 */
void LinearGaugeScene::setDrawFrom(double from)
{
    // Only affects the rect, so not dirty
    drawFrom = from;
}


/*!
 * Set horizontal or vertical orientation for the gauge
 * \param horiz should be true for horizontal
 */
void LinearGaugeScene::setHorizontal(bool horiz)
{
    horizontal = horiz;
    dirty = true;
}


/*!
 * Set if the ticks marks are on the right (vertical) or bottom (horizontal)
 * \param rightOrBottom should be true to place the tick marks on the right
 *        side (for vertical display) or the bottom side (for horizontal display)
 */
void LinearGaugeScene::setTicksRightOrbottom(bool rightOrBottom)
{
    ticksRightOrBottom = rightOrBottom;
    dirty = true;
}


/*!
 * Set the gauge reading.  If any other set call has been made before this one the scene
 * will be considered dirty and completely rebuilt, otherwise only the text display and
 * pointer will be updated.  In this case the scene updated is the internal scene
 * \param value is the new reading, which must lie within the scale range.
 */
void LinearGaugeScene::setReading(double value)
{
    setReading(value, value);
}


/*!
 * Set the gauge reading.  If any other set call has been made before this one the scene
 * will be considered dirty and completely rebuilt, otherwise only the text display and
 * pointer will be updated.
 * \param scene will be updated with the new reading
 * \param pointerValue is the value to display with the pointer
 * \param textValue is the value to display with the text lable.
 */
void LinearGaugeScene::setReading(double pointerValue, double textValue)
{
    gaugeReading = pointerValue;
    textReading = textValue;

    if(dirty)
        createScene(myScene);
    else
        updateReading(myScene);
}


/*!
 * Draw the gauge reading, assume all other drawing is done and good
 * \param scene will be updated with the new reading
 */
void LinearGaugeScene::updateReading(QGraphicsScene& scene)
{
    buildRect(scene);
    buildReadingLabel(scene);
}


void LinearGaugeScene::buildRect(QGraphicsScene& scene)
{
    QBrush brush = QBrush(getColorFromReading(gaugeReading));
    QPen pen = QPen(getColorFromReading(gaugeReading));

    // One pixel wide
    pen.setWidthF(1);

    // Remove the old rect
    if(rectItem)
    {
        scene.removeItem(rectItem);
        delete rectItem;
        rectItem = 0;
    }

    // This is a border rect which is needed to make sure the view is scaling
    //   the whole picture, even if the current reading does not extend that
    //   far.  It also provides color data, even when the reading is zero
    rectItem = scene.addRect(0, 0, size.width(), size.height(), pen, QBrush());

    QGraphicsRectItem* item;

    double signal = scaleToPixel(gaugeReading);
    double from;
    if((drawFrom > scaleStart) && (drawFrom < getTopOfScale()))
        from = scaleToPixel(drawFrom);
    else
        from = scaleToPixel(scaleStart);

    // Now draw the gauge rect, which is filled in as far as needed to display the reading
    if(horizontal)
    {     
        if(from < signal)
            item = scene.addRect(from, 0, signal-from, size.height(), pen, brush);
        else
            item = scene.addRect(signal, 0, from-signal, size.height(), pen, brush);
    }
    else
    {         
        // Y axis is reversed (0 on top, positive downward)
        if(from < signal)
            item = scene.addRect(0, from, size.width(), signal-from, pen, brush);
        else
            item = scene.addRect(0, signal, size.width(), from - signal, pen, brush);
    }

    // Needs to be under everything else
    item->setZValue(-10.0);
    rectItem->setZValue(-10.0);

    // I would like to treat these two rects as one item
    item->setParentItem(rectItem);
}


/*!
 * Build and add the path for the major tickmarks of the scale
 * \param scene is updated with the tickmarks.  The caller must make sure that
 *        the previous tickmarks have been removed from the scene.
 */
void LinearGaugeScene::buildMajorTicks(QGraphicsScene& scene)
{
    // Setting the tick mark length to 0 prevents drawing
    if((majorTickMarkLength <= 0.0) || (majorSpacing <= 0.0))
        return;

    QPen pen;
    QPainterPath path;

    if(horizontal)
        buildHorizontalTicks(path, majorTickMarkLength, majorSpacing);
    else
        buildVerticalTicks(path, majorTickMarkLength, majorSpacing);

    pen.setWidthF(1.5);
    pen.setColor(tickMarkColor);
    scene.addPath(path, pen);
}


/*!
 * Build and add the path for the major tickmarks of the scale
 * \param scene is updated with the tickmarks.  The caller must make sure that
 *        the previous tickmarks have been removed from the scene.
 */
void LinearGaugeScene::buildMinorTicks(QGraphicsScene& scene)
{
    // Setting the tick mark length to 0 prevents drawing
    if((minorTickMarkLength <= 0.0) || (minorSpacing <= 0.0))
        return;

    QPen pen;
    QPainterPath path;

    if(horizontal)
        buildHorizontalTicks(path, minorTickMarkLength, minorSpacing);
    else
        buildVerticalTicks(path, minorTickMarkLength, minorSpacing);

    pen.setWidthF(1.0);
    pen.setColor(tickMarkColor);
    scene.addPath(path, pen);
}

/*!
 * Build and add the path for the major horizontal tickmarks of the scale
 * \param path is updated with the tickmarks.
 * \param tickLength is the length of the tickmarks as a fraction of the size.
 * \param spacing is spacing between ticks.
 */
void LinearGaugeScene::buildHorizontalTicks(QPainterPath& path, double tickLength, double spacing)
{
    double scale = scaleStart;
    double endScale = scaleStart+getTotalRange();

    tickLength = size.height()*tickLength;

    while(scale <= endScale)
    {
        double pixel = scaleToPixel(scale);

        // don't step on outside border
        if((pixel > 0) && (pixel < size.width()))
        {
            if(ticksRightOrBottom)
            {
                // Draw vertical line from bottom up
                path.moveTo(pixel, size.height()-1);
                path.lineTo(pixel, size.height()-tickLength-1);
            }
            else
            {
                // Draw vertical line from top down
                path.moveTo(pixel, 1);
                path.lineTo(pixel, tickLength - 1);
            }
        }

        // the next tick mark
        scale += spacing;
    }
}


/*!
 * Build and add the path for the horizontal tickmarks of the scale, arrayed vertically
 * \param path is updated with the tickmarks.
 * \param tickLength is the length of the tickmarks as a fraction of the size.
 * \param spacing is spacing between ticks.
 */
void LinearGaugeScene::buildVerticalTicks(QPainterPath& path, double tickLength, double spacing)
{
    double scale = scaleStart;
    double endScale = scaleStart+getTotalRange();

    tickLength = size.width()*tickLength;

    while(scale <= endScale)
    {
        double pixel = scaleToPixel(scale);

        // don't step on outside border
        if((pixel > 0) && (pixel < size.height()))
        {
            if(ticksRightOrBottom)
            {
                // Draw horizontal line from right side to the left
                path.moveTo(size.width()-1, pixel);
                path.lineTo(size.width()-tickLength-1, pixel);
            }
            else
            {
                // Draw horizontal line, from left side to the right
                path.moveTo(1, pixel);
                path.lineTo(tickLength+1, pixel);
            }
        }

        // the next tick mark
        scale += spacing;
    }
}


/*!
 * Build and add the tick mark labels of the scale.
 * \param scene is updated with the labels.  The caller must make sure that
 *        the previous labels have been removed from the scene.
 */
void LinearGaugeScene::buildLabels(QGraphicsScene& scene)
{
    // Setting the tick mark length to 0 prevents drawing
    if((majorTickMarkLength <= 0.0) || (majorSpacing <= 0.0) || (tickMarkLabelEnable == false))
        return;

    double scale = scaleStart;
    double endScale = scaleStart+getTotalRange();

    double tickLength;
    if(horizontal)
        tickLength = size.height()*majorTickMarkLength;
    else
        tickLength = size.width()*majorTickMarkLength;

    // This is where the text starts
    double moveLength = 1.0*tickLength;

    while(scale <= endScale)
    {
        double pixel = scaleToPixel(scale);

        // the text
        QString text = QString("%1").arg(scale, 0, 'f', tickMarkPrecision);
        QGraphicsTextItem* item = scene.addText(text, QFont());
        item->setDefaultTextColor(tickMarkColor);

        QPointF location;

        // The size of the text, This is a lot bigger than the text actually is
        QRectF rect = item->boundingRect();

        // This is the size of the actual text
        QRectF textRect = QFontMetricsF(item->font()).boundingRect(text);

        if(horizontal)
        {
            if(ticksRightOrBottom)
            {
                // From the right side, move to the left to clear the tick mark
                location += QPointF(pixel, size.height() - moveLength);

                location += rectOffsetForBottomCenter(rect);
            }
            else
            {
                // Move to the right to clear the tick mark
                location += QPointF(pixel, moveLength);

                location += rectOffsetForTopCenter(rect);
            }

            // Prevent the text from falling off the left or right side of the display
            double overWidth = (rect.width() - textRect.width())/2;

            if(scale <= scaleStart)
                location.setX(location.rx() + overWidth);
            else if(scale >= getTopOfScale())
                // I'm at a loss to explain why we need this extra four pixels
                location.setX(location.rx() - overWidth - 4);
        }
        else
        {
            if(ticksRightOrBottom)
            {
                // Move from the bottom up to the location we need
                location += QPointF(size.width()-moveLength, pixel);

                location += rectOffsetForRightCenter(rect);
            }
            else
            {
                // Move to the location we need
                location += QPointF(moveLength, pixel);

                location += rectOffsetForLeftCenter(rect);
            }

            // Prevent the text from falling off the left or right side of the display
            double overHeight = (rect.height() - textRect.height())/2;

            if(scale <= scaleStart)
                location.setY(location.ry() - overHeight);
            else if(scale >= getTopOfScale())
                location.setY(location.ry() + overHeight);
        }

        item->setPos(location);

        // Turn this on for some debug action
        //scene.addRect(location.rx() + xoff, location.ry() + yoff, rect.width(), rect.height());

        // the next tick mark
        scale += majorSpacing;

    }// while

}// LinearGaugeScene::buildLabels


/*!
 * Build the label in the scene
 * \param scene is updated with the label.  The caller must make sure that
 *        the previous label has been removed from the scene.
 */
void LinearGaugeScene::buildReadingLabel(QGraphicsScene& scene)
{
    if(readingTextItem)
    {
        scene.removeItem(readingTextItem);
        delete readingTextItem;
        readingTextItem = 0;
    }

    if(textEnable == false)
      return;

    QString text = QString("%1").arg(textReading, 0, 'f', readingPrecision);

    // Halfway along the scale
    double halfScale = (getTopOfScale()+scaleStart)*0.5;

    // Add the text
    readingTextItem = scene.addText(text, QFont());
    readingTextItem->setDefaultTextColor(tickMarkColor);

    QPointF location;

    // Get the location information of the text
    // The size of the text, currently the text is located at 0, 0 in the
    //   scene.  0, 0 refers to the upper left corner of the bounding
    //   rectangle of the text
    QRectF rect = readingTextItem->boundingRect();

    // The label location depends on the tick mark location.  If there are no
    //   tick marks, then the label is centered (left to right for vertical
    //   displays and top to bottom for horizontal displays).  If there are
    //   tick marks then the text is offset away from the tick marks
    if(horizontal)
    {
        // Horizonal lay out
        if((majorSpacing > 0.0) && (majorTickMarkLength > 0.0))
        {
            // Tick marks are being drawn

            // Find the major ticks related to half scale
            double scale = scaleStart;
            while(scale < halfScale)
                scale += majorSpacing;

            // now scale is at halfScale, or just past it, back up half the major spacing
            scale -= 0.5*majorSpacing;

            // X location is between major tick marks
            location.setX(scaleToPixel(scale));

            if(ticksRightOrBottom)
            {
                // Ticks on bottom, so text on top
                location.setY(0);

                location += rectOffsetForTopCenter(rect);
            }
            else
            {
                // Ticks on top, so text on bottom
                location.setY(size.height());

                location += rectOffsetForBottomCenter(rect);
            }

        }// with tick marks
        else
        {
            // No ticks marks, text centered

            // X and Y location is halfway
            location.setX(size.width()/2);
            location.setY(size.height()/2);

            // Offset for the text rectangle.  0, 0 is the upper left corner.
            location += rectOffsetForCenter(rect);

        }// no tickmarks

    }// horizontal
    else
    {
        // Vertical lay out
        if((majorSpacing > 0.0) && (majorTickMarkLength > 0.0))
        {
            // Tick marks are being drawn

            // Find the major ticks related to half scale
            double scale = scaleStart;
            while(scale < halfScale)
                scale += majorSpacing;

            // now scale is at halfScale, or just past it, back up half the major spacing
            scale -= 0.5*majorSpacing;

            // Y location is between major tick marks
            location.setY(scaleToPixel(scale));

            if(ticksRightOrBottom)
            {
                // Ticks on right, so text on left
                location.setX(0);

                location += rectOffsetForLeftCenter(rect);
            }
            else
            {
                // Ticks on left, so text on right
                location.setX(size.width());

                location += rectOffsetForRightCenter(rect);
            }

        }// with tickmarks
        else
        {
            // No ticks marks, text centered

            // X and Y location is halfway
            location.setX(size.width()/2);
            location.setY(size.height()/2);

            // Offset for the text rectangle.  0, 0 is the upper left corner.
            location += rectOffsetForCenter(rect);

        }// no tickmarks

    }// vertical

    // Now set the position of the text
    readingTextItem->setPos(location);

}// LinearGaugeScene::buildReadingLabel


/*!
 * Convert a reading from the users units to the pixel location needed for display
 * \param scale is the value in users units
 * \return the vertical or horizontal pixel location
 */
double LinearGaugeScene::scaleToPixel(double scale)
{
    // Make sure in bounds
    if(scale < scaleStart)
        scale = scaleStart;
    else if(scale > scaleStart + getTotalRange())
        scale = scaleStart + getTotalRange();

    // The pixel location within the scene
    // The fraction of the range
    double pix = (scale - scaleStart)/getTotalRange();

    // Map to pixel range
    if(horizontal)
        pix *= (size.width());
    else
    {
        pix *= (size.height());

        // Vertical pixels are reversed
        pix = size.height() - pix;
    }

    return pix;

}// scaleToPixel


/*!
 * Create the scene, redrawing all elements
 * \param scene will be cleared and then completely rebuilt
 */
void LinearGaugeScene::createScene(QGraphicsScene& scene)
{
    // Set the background
    scene.setBackgroundBrush(backgroundColor);

    // remove all items currently in the scene
    scene.clear();

    // Set the size of the scene
    scene.setSceneRect(0, 0, size.width(), size.height());

    // Does not exist now
    rectItem = 0;
    readingTextItem = 0;

    /*
    if(majorTickMarkLength > 0)
    {
        double lowPos = scaleToPixel(scaleStart+scaleLowRange);
        double okPos  = scaleToPixel(scaleStart+scaleLowRange+scaleMidRange);
        QPen pen;
        pen.setWidthF(1.0);
        pen.setColor(Qt::gray);

        if(horizontal)
        {
            double height = majorTickMarkLength*size.height();
            double topY = 0;

            if(ticksRightOrBottom)
                topY = size.height() - height;

            scene.addRect(0,      topY, lowPos,       height, QPen(lowColor), QBrush(lowColor));
            scene.addRect(lowPos, topY, okPos,        height, QPen(midColor), QBrush(midColor));
            scene.addRect(okPos,  topY, size.width(), height, QPen(highColor), QBrush(highColor));
            scene.addRect(0,      topY, size.width(), height, pen, QBrush(Qt::transparent));
        }
        else
        {
            double width = majorTickMarkLength*size.width();
            double leftX = 0;

            if(ticksRightOrBottom)
                leftX = size.width() - width;

            scene.addRect(leftX, 0,     width, okPos,        QPen(highColor), QBrush(highColor));
            scene.addRect(leftX, okPos, width, lowPos,       QPen(midColor), QBrush(midColor));
            scene.addRect(leftX, lowPos,width, size.height(),QPen(lowColor), QBrush(lowColor));
            scene.addRect(leftX, 0,     width, size.height(),pen, QBrush(Qt::transparent));
        }
    }
    */

    // Now the tick marks
    buildMinorTicks(scene);
    buildMajorTicks(scene);

    // The tick mark labels
    buildLabels(scene);

    // No longer dirty
    dirty = false;

    // The variable stuff
    updateReading(scene);

}// LinearGaugeScene::createScene
