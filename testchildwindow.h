#ifndef TESTCHILDWINDOW_H
#define TESTCHILDWINDOW_H

#include <QtWidgets/QMainWindow>

namespace Ui {
class TestChildWindow;
}

class TestChildWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestChildWindow(QWidget *parent = nullptr);
    ~TestChildWindow();

private:
    Ui::TestChildWindow *ui;
};

#endif // TESTCHILDWINDOW_H
