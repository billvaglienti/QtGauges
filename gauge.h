#ifndef GAUGE_H
#define GAUGE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QColor>
#include <QRect>

/*!
 * \brief The guage class uses QGraphicsScene to display a gauge.
 *
 * The gauge has up to three different regions of display, the low, OK, and high
 * regions.  Each region can be colored to indicate when the value being
 * displayed (the reading) is within the region.  To use the DialScene, first
 * call any set functions as needed to configure the guage. Then call setupView()
 * to setup the QGraphicsView that will display this scene.  Finally call
 * setReading() to change the value being displayed.
 *
 * Gauge is an abstract class
 */
class Gauge
{
public:
    Gauge();
    ~Gauge();

    //! Setup the view to contain our scene
    virtual void setupView(QGraphicsView* view);

    //! \return the value at the bottom of the gauge scale
    double getScaleStart(void) const {return scaleStart;}

    //! \return The range of the low range of the scale
    double getScaleLowRange(void) const {return scaleLowRange;}

    //! \return The range of the mid range of the scale
    double getScaleMidRange(void) const {return scaleMidRange;}

    //! \return The range of the High range of the scale
    double getScaleHighRange(void) const {return scaleHighRange;}

    //! \return the reading at the top of the scale
    double getTopOfScale(void) const;

    //! \return the top of the low range of the scale
    double getTopOfLowRange(void) const {return scaleStart+scaleLowRange;}

    //! \return the top of the mid range of the scale
    double getTopOfMidRange(void) const {return scaleStart+scaleLowRange+scaleMidRange;}

    //! \return the total range of the scale
    double getTotalRange(void) const;

    //! \return Major tick mark spacing (also sets label spacing)
    double getMajorSpacing(void) const {return majorSpacing;}

    //! \return Minor tick mark spacing
    double getMinorSpacing(void) const {return minorSpacing;}

    //! \return the label text
    QString getLabel(void) const {return label;}

    //! \return the enable flag for reading text
    bool getTextEnable(void) const {return textEnable;}

    //! \return the enable flag for tick mark labels
    bool getTickMarkLabelEnable(void) const {return tickMarkLabelEnable;}

    //! \return The color when the reading is low
    QColor getLowColor(void) const {return lowColor;}

    //! \return The color when the reading is mid
    QColor getMidColor(void) const {return midColor;}

    //! \return The color when the reading is high
    QColor getHighColor(void) const {return highColor;}

    //! \return The color of the background
    QColor getBackgroundColor(void) const {return backgroundColor;}

    //! \return The color of the tickmarks and text
    QColor getTickMarkColor(void) const {return tickMarkColor;}

    //! \return Length of the major tick mark as a fraction of the size
    double getMajorTickMarkLength(void) const {return majorTickMarkLength;}

    //! \return Length of the minor tick mark as a fraction of the size
    double getMinorTickMarkLength(void) const {return minorTickMarkLength;}

    //! \return The number of decimal places in the tick mark label
    int getTickMarkPrecision(void) const {return tickMarkPrecision;}

    //! \return The number of decimal places in the reading text
    int getReadingPrecision(void) const {return readingPrecision;}

    //! \return The gauge reading used for the pointer
    double getGaugeReading(void) const {return gaugeReading;}

    //! \return The gauge reading used for the text display
    double getTextReading(void) const {return textReading;}

    //! \return The color based on a reading
    QColor getColorFromReading(double read);

    //! Set the value at the bottom of the gauge scale
    void setScaleStart(double start);

    //! Set The range of the low range of the scale
    void setScaleLowRange(double lowRange);

    //! Set The range of the mid range of the scale
    void setScaleMidRange(double midRange);

    //! Set The range of the High range of the scale
    void setScaleHighRange(double highRange);

    //! Set major tick mark spacing (also sets label spacing)
    void setMajorSpacing(double major);

    //! Set minor tick mark spacing
    void setMinorSpacing(double minor);

    //! Set the color when the reading is low
    void setLowColor(QColor low);

    //! Set the color when the reading is mid
    void setMidColor(QColor mid);

