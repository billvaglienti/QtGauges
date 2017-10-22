#include "dialscene.h"
#include <QGraphicsTextItem>
#include <QPen>
#include <Qt>
#include <math.h>

using namespace Qt;

DialScene::DialScene(void) :
    size(200),
    lowAngle(225),
    highAngle(-45),
    pointer(),
    pointerItem(),
    arcWidth(0.04),
    labelItem(0),
    readingRectItem(0)
{
    minorTickMarkLength = 0.025;
    majorTickMarkLength = 0.05;
    constructPointer();
}


/*!
 * Set the size of the scene from the view rect.
 * \param rect is the size of the view.
 */
void DialScene::setSize(QRect rect)
{
    scenerect = rect;
    dirty = true;
}


/*!
 * Set the angle in degrees used to represent the bottom of the scale.  The angle
 * follows the convetions of QPainterPath.arcMoveTo().  0 points to the right,
 * and the angle increases counter clockwise.  For a clockwise scale the high
 * angle has a lower magnitude than the low angle.
 * \param low is the angle at the bottom of the scale in degrees
 */
void DialScene::setLowAngle(double low)
{
    lowAngle = low;
    dirty = true;
}


/*!
 * Set the angle in degrees used to represent the top of the scale.  The angle
 * follows the convetions of QPainterPath.arcMoveTo().  0 points to the right,
 * and the angle increases counter clockwise.  For a clockwise scale the high
 * angle has a lower magnitude than the low angle.
 * \param high is the angle at the top of the scale in degrees
 */
void DialScene::setHighAngle(double high)
{
    highAngle = high;
    dirty = true;
}


/*!
 * Set the gauge reading.  If any other set call has been made before this one the scene
 * will be considered dirty and completely rebuilt, otherwise only the text display and
 * pointer will be updated.  In this case the scene updated is the internal scene
 * \param value is the new reading, which must lie within the scale range.
 */
void DialScene::setReading(double value)
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
void DialScene::setReading(double pointerValue, double textValue)
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
void DialScene::updateReading(QGraphicsScene& scene)
{
    buildReadingLabel(scene);
    placePointer(scene);
}


/*!
 * Build and add the path for the low section of the scale
 * \param scene is updated with the low path.  The caller must make sure that
 *        the previous path has been removed from the scene.
 */
void DialScene::buildLowPath(QGraphicsScene& scene)
{
    double highangle = scaleToAngle(scaleStart+scaleLowRange);
    double lowangle = scaleToAngle(scaleStart);
    QPen pen;
    QPainterPath path;

    // The low path, notice that the path is built counterclockwise,
    // so we start at the top and go down in value
    path.arcMoveTo(rectx, recty, arcsize, arcsize, highangle);
    path.arcTo(rectx, recty, arcsize, arcsize, highangle, lowangle-highangle);

    pen.setWidthF(size*arcWidth);
    pen.setColor(lowColor);
    pen.setCapStyle((Qt::PenCapStyle)0);

    scene.addPath(path, pen);
}


/*!
 * Build and add the path for the OK section of the scale
 * \param scene is updated with the OK path.  The caller must make sure that
 *        the previous path has been removed from the scene.
 */
void DialScene::buildOkPath(QGraphicsScene& scene)
{
    double highangle = scaleToAngle(scaleStart+scaleLowRange+scaleMidRange);
    double lowangle = scaleToAngle(scaleStart+scaleLowRange);
    QPen pen;
    QPainterPath path;
    
    // The low path, notice that the path is built counterclockwise,
    // so we start at the top and go down in value
    path.arcMoveTo(rectx, recty, arcsize, arcsize, highangle);
    path.arcTo(rectx, recty, arcsize, arcsize, highangle, lowangle-highangle);

    pen.setWidthF(size*arcWidth);
    pen.setColor(midColor);
    pen.setCapStyle((Qt::PenCapStyle)0);
    scene.addPath(path, pen);
}


/*!
 * Build and add the path for the high section of the scale
 * \param scene is updated with the high path.  The caller must make sure that
 *        the previous path has been removed from the scene.
 */
