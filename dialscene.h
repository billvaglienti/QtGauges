#ifndef DIALSCENE_H
#define DIALSCENE_H

#include "gauge.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QColor>
#include <QRect>
#include <QPainterPath>

/*!
 * \brief The DialScene class uses QGraphicsScene to display an analog dial gauge.
 *
 * The gauge has up to three different regions of display, the low, OK, and high
 * regions.  Each region can be colored to indicate when the value being
 * displayed (the reading) is within the region.  To use the DialScene, first
 * call any set functions as needed to configure the guage. Then call setupView()
 * to setup the QGraphicsView that will display this scene.  Finally call
 * setReading() to change the value being displayed.
 */
class DialScene : public Gauge
{
public:
    DialScene();

    //! \return the size parameters, which sets the size of the square that bounds the arc which defines the gauge
    double getSize(void) const {return size;}

    //! \return the angle at the bottom of the scale in degrees
    double getLowAngle(void) const {return lowAngle;}

    //! \return the angle at the top of the scale in degrees
    double getHighAngle(void) const {return highAngle;}

    //! Set the size the scene from the view rect
    void setSize(QRect rect);

    //! Set the low angle
    void setLowAngle(double low);

    //! set the high angle
    void setHighAngle(double high);

    //! Set the gauge reading
    void setReading(double value);

    //! Set the gauge reading
    void setReading(double pointerValue, double textValue);

private: // methods
    void createScene(QGraphicsScene& scene);
    void buildLowPath(QGraphicsScene& scene);
    void buildOkPath(QGraphicsScene& scene);
    void buildHighPath(QGraphicsScene& scene);
    void buildMajorTicks(QGraphicsScene& scene);
    void buildMinorTicks(QGraphicsScene& scene);
    void buildLabels(QGraphicsScene& scene);
    void buildLabel(QGraphicsScene& scene);
    void constructPointer(void);
    void placePointer(QGraphicsScene& scene);
    double scaleToAngle(double scale);
    double scaleRangeToAngleRange(double range);
    QPointF getCenterOfDial(void);
    void buildReadingLabel(QGraphicsScene& scene); 
    void updateReading(QGraphicsScene& scene);

private:    // properties
    QRect scenerect;
    double size;                //!< The size of the rectangle that contains the arc
    double arcsize;             //!< The size of the arc
    double rectx;               //!< The x position of the arc drawing rect
    double recty;               //!< The y position of the arc drawing rect
    double lowAngle;            //!< The angle of the arc that represents the bottom of the scale
    double highAngle;           //!< The angle of the arc that represents the top of the scale
    QPolygonF pointer;          //!< The pointer shape
    QGraphicsItem* pointerItem; //!< pointer to the pointer in the scene
    double arcWidth;            //!< The width of the arc as a fraction of the size
    QGraphicsTextItem* labelItem;   //!< The label item pointer in the scene
    QGraphicsRectItem* readingRectItem; //!< The reading text rect holder pointer in the scene
};

#endif // DIALSCENE_H
