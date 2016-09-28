#include "attitudescene.h"
#include <QGraphicsPathItem>
#include <math.h>

AttitudeScene::AttitudeScene() :
    Gauge(),
    size(),
    transform(),
    roll(0),
    pitch(0),
    yaw(0),
    pitchRange(60.0),
    height(0),
    width(0),
    pixelsPerDegree(0),
    pitchMajorTickMarkLength(0.2),
    pitchMinorTickMarkLength(0.05),
    rollMajorSpacing(30),
    rollMinorSpacing(5),
    rollAlarm(45),
    pitchAlarm(25),
    alarmColor(Qt::red),
    skyItem(0),
    ladderItem(0),
    rollLabelItem(0),
    reticleItem(0),
    yawScaleItem(0)
{

    // These come form the base class, but they are too big for this case
    majorTickMarkLength = 0.10;
    minorTickMarkLength = 0.05;

    // An orangish brown color for the ground
    lowColor = QColor(255, 128, 0);

    // Blue for the sky
    highColor = QColor(0, 0, 255);

    // Yellow for the default tick marks (black does not work well)
    tickMarkColor = Qt::yellow;
}


AttitudeScene::~AttitudeScene()
{

}


/*!
 * Set the size of the scene from the view rect.
 * \param rect is the size of the view.
 */
void AttitudeScene::setSize(QRect rect)
{
    size = rect;

    size.setHeight(rect.height()-1);
    size.setWidth(rect.width()-1);

    dirty = true;
}


/*!
 * Set length of the major tick mark
 * \param major is the length of the mjaor tick mark as a fraction of the display size
 */
void AttitudeScene::setPitchMajorTickMarkLength(double major)
{
    if(major < 0.0)
        major = 0.0;

    pitchMajorTickMarkLength = major;

    dirty = true;
}


/*!
 * Set length of the minor tick mark
 * \param minor is the length of the minor tick mark as a fraction of the display size
 */
void AttitudeScene::setPitchMinorTickMarkLength(double minor)
{
    if(minor < 0.0)
        minor = 0.0;

    pitchMinorTickMarkLength = minor;

    dirty = true;
}


/*!
 * Set roll major tick mark spacing (also sets label spacing).
 * \param major is the roll major tick mark spacing
 */
void AttitudeScene::setRollMajorSpacing(double major)
{
    if(major < 0.0)
        major = 0.0;

    rollMajorSpacing = major;

    dirty = true;
}


/*!
 * Set roll minor tick mark spacing.
 * \param minor is the roll minor tick mark spacing
 */
void AttitudeScene::setRollMinorSpacing(double minor)
{
    if(minor < 0.0)
        minor = 0.0;
    else if(minor > rollMajorSpacing)
        minor = rollMajorSpacing;

    rollMinorSpacing = minor;

    dirty = true;
}


/*!
 * Set the angle above which roll is alarming
 * \param alarm is the degrees above which roll is alarming
 */
void AttitudeScene::setRollAlarm(double alarm)
{
    rollAlarm = alarm;
}


/*!
 * Set the angle above which pitch is alarming
 * \param alarm is the degrees above which pitch is alarming
 */
void AttitudeScene::setPitchAlarm(double alarm)
{
    pitchAlarm = alarm;
}


/*!
 * Set the color to use when roll or pitch is alarming
 * \param alarm is the color to use when roll or pitch is alarming
 */
void AttitudeScene::setAlarmColor(QColor alarm)
{
    alarmColor = alarm;
}


/*!
 * Compute the size of the virtual scene and its relationship to the display
 * scene.  The virtual scene is large enough to handle the largest possible
 * pitch and worst case roll angle.
 */