void DialScene::buildHighPath(QGraphicsScene& scene)
{
    double highangle = scaleToAngle(scaleStart+scaleLowRange+scaleMidRange+scaleHighRange);
    double lowangle = scaleToAngle(scaleStart+scaleLowRange+scaleMidRange);
    QPen pen;
    QPainterPath path;

    // The low path, notice that the path is built counterclockwise,
    // so we start at the top and go down in value
    path.arcMoveTo(rectx, recty, arcsize, arcsize, highangle);
    path.arcTo(rectx, recty, arcsize, arcsize, highangle, lowangle-highangle);

    pen.setWidthF(size*arcWidth);
    pen.setColor(highColor);
    pen.setCapStyle((Qt::PenCapStyle)0);
    scene.addPath(path, pen);
}


/*!
 * Build and add the path for the major tickmarks of the scale
 * \param scene is updated with the tickmarks.  The caller must make sure that
 *        the previous tickmarks have been removed from the scene.
 */
void DialScene::buildMajorTicks(QGraphicsScene& scene)
{
    // Setting the tick mark length to 0 prevents drawing
    if((majorTickMarkLength <= 0.0) || (majorSpacing <= 0.0))
        return;

    double scale = scaleStart;
    double endScale = scaleStart+getTotalRange();
    double angle;

    double tickLength = size*majorTickMarkLength;
    double offsetLength = 0.5*size*majorTickMarkLength;

    QPen pen;
    QPainterPath path;

    while(scale <= endScale)
    {
        angle = scaleToAngle(scale);

        // Go to a location on the elipse
        path.arcMoveTo(rectx, recty, arcsize, arcsize, angle);

        // Move from that location towards the center
        path.moveTo(path.currentPosition()+=QPointF(-offsetLength*cos(deg2rad(-angle)), -offsetLength*sin(deg2rad(-angle))));

        // Move from that location towards the center
        path.lineTo(path.currentPosition()+=QPointF(-tickLength*cos(deg2rad(-angle)), -tickLength*sin(deg2rad(-angle))));

        // the next tick mark
        scale += majorSpacing;
    }

    pen.setWidthF(1.0);
    pen.setColor(tickMarkColor);
    scene.addPath(path, pen);
}


/*!
 * Build and add the path for the minor tickmarks of the scale
 * \param scene is updated with the tickmarks.  The caller must make sure that
 *        the previous tickmarks have been removed from the scene. */
void DialScene::buildMinorTicks(QGraphicsScene& scene)
{
    // Setting the tick mark length to 0 prevents drawing
    if((minorTickMarkLength <= 0.0) || (minorSpacing <= 0.0))
        return;

    double scale = scaleStart;
    double endScale = scaleStart+getTotalRange();
    double angle;

    double tickLength = size*minorTickMarkLength;
    double offsetLength = 0.5*size*majorTickMarkLength;

    QPen pen;
    QPainterPath path;

    while(scale <= endScale)
    {
        angle = scaleToAngle(scale);

        // Go to a location on the elipse
        path.arcMoveTo(rectx, recty, arcsize, arcsize, angle);

        // Move from that location towards the center
        path.moveTo(path.currentPosition()+=QPointF(-offsetLength*cos(deg2rad(-angle)), -offsetLength*sin(deg2rad(-angle))));

        // Move from that location towards the center
        path.lineTo(path.currentPosition()+=QPointF(-tickLength*cos(deg2rad(-angle)), -tickLength*sin(deg2rad(-angle))));

        // the next tick mark
        scale += minorSpacing;
    }

    pen.setWidthF(1.0);
    pen.setColor(tickMarkColor);
    scene.addPath(path, pen);
}


