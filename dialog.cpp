#include "dialog.h"
#include "ui_dialog.h"
#include <QTime>
#include <math.h>
#include <stdint.h>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    timer(this),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // 50 Hz updates
    timer.start(50);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    // Setup the displays
    attitude.setupView(ui->attitudeView);
    dial.setupView(ui->dialView);
    vertLinearGauge1.setupView(ui->vertLinearView1);
    vertLinearGauge1.setDualValue(true);
    vertLinearGauge2.setupView(ui->vertLinearView2);
    vertLinearGauge2.setTicksBothSides(true);
    vertLinearGauge2.setDualValue(true);

    horLinearGauge1.setupView(ui->horLinearView1);
    horLinearGauge1.setDualValue(true);
    horLinearGauge2.setupView(ui->horLinearView2);
    horLinearGauge2.setTicksBothSides(true);
    horLinearGauge2.setDualValue(true);

    horLinearGauge1.setHorizontal(true);
    horLinearGauge2.setHorizontal(true);

    // We need to adjust scenes after the window is finished being built
    // as we need the size data. This is the simplest method I can find
    QTimer::singleShot(0, this, SLOT(adjustScenes()));

}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::resizeEvent(QResizeEvent * event)
{
    adjustScenes();
}

void Dialog::adjustScenes(void)
{
    attitude.setupView(ui->attitudeView);
    dial.setupView(ui->dialView);
    vertLinearGauge1.setupView(ui->vertLinearView1);
    vertLinearGauge2.setupView(ui->vertLinearView2);
    horLinearGauge1.setupView(ui->horLinearView1);
    horLinearGauge2.setupView(ui->horLinearView2);
}


void Dialog::onTimer(void)
{
    QTime time = QTime::currentTime();
    uint32_t msecs = 1000*time.second() + time.msec();

    // 10 second period
    msecs %= 10000;

    // convert to 0 to 2PI
    double input = 2.0*3.1415926535897932384626433832795*msecs/10000.0;
    double input2 = input;

    // A number from 0 to 100 that moves sinusoidally with time
    input = 50.0*sin(input) + 50.0;
    input2 = 50.0*cos(input2) + 50.0;

    attitude.setAngles(input-50,input-50,input*3.6);
    dial.setReading(input, input*1000);
    horLinearGauge1.setDualReadings(input, input2);
    horLinearGauge2.setDualReadings(input, input2);
    vertLinearGauge1.setDualReadings(input, input2);
    vertLinearGauge2.setDualReadings(input, input2);
}