void AttitudeScene::computeSizeOfVirtualScene(void)
{
    // The visible region (given by size) defines a pitch extent of pitchRange.
    //   However we need a virtual display that covers the area from -90
    //   degrees to +90 degrees.
    double angleToCover = 180+pitchRange;

    // Worst case occurs when we are rolled to the diaganol, and pitched to 90.
    //double screenSize = sqrt(size.height()*size.height()+size.width()*size.width());

    // Each degree of pitch gives this many pixels of vertical display
    pixelsPerDegree = size.height()/pitchRange;

    // The virtual scene is square and this size
    height = width = pixelsPerDegree*angleToCover;
}


/*!
 * Redraw the display as needed to update the roll, pitch, and yaw angles.
 * \param phi is the roll angle in degrees
 * \param theta is the pitch angle in degrees
 * \param psi is the yaw angle in degrees
 */
void AttitudeScene::setAngles(double phi, double theta, double psi)
{
    pitch = theta;
    roll = phi;
    yaw = psi;

    // set angle bounds accordingly
    if(roll > 180)
        roll -= 360;
    else if(roll < -180)
        roll += 360;

    if(yaw < 0)
        yaw += 360;

    if(dirty)
        createScene(myScene);
    else
    {
        updateTransform();
        skyItem->setTransform(transform);
        buildPitchLadder(myScene);
        buildReticle(myScene);
        buildYawScale(myScene);
        buildRollLabel(myScene);
    }
}


/*!
 * Build the pitch ladder (and its text) and add it to the scene, the parent
 * skyItem must alrady exist.  The pitch ladder must be redrawn whenever pitch
 * changes
 * \param scene has the pitch ladder added to it
 */
void AttitudeScene::buildPitchLadder(QGraphicsScene& scene)
{
    QPen whitePen(tickMarkColor);
    whitePen.setWidthF(1);

    QPainterPath ladder;
    QFont font;
    QFontMetricsF fontMetrics(font);

    // If it wasn't for the fact that we need to clip the pitch ladder before
    //   reaching the edge of the display, we could just paint it once and do
    //   all the work with the tranform.  Alas I cannot figure out how to do
    //   that, so we must repaint it each time pitch changes
    if(ladderItem)
    {
        scene.removeItem(ladderItem);
        delete ladderItem;
        ladderItem = 0;
    }

    if((pitchMajorTickMarkLength <= 0.0) || (majorSpacing <= 0.0))
        return;

    // Set this to 0.5 for the pitch ladder to occupy the whole display
    double fractionToDraw = 0.5 - 3.0*majorTickMarkLength;

    // Bottom of the range
    double angle = pitch - fractionToDraw*pitchRange;

    angle -= fmod(angle, majorSpacing);

    // Major pitch ladder tick marks
    while(angle <= pitch + fractionToDraw*pitchRange)
    {
        double y = -angle*pixelsPerDegree;
        double x = pitchMajorTickMarkLength*size.width()/2;
        ladder.moveTo(-x, y);
        ladder.lineTo( x, y);

        QString text = QString("%1").arg(angle, 0, 'f', tickMarkPrecision);

        ladder.addText(x+4, y + fontMetrics.xHeight(), font, text);

        // Next angle
        angle += majorSpacing;

    }// while still drawing major marks


    if((pitchMinorTickMarkLength > 0.0) && (minorSpacing > 0.0))
    {
        angle = pitch - fractionToDraw*pitchRange;
        angle -= fmod(angle, minorSpacing);

        // Minor pitch ladder tick marks
        while(angle <= pitch + fractionToDraw*pitchRange)
        {
            if(fmod(angle, majorSpacing) != 0.0)
            {
                double y = -angle*pixelsPerDegree;
                double x = pitchMinorTickMarkLength*size.width()/2;
                ladder.moveTo(-x, y);
                ladder.lineTo( x, y);
            }

            // Next angle
            angle += minorSpacing;

        }// while still drawing minor marks

    }// if minor tick marks

    ladderItem = scene.addPath(ladder, whitePen, QBrush(tickMarkColor));
    ladderItem->setParentItem(skyItem);

}// buildPitchLadder


/*!
 * Build the roll scale which does not change with the angles
 * \param scene receives the roll scale
 */
