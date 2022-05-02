#include "pca.h"
#include "ui_pca.h"

PCA::PCA(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PCA)
{
    ui->setupUi(this);
}

PCA::~PCA()
{
    delete ui;
}
