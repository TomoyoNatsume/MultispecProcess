﻿#include "auto_templatematching.h"
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
	if (ui->lineEdit_refPos->text().split(' ').size() != 2 || ui->lineEdit_refSize->text().split(' ').size() != 2)
	{
		QMessageBox::warning(this, "Warning", QString::fromStdWString(L"请输入正确格式的坐标值/尺寸值"), QMessageBox::Ok);
		return;
	}

	bool toInt_isok1, toInt_isok2, toInt_isok3, toInt_isok4;
	ref_pos.x = ui->lineEdit_refPos->text().split(' ')[0].toInt(&toInt_isok1);
	ref_pos.y = ui->lineEdit_refPos->text().split(' ')[1].toInt(&toInt_isok2);
	ref_size.x = ui->lineEdit_refSize->text().split(' ')[0].toInt(&toInt_isok3);
	ref_size.y = ui->lineEdit_refSize->text().split(' ')[1].toInt(&toInt_isok4);
	if (!(toInt_isok1 && toInt_isok2 && toInt_isok3 && toInt_isok4))
	{
		QMessageBox::warning(this, "Warning", QString::fromStdWString(L"请输入正确格式的坐标值/尺寸值"), QMessageBox::Ok);
		return;
	}


	if ((img1.data == NULL) || (img2.data == NULL)) {
		QMessageBox::warning(this, "Warning", QString::fromStdWString(L"读取图像失败"), QMessageBox::Ok);
		return;
	}

	int width = img1_.cols;
	int height = img1_.rows;
	//设置样本框
	ref_x = ref_pos.x;
	ref_y = ref_pos.y;
	ref_width = ref_size.x;
	ref_height = ref_size.y;
	//设置offx_range,offy_range:
    if (ui->checkBox_Overallscan->isChecked())
    {
        offx_range1 = 0;
        offy_range1 = 0;
        offx_range2 = width - ref_width + 1;
        offy_range2 = height - ref_height + 1;
        offx_range = offx_range2 - offx_range1;
        offy_range = offy_range2 - offy_range1;

    }
    else
    {
        offx_range1 = (ref_x - 200 < 0) ? 0 : (ref_x - 200);
        offy_range1 = (ref_y - 200 < 0) ? 0 : (ref_y - 200);
        offx_range2 = (ref_x + 200 + ref_width > width + 1) ? (width - ref_width + 1) : (ref_x + 200);
        offy_range2 = (ref_y + 200 + ref_height > height + 1) ? (height - ref_height + 1) : (ref_y + 200);
    }

	vector<double>dif_arr;
	double dif = 0, dif_ = 0;
	double d1, d2;
	//求均方差
	for (int i = offy_range1; i < offy_range2; i++)
	{
		for (int j = offx_range1; j < offx_range2; j++)
		{
			for (int i_ = 0; i_ < ref_height; i_++)
			{
				for (int j_ = 0; j_ < ref_width; j_++)
				{
					d1 = img1_.at<uchar>(ref_y + i_, ref_x + j_);
					d2 = img2_.at<uchar>(i_ + i, j_ + j);
					dif_ = (d1 * d2);
					//dif_ = dif_ * dif_;
					dif += dif_;
				}
			}
			prog = (i - offy_range1 + 1) / (offy_range2 - offy_range1);
			dif = dif / ref_height / ref_width;
			dif_arr.push_back(dif);
			dif = 0;
		}
	}



	vector<double>::iterator min_pos = max_element(dif_arr.begin(), dif_arr.end());
	int dif_pos = min_pos - dif_arr.begin();
	int minor = *min_pos;
	int offy = int(floor(double(dif_pos) / double(offx_range)));
	int offx = dif_pos - offy * offx_range;
	final_off.y = offy + offy_range1 - ref_y;
	final_off.x = offx + offx_range1 - ref_x;
	//ui->label_offset->setText(QString::number(final_off.x) + " " + QString::number(final_off.y));
	ref_pos = Point(-1, -1);

	return;

}

void CalDifAT(Mat& img1_, Mat& img2_, vector<double>& dif_arr, Auto_TemplateMatching* tp)
{

    double dif = 0, dif_ = 0;
    double d1, d2;
    //求均方差
    for (int i = tp->offy_range1; i < tp->offy_range2; i++)
    {
        for (int j = tp->offx_range1; j < tp->offx_range2; j++)
        {
            for (int i_ = 0; i_ < tp->ref_height; i_++)
            {
                for (int j_ = 0; j_ < tp->ref_width; j_++)
                {
                    d1 = img1_.at<double>(tp->ref_y + i_, tp->ref_x + j_);
                    d2 = img2_.at<double>(i_ + i, j_ + j);
                    dif_ = (d1 - d2);
                    //dif_ = (double(IndexGray(img1_, ref_x + j_, ref_y + i_)) - double(IndexGray(img2_, j_ + j, i_ + i)));
                    dif_ = dif_ * dif_;
                    dif += dif_;
                }
            }
            tp->prog = (i - tp->offy_range1 + 1) / (tp->offy_range2 - tp->offy_range1);
            dif = dif / tp->ref_height / tp->ref_width;
            dif_arr.push_back(dif);
            dif = 0;
        }
    }

    return;

}

void Auto_TemplateMatching::on_button_help_clicked()
{

}