/*!
 * Build and add the path for the minor tickmarks of the scale
 * \param scene is updated with the tickmarks.  The caller must make sure that
 *        the previous tickmarks have been removed from the scene.
void DialScene::buildMinorTicks(QGraphicsScene& scene)
{
    double scale = scaleStart;
    double endScale = scaleStart+scaleLowRange;
    double angle;

    double tickLength = size*minorTickMarkLength;
    double offsetLength = 0.5*size*majorTickMarkLength;

    QPen pen;
    QPainterPath path;
    pen.setWidthF(1);

    while(scale < endScale)
    {
        angle = scaleToAngle(scale);

        // Go to a location on the elipse
        path.arcMoveTo(rectx, recty, arcsize, arcsize, angle);

        // Move from that location towards the center
        path.moveTo(path.currentPosition()+=QPointF(-offsetLength*cos(deg2rad(-angle)), -offsetLength*sin(deg2rad(-angle))));

        // Move from that location towards the center
        path.lineTo(path.currentPosition()+=QPointF(-tickLength*cos(deg2rad(-angle)), -tickLength*sin(deg2rad(-angle))));

        // the next tick mark
        scale += minorSpacing;
    }
    pen.setColor(lowColor);
    scene.addPath(path, pen);

    path = QPainterPath();
    endScale += scaleOkRange;
    while(scale < endScale)
    {
        angle = scaleToAngle(scale);

        // Go to a location on the elipse
        path.arcMoveTo(rectx, recty, arcsize, arcsize, angle);

        // Move from that location towards the center
        path.moveTo(path.currentPosition()+=QPointF(-offsetLength*cos(deg2rad(-angle)), -offsetLength*sin(deg2rad(-angle))));

        // Move from that location towards the center
        path.lineTo(path.currentPosition()+=QPointF(-tickLength*cos(deg2rad(-angle)), -tickLength*sin(deg2rad(-angle))));

        // the next tick mark
        scale += minorSpacing;
    }
    pen.setColor(okColor);
    scene.addPath(path, pen);

    path = QPainterPath();
    endScale += scaleHighRange;
    while(scale <= endScale)
    {
        angle = scaleToAngle(scale);

        // Go to a location on the elipse
        path.arcMoveTo(rectx, recty, arcsize, arcsize, angle);

        // Move from that location towards the center
        path.moveTo(path.currentPosition()+=QPointF(-offsetLength*cos(deg2rad(-angle)), -offsetLength*sin(deg2rad(-angle))));

        // Move from that location towards the center
        path.lineTo(path.currentPosition()+=QPointF(-tickLength*cos(deg2rad(-angle)), -tickLength*sin(deg2rad(-angle))));

        // the next tick mark
        scale += minorSpacing;
    }
    pen.setColor(highColor);
    scene.addPath(path, pen);
}*/


/*!
 * Build and add the tick mark labels of the scale.
 * \param scene is updated with the labels.  The caller must make sure that
 *        the previous labels have been removed from the scene.
 */
