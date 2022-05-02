#include "auto_templatematching.h"
#include "ui_auto_templatematching.h"

#include<atlconv.h>
#include<qmessagebox.h>
#include<thread>
#include<qprogressbar.h>

Auto_TemplateMatching::Auto_TemplateMatching(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Auto_TemplateMatching)
{
    ref_pos = Point(-1, -1);
    ref_size = Point(0, 0);
    low_threshold1 = 20;
    low_threshold2 = 20;
    ui->setupUi(this);
}

Auto_TemplateMatching::~Auto_TemplateMatching()
{
    delete ui;
}

void Auto_TemplateMatching::on_button_openFolder_clicked()
{
    BROWSEINFO bi = FolderExplorer();
    wcscpy(folder_name, bi.pszDisplayName);
    ui->label_opened_folder->setText(QString::fromStdWString(L"已打开文件夹：") + QString::fromStdWString(folder_name));

    return;
}

void CannyTrackbarCallbackAT(int, void*userdata)
{
    Auto_TemplateMatching* at_temp = (Auto_TemplateMatching*)userdata;
    cv::Canny(at_temp->img1, at_temp->img1_, at_temp->low_threshold1, at_temp->low_threshold1 * 3);
    imshow("canny", at_temp->img1_);
}
void Auto_TemplateMatching::on_button_canny_clicked()
{
    if (img1.empty())
    {
        QMessageBox::information(this, "warning", QString::fromStdWString(L"没有已打开图片"), QMessageBox::Ok);
        return;
    }
    imshow("canny", img1);
    namedWindow("canny", WINDOW_AUTOSIZE);
    createTrackbar("Threshold", "canny", &low_threshold1, 255, CannyTrackbarCallbackAT);

}

//鼠标追踪回调函数
void OnMouseForRefAT(int event, int x, int y, int, void* userdata)
{

    if (event == EVENT_LBUTTONDOWN)
    {
        ((Auto_TemplateMatching*)(userdata))->ref_pos = Point(x, y);
        return;
    }
    if (event == EVENT_LBUTTONUP)
    {
        Auto_TemplateMatching* at_instance=((Auto_TemplateMatching*)(userdata));
        at_instance->ref_size = Point(x - at_instance->ref_pos.x, y - at_instance->ref_pos.y);
        at_instance->ui->lineEdit_refPos->setText(QString::number(at_instance->ref_pos.x) + " " + QString::number(at_instance->ref_pos.y));
        at_instance->ui->lineEdit_refSize->setText(QString::number(at_instance->ref_size.x) + " " + QString::number(at_instance->ref_size.y));
        at_instance->ref_pos = Point(-1, -1);
        return;
    }
    if (event == EVENT_MOUSEMOVE)
    {
        Auto_TemplateMatching* at_instance = ((Auto_TemplateMatching*)(userdata));
        if (at_instance->ref_pos.x == -1)
        {
            return;
        }
        at_instance->img1_.copyTo(at_instance->img1_clone);
        //绘制矩形框，从ref_pos到当前位置
        rectangle(at_instance->img1_clone, at_instance->ref_pos, Point(x, y), Scalar(255, 0, 0));
        imshow("select sample", at_instance->img1_clone);
        return;
    }
    return;
}

void Auto_TemplateMatching::on_button_cutout_clicked()
{
    if (img1.empty())
    {
        QMessageBox::information(this, "warning", QString::fromStdWString(L"没有已打开图片"), QMessageBox::Ok);
        return;
    }
    Canny(img1, img1_clone, low_threshold1, low_threshold1 * 3);
    imshow("select sample", img1_);
    setMouseCallback("select sample", OnMouseForRefAT, this);
    return;
}

void Auto_TemplateMatching::on_button_calDif_clicked()
{

}

void CalDifAT(Mat& img1_, Mat& img2_, vector<double>& dif_arr, Auto_TemplateMatching* tp)
{

}



namespace Ui {
    class myclass
    {
    public:
        int x;
        myclass()
        {
            x = 1;
        }
    };
}

void func()
{
    myclass m1;
}