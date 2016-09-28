#ifndef LINEARGAUGESCENE_H
#define LINEARGAUGESCENE_H

#include "gauge.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QColor>
#include <QRect>
#include <QPainterPath>

/*!
 * \brief The DialScene class uses QGraphicsScene to display an vertical or horizontal gauge.
 *
 * The gauge has up to three different regions of display, the low, OK, and high
 * regions.  Each region can be colored to indicate when the value being
 * displayed (the reading) is within the region.  To use the DialScene, first
 * call any set functions as needed to configure the guage. Then call setupView()
 * to setup the QGraphicsView that will display this scene.  Finally call
 * setReading() to change the value being displayed.
 */
class LinearGaugeScene : public Gauge
{
public:
    LinearGaugeScene();

    //! \return true if the guage is horizontal
    bool getHorizontal(void) const {return horizontal;}

    //! \return true if the ticks marks are on the right (vertical) or bottom (horizontal)
    bool getTicksRightOrbottom(void) const {return ticksRightOrBottom;}

    //! \return the starting point of the drawing rectangle
    double getDrawFrom(void) const {return drawFrom;}

    //! Set the starting point of the drawing rectangle
    void setDrawFrom(double from);

    //! Set the size the scene from the view rect
    void setSize(QRect rect);

    //! Set the scene to be horizontal or vertical
    void setHorizontal(bool horiz);

    //! Set if the ticks marks are on the right (vertical) or bottom (horizontal)
    void setTicksRightOrbottom(bool rightOrBottom);

    //! Set the gauge reading
    void setReading(double value);

    //! Set the gauge reading
    void setReading(double pointerValue, double textValue);

protected: // methods
    void createScene(QGraphicsScene& scene);
    void buildRect(QGraphicsScene& scene);
    void buildMajorTicks(QGraphicsScene& scene);
    void buildMinorTicks(QGraphicsScene& scene);
    void buildLabels(QGraphicsScene& scene);
    void buildReadingLabel(QGraphicsScene& scene);
    void buildHorizontalTicks(QPainterPath& path, double tickLength, double spacing);
    void buildVerticalTicks(QPainterPath& path, double tickLength, double spacing);
    double scaleToPixel(double scale);
    double scaleRangeToPixelRange(double scale);
    void updateReading(QGraphicsScene& scene);

protected:    // properties
    double drawFrom;                //!< The starting point of the drawing rect
    bool horizontal;                //!< Flag for horizontal versus vertical
    bool ticksRightOrBottom;        //!< Flag to put the tick marks on the right or bottom side
    QRect size;                     //!< The size of the viewing rect
    QGraphicsRectItem* rectItem;    //!< The primary display of value
    QGraphicsTextItem* readingTextItem; //!< The text display of value
};

#endif // LINEARGAUGESCENE_H
