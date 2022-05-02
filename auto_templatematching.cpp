#include "auto_templatematching.h"
#include "ui_auto_templatematching.h"

#include<atlconv.h>

Auto_TemplateMatching::Auto_TemplateMatching(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Auto_TemplateMatching)
{
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
    specextract_flags = specextract_flags || 0b00000001;
    ui->label_opened_folder->setText(QString::fromStdWString(L"已打开文件夹：") + QString::fromStdWString(folder_name));

    return;
}

void Auto_TemplateMatching::on_button_canny_clicked()
{
    MouseEventFlags f;

}

void Auto_TemplateMatching::on_button_calDif_clicked()
{

}

void Auto_TemplateMatching::on_button_cutout_clicked()
{

}