#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include "attitudescene.h"
#include "dialscene.h"
#include "lineargaugescene.h"
#include "lineargaugescene2.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    virtual void resizeEvent(QResizeEvent * event);

private slots:
    void onTimer(void);

    void adjustScenes(void);

private:

    // The gauges to demonstrate
    AttitudeScene attitude;
    DialScene dial;
    LinearGaugeScene horLinearGauge1;
    LinearGaugeScene2 horLinearGauge2;
    LinearGaugeScene vertLinearGauge1;
    LinearGaugeScene2 vertLinearGauge2;

    QTimer timer;

    Ui::Dialog *ui;
};

#endif // DIALOG_H
