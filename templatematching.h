#ifndef TEMPLATEMATCHING_H
#define TEMPLATEMATCHING_H

#include <QMainWindow>
#include"FileExplorer.h"
#include<opencv2/opencv.hpp>
using namespace cv;



namespace Ui {
class TemplateMatching;
}

extern Mat img1;
extern Mat img2;
extern Point ref_pos, ref_size, final_off;
class TemplateMatching : public QMainWindow
{
    Q_OBJECT

public:
    explicit TemplateMatching(QWidget *parent = nullptr);
    ~TemplateMatching();
    friend void OnMouseToRef(int event, int x, int y, int, void* userdata);
    friend void CalDif(Mat& img1_, Mat& img2_, vector<double>& dif_arr,TemplateMatching *tp);
private slots:
    void on_openFileButton1_clicked();
    void on_openFileButton2_clicked();
    void on_button_calDif_clicked();
    void on_button_help_clicked();
    void on_button_selectRef_clicked();
    void on_button_showEffect_clicked();
    void on_button_canny1_clicked();
    void on_button_canny2_clicked();
private:
    Ui::TemplateMatching *ui;
};

#endif // TEMPLATEMATCHING_H
