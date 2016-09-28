#include "gauge.h"

Gauge::Gauge(void) :
    scaleStart(0.0),
    scaleLowRange(30.0),
    scaleMidRange(40.0),
    scaleHighRange(30.0),
    majorSpacing(10.0),
    minorSpacing(2.0),
    lowColor(Qt::white),
    midColor(Qt::green),
    highColor(Qt::red),
    backgroundColor(Qt::transparent),
    tickMarkColor(Qt::black),
    majorTickMarkLength(0.25),
    minorTickMarkLength(0.10),
    textReading(0.0),
    gaugeReading(0.0),
    dirty(true),
    label(""),
    textEnable(true),
    tickMarkPrecision(0),
    readingPrecision(0),
    tickMarkLabelEnable(true),
    pi(3.1415926535897932384626433832795)
{
}


Gauge::~Gauge(void)
{
    myScene.clear();
}


/*! Set the value at the bottom of the gauge scale.
 * \param start is the reading at the bottom of the scale
 */
void Gauge::setScaleStart(double start)
{
    scaleStart = start;

    dirty = true;
}

/*! Set the range of the low section of the scale.
 * \param lowRange is the range of the low section of the scale.
 */
void Gauge::setScaleLowRange(double lowRange)
{
    if(lowRange < 0.0)
        scaleLowRange = 0.0;
    else
        scaleLowRange = lowRange;

    dirty = true;
}


/*! Set the range of the mid section of the scale.
 * \param midRange is the range of the mid section of the scale.
 */
void Gauge::setScaleMidRange(double midRange)
{
    if(midRange < 0.0)
        scaleMidRange = 0.0;
    else
        scaleMidRange = midRange;

    dirty = true;
}


/*! Set the range of the high section of the scale.
 * \param highRange is the range of the high section of the scale.
 */
void Gauge::setScaleHighRange(double highRange)
{
    if(highRange < 0.0)
        scaleHighRange = 0.0;
    else
        scaleHighRange = highRange;

    dirty = true;
}


/*!
 * Set major tick mark spacing (also sets label spacing).
 * \param major is the major tick mark spacing
 */
void Gauge::setMajorSpacing(double major)
{
    if(major < 0.0)
        major = 0.0;

    majorSpacing = major;

    dirty = true;
}


/*!
 * Set minor tick mark spacing.
 * \param minor is the minor tick mark spacing
 */
void Gauge::setMinorSpacing(double minor)
{
    if(minor < 0.0)
        minor = 0.0;
    else if(minor > majorSpacing)
        minor = majorSpacing;

    minorSpacing = minor;

    dirty = true;
}


/*!
 * Set the color of the low section of the scale
 * \param low is the color for the low section of the scale
 */
void Gauge::setLowColor(QColor low)
{
    lowColor = low;

    dirty = true;
}


/*!
 * Set the color of the mid section of the scale
 * \param mid is the color for the mid section of the scale
 */
void Gauge::setMidColor(QColor mid)
{
    midColor = mid;

    dirty = true;
}


/*!
 * Set the color of the high section of the scale
 * \param high is the color for the high section of the scale
 */
void Gauge::setHighColor(QColor high)
{
    highColor = high;

    dirty = true;
}


/*!
 * Set the color of the background. Use Qt::transparent for no background color
 * \param back is the color for the background of the display
 */
void Gauge::setBackgroundColor(QColor back)
{
    backgroundColor = back;

    dirty = true;
}


/*!
 * Set the color of the tickmarks and text
 * \param color is the color for the tickmarks and labels of the display
 */
void Gauge::setTickMarkColor(QColor color)
{
    tickMarkColor = color;
    dirty = true;
}


/*!
 * Set length of the major tick mark
 * \param major is the length of the mjaor tick mark as a fraction of the display size
 */
void Gauge::setMajorTickMarkLength(double major)
{
    if(major < 0.0)
        major = 0.0;

    majorTickMarkLength = major;

    dirty = true;
}