void AttitudeScene::buildRollScale(QGraphicsScene& scene)
{
    if((rollMajorSpacing <= 0.0) || (majorTickMarkLength <= 0.0))
        return;

    double innerRadius = 0.5*size.height();
    double outerRadius = (0.5+majorTickMarkLength)*size.height();

    QPainterPath rollScale;

    // Starting angle, even divisions of major spacing
    double angle = -120;
    double rem = fmod(angle, rollMajorSpacing);
    if(rem != 0.0)
        angle += rollMajorSpacing - rem;

    // Major roll scale tick marks
    while(angle <= 120)
    {
        rollScale.moveTo(-innerRadius*sin(deg2rad(angle)), innerRadius*cos(deg2rad(angle)));
        rollScale.lineTo(-outerRadius*sin(deg2rad(angle)), outerRadius*cos(deg2rad(angle)));

        // Setup for next loop
        angle += rollMajorSpacing;
    }

    if((rollMinorSpacing > 0.0) && (minorTickMarkLength > 0.0))
    {
        // Starting angle, even divisions of minor spacing
        angle = -120;
        rem = fmod(angle, rollMinorSpacing);
        if(rem != 0.0)
            angle += rollMinorSpacing - rem;

        // Minor roll scale tick marks
        outerRadius = (0.5+minorTickMarkLength)*pitchRange*pixelsPerDegree;
        while(angle <= 120)
        {
            if(fmod(angle, rollMajorSpacing) != 0.0)
            {
                rollScale.moveTo(-innerRadius*sin(deg2rad(angle)), innerRadius*cos(deg2rad(angle)));
                rollScale.lineTo(-outerRadius*sin(deg2rad(angle)), outerRadius*cos(deg2rad(angle)));
            }// if location different than major ticks

            angle += rollMinorSpacing;

        }// while drawing minor ticks

    }// if drawing minor ticks

    // This scale stays fixed so we don't need to remember the item
    scene.addPath(rollScale, QPen(tickMarkColor), QBrush());

}// buildRollScale


/*!
 * Draw the roll label, which must be redone each time roll changes.
 * \param scene receives the roll label
 */
void AttitudeScene::buildRollLabel(QGraphicsScene& scene)
{
    // Must delete this each time roll changes
    if(rollLabelItem)
    {
        scene.removeItem(rollLabelItem);
        delete rollLabelItem;
        rollLabelItem = 0;
    }

    QPainterPath label;
    QTransform transform;

    if(textEnable && (fabs(roll) < 120.0))
    {
        QFont font;
        QFontMetricsF fontMetrics(font);

        // This is the height of the triangle
        double tall = majorTickMarkLength*size.height();

        // Draw the triangle
        label.moveTo(0, 0.5*size.height());
        label.lineTo(tall/2, 0.5*size.height()-tall);
        label.lineTo(-tall/2, 0.5*size.height()-tall);
        label.lineTo(0, 0.5*size.height());

        // Now the text that goes above it
        QString text = QString("%1").arg(roll, 0, 'f', tickMarkPrecision);
        QRectF rect = fontMetrics.boundingRect(text);

        // This is the bottom center of the triangle
        double x = 0;
        double y = 0.5*size.height()-tall;

        // Shift to account for the font size
        x-= rect.width()/2;

        // A little gap so the white doesn't run together
        y -= 4;

        // Add to the path
        label.addText(x, y, font, text);

    }// if drawing the roll label

    if(fabs(roll) > rollAlarm)
        rollLabelItem = scene.addPath(label, QPen(alarmColor), QBrush(alarmColor));
    else
        rollLabelItem = scene.addPath(label, QPen(tickMarkColor), QBrush(tickMarkColor));

    // The location below the pitch ladder
    transform.rotate(-roll);
    rollLabelItem->setTransform(transform);

}// AttitudeScene::buildRollLabel


