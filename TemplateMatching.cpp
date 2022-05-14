#include "templatematching.h"
#include "ui_templatematching.h"

#include<atlconv.h>
#include<qmessagebox.h>
#include<thread>
#include<qprogressbar.h>
Mat img1,img1_clone,img1_;
Mat img2,img2_;
Point ref_pos(-1,-1), ref_size(0,0),final_off;
int ref_x, ref_y, ref_width, ref_height, offx_range1, offx_range2, offy_range1, offy_range2, offx_range, offy_range,low_threshold1=20,low_threshold2=20;
double prog = 0;
TemplateMatching::TemplateMatching(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TemplateMatching)
{
    ui->setupUi(this);
}

TemplateMatching::~TemplateMatching()
{
	img1.release();
	img1_clone.release();
	img2.release();
    delete ui;
}

void TemplateMatching::on_openFileButton1_clicked()
{
    USES_CONVERSION;
    OPENFILENAME file;
    file = FileExplorer();
    if (file.lpstrFile == NULL)
    {
        ui->label_filename1->setText(QString::fromStdWString(L"打开文件失败"));
        return;
    }
    OPENFILENAME* fileaddr = &file;
    QString qstr_file_name;
    char* astr_file_name;
    astr_file_name = W2A(file.lpstrFile);
    wchar_t wstr_file_name[MAX_PATH] = L"已打开文件：";
    wcscat(wstr_file_name, file.lpstrFile);
    qstr_file_name = QString::fromStdWString(wstr_file_name);
	string str_file_name(astr_file_name);
    img1 = imread(str_file_name,IMREAD_COLOR);
	if (img1.empty())
	{
		ui->label_filename1->setText(QString::fromStdWString(L"打开文件失败"));
		return;
	}
    ui->label_filename1->setText(qstr_file_name);
	cvtColor(img1, img1, cv::COLOR_RGB2GRAY);
	normalize(img1, img1, 0, 255, cv::NORM_MINMAX);
	cv::Canny(img1, img1_, low_threshold1, low_threshold1 * 3);
    return;
}
void TemplateMatching::on_openFileButton2_clicked()
{
    USES_CONVERSION;
    OPENFILENAME file;
    file = FileExplorer();
    if (file.lpstrFile == NULL)
    {
        ui->label_filename2->setText(QString::fromStdWString(L"打开文件失败"));
        return;
    }
    QString qstr_file_name;
    char* astr_file_name;
    astr_file_name = W2A(file.lpstrFile);
    wchar_t wstr_file_name[MAX_PATH] = L"已打开文件：";
    wcscat(wstr_file_name, file.lpstrFile);
    qstr_file_name = QString::fromStdWString(wstr_file_name);
    img2 = imread(astr_file_name, IMREAD_COLOR);
	if (img2.empty())
	{
		ui->label_filename1->setText(QString::fromStdWString(L"打开文件失败"));
		return;
	}
    ui->label_filename2->setText(qstr_file_name);
	cvtColor(img2, img2, cv::COLOR_RGB2GRAY);
	normalize(img2, img2, 0, 255, cv::NORM_MINMAX);
	cv::Canny(img2, img2_, low_threshold2, low_threshold2 * 3);
    return;
}
//提取单通道图片的索引处的亮度值（x、y从0开始）
unsigned char& IndexGray(Mat &img, const int &x, const int &y) {
	return img.data[y * img.step + x];
}
//输入单通道矩阵
void eff(Mat& img_src,Mat &img_des)
{
	double avg = 0;
	//先求平均值
	for (int i = 0; i < img_src.rows; i++)
	{
		for (int j = 0; j < img_src.cols; j++)
		{
			avg += double(img_src.at<uchar>(i,j));
		}
	}

	avg = avg / img_src.rows / img_src.cols;
	//所有像素减去平均值
	for (int i = 0; i < img_src.rows; i++)
	{
		for (int j = 0; j < img_src.cols; j++)
		{
			img_des.at<double>(i, j) = double(img_src.at<uchar>(i, j)) - avg;
		}
	}
	return ;
}

