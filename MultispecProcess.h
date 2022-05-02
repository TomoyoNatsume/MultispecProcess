#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MultispecProcess.h"
#include<opencv2/opencv.hpp>
#include<QFileSystemModel>
#include<Windows.h>
#include<WinBase.h>
#include<qdesktopservices.h>
#include<iostream>
#include"testchildwindow.h"
#include"templatematching.h"
#include"specextract.h"
#include"auto_templatematching.h"
using namespace std;
using namespace cv;

extern Mat img;
extern Mat image;
extern string posOutput;

class MultispecProcess : public QMainWindow
{
    Q_OBJECT

public:
    MultispecProcess(QWidget *parent = Q_NULLPTR);

    //友元函数
    friend void OnMouse(int event, int x, int y, int, void*);
    friend COORD GetConsoleCursorPosition(HANDLE hConsoleOutput);

//槽函数
private slots:
    void on_openFileButton_clicked();
    void on_actionTemplateMatching_triggered();
    void on_action_specExtract_triggered();
    void on_actionAuto_TemplateMatching_triggered();

private:
    Ui::MultispecProcessClass ui;

    //子框
    TestChildWindow* test_child_window;
    TestChildWindow tcw;
    TemplateMatching* tm;
    SpecExtract* se;
    PCA* pca;
    Auto_TemplateMatching* auto_tm;
};

//(自定义)图片处理类
class PicAccess
{
public:
    PicAccess()
    {
        m_path = NULL;
    }
    PicAccess(char* path)
    {
        m_path = path;
        m_img = cv::imread(path);

    }
    void ShowPic()
    {
        cv::imshow("Pic", m_img);
        return;
    }
    void ShowPic(char* title)
    {
        cv::imshow(title, m_img);
        return;
    }
    char* m_path;
    cv::Mat m_img;

};
