#include "twodimensionalplot.h"
#include <QGraphicsTextItem>

TwoDimensionalPlot::TwoDimensionalPlot() :
    TwoDimensionalScene()
{
}


//! Make sure everything that was drawn is in view
void TwoDimensionalPlot::fitAllInView(void)
{
    QRectF sceneRect = myScene.itemsBoundingRect();

    // Make sure it is all visible
    foreach(QGraphicsView* view, myScene.views())
    {
        QRectF viewRect = view->rect();

        if((viewRect.width() <= 0) || (viewRect.height() <= 0))
            continue;

        // No transformations yet
        view->resetTransform();

        // Scale it to fit
        double horscaler = viewRect.width()/sceneRect.width();
        double verscaler = viewRect.height()/sceneRect.height();

        // Make it big enough to fit
        if(horscaler > verscaler)
            view->scale(verscaler, verscaler);
        else
            view->scale(horscaler, horscaler);

        view->centerOn(0.5*(sceneRect.right()+sceneRect.left()), 0.5*(sceneRect.top()+sceneRect.bottom()));


        // view->fitInView(sceneRect, Qt::KeepAspectRatio);
    }
}


/*!
 * Generate a two dimensional plot of data with multiple dependent variables on
 * the left and right vertical axis. This version fills the entire scene, clearing
 * any data that was already there
 * \param title is the plot title
 * \param xTicks is the approximate number of desired tick marks in the x axis
 * \param yTicks is the approximate number of desired tick marks in the y axis
 * \param xData is the x axis independent variable data
 * \param yData is one or more y axis dependent variable data
 * \param rData is one or more r axis (right vertical) dependent variable data
 * \param yScaleEqualsxScale should be true to force the y scale to match the x
 *        scale, which is useful for things that have physical shape.
 * \param rScaleEqualsxScale should be true to force the r scale to match the x
 *        scale, which is useful for things that have physical shape.
 * \param aspectRatio is used to control the aspect ratio (ratio of width to
 *        height). Pass 0 or negative to allow the aspect ratio to float
 */
void TwoDimensionalPlot::generatePlot(const QString & title, uint32_t xTicks, uint32_t yTicks, const plotData * xData, const plotSet & yData, const plotSet & rData, bool yScaleEqualsxScale, bool rScaleEqualsxScale, double aspectRatio)
{
    myScene.clear();

    generatePlot(title, size, xTicks, yTicks, xData, yData, rData, yScaleEqualsxScale, rScaleEqualsxScale, aspectRatio);

    fitAllInView();

}// TwoDimensionalPlot::generatePlot


/*!
 * Generate a two dimensional plot of data with multiple dependent variables on
 * the left and right vertical axis
 * \param title is the plot title
 * \param region is the scene/view region over which the plot should be generated
 * \param xTicks is the approximate number of desired tick marks in the x axis
 * \param yTicks is the approximate number of desired tick marks in the y axis
 * \param xData is the x axis independent variable data
 * \param yData is one or more y axis dependent variable data
 * \param rData is one or more r axis (right vertical) dependent variable data
 * \param yScaleEqualsxScale should be true to force the y scale to match the x
 *        scale, which is useful for things that have physical shape.
 * \param rScaleEqualsxScale should be true to force the r scale to match the x
 *        scale, which is useful for things that have physical shape.
 * \param aspectRatio is used to control the aspect ratio (ratio of width to
 *        height). Pass 0 or negative to allow the aspect ratio to float
 */
