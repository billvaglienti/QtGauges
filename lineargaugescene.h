#ifndef LINEARGAUGESCENE_H
#define LINEARGAUGESCENE_H

#include "gauge.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QColor>
#include <QRect>
#include <QPainterPath>

/*!
 * \brief The LinearGaugeScene class uses QGraphicsScene to display an vertical or horizontal gauge.
 *
 * The gauge has up to three different regions of display, the low, OK, and high
 * regions.  Each region can be colored to indicate when the value being
 * displayed (the reading) is within the region.  To use the LinearGaugeScene, first
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

    //! \return true if the ticks marks are on both sides
    bool getTicksBothSides(void) const {return ticksBothSides;}

    //! \return true if the gauge displays two values, else just one
    bool getDualValue(void) const {return dualvalue;}

    //! \return true if the gauge displays two text labels, else just one
    bool getDualValueLabel(void) const {return dualvaluelabel;}

    //! \return the starting point of the drawing rectangle
    double getDrawFrom(void) const {return drawFrom;}

    //! Set the starting point of the drawing rectangle
    void setDrawFrom(double from);

    //! Set the size the scene from the view rect
    void setSize(QRect rect)  override;

    //! Set the scene to be horizontal or vertical
    void setHorizontal(bool horiz);

    //! Set if the ticks marks are on the right (vertical) or bottom (horizontal)
    void setTicksRightOrbottom(bool rightOrBottom);

    //! Set if the ticks marks are on both sides
    void setTicksBothSides(bool bothSides);

    //! Set if the gauge displays two values or just one (the default)
    void setDualValue(bool dual) {dualvalue = dual;}

    //! Set if the gauge displays two text labels or just one (the default)
    void setDualValueLabel(bool dual) {dualvaluelabel = dual;}

    //! Set the gauge reading
    void setReading(double value) override;

    //! Set the second gauge reading, dual readings must be enabled
    void setReading2(double value2);

    //! Set the gauge reading
    void setReading(double pointerValue, double textValue) override;

    //! Set the second gauge reading, dual readings must be enabled
    void setReading2(double pointerValue2, double textValue2);

    //! Set the gauge reading for the first and second reading
    void setDualReadings(double value, double value2);

    //! Set the gauge reading for the first and second reading
    void setDualReadings(double pointerValue, double textValue, double pointerValue2, double textValue2);

protected: // methods
    virtual void createScene(QGraphicsScene& scene) override;
    virtual void updateReading(QGraphicsScene& scene);

    void buildRect(QGraphicsScene& scene);
    void buildMajorTicks(QGraphicsScene& scene);
    void buildMinorTicks(QGraphicsScene& scene);
    void buildLabels(QGraphicsScene& scene);
    void buildReadingLabel(QGraphicsScene& scene);
    void buildHorizontalTicks(QPainterPath& path, double tickLength, double spacing);
    void buildVerticalTicks(QPainterPath& path, double tickLength, double spacing);
    double scaleToPixel(double scale);
    double scaleRangeToPixelRange(double scale);

protected:    // properties

    double textReading2;            //!< The second gauge reading for the text
    double gaugeReading2;           //!< The second gauge reading for the graphics
    double drawFrom;                //!< The starting point of the drawing rect
    bool horizontal;                //!< Flag for horizontal versus vertical
    bool dualvalue;                 //!< Flag for dual pointers
    bool dualvaluelabel;            //!< Flag for dual labels
    bool ticksRightOrBottom;        //!< Flag to put the tick marks on the right or bottom side
    bool ticksBothSides;            //!< Flag to put the tick marks on both sides
    QRect size;                     //!< The size of the viewing rect
    QGraphicsRectItem* rectItem;    //!< The primary display of the first value
    QGraphicsTextItem* readingTextItem; //!< The text display of the values
};

#endif // LINEARGAUGESCENE_H
