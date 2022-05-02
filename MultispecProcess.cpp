#include "MultispecProcess.h"
#include"FileExplorer.h"
#include"OnMouse.h"
#include<atlconv.h>

MultispecProcess::MultispecProcess(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}


void MultispecProcess::on_openFileButton_clicked()
{

    USES_CONVERSION;
    OPENFILENAME file;
    file = FileExplorer();
    if (file.lpstrFile == NULL)
    {
        ui.fileName->setText(QString::fromStdWString(L"打开文件失败"));
        return;
    }
    QString qstr_file_name;
    String astr_file_name;
    astr_file_name = W2A(file.lpstrFile);
    wchar_t wstr_file_name[MAX_PATH] = L"已打开文件：";
    wcscat(wstr_file_name, file.lpstrFile);
    qstr_file_name = QString::fromStdWString(wstr_file_name);
    ui.fileName->setText(qstr_file_name);
    img = imread(astr_file_name);
    image = img.clone();
    //窗口名尽量不要改，鼠标回调函数里会用到。
    namedWindow("ImagePointer", WINDOW_AUTOSIZE);
    imshow("ImagePointer", img);
    setMouseCallback("ImagePointer", OnMouse, this);
    return;
}

void MultispecProcess::on_actionTemplateMatching_triggered()
{
    tm = new TemplateMatching(this);
    tm->show();
    return;
}

void MultispecProcess::on_actionAuto_TemplateMatching_triggered()
{
    auto_tm = new Auto_TemplateMatching(this);
    auto_tm->show();
    return;

}

void MultispecProcess::on_action_specExtract_triggered()
{
    se = new SpecExtract(this);
    se->show();
    return;
}