#define ParsePoint(str) Point(str.split(',')[0].toInt(),str.split(',')[1].toInt())
void CalDif(Mat &img1_, Mat &img2_, vector<double>& dif_arr,TemplateMatching*tp)
{

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
					d1 = img1_.at<double>(ref_y + i_, ref_x + j_);
					d2 = img2_.at<double>(i_ + i, j_ + j);
					dif_ = (d1 - d2);
					//dif_ = (double(IndexGray(img1_, ref_x + j_, ref_y + i_)) - double(IndexGray(img2_, j_ + j, i_ + i)));
					dif_ = dif_ * dif_;
					dif += dif_;
				}
			}
			prog = (i - offy_range1+1) / (offy_range2 - offy_range1);
			dif = dif / ref_height / ref_width;
			dif_arr.push_back(dif);
			dif = 0;
		}
	}

	return;
}

void TemplateMatching::on_button_calDif_clicked()
{
	//int ias = ui->lineEdit_refPos->text().split(' ')[0].toInt();
	if (ui->lineEdit_refPos->text().split(' ').size() != 2|| ui->lineEdit_refSize->text().split(' ').size() != 2)
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
	//Mat img1__, img2__;
	//Mat img1_(img1.rows, img1.cols, CV_8UC1);
	//Mat img2_(img2.rows, img2.cols, CV_8UC1);
	//eff(img1__, img1_);
	//eff(img2__, img2_);
	//img1__.release();
	//img2__.release();
	//Mat img3 = img1_ - img2_;
	
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
	//std::thread th_cal_dif(CalDif, std::ref(img1_), std::ref(img2_), std::ref(dif_arr),this);
	//std::thread th_show_progress(&TemplateMatching::ShowProgress, this);

	//th_cal_dif.detach();	
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
					dif_ = (d1* d2);
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
	ui->label_offset->setText(QString::number(final_off.x)+" " + QString::number(final_off.y));
	ref_pos = Point(-1, -1);

	return;
}


void TemplateMatching::on_button_help_clicked()
{
	LPCWSTR help_msg = L"1.通过<打开图片>按钮，分别打开两幅需要对其的图片\n"
		"2.在图1中确定一个样本框，程序将根据样本框进行对齐\n"
		"3.在<样本框位置>一栏填入样本框的x、y轴位置。两个整数，用空格分开，如：100 200\n"
		"4.同样地，在样本框尺寸中填入样本框的x、y方向长度。格式同上\n"
		"5.或者直接点击<样本框选取>按钮，手动绘制矩形框，结果会自动填入\n"
		"6.程序将给出计算出的两幅图片偏差值，显示在<偏差值>一栏中\n"
		"7.程序会自动将rgb图像转为灰度图像，再进行计算\n"
		"8.如果勾选全图扫描，则计算偏差时会更慢，但更精确。否则只扫描样本框附近的部分区域，速度更快";
	QMessageBox::information(this, "Help", QString::fromStdWString(help_msg), QMessageBox::Ok);
	return;
}

//鼠标追踪回调函数
void OnMouseToRef(int event, int x, int y, int, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		ref_pos = Point(x, y);
		return;
	}
	if (event == EVENT_LBUTTONUP)
	{
		ref_size = Point(x - ref_pos.x, y - ref_pos.y);
		TemplateMatching* tm_instance = (TemplateMatching*)(userdata);
		tm_instance->ui->lineEdit_refPos->setText(QString::number(ref_pos.x) + " " + QString::number(ref_pos.y));
		tm_instance->ui->lineEdit_refSize->setText(QString::number(ref_size.x) + " " + QString::number(ref_size.y));
		ref_pos = Point(-1, -1);
		return;
	}
	if (event == EVENT_MOUSEMOVE)
	{
		if (ref_pos.x == -1)
		{
			return;
		}
		img1_.copyTo(img1_clone);
		//绘制矩形框，从ref_pos到当前位置
		rectangle(img1_clone, ref_pos, Point(x, y), Scalar(255, 0, 0));
		imshow("select sample", img1_clone);
		return;
	}
	return;
}

