#ifndef SPECEXTRACT_H
#define SPECEXTRACT_H

#include <QMainWindow>
#include<fstream>
#include"FileExplorer.h"
#include<opencv2/opencv.hpp>
using namespace cv;
//调取增益等镜头数据


//提取某点光谱算法


//存储光谱数据算法(csv)


//提取csv文件算法


//

namespace Ui {
class SpecExtract;
}

class SpecExtract : public QMainWindow
{
    Q_OBJECT

public:
    explicit SpecExtract(QWidget *parent = nullptr);
    friend void OnMouseToRSpecExact(int event, int x, int y, int, void* userdata);
    ~SpecExtract();
private slots:
    void on_button_openDifFile_clicked();
    void on_button_openConfFile_clicked();
    void on_button_getSpec_clicked();
    void on_button_openFolder_clicked();
    void on_button_help_clicked();
    void on_button_cutout_clicked();
private:
    Ui::SpecExtract *ui;
    //自定义成员函数:
    void readDifFile(TCHAR const*filename);
    void readConfFile(TCHAR const* filename);

    //自定义变量部分：

    TCHAR folder_name[MAX_PATH];
    QString conf_filename;
    QString dif_filename;
    unsigned time_pos;
    //存储偏差值数据
    vector<Point> dif_vec=vector<Point>(36,Point(0,0));
    //存储增益等配置数据
    vector<Point>conf_vec = vector<Point>(36, Point(0, 0));
    //存储增益等配置数据线性组合后的归一化系数
    vector<double>coef_vec = vector<double>(36,0);
    int specextract_flags=0;
};

#endif // SPECEXTRACT_H