void TwoDimensionalPlot::generatePlot(const QString & title, QRectF region, uint32_t xTicks, uint32_t yTicks, const plotData * xData, const plotSet & yData, const plotSet & rData, bool yScaleEqualsxScale, bool rScaleEqualsxScale, double aspectRatio)
{
    QPainterPath path;

    // Origin of the grid in pixels
    double yOrg = region.bottom();
    double xOrg = region.left();

    // Tick mark spacing details for the X axis
    uint32_t numXticks = xTicks;
    double xMin, xMax;
    int32_t xdigits = tickMarks(xData, &numXticks, &xMin, &xMax);

    // Tick mark spacing details for the Y axis
    uint32_t numYticks = yTicks;
    double yMin, yMax;
    int32_t ydigits = tickMarks(yData, &numYticks, &yMin, &yMax);

    // Tick mark spacing details for the R axis
    uint32_t numRticks = yTicks;
    double rMin, rMax;
    int32_t rdigits = tickMarks(rData, &numRticks, &rMin, &rMax);

    // Now we have to decide whose tick marks to go with, the y or r axis.
    //   Choose the larger of the two tick marks values, and then re-run the
    //   other axis spacing, with a fixed number of tick marks
    if(numYticks > numRticks)
    {
        numRticks = numYticks;
        rdigits = tickMarks(rData, numRticks, &rMin, &rMax);
    }
    else if(numRticks > numYticks)
    {
        numYticks = numRticks;
        ydigits = tickMarks(yData, numYticks, &yMin, &yMax);
    }

    double width = region.width();
    double height = region.height();

    // Does caller want to control the aspect ratio?
    if(aspectRatio > 0)
    {
        if(height*aspectRatio > width)
            height = width/aspectRatio;
        else
            width = height*aspectRatio;
    }

    // Figure out graph scaling, remember top - bottom is a negative number
    double xScale = width/(xMax - xMin);
    double yScale = height/(yMin - yMax);
    double rScale = height/(rMin - rMax);

    // For controlling plot distortion, remember that vertical dimensions have reversed sign
    if(yScaleEqualsxScale)
        yScale = -xScale;
    if(rScaleEqualsxScale)
        rScale = -xScale;

    // Draw the grid lines
    drawGrid(xOrg, yOrg, xScale, xMin, xMax, numXticks*2, yScale, yMin, yMax, numYticks*2);

    // The X axis and its tick marks and labels
    if(xData->drawAxis)
        DrawHorizontalAxis(xOrg, yOrg, xScale, xMin, xMax, numXticks, xData->title, xdigits, xData->color, xData->labelsHigh);

    // The left vertical (Y) axis and its tick marks and lables
    if(yData.size() > 0)
        DrawVerticalAxis(xOrg, yOrg, yScale, yMin, yMax, numYticks, labelsFromPlotSet(yData), ydigits, colorsFromPlotSet(yData), !yData.at(0)->labelsHigh, drawFirstLabelFromPlotSet(yData));

    // The right vertical (R) axis and its tick marks and lables
    if(rData.size() > 0)
        DrawVerticalAxis(region.right(), yOrg, rScale, rMin, rMax, numRticks, labelsFromPlotSet(rData), rdigits, colorsFromPlotSet(rData), rData.at(0)->labelsHigh, drawFirstLabelFromPlotSet(rData));

    // Add the title to the plot
    if(!title.isEmpty())
    {
        QPointF pos;
        QGraphicsTextItem* item = myScene.addText(title);

        // Title is centered at the top
        pos.setY(region.top());
        pos.setX((region.left()+region.right())*0.5);
        pos += rectOffsetForTopCenter(item->boundingRect());
        item->setPos(pos);
    }

    for(size_t j = 0; j < yData.size(); j++)
    {
        // Get the shorter of the two arrays, they should be the same, but the caller may make mistakes
        size_t length = xData->data.size();
        if(yData[j]->data.size() < length)
            length = yData[j]->data.size();

        // Skip this loop if we don't have enough data
        if(length < 2)
            continue;

        // starting point
        double X = xOrg + (xData->data[0]*xData->scaler - xMin)*xScale;
        double Y = yOrg + (yData[j]->data[0]*yData[j]->scaler - yMin)*yScale;
        path.moveTo(X,Y);

        for(size_t i = 1; i < length; i++)
        {
            X = xOrg + (xData->data[i]*xData->scaler - xMin)*xScale;
            Y = yOrg + (yData[j]->data[i]*yData[j]->scaler - yMin)*yScale;
            path.lineTo(X,Y);

        }// for all the solution points

        // Put the path in the scene with the right color
        myScene.addPath(path, yData[j]->color);

        // reset for new path
        path = QPainterPath();

    }// For all curves plotted against the y axis


    for(size_t j = 0; j < rData.size(); j++)
    {
        // Get the shorter of the two arrays, they should be the same, but the caller may make mistakes
        size_t length = xData->data.size();
        if(rData[j]->data.size() < length)
            length = rData[j]->data.size();

        // Skip this loop if we don't have enough data
        if(length < 2)
            continue;

        // starting point
        double X = xOrg + (xData->data[0]*xData->scaler - xMin)*xScale;
        double Y = yOrg + (rData[j]->data[0]*rData[j]->scaler - rMin)*rScale;
        path.moveTo(X,Y);

        for(size_t i = 1; i < length; i++)
        {
            X = xOrg + (xData->data[i]*xData->scaler - xMin)*xScale;
            Y = yOrg + (rData[j]->data[i]*rData[j]->scaler - rMin)*rScale;
            path.lineTo(X,Y);

        }// for all the solution points

        // Put the path in the scene with the right color
        myScene.addPath(path, rData[j]->color);

        // reset for new path
        path = QPainterPath();

     }// For all curves plotted against the r axis

}// TwoDimensionalPlot::generatePlot


