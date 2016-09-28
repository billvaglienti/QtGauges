#include "lineargaugescene2.h"
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>

LinearGaugeScene2::LinearGaugeScene2() :
    LinearGaugeScene(),
    pathItem(0),
    thumbWidth(0.05),
    barWidth(0.25),
    thumbColor(Qt::white)
{
}

/*!
 * Set the gauge reading.  If any other set call has been made before this one the scene
 * will be considered dirty and completely rebuilt, otherwise only the text display and
 * pointer will be updated.  In this case the scene updated is the internal scene
 * \param value is the new reading, which must lie within the scale range.
 */
void LinearGaugeScene2::setReading(double value)
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
void LinearGaugeScene2::setReading(double pointerValue, double textValue)
{
    gaugeReading = pointerValue;
    textReading = textValue;

    if(dirty)
        createScene(myScene);
    else
        buildThumb(myScene);
}


/*!
 * Set the width of the thumb
 * \param width is the width of the thumb, in fraction of the bar length
 */
void LinearGaugeScene2::setThumbWidth(double width)
{
    thumbWidth = width;
    dirty = true;
}


/*!
 * Set the width of the bar
 * \param width is the width of the bar, in fraction of the scene width
 */
void LinearGaugeScene2::setBarWidth(double width)
{
    barWidth = width;
    dirty = true;
}


/*!
 * Set the color of the thumb fill area.  The border will be drawn accoridng to setTickMarkColor()
 * \param color is the color of the thumb fill.
 */
void LinearGaugeScene2::setThumbColor(QColor color)
{
    thumbColor = color;
    dirty = true;
}


void LinearGaugeScene2::buildRects(QGraphicsScene& scene)
{
    double lowPos = scaleToPixel(scaleStart+scaleLowRange);
    double okPos  = scaleToPixel(scaleStart+scaleLowRange+scaleMidRange);

    if(horizontal)
    {
        double topY = size.height()*0.5 - barWidth*0.5*size.height();
        double height = barWidth*size.height();

        scene.addRect(0,      topY, lowPos,       height, QPen(lowColor), QBrush(lowColor));
        scene.addRect(lowPos, topY, okPos,        height, QPen(midColor), QBrush(midColor));
        scene.addRect(okPos,  topY, size.width(), height, QPen(highColor), QBrush(highColor));
    }
    else
    {
        double leftX = size.width()*0.5 - barWidth*0.5*size.width();
        double width = barWidth*size.width();

        scene.addRect(leftX, 0,     width, okPos,        QPen(highColor), QBrush(highColor));
        scene.addRect(leftX, okPos, width, lowPos,       QPen(midColor), QBrush(midColor));
        scene.addRect(leftX, lowPos,width, size.height(),QPen(lowColor), QBrush(lowColor));
    }

}// LinearGaugeScene2::buildRects


/*!
 * Draw the gauge reading, assume all other drawing is done and good
 * \param scene will be updated with the new reading
 */
void LinearGaugeScene2::buildThumb(QGraphicsScene& scene)
{
    // Remove the old rect
    if(pathItem)
    {
        scene.removeItem(pathItem);
        delete pathItem;
        pathItem = 0;
    }

    if(thumbWidth <= 0.0)
        return;

    QPen pen(tickMarkColor);
    pen.setWidthF(0.5);
    QBrush brush(thumbColor);
    QPainterPath path;

    // Location of the center of the thumb
    double location = scaleToPixel(gaugeReading);

    if(horizontal)
    {
        double width = size.width()*thumbWidth;
        double left = location - width*0.5;

        if( ((majorSpacing > 0.0) && (majorTickMarkLength > 0.0)) ||
            ((minorSpacing > 0.0) && (minorTickMarkLength > 0.0)) )
        {
            // If we are drawing tick marks then I want the thumb to be a pointer at the tick marks
            if(ticksRightOrBottom)
            {
                // Thumb on the top, pointing down
                path.moveTo(left, 0);
                path.lineTo(left+width, 0);
                path.lineTo(left+width, 0.5*size.height());
                path.lineTo(location, size.height());
                path.lineTo(left, 0.5*size.height());
                path.lineTo(left, 0);

            }// if ticks on the bottom
            else
            {
                // Thumb on the bottom, pointing up
                path.moveTo(left, size.height());
                path.lineTo(left+width, size.height());
                path.lineTo(left+width, 0.5*size.height());
                path.lineTo(location, 0);
                path.lineTo(left, 0.5*size.height());
                path.lineTo(left, size.height());

            }// if ticks on the top

        }// if drawing tickmarks
        else
        {
            // This is the easy case, just draw a bar
            path.moveTo(left, 0);
            path.lineTo(left+width, 0);
            path.lineTo(left+width, size.height());
            path.lineTo(left, size.height());
            path.lineTo(left, 0);

        }// else if not drawing tick marks

    }// if horizontal gauge
    else
    {
        double height = size.height()*thumbWidth;
        double top = location - height*0.5;

        if( ((majorSpacing > 0.0) && (majorTickMarkLength > 0.0)) ||
            ((minorSpacing > 0.0) && (minorTickMarkLength > 0.0)) )
        {
            // If we are drawing tick marks then I want the thumb to be a pointer at the tick marks
            if(ticksRightOrBottom)
            {
                // Thumb on the left, pointing right
                path.moveTo(0, top);
                path.lineTo(0.5*size.width(), top);
                path.lineTo(size.width(), location);
                path.lineTo(0.5*size.width(), top+height);
                path.lineTo(0, top+height);
                path.lineTo(0, top);

            }// if ticks on the right
            else
            {
                // Thumb on the right, pointing left
                path.moveTo(size.width(), top);
                path.lineTo(0.5*size.width(), top);
                path.lineTo(0, location);
                path.lineTo(0.5*size.width(), top+height);
                path.lineTo(size.width(), top+height);
                path.lineTo(size.width(), top);

            }// if ticks on the left

        }// if tick marks are being drawn
        else
        {
            // This is the easy case, just draw a bar
            path.moveTo(0, top);
            path.lineTo(size.width(), top);
            path.lineTo(size.width(), top+height);
            path.lineTo(0, top+height);
            path.lineTo(0, top);

        }// else if no tickmarks

    }// if vertical gauge

    pathItem = scene.addPath(path, pen, brush);

}// LinearGaugeScene2::updateReading


/*!
 * Create the scene, redrawing all elements
 * \param scene will be cleared and then completely rebuilt
 */
void LinearGaugeScene2::createScene(QGraphicsScene& scene)
{
    // Set the background
    scene.setBackgroundBrush(backgroundColor);

    // remove all items currently in the scene
    scene.clear();

    // Explicitly set the coordinates of the scene
    scene.setSceneRect(0, 0, size.width(), size.height());

    // Does not exist now
    rectItem = 0;
    readingTextItem = 0;
    pathItem = 0;

    // Build the rects
    buildRects(scene);

    // Now the tick marks
    buildMinorTicks(scene);
    buildMajorTicks(scene);

    // The tick mark labels
    buildLabels(scene);

    // No longer dirty
    dirty = false;

    // The variable stuff
    buildThumb(scene);

}// LinearGaugeScene::createScene

