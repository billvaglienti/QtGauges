#ifndef TWODIMENSIONALPLOT_H
#define TWODIMENSIONALPLOT_H

#include "twodimensionalscene.h"
#include <QList>
#include <QRectF>
#include <QStringList>
#include <vector>

typedef std::vector<double> array;

class plotData
{
public:
    plotData() :
        scaler(1.0),
        includeZero(false),
        drawAxis(true),
        drawFirstLabel(false),
        labelsHigh(false)
    {
    }

    plotData(const array & x, QString z = QString(), QColor y = Qt::black) :
        data(x), color(y), title(z), scaler(1.0), includeZero(false), drawAxis(true), drawFirstLabel(true), labelsHigh(false)
    {
    }

    array data;
    QColor color;
    QString title;
    double scaler;
    bool includeZero;
    bool drawAxis;
    bool drawFirstLabel;
    bool labelsHigh;
};


//! Multiple arrays of data for plotting
typedef std::vector<const plotData *> plotSet;

class TwoDimensionalPlot : public TwoDimensionalScene
{
public:
    TwoDimensionalPlot();

    //! Plot of data with multiple dependent variables on the left and right vertical axis
    void generatePlot(const QString & title, uint32_t xTicks, uint32_t yTicks, const plotData * xData, const plotSet & yData, const plotSet & rData, bool yScaleEqualsxScale = false, bool rScaleEqualsxScale = false, double aspectRatio = -1.0);

    //! Plot of data with multiple dependent variables on the left and right vertical axis
    void generatePlot(const QString & title, QRectF region, uint32_t xTicks, uint32_t yTicks, const plotData * xData, const plotSet & yData, const plotSet & rData, bool yScaleEqualsxScale = false, bool rScaleEqualsxScale = false, double aspectRatio = -1.0);

    //! Make sure everything that was drawn is in view
    void fitAllInView(void);

    //! Determine if the first label of the axis should be drawn
    static bool drawFirstLabelFromPlotSet(const plotSet & set);

    //! Assemble the list of colors from an axis plot set
    static QList<QColor> colorsFromPlotSet(const plotSet & set);

    //! Assemble label list from a plot set
    static QStringList labelsFromPlotSet(const plotSet & set);

    //! Compute tick mark information for an axis given range of data
    static int32_t tickMarks(const plotData * axis, uint32_t numTicks, double* startValue, double* endValue);

    //! Compute tick mark information for an axis given range of data
    static int32_t tickMarks(const plotData * axis, uint32_t* numTicks, double* startValue, double* endValue);

    //! Get the range of data in a array
    static bool getRange(const plotData * axis, double* minValue, double* maxValue);

    //! Compute tick mark information for an axis plot set
    static int32_t tickMarks(const plotSet & set, uint32_t numTicks, double* startValue, double* endValue);

    //! Compute tick mark information for an axis plot set
    static int32_t tickMarks(const plotSet & set, uint32_t* numTicks, double* startValue, double* endValue);

    //! return the range of data in a plot set
    static bool getRange(const plotSet & set, double* minValue, double* maxValue);

};

#endif // TWODIMENSIONALPLOT_H