void TemplateMatching::on_button_selectRef_clicked()
{
	if (img1.empty())
	{
		QMessageBox::information(this, "warning", QString::fromStdWString(L"没有已打开图片"), QMessageBox::Ok);
		return;
	}
	Canny(img1, img1_clone, low_threshold1, low_threshold1 * 3);
	imshow("select sample", img1_);
	setMouseCallback("select sample", OnMouseToRef, this);
	return;
}

void CannyTrackbarCallback1(int,void*)
{
	cv::Canny(img1, img1_, low_threshold1, low_threshold1 * 3);
	imshow("canny1",img1_);
}

void TemplateMatching::on_button_canny1_clicked()
{
	if (img1.empty())
	{
		QMessageBox::information(this, "warning", QString::fromStdWString(L"没有已打开图片"), QMessageBox::Ok);
		return;
	}
	imshow("canny1", img1);
	namedWindow("canny1", WINDOW_AUTOSIZE);
	createTrackbar("Threshold", "canny1", &low_threshold1, 255, CannyTrackbarCallback1);
}
void CannyTrackbarCallback2(int, void*)
{
	cv::Canny(img2, img2_, low_threshold2, low_threshold2 * 3);
	imshow("canny2", img2_);
}

void TemplateMatching::on_button_canny2_clicked()
{
	if (img2.empty())
	{
		QMessageBox::information(this, "warning", QString::fromStdWString(L"没有已打开图片"), QMessageBox::Ok);
		return;
	}
	imshow("canny2", img2);
	namedWindow("canny2", WINDOW_AUTOSIZE);
	createTrackbar("Threshold", "canny2", &low_threshold2, 255, CannyTrackbarCallback2);
}


void TemplateMatching::on_button_showEffect_clicked()
{
	if (img1.empty()||img2.empty())
	{
		QMessageBox::information(this, "warning", QString::fromStdWString(L"没有已打开图片"), QMessageBox::Ok);
		return;
	}
	if (ui->label_offset->text().isEmpty())
	{
		QMessageBox::information(this, "warning", QString::fromStdWString(L"请先点击<计算偏差>"), QMessageBox::Ok);
		return;
	}
	QStringList str_ref_pos=ui->lineEdit_refPos->text().split(" ");
	QStringList str_ref_size = ui->lineEdit_refSize->text().split(" ");
	if (str_ref_pos.size() != 2 || str_ref_size.size() != 2)
	{
		QMessageBox::information(this, "warning", QString::fromStdWString(L"请输入正确样本框"), QMessageBox::Ok);
		return;
	}
	int x=str_ref_pos[0].toInt(), y=str_ref_pos[1].toInt(), sizex= str_ref_size[0].toInt(), sizey=str_ref_size[1].toInt();
	Mat img_ref1(img1, Rect(x, y, sizex, sizey));
	Mat img_ref2(img2, Rect(x, y, sizex, sizey));
	Mat img_ref3(img2, Rect(x+final_off.x, y+final_off.y, sizex, sizey));
	imshow("fig1", img_ref1);
	imshow("fig2", img_ref2);
	imshow("fixed", img_ref3);
	namedWindow("fig1", WINDOW_NORMAL| WINDOW_GUI_EXPANDED);
	namedWindow("fig2", WINDOW_NORMAL);
	namedWindow("fixed", WINDOW_NORMAL);
	if (sizex < 200 || sizey < 200)
	{
		resizeWindow("fig1", Size(200, 200));
		resizeWindow("fig2", Size(200, 200));
		resizeWindow("fixed", Size(200, 200));
	}
	return;
}