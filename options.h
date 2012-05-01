#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>
#include "GLOBALS.h"


namespace Ui {
    class Options;
}

class Options : public QWidget
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

signals:
    void optionsChanged(); //useful in future

public slots:
    void setinjectionRate(const int rate);

private slots:
    void applyOptions();

private:
    Ui::Options *ui;
};

#endif // OPTIONS_H