/*!
 * Determine if the first label of the axis should be drawn based on the
 * individual axis information in the plot set
 * \param set is the axis plot set data
 * \return true if the first label should be drawn
 */
bool TwoDimensionalPlot::drawFirstLabelFromPlotSet(const plotSet & set)
{
    for(size_t i = 0; i < set.size(); i++)
    {
        if(set[i]->drawFirstLabel)
            return true;
    }

    return false;

}// TwoDimensionalPlot::drawFirstLabelFromPlotSet


/*!
 * Assemble the list of colors from an axis plot set
 * \param set is the axis plot set data
 * \return the list of colors
 */
QList<QColor> TwoDimensionalPlot::colorsFromPlotSet(const plotSet & set)
{
    QList<QColor> list;
    for(size_t i = 0; i < set.size(); i++)
        list.append(set[i]->color);

    return list;

}// TwoDimensionalPlot::colorsFromPlotSet


/*!
 * Assemble the list of labels from an axis plot set
 * \param set is the axis plot set data
 * \return the list of string labels
 */
QStringList TwoDimensionalPlot::labelsFromPlotSet(const plotSet & set)
{
    QStringList list;
    for(size_t i = 0; i < set.size(); i++)
    {
        if(!set.at(i)->title.isEmpty())
            list.append(set.at(i)->title);
    }

    return list;

}// TwoDimensionalPlot::labelsFromPlotSet


/*!
 * Compute tick mark information for an axis given range of data
 * \param axis is the axis single variable data set
 * \param numTicks is the number of tick marks to include
 * \param startValue receives the starting value for the axis
 * \param endValue receives the ending value for the axis
 * \return the number of decimal digits after the point needed to display the full resolution of the tick mark spacing
 */
int32_t TwoDimensionalPlot::tickMarks(const plotData * axis, uint32_t numTicks, double* startValue, double* endValue)
{
    double minValue, maxValue;

    if(getRange(axis, &minValue, &maxValue))
        return TwoDimensionalScene::tickMarks(minValue, maxValue, numTicks, startValue, endValue);
    else
    {
        *startValue = 0.0;
        *endValue = 1.0;
        return 0;
    }

}// TwoDimensionalPlot::tickMarks


/*!
 * Compute tick mark information for an axis given range of data
 * \param axis is the axis single variable data set
 * \param numTicks is the suggested number of tick marks to include, and will be updated with the correct number
 * \param startValue receives the starting value for the axis
 * \param endValue receives the ending value for the axis
 * \return the number of decimal digits after the point needed to display the full resolution of the tick mark spacing
 */
int32_t TwoDimensionalPlot::tickMarks(const plotData * axis, uint32_t* numTicks, double* startValue, double* endValue)
{
    double minValue, maxValue;

    if(getRange(axis, &minValue, &maxValue))
        return TwoDimensionalScene::tickMarks(minValue, maxValue, numTicks, startValue, endValue);
    else
    {
        *startValue = 0.0;
        *endValue = 1.0;
        return 0;
    }

}// TwoDimensionalPlot::tickMarks


