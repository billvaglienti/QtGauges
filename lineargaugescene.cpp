#include "lineargaugescene.h"
#include <QGraphicsTextItem>
#include <limits>

LinearGaugeScene::LinearGaugeScene(void) :
    textReading2(0),
    gaugeReading2(0),
    drawFrom(std::numeric_limits<double>::max()),
    horizontal(false),
    dualvalue(false),
    dualvaluelabel(false),
    ticksRightOrBottom(false),
    ticksBothSides(false),
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
 * Set if the ticks marks are on both sides
 * \param bothSides should be true to place the tick marks on both sides of the display
 */
void LinearGaugeScene::setTicksBothSides(bool bothSides)
{
    ticksBothSides = bothSides;
    dirty = true;
}


/*!
 * Set the gauge reading.  If any other set call has been made before this one the scene
 * will be considered dirty and completely rebuilt, otherwise only the text display and
 * pointer will be updated.  In this case the scene updated is the internal scene
 * \param value is the new reading.
 */
void LinearGaugeScene::setReading(double value)
{
    setReading(value, value);
}


/*!
 * Set the gauge reading.  If any other set call has been made before this one the scene
 * will be considered dirty and completely rebuilt, otherwise only the text display and
 * pointer will be updated.
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
 * Set the second gauge reading, but only if the dual values are enabled.  If any other
 * set call has been made before this one the scene will be considered dirty and
 * completely rebuilt, otherwise only the text display and pointer will be updated.
 * \param value2 is the new second reading.
 */
void LinearGaugeScene::setReading2(double value2)
{
    setReading2(value2, value2);
}


/*!
 * Set the second gauge reading, but only if the dual values are enabled.  If any other
 * set call has been made before this one the scene will be considered dirty and
 * completely rebuilt, otherwise only the text display and pointer will be updated.
 * \param pointerValue2 is the value to display with the second pointer
 * \param textValue2 is the value to display with the second text label.
 */
void LinearGaugeScene::setReading2(double pointerValue2, double textValue2)
{
    gaugeReading2 = pointerValue2;
    textReading2 = textValue2;

    // Do nothing unless dual values are enabled
    if(!(dualvalue || dualvaluelabel))
        return;

    if(dirty)
        createScene(myScene);
    else
        updateReading(myScene);
}


/*!
 * Set the gauge reading for the first and second reading
 * \param value is the value to display with the first pointer
 * \param value2 is the value to display with the second pointer, if dual values are enabled
 */
void LinearGaugeScene::setDualReadings(double value, double value2)
{
    setDualReadings(value, value, value2, value2);
}


/*!
 * Set the gauge reading for the first and second reading
 * \param pointerValue is the value to display with the first pointer
 * \param textValue is the value to display with the text label.
 * \param pointerValue2 is the value to display with the second pointer, if dual values are enabled
 * \param textValue2 is the value to display with the second text label.
 */
void LinearGaugeScene::setDualReadings(double pointerValue, double textValue, double pointerValue2, double textValue2)
{
    gaugeReading = pointerValue;
    textReading = textValue;
    gaugeReading2 = pointerValue2;
    textReading2 = textValue2;

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


/*!
 * Build the rects that describes the two readings
 * \param scene receives the rects
 */
void LinearGaugeScene::buildRect(QGraphicsScene& scene)
{
    QColor color = getColorFromReading(gaugeReading);
    QBrush brush = QBrush(color);
    QPen pen = QPen(color);

    // One pixel wide
    pen.setWidthF(1);

    // Remove the old first rect
    if(rectItem)
    {
        scene.removeItem(rectItem);
        delete rectItem;
        rectItem = 0;
    }

    double from;
    if((drawFrom > scaleStart) && (drawFrom < getTopOfScale()))
        from = scaleToPixel(drawFrom);
    else
        from = scaleToPixel(scaleStart);

    QGraphicsRectItem* item = 0;
    double signal = scaleToPixel(gaugeReading);

    if(dualvalue)
    {
        QColor color2 = getColorFromReading(gaugeReading2);
        QBrush brush2 = QBrush(color2);
        QPen pen2 = QPen(color2);

        // A 50/50 blend of both colors
        QColor blended = QColor((color.red()+color2.red())/2, (color.green()+color2.green())/2, (color.blue()+color2.blue())/2);

        // One pixel wide
        pen2.setWidthF(1);

        double signal2 = scaleToPixel(gaugeReading2);
        QGraphicsRectItem* item2 = 0;

        // This is a border rect which is needed to make sure the view is scaling
        //   the whole picture, even if the current reading does not extend that
        //   far.  It also provides color data, even when the reading is zero
        rectItem = scene.addRect(0, 0, size.width(), size.height(), QPen(blended), QBrush());

        // Now draw the gauge rect, which is filled in as far as needed to display the reading
        if(horizontal)
        {
            if(from < signal)
                item = scene.addRect(from, 0, signal-from, size.height()/2, pen, brush);
            else
                item = scene.addRect(signal2, 0, from-signal, size.height()/2, pen, brush);

            if(from < signal2)
                item2 = scene.addRect(from, size.height()/2, signal2-from, size.height(), pen2, brush2);
            else
                item2 = scene.addRect(signal2, size.height()/2, from-signal2, size.height(), pen2, brush2);
        }
        else
        {
            // Y axis is reversed (0 on top, positive downward)
            if(from < signal)
                item = scene.addRect(0, from, size.width()/2, signal-from, pen, brush);
            else
                item = scene.addRect(0, signal, size.width()/2, from - signal, pen, brush);

            if(from < signal2)
                item2 = scene.addRect(size.width()/2, from, size.width(), signal2-from, pen2, brush2);
            else
                item2 = scene.addRect(size.width()/2, signal2, size.width(), from - signal2, pen2, brush2);
        }

        // Needs to be under everything else
        item2->setZValue(-11.0);
        item2->setParentItem(rectItem);

    }// if dual valued
    else
    {
        // This is a border rect which is needed to make sure the view is scaling
        //   the whole picture, even if the current reading does not extend that
        //   far.  It also provides color data, even when the reading is zero
        rectItem = scene.addRect(0, 0, size.width(), size.height(), pen, QBrush());

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

    }// else single values

    // Needs to be under everything else
    item->setZValue(-10.0);
    rectItem->setZValue(-10.0);

    // Treat these two rects as one item
    item->setParentItem(rectItem);

}// LinearGaugeScene::buildRect


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
            // In both sides there is no "right or bottom" we do both
            if(ticksRightOrBottom || ticksBothSides)
            {
                // Draw vertical line from bottom up
                path.moveTo(pixel, size.height()-1);
                path.lineTo(pixel, size.height()-tickLength-1);
            }

            if(!ticksRightOrBottom || ticksBothSides)
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
            // In both sides there is no "right or bottom" we do both
            if(ticksRightOrBottom || ticksBothSides)
            {
                // Draw horizontal line from right side to the left
                path.moveTo(size.width()-1, pixel);
                path.lineTo(size.width()-tickLength-1, pixel);
            }

            if(!ticksRightOrBottom || ticksBothSides)
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

    if(dualvaluelabel)
        text += QString(" : %1").arg(textReading2, 0, 'f', readingPrecision);

    // Halfway along the scale
    double halfScale = (getTopOfScale()+scaleStart)*0.5;

    // Add the text
    readingTextItem = scene.addText(text, QFont());
    readingTextItem->setDefaultTextColor(textReadingColor);

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