void DialScene::buildLabels(QGraphicsScene& scene)
{
    // Setting the tick mark length to 0 prevents drawing
    if((majorTickMarkLength <= 0.0) || (majorSpacing <= 0.0) || (tickMarkLabelEnable == false))
        return;

    double scale = scaleStart;
    double endScale = scaleStart+getTotalRange();
    double angle;

    double tickLength = size*majorTickMarkLength;
    double offsetLength = 0.5*size*majorTickMarkLength;
    double moveLength = offsetLength+tickLength;

    QPainterPath path;

    // This move length would be perfect, except there is a lot of extraneous
    //   space in the rect surrounding the text, so take some of it off here
    moveLength -= 0.5*offsetLength;

    while(scale <= endScale)
    {
        angle = scaleToAngle(scale);

        QPointF location;

        // Go to a location on the elipse
        path.arcMoveTo(rectx, recty, arcsize, arcsize, angle);
        location = path.currentPosition();

        // Move from that location towards the center, to the end of the tick mark
        location += QPointF(-moveLength*cos(deg2rad(-angle)), -moveLength*sin(deg2rad(-angle)));

        // the text
        QGraphicsTextItem* item = scene.addText(QString("%1").arg(scale, 0, 'f', tickMarkPrecision), QFont());

        item->setDefaultTextColor(tickMarkColor);

        // The size of the text, currently the text is located at 0, 0 in the
        //   scene.  0, 0 refers to the upper left corner of the bounding
        //   rectangle of the text
        QRectF rect = item->boundingRect();

        // Move from the upper left corner location to the center of the rect
        location += QPointF(-rect.width()/2, -rect.height()/2);

        // Now we want to shift the rect so that the center is not at the end
        //   of the tick.  We need the offsets from the center to where the ray
        //   intersects the rect walls

        // The angle from the center to the upper right, lower right, upper left,
        //   and lower left corners in radians.  From 0 to 2PI
        double angleLRRect = atan2(-rect.height(), rect.width()) + 2.0*pi;
        double angleURRect = atan2(rect.height(), rect.width());
        double angleULRect = atan2(rect.height(), -rect.width());
        double angleLLRect = atan2(-rect.height(), -rect.width()) + 2.0*pi;

        // The ray angle, in radians from 0.0 to 2PI
        double anglerad = deg2rad(angle);
        if(anglerad < 0.0)
            anglerad += 2.0*pi;

        double xoff = 0.0;
        double yoff = 0.0;
        if((anglerad >= angleURRect) && (anglerad < angleULRect))
        {
            // Vertical offset is half the height
            yoff = rect.height()/2;

            // Horizontal offset depends on the ray length to the top of the box
            xoff = -yoff/tan(anglerad);

        }// if ray goes through the top
        else if((anglerad >= angleULRect) && (anglerad < angleLLRect))
        {
            // Horizontal offset is half the width
            xoff = rect.width()/2;

            // Vertical offset depends on the ray length to the side of the box
            yoff = -xoff*tan(anglerad);

        }// else if ray goes through the left side
        else if((anglerad >= angleLLRect) && (anglerad < angleLRRect))
        {
            // Vertical offset is half the height
            yoff = -rect.height()/2;

            // Horizontal offset depends on the ray length to the top of the box
            xoff = -yoff/tan(anglerad);

        }// else if ray goes through the bottom
        else
        {
            // Horizontal offset is half the width
            xoff = -rect.width()/2;

            // Vertical offset depends on the ray length to the side of the box
            yoff = -xoff*tan(anglerad);

        }// else ray goes through the right side

        // Finally locate the text in the scene
        item->setPos(location.rx() + xoff, location.ry() + yoff);

        // Turn this on for some debug action
        //scene.addRect(location.rx() + xoff, location.ry() + yoff, rect.width(), rect.height());

        // the next tick mark
        scale += majorSpacing;

    }// while

}// DialScene::buildLabels


/*!
 * Get the coordinates of the center of the dial
 * \return the center coordinates
 */
QPointF DialScene::getCenterOfDial(void)
{
    // Find the center of the arc of averaging the 0 and 180 points
    QPainterPath path;
    path.arcMoveTo(rectx, recty, arcsize, arcsize, 0);
    QPointF center = path.currentPosition();
    path.arcMoveTo(rectx, recty, arcsize, arcsize, 180);

    // Take the average
    center += path.currentPosition();
    center *= 0.5;

    return center;

}// DialScene::getCenterOfDial


/*!
 * Construct the polygon that describes the pointer.
 */
void DialScene::constructPointer(void)
{
    double tickLength = size*majorTickMarkLength;
    double offsetLength = 0.5*size*majorTickMarkLength;
    double moveLength = offsetLength+tickLength;

    double length = arcsize/2.0 - moveLength;
    double backlength = 0.0;//length/3.0;
    double halfwidth = length/20.0;
    double arrowlength = 8.0*halfwidth;
    double forelength = length - arrowlength;

    pointer = QPolygonF();

    pointer<< QPointF(+halfwidth, -backlength)
           << QPointF(-halfwidth, -backlength)
           << QPointF(-halfwidth, +forelength)
           << QPointF(0,          +length)
           << QPointF(+halfwidth, +forelength)
           << QPointF(+halfwidth, -backlength);
}


/*!
 * Build the label in the scene
 * \param scene is updated with the label.  The caller must make sure that
 *        the previous label has been removed from the scene.
 */
