#ifndef PCA_H
#define PCA_H

#include <QMainWindow>
//#include<Dense>
#include<eigen3/Eigen/Dense>

//提取csv文件算法



//PCA算法

    //求出特征值，取

namespace Ui {
class PCA;
}

class PCA : public QMainWindow
{
    Q_OBJECT

public:
    explicit PCA(QWidget *parent = nullptr);
    ~PCA();

private:
    Ui::PCA *ui;
};

#endif // PCA_H
