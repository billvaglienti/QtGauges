#ifndef LINEARGAUGESCENE2_H
#define LINEARGAUGESCENE2_H

#include "lineargaugescene.h"

class LinearGaugeScene2 : public LinearGaugeScene
{
public:
    LinearGaugeScene2();

    //! Set the gauge reading
    void setReading(double value);

    //! Set the gauge reading
    void setReading(double pointerValue, double textValue);

    //! \return the width of the thumb as a fraction of the scene
    double getThumbWidth(void) const {return thumbWidth;}

    //! \return the width of the bar as a fraction of the scene
    double getBarWidth(void) const {return barWidth;}

    //! \return the color of the thumb
    QColor gettThumbColor(void) const {return thumbColor;}

    //! Set the width of the thumb
    void setThumbWidth(double width);

    //! Set the width of the bar
    void setBarWidth(double width);

    //! Set the color of the thumb
    void setThumbColor(QColor color);

private: // methods

    void createScene(QGraphicsScene& scene);
    void buildRects(QGraphicsScene& scene);
    void buildThumb(QGraphicsScene& scene);

private: // properties
    QGraphicsPathItem* pathItem;

    double thumbWidth;  //!< Size of the thumb as a fraction of the scene
    double barWidth;    //!< Width of the bar as a fraction of the scene
    QColor thumbColor;  //!< The color of the thumb
};

#endif // LINEARGAUGESCENE2_H