void DialScene::buildLabel(QGraphicsScene& scene)
{
    // Do nothing if we have no text
    if(label.isEmpty())
    {
        labelItem = 0;
        return;
    }

    QFont font;

    // A big font
    font.setPixelSize(size/10);

    // This is the center of the dial
    QPointF location = getCenterOfDial();

    // Add the text
    labelItem = scene.addText(label, font);
    labelItem->setDefaultTextColor(tickMarkColor);

    // Get the location information of the text
    // The size of the text, currently the text is located at 0, 0 in the
    //   scene.  0, 0 refers to the upper left corner of the bounding
    //   rectangle of the text
    QRectF rect = labelItem->boundingRect();

    // Move from the upper left corner location to the center of the rect
    location += QPointF(-rect.width()/2, -rect.height()/2);

    // Now shift downwards to clear the pointer center dot
    // location.setY(location.ry() + arcsize/10.0);

    // Now shift upwards to clear the pointer center dot
    location.setY(location.ry() - (rect.height()/2.0 + arcsize/20.0));

    // Now set the position of the text
    labelItem->setPos(location);

}// DialScene::buildLabel


/*!
 * Place the pointer on the scene, oriented to reflect the current reading
 * \param scene is updated to remove the previous pointer and place the new one.
 */
void DialScene::placePointer(QGraphicsScene& scene)
{
    QMatrix matrix;
    QPolygonF localPointer;
    QPen pen;
    QBrush brush;

    // Find the center of the arc
    QPointF center = getCenterOfDial();

    // Get rid of the existing pointer, if any
    if(pointerItem != 0)
    {
        scene.removeItem(pointerItem);
        delete pointerItem;
        pointerItem = 0;
    }

    // Don't let the pointer go out of range
    double read = gaugeReading;
    if(read < scaleStart)
        read = scaleStart;
    else if(read > scaleStart+getTotalRange())
        read = scaleStart+getTotalRange();

    // Spin the pointer to the correct reading.  Zero rotation is pointing
    //   straight down (inverted y axis).  However our angle is zero pointing
    //   to the right, and increases going to the left
    double angle = scaleToAngle(read);

    // account for rotation sign change
    angle *= -1.0;

    // account for angle offset between zeros
    angle -= 90.0;

    matrix.rotate(angle);
    localPointer = matrix.map(pointer);

    // Locate the needle at the center of the arc
    localPointer.translate(center);

    // The brush is colored based on the reading
    brush = QBrush(getColorFromReading(gaugeReading));

    // Place it in the scene
    pen.setWidthF(0.0);
    pen.setColor(Qt::black);
    pointerItem = scene.addPolygon(localPointer, pen, brush);

    // And the center dot, notice how 0,0 of the dot is not the center of the dot
    double centersize = arcsize/10.0;
    double halfsize = 0.5*centersize;
    QGraphicsItem* item = scene.addEllipse(center.rx()-halfsize, center.ry()-halfsize, centersize, centersize, pen, QBrush(Qt::black));

    // Set this parent to be the pointer, which allows this item to be deleted with the parent
    item->setParentItem(pointerItem);

}// DialScene::placePointer


/*!
 * Build the label in the scene
 * \param scene is updated with the label.  The caller must make sure that
 *        the previous label has been removed from the scene.
 */
