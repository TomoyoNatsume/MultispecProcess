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

    //友元函数
    friend void CannyTrackbarCallbackAT(int, void*);
    friend void OnMouseForRefAT(int event, int x, int y, int, void* userdata);
    friend void CalDifAT(Mat& img1_, Mat& img2_, vector<double>& dif_arr, Auto_TemplateMatching* tp);

private slots:
    void on_button_openFolder_clicked();
    void on_button_canny_clicked();
    void on_button_calDif_clicked();
    void on_button_cutout_clicked();
    void on_button_help_clicked();

private:
    Ui::Auto_TemplateMatching *ui;
    //自定义成员函数:

    //自定义变量部分：
    Mat img1, img1_clone, img1_;
    Mat img2, img2_;
    Point ref_pos, ref_size, final_off;
    int ref_x, ref_y, ref_width, ref_height, offx_range1, offx_range2, offy_range1, offy_range2, offx_range, offy_range, low_threshold1, low_threshold2 ;
    double prog = 0;

    //样本框强度总量
    int ref_total_intense;

    vector<Point>dif_array;
        //文件夹路径：
    TCHAR folder_name[MAX_PATH];
        //参考通道号：
        
        //要处理的时间序号：
    unsigned time_pos;
};
#endif // AUTO_TEMPLATEMATCHING_H
