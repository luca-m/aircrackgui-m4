#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>
#include "GLOBALS.h"
#include "debug.h"


namespace Ui {
    class Options;
}

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

private slots:
    void applyOptions();

signals:
    void optionsChanged(); //useful in future


private:
    Ui::Options *ui;
};

#endif // OPTIONS_H