/*!
 * Draw the yaw scale and label, which must be redone each time yaw changes.
 * \param scene receives the yaw label and scale
 */
void AttitudeScene::buildYawScale(QGraphicsScene& scene)
{
    double x, y;
    QFont font;
    QFontMetrics fontMetrics(font);

    if(yawScaleItem)
    {
        scene.removeItem(yawScaleItem);
        delete yawScaleItem;
        yawScaleItem = 0;
    }

    if((majorSpacing <= 0.0) || (majorTickMarkLength <= 0.0))
        return;

    QPainterPath scale;

    // Yaw should go from 0 to 360
    if(yaw < 0.0)
        yaw += 360.0;

    // This is the height of the triangle
    double tall = majorTickMarkLength*size.height();

    QString text;
    QRectF labelRect;

    if(textEnable)
    {
        // The actual yaw angle display
        text = QString("%1").arg(yaw, 0, 'f', tickMarkPrecision);
        labelRect = fontMetrics.boundingRect(text);
        x = -labelRect.width()/2;
        y = -.5*size.height() +tall + labelRect.height();
        scale.addText(x, y, font, text);

        // Draw the triangle
        scale.moveTo(0, -.5*size.height());
        scale.lineTo(tall/2, -.5*size.height() + tall);
        scale.lineTo(-tall/2, -.5*size.height() + tall);
        scale.lineTo(0, -.5*size.height());
    }

    // Start farthest left side of the display
    double startAngle = yaw - 0.5*size.width()/pixelsPerDegree;
    double endAngle = yaw + 0.5*size.width()/pixelsPerDegree;
    double angle = startAngle - fmod(startAngle, majorSpacing);

    // Draw the major ticks
    while(angle <= endAngle)
    {
        x = (angle-yaw)*pixelsPerDegree;

        // The top of the display
        y = -.50*size.height();

        // The tick mark
        scale.moveTo(x, y);
        y += tall;
        scale.lineTo(x, y);

        // The text label, always positive
        double labelAngle = angle;
        if(labelAngle < 0)
            labelAngle += 360.0;
        else if(labelAngle > 360)
            labelAngle -= 360.0;

        // The text
        text = QString("%1").arg(labelAngle, 0, 'f', tickMarkPrecision);
        QRectF rect = fontMetrics.boundingRect(text);

        // Just above the ticks

        // Shift to account for the font size
        x -= rect.width()/2;
        y += rect.height();

        // We only add the text if it does not occlude the center text plus 4 pixels
        if((x+rect.width() < -4 - 0.5*labelRect.width()) || (x > 4 + 0.5*labelRect.width()))
            scale.addText(x, y, font, text);

        angle += majorSpacing;

    }// while building major tick marks


    if((minorSpacing > 0.0) || (minorTickMarkLength > 0.0))
    {
        // The top of the display
        y = -.50*size.height();

        // Now draw the minor ticks
        angle = startAngle - fmod(startAngle, minorSpacing);
        while(angle <= endAngle)
        {
            if(fmod(angle, majorSpacing) != 0.0)
            {
                double x = (angle-yaw)*pixelsPerDegree;

                scale.moveTo(x, y);
                scale.lineTo(x, y+minorTickMarkLength*size.height());
            }

            angle += minorSpacing;

        }// while building major tick marks
    }

    yawScaleItem = scene.addPath(scale, QPen(tickMarkColor), QBrush(tickMarkColor));

}// AttitudeScene::buildYawScale


/*!
 * The reticle is the center cross and pitch lable that is used to reference
 * the horizon to.  The reticle has to be redrawn each time pitch changes.
 * \param scene has the reticle drawn in it
 */