    //! Set the color when the reading is high
    void setHighColor(QColor high);

    //! Set the color of the background, use Qt::transparent for no background
    void setBackgroundColor(QColor back);

    //! Set the color of the tickmarks and text
    void setTickMarkColor(QColor color);

    //! Set length of the major tick mark as a fraction of the size
    void setMajorTickMarkLength(double major);

    //! Set length of the minor tick mark as a fraction of the size
    void setMinorTickMarkLength(double minor);

    //! Set the label text
    void setLabel(const QString& text);

    //! Set the enable/disable flag for displaying reading text
    void setTextEnable(bool enable);

    //! Set the enable flag for tick mark labels
    void setTickMarkLabelEnable(bool enable);

    //! Set the number of decimal places in the tick mark label
    void setTickMarkPrecision(int precision);

    //! Set the number of decimal places in the reading text
    void setReadingPrecision(int precision);

    //! Set the gauge reading
    virtual void setReading(double value){}

    //! Set the gauge reading
    virtual void setReading(double pointerValue, double textValue){}

    //! Redraw the guage as needed
    void redraw(void) {setReading(gaugeReading, textReading);}

protected: // methods
    inline double rad2deg(double rad){return rad*180.0/pi;}
    inline double deg2rad(double deg){return deg*pi/180.0;}

    //! Set the size the scene from the view rect
    virtual void setSize(QRect rect) = 0;

    //! Create the entire scene
    virtual void createScene(QGraphicsScene& scene) = 0;

    //! location is with respect to the rect center
    QPointF rectOffsetForCenter(const QRectF rect) const;

    //! location is with respect to the center of the top side.
    QPointF rectOffsetForTopCenter(const QRectF rect) const;

    //! location is with respect to the top left corner.
    QPointF rectOffsetForTopLeft(void) const;

    //! location is with respect to the top right corner.
    QPointF rectOffsetForTopRight(const QRectF rect) const;

    //! location is with respect to the center of the bottom side.
    QPointF rectOffsetForBottomCenter(const QRectF rect) const;

    //! location is with respect to the bottom left corner
    QPointF rectOffsetForBottomLeft(const QRectF rect) const;

    //! location is with respect to the bottom right tcorner.
    QPointF rectOffsetForBottomRight(const QRectF rect) const;

    //! the location is with respect to the center of the left side
    QPointF rectOffsetForLeftCenter(const QRectF rect) const;

    //! the location is with respect to the cente of the right side.
    QPointF rectOffsetForRightCenter(const QRectF rect) const;

protected:    // properties
    double scaleStart;          //!< The value at the bottom of the gauge scale
    double scaleLowRange;       //!< The range of the low range of the scale
    double scaleMidRange;       //!< The range of the Mid range of the scale
    double scaleHighRange;      //!< The range of the High range of the scale
    double majorSpacing;        //!< Major tick mark spacing (also sets label spacing)
    double minorSpacing;        //!< Minor tick mark spacing
    QColor lowColor;            //!< The color when the reading is low
    QColor midColor;            //!< The color when the reading is mid
    QColor highColor;           //!< The color when the reading is high
    QColor backgroundColor;     //!< The color of the background
    QColor tickMarkColor;       //!< The color of the tickmarks and label text
    double majorTickMarkLength; //!< Length of the major tick mark as a fraction of the size
    double minorTickMarkLength; //!< Length of the minor tick mark as a fraction of the size
    double textReading;         //!< The gauge reading for the text
    double gaugeReading;        //!< The gauge reading for the graphics
    bool dirty;                 //!< Flap indicating the scene needs to be redrawn
    QGraphicsScene myScene;     //!< The scene we render into
    QString label;              //!< The label text
    bool textEnable;            //!< Flag to enable the reading text
    int tickMarkPrecision;      //!< Number of decimal places to use on the tick mark labels
    int readingPrecision;       //!< Number of decimal places to use on the reading display
    bool tickMarkLabelEnable;   //!< Flag to enable the text mark label
    const double pi;
};

#endif // GAUGE_H
