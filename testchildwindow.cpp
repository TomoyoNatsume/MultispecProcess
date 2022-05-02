#include "testchildwindow.h"
#include "ui_testchildwindow.h"

TestChildWindow::TestChildWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TestChildWindow)
{
    ui->setupUi(this);
}

TestChildWindow::~TestChildWindow()
{
    delete ui;
}
