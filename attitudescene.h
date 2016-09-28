#ifndef ATTITUDESCENE_H
#define ATTITUDESCENE_H

#include "gauge.h"

class AttitudeScene : public Gauge
{
public: // methods

    AttitudeScene();
    ~AttitudeScene();

    //! Set the length of the pitch ladder major tick mark
    double getPitchMajorTickMarkLength(void) const {return pitchMajorTickMarkLength;}

    //! Set the length of the pitch ladder minor tick mark
    double getPitchMinorTickMarkLength(void) const {return pitchMinorTickMarkLength;}

    //! \return Roll major tick mark spacing (also sets label spacing)
    double getRollMajorSpacing(void) const {return rollMajorSpacing;}

    //! \return Roll minor tick mark spacing
    double getRollMinorSpacing(void) const {return rollMinorSpacing;}

    //! \return the angle in degrees above which roll is alarming
    double getRollAlarm(void) const {return rollAlarm;}

    //! \return the angle in degrees above which pitch is alarming
    double getPitchAlarm(void) const {return pitchAlarm;}

    //! \return the color to use when roll or pitch is alarming
    QColor getAlarmColor(void) const {return alarmColor;}

    //! Set the angles that update the display
    void setAngles(double phi, double theta, double psi);

    //! Set the length of the pitch ladder major tick mark
    void setPitchMajorTickMarkLength(double length);

    //! Set the length of the pitch ladder minor tick mark
    void setPitchMinorTickMarkLength(double length);

    //! Set major tick mark spacing (also sets label spacing)
    void setRollMajorSpacing(double major);

    //! Set minor tick mark spacing
    void setRollMinorSpacing(double minor);

    //! Set the angle above which roll is alarming
    void setRollAlarm(double alarm);

    //! Set the angle above which pitch is alarming
    void setPitchAlarm(double alarm);

    //! Set the color to use when roll or pitch is alarming
    void setAlarmColor(QColor alarm);

private: // members

    void setSize(QRect rect);
    void createScene(QGraphicsScene& scene);
    void buildReticle(QGraphicsScene& scene);
    void buildPitchLadder(QGraphicsScene& scene);
    void buildRollScale(QGraphicsScene& scene);
    void buildRollLabel(QGraphicsScene& scene);
    void buildYawScale(QGraphicsScene& scene);
    void computeSizeOfVirtualScene(void);
    void updateTransform(void);

private: // members
    QRect size;                     //!< The size of the viewing rect
    QTransform transform;           //!< The transform for pitch and roll
    double roll;                    //!< The roll angle in radians
    double pitch;                   //!< The pitch angle in radians
    double yaw;                     //!< The yaw angle in radians
    double pitchRange;              //!< The pitch range in radians
    double height;                  //!< The height of the virtual display
    double width;                   //!< The width of the virtual display
    double pixelsPerDegree;         //!< The number of pixels per pitch degree
    double pitchMajorTickMarkLength;//!< The length of the pitch ladder major tick mark
    double pitchMinorTickMarkLength;//!< The length of the pitch ladder minor tick mark
    double rollMajorSpacing;        //!< The degrees between major roll scale ticks
    double rollMinorSpacing;        //!< The degrees between minor roll scale ticks
    double rollAlarm;               //!< The degrees above which roll is alarm color
    double pitchAlarm;              //!< The degrees above which pitch is alarm color
    QColor alarmColor;              //!< Color to use when alarming
    QGraphicsRectItem* skyItem;     //!< The rect that is the sky, this is the parent to all transformed items
    QGraphicsPathItem* ladderItem;
    QGraphicsPathItem* rollLabelItem;
    QGraphicsPathItem* reticleItem;
    QGraphicsPathItem* yawScaleItem;
};

#endif // ATTITUDESCENE_H