/*!
 * Set length of the minor tick mark
 * \param minor is the length of the minor tick mark as a fraction of the display size
 */
void Gauge::setMinorTickMarkLength(double minor)
{
    if(minor < 0.0)
        minor = 0.0;

    minorTickMarkLength = minor;

    dirty = true;
}


/*!
 * Set the label text that appears above the dial
 * \param text is the text to display
 */
void Gauge::setLabel(const QString& text)
{
    label = text;
    dirty = true;
}


/*! Set the enable/disable flag for displaying reading text
 *  \param enable should be true to enable the reading text
 */
void Gauge::setTextEnable(bool enable)
{
    textEnable = enable;
}


/*!
 * Set the enable flag for tick mark labels
 * \param enable should be true to enable the labels
 */
void Gauge::setTickMarkLabelEnable(bool enable)
{
    tickMarkLabelEnable = enable;
    dirty = true;
}


/*!
 * Set the number of decimal places in the tick mark label
 * \param precision is the number of decimal places
 */
void Gauge::setTickMarkPrecision(int precision)
{
    tickMarkPrecision = precision;
    dirty = true;
}


/*!
 * Set the number of decimal places in the reading text
 * \param precision is the number of decimal places
 */
void Gauge::setReadingPrecision(int precision)
{
    readingPrecision = precision;
}


/*!
 * Return the color based on the reading, either low, mid, or high colors
 * \param read is the reading
 * \return the color associatd with read
 */
QColor Gauge::getColorFromReading(double read)
{
    if(read < scaleStart+scaleLowRange)
        return lowColor;
    else if(read < scaleStart+scaleLowRange+scaleMidRange)
        return midColor;
    else
        return highColor;

}// getColorFromReading


/*!
 * Get the total range, which is the sum of the low, mid, and high ranges
 * \return the total range of the scale
 */
double Gauge::getTotalRange(void) const
{
    return scaleLowRange+scaleMidRange+scaleHighRange;
}


/*!
 * Get the reading at the top of the scale
 * \return the reading at the top of the scale
 */
double Gauge::getTopOfScale(void) const
{
    return scaleStart+getTotalRange();
}


/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the rect center
 * \param rect is the bounding rectangle of the item.
 * \return the location offset to be added to the desired location.
 */
QPointF Gauge::rectOffsetForCenter(const QRectF rect) const
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
QPointF Gauge::rectOffsetForTopCenter(const QRectF rect) const
{
    return QPointF(-rect.width()/2, 0);
}

/*!
 * Given a rect that bounds an item, return a location offset such that when
 * passed to QGraphicsScene->setPos() the location is with respect to the top
 * left corner.
 * \return the location offset to be added to the desired location.
 */
QPointF Gauge::rectOffsetForTopLeft(void) const
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
QPointF Gauge::rectOffsetForTopRight(const QRectF rect) const
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
QPointF Gauge::rectOffsetForBottomCenter(const QRectF rect) const
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
QPointF Gauge::rectOffsetForBottomLeft(const QRectF rect) const
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
QPointF Gauge::rectOffsetForBottomRight(const QRectF rect) const
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
QPointF Gauge::rectOffsetForLeftCenter(const QRectF rect) const
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
QPointF Gauge::rectOffsetForRightCenter(const QRectF rect) const
{
    return QPointF(-rect.width(), -rect.height()/2);
}


void Gauge::setupView(QGraphicsView* view)
{
    // Make the view background transparent
    view->setStyleSheet("background: transparent");

    // turn on anti-aliasing
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    // No dragging
    view->setDragMode(QGraphicsView::NoDrag);

    // Set the size of our view
    setSize(view->rect());

    // Build the scene
    // createScene(myScene);

    // In case the developer forgets on the form editor...
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Set this scene in the view
    view->setScene(&myScene);

}// setupView