void AttitudeScene::buildReticle(QGraphicsScene& scene)
{
    if(reticleItem)
    {
        scene.removeItem(reticleItem);
        delete reticleItem;
        reticleItem = 0;
    }

    QPainterPath reticle;
    QFont font;
    QFontMetricsF fontMetrics(font);
    QTransform matrix;

    double center = (pitchMajorTickMarkLength+0.05)*size.width()/2;
    double length = pitchMajorTickMarkLength*size.width()/2;

    // The left side of the reticle
    reticle.moveTo(-(length+center), 0);
    reticle.lineTo(-(center), 0);

    /* The top of the reticle
    reticle.moveTo(0, -(length+center));
    reticle.lineTo(0, -(center));*/

    /* The bottom of the reticle
    reticle.moveTo(0, (length+center));
    reticle.lineTo(0, (center));*/

    // The right side of the reticle
    reticle.moveTo((length+center), 0);
    reticle.lineTo((center), 0);

    // Now the text that goes beside it
    QString text = QString("%1").arg(pitch, 0, 'f', tickMarkPrecision);

    double x = length+center + 4;
    double y = fontMetrics.xHeight();

    // Add the text to the path
    reticle.addText(x, y, font, text);

    if(fabs(pitch) > pitchAlarm)
        reticleItem = scene.addPath(reticle, QPen(alarmColor), QBrush(alarmColor));
    else
        reticleItem = scene.addPath(reticle, QPen(tickMarkColor), QBrush(tickMarkColor));

    matrix.rotate(-roll);
    reticleItem->setTransform(matrix);

}// AttitudeScene::buildReticle


/*!
 * Compute the transform needed to affect items for roll and pitch.  Must be
 * called after computeSizeOfVirtualScene()
 */
void AttitudeScene::updateTransform(void)
{
    // Push the horizon down to show positive pitch
    double location = pitch*pixelsPerDegree;

    transform.reset();

    // Notice how we want the horizon to go the opposite direction of the roll angle
    transform.rotate(-roll);
    transform.translate(0, location);

}// AttitudeScene::updateTransform


/*!
 * Create the scene, redrawing all elements
 * \param scene will be cleared and then completely rebuilt
 */
void AttitudeScene::createScene(QGraphicsScene& scene)
{
    // Set the background
    scene.setBackgroundBrush(backgroundColor);

    // remove all items currently in the scene
    scene.clear();

    // This is deleted by virtue of the clear()
    skyItem = 0;
    ladderItem = 0;
    rollLabelItem = 0;
    reticleItem = 0;
    yawScaleItem = 0;

    // The size of the virtual scene
    computeSizeOfVirtualScene();

    // this is an interesting concept, we want pixel location 0,0 to be in the center
    scene.setSceneRect(-size.width()/2, -size.height()/2, size.width(), size.height());

    // Gradients for the background, go to black as we get towards the top and bottom
    QLinearGradient highGrad(0, 0, 0, -height/2);
    highGrad.setColorAt(0, highColor);
    highGrad.setColorAt(1, Qt::black);
    QBrush highBrush = QBrush(highGrad);

    QLinearGradient lowGrad(0, 0, 0, height/2);
    lowGrad.setColorAt(0, lowColor);
    lowGrad.setColorAt(1, Qt::black);
    QBrush lowBrush = QBrush(lowGrad);

    // Adjust for roll and pitch angles
    updateTransform();

    // A whole bunch of things in the scene are transformed according to pitch
    //   and roll, they are tied together through a parent->child relationship,
    //   which makes deleting and transforming them a single operation.  The
    //   parent item is the rect that forms the sky
    skyItem = scene.addRect(-width/2, -height/2, width, height/2, QPen(Qt::transparent), highBrush);
    skyItem->setTransform(transform);

    // Add the ground
    scene.addRect(-width/2, 0, width, height/2, QPen(Qt::transparent), lowBrush)->setParentItem(skyItem);

    // And the pitch ladder
    buildPitchLadder(scene);

    // The roll axis scale and its label
    buildRollScale(scene);
    buildRollLabel(scene);

    buildYawScale(scene);

    // The reticle with pitch label
    buildReticle(scene);

    // No longer dirty
    dirty = false;

}// AttitudeScene::createScene
