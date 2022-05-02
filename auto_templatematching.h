#ifndef AUTO_TEMPLATEMATCHING_H
#define AUTO_TEMPLATEMATCHING_H

#include <QMainWindow>
#include<fstream>
#include"FileExplorer.h"
#include<opencv2/opencv.hpp>
using namespace cv;

namespace Ui {
class Auto_TemplateMatching;
}

class Auto_TemplateMatching : public QMainWindow
{
    Q_OBJECT

public:
    explicit Auto_TemplateMatching(QWidget *parent = nullptr);
    ~Auto_TemplateMatching();

private slots:
    void on_button_openFolder_clicked();
    void on_button_canny_clicked();
    void on_button_calDif_clicked();
    void on_button_cutout_clicked();

private:
    Ui::Auto_TemplateMatching *ui;
    //自定义成员函数:

    //自定义变量部分：

        //文件夹路径：
    TCHAR folder_name[MAX_PATH];
        //参考通道号：
        
        //要处理的时间序号：
    unsigned time_pos;
    //存储增益等配置数据线性组合后的归一化系数
    vector<double>coef_vec = vector<double>(36, 0);
    int specextract_flags = 0;
};

#endif // AUTO_TEMPLATEMATCHING_H