void DialScene::buildReadingLabel(QGraphicsScene& scene)
{    
    // Get rid of the existing text, if any
    if(readingRectItem != 0)
    {
        scene.removeItem(readingRectItem);
        delete readingRectItem;
        readingRectItem = 0;
    }

    if(textEnable == false)
      return;

    QString text = QString("%1").arg(textReading, 0, 'f', readingPrecision);

    QFont font;

    // A big font
    font.setPixelSize(size/10);

    // This is the center of the dial
    QPointF center = getCenterOfDial();

    QPointF location;

    // Add the rect, we'll figure out the geometry later, but we add rect first so the z-order is correct, with text on top
    readingRectItem = scene.addRect(0, 0, 1, 1, QPen(getColorFromReading(gaugeReading)), QBrush(getColorFromReading(gaugeReading)));

    // Add the text, and make it a child of the rect
    QGraphicsTextItem* readingTextItem = scene.addText(text, font);
    readingTextItem->setDefaultTextColor(tickMarkColor);
    readingTextItem->setParentItem(readingRectItem);

    // The text bounding rectangle
    QRectF textRect = readingTextItem->boundingRect();

    // Now shift downwards to clear the pointer center dot, plus an extra quarter height
    center.setY(center.ry() + textRect.height()/4.0 + textRect.height()/2.0 + arcsize/20.0);

    // Move from the upper left corner location to the center of the rect
    location = center + QPointF(-textRect.width()/2, -textRect.height()/2);

    // Now set the position of the text
    readingTextItem->setPos(location);
    readingRectItem->setRect(location.rx(), location.ry(), textRect.width(), textRect.height());

}// DialScene::buildReadingLabel


/*!
 * Convert a reading from the users units to the angle needed for display on the arc
 * \param scale is the value in users units
 * \return the angle in degrees that scale represents on the arc.
 */
double DialScene::scaleToAngle(double scale)
{
    // The angles go counterclockwise from -30 to -150 (a range of 240 degrees)
    // -30 represents the high value, and -150 (or 210) is the low value
    
    // The fraction of the range
    double angle = (scale - scaleStart)/getTotalRange();
    
    // Map to angle range, notice inversion
    angle *= (highAngle - lowAngle);
    
    // Starting point offset
    angle += lowAngle;
    
    return angle;

}// scaleToAngle


/*!
 * Convert a users units range to a range in angle on the arc
 * \param range is the users units range
 * \return the angle range in degrees that range represents on the arc.
 */
double DialScene::scaleRangeToAngleRange(double range)
{
    // The fraction of the range
    return fabs(highAngle - lowAngle)*range/getTotalRange();
}


/*!
 * Create the scene, redrawing all elements
 * \param scene will be cleared and then completely rebuilt
 */
void DialScene::createScene(QGraphicsScene& scene)
{
    // Set the background
    scene.setBackgroundBrush(backgroundColor);

    // remove all items currently in the scene
    scene.clear();

    // This has already been deleted, don't try to delete it again
    pointerItem = 0;
    labelItem = 0;
    readingRectItem = 0;

    // This is a border rect which is needed to make sure the view is showing everything
    scene.addRect(scenerect, QPen(Qt::transparent), QBrush());

    // The size reference, as the larger of the height or width
    if(scenerect.width() > scenerect.height())
        size = scenerect.width();
    else
        size = scenerect.height();

    // Margin for the arc
    arcsize = size*(1 - 2*arcWidth);

    // Draw an arc, so we can see what space its going to occupy.
    // I know this is lame, but its a lot simpler than trying to reverse engineer the arc functions
    QPainterPath path;
    QPen pen;
    pen.setWidthF(size*arcWidth);
    pen.setColor(Qt::black);
    pen.setCapStyle((Qt::PenCapStyle)0);
    path.arcMoveTo(0, 0, arcsize, arcsize, highAngle);
    path.arcTo(0, 0, arcsize, arcsize, highAngle, lowAngle - highAngle);
    QGraphicsItem* item = scene.addPath(path, pen);

    // Get the size of the item
    QRectF bound = item->boundingRect();

    // Done with this, don't let it pollute our scene
    scene.removeItem(item);

    rectx = (scenerect.width() - bound.width())/2;
    recty = (scenerect.height() - bound.height())/2;
    rectx -= bound.x();
    recty -= bound.y();

    // Build the arcs
    buildHighPath(scene);
    buildOkPath(scene);
    buildLowPath(scene);

    // Now the tick marks
    buildMinorTicks(scene);
    buildMajorTicks(scene);

    // The tick mark labels
    buildLabels(scene);

    // The main display label
    buildLabel(scene);

    // Build pointer
    constructPointer();

    // No longer dirty
    dirty = false;

    // Display the stuff that varies
    updateReading(scene);

}// DialScene::createScene