/*!
 * return the range of data in a array
 * \param axis is the axis single variable data set
 * \param minValue receives the minimum value in the array
 * \param maxValue receives the maximum value in the array
 * \return true if the array contains two or more entries, else false, in which
 *         case the minValue and maxValue will be set to 0.0 and 1.0 respectively
 */
bool TwoDimensionalPlot::getRange(const plotData * axis, double* minValue, double* maxValue)
{
    if(axis->data.size() > 1)
    {
        // Initial value, this is where we can force zero as part of the range if requested
        if(axis->includeZero)
            *minValue = *maxValue = 0.0;
        else
            *minValue = *maxValue = axis->data[0]*axis->scaler;

        // Get the min and max range of the data
        for(size_t i = 0; i < axis->data.size(); i++)
        {
            double value = axis->data[i]*axis->scaler;
            if(value < *minValue)
                *minValue = value;
            else if(value > *maxValue)
                *maxValue = value;

        }// for all data entries

        return true;
    }
    else
    {
        return false;
    }

}// TwoDimensionalPlot::getRange


/*!
 * Compute tick mark information for an axis plot set
 * \param set is the axis data set
 * \param numTicks is the number of tick marks to include
 * \param startValue receives the starting value for the axis
 * \param endValue receives the ending value for the axis
 * \return the number of decimal digits after the point needed to display the full resolution of the tick mark spacing
 */
int32_t TwoDimensionalPlot::tickMarks(const plotSet & set, uint32_t numTicks, double* startValue, double* endValue)
{
    double minValue, maxValue;

    if(getRange(set, &minValue, &maxValue))
        return TwoDimensionalScene::tickMarks(minValue, maxValue, numTicks, startValue, endValue);
    else
    {
        *startValue = 0.0;
        *endValue = 1.0;
        return 0;
    }

}// TwoDimensionalPlot::tickMarks


/*!
 * Compute tick mark information for an axis plot set
 * \param set is the axis data set
 * \param numTicks is the suggested number of tick marks to include, and will be updated with the correct number
 * \param startValue receives the starting value for the axis
 * \param endValue receives the ending value for the axis
 * \return the number of decimal digits after the point needed to display the full resolution of the tick mark spacing
 */
int32_t TwoDimensionalPlot::tickMarks(const plotSet & set, uint32_t* numTicks, double* startValue, double* endValue)
{
    double minValue, maxValue;

    if(getRange(set, &minValue, &maxValue))
        return TwoDimensionalScene::tickMarks(minValue, maxValue, numTicks, startValue, endValue);
    else
    {
        *startValue = 0.0;
        *endValue = 1.0;
        return 0;
    }

}// TwoDimensionalPlot::tickMarks


/*!
 * return the range of data in a plot set
 * \param set is the set of multiple arrays of data
 * \param minValue receives the minimum value in the array
 * \param maxValue receives the maximum value in the array
 * \return true if the array contains two or more entries, else false, in which
 *         case the minValue and maxValue will be set to 0.0 and 1.0 respectively
 */
bool TwoDimensionalPlot::getRange(const plotSet & set, double* minValue, double* maxValue)
{
    size_t i;

    // Get the initial values
    for(i = 0; i < set.size(); i++)
    {
        if(set[i]->data.size() > 0)
        {
            *minValue = *maxValue = set[i]->data[0]*set[i]->scaler;
            break;
        }
    }

    // If we have no data, then get out
    if(i >= set.size())
        return false;

    for(i = 0; i < set.size(); i++)
    {
        double value = 0.0;

        // Make sure zero is included if asked
        if(set[i]->includeZero)
        {
            if(value < *minValue)
                *minValue = value;
            else if(value > *maxValue)
                *maxValue = value;
        }

        // Get the min and max range of the data
        for(size_t j = 0; j < set[i]->data.size(); j++)
        {
            value = set[i]->data[j]*set[i]->scaler;

            if(value < *minValue)
                *minValue = value;
            else if(value > *maxValue)
                *maxValue = value;

        }// for all data entries

    }// for all sets

    return true;

}// TwoDimensionalPlot::getRange
