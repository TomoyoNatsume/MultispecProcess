#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MultispecProcess.h"

class MultispecProcess : public QMainWindow
{
    Q_OBJECT

public:
    MultispecProcess(QWidget *parent = Q_NULLPTR);

private:
    Ui::MultispecProcessClass ui;
};
