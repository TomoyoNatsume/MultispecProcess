#include "specextract.h"
#include "ui_specextract.h"
#include<qmessagebox.h>
#include<regex>
#include<atlconv.h>
Mat img_se, img_se_clone;
Point ref_se_pos(-1, -1), ref_se_size(0, 0);
bool mouse_flag_se=false;


//鼠标追踪回调函数
void OnMouseToRSpecExact(int event, int x, int y, int, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        ref_se_pos = Point(x, y);
        mouse_flag_se = true;
        return;
    }
    if (event == EVENT_LBUTTONUP)
    {
        ref_se_size = Point(x - ref_se_pos.x, y - ref_se_pos.y);
        SpecExtract* se_instance = (SpecExtract*)(userdata);
        se_instance->ui->label_ref->setText(QString::number(ref_se_pos.x) + " " + QString::number(ref_se_pos.y)
            +";"+QString::number(ref_se_size.x)+" "+QString::number(ref_se_size.y));
        //ref_se_pos = Point(-1, -1);
        mouse_flag_se = false;
        return;
    }
    if (event == EVENT_MOUSEMOVE)
    {
        //if (ref_se_pos.x == -1)
        //{
        //    return;
        //}
        if (mouse_flag_se == false)
        {
            return;
        }
        img_se.copyTo(img_se_clone);
        //绘制矩形框，从ref_se_pos到当前位置
        rectangle(img_se_clone, ref_se_pos, Point(x, y), Scalar(255, 0, 0));
        imshow("select sample", img_se_clone);
        return;
    }
    return;
}

SpecExtract::SpecExtract(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SpecExtract)
{
    ui->setupUi(this);
}
void SpecExtract::on_button_openFolder_clicked()
{
    BROWSEINFO bi = FolderExplorer();
    wcscpy(folder_name, bi.pszDisplayName);
    specextract_flags = specextract_flags | 0b00000001;
    ui->label_opened_folder->setText(QString::fromStdWString(L"已打开文件夹：") + QString::fromStdWString(folder_name));
    USES_CONVERSION;
    dif_filename = QString::fromStdWString(folder_name) + "\\OffsetData.csv";
    conf_filename = QString::fromStdWString(folder_name) + "\\gpcx.ini";
    dif_vec.clear();
    this->readDifFile(dif_filename.toStdWString().c_str());
    this->readConfFile(conf_filename.toStdWString().c_str());
    return;
}
void SpecExtract::readDifFile(TCHAR const* filename)
{
    fstream fs(filename, ios::in);
    if (!fs.is_open())
    {
        int i;
        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"打开文件失败"), QMessageBox::Ok);
        return;
    }
    string str;
    while (!fs.eof())
    {

        getline(fs, str);
        std::regex reg("[-]?\\d+\\s[-]?\\d+");
        string parse1, parse2, parse3;
        if (std::regex_match(str, reg))
        {
            std::regex space("\\s");
            std::sregex_token_iterator pos(str.begin(), str.end(), space, -1);
            Point dif;
            dif.x = atoi(pos->str().c_str());
            pos++;
            dif.y = atoi(pos->str().c_str());
            dif_vec.push_back(dif);
            cout << dif.x << ',' << dif.y << endl;
        }
    }
    WCHAR header_file_name[MAX_PATH];
    wcscpy(header_file_name, folder_name);
    wcscat(header_file_name, L"\\OffsetDataHeader.txt");
    fstream fs_header(header_file_name, ios::in);
    string buffer;
    //自动获取ref尺寸和位置
    if (fs_header.is_open())
    {
        Point ref_pos, ref_size;
        regex reg1("Position:.*");
        regex reg2("Size:.*");
        regex reg3("[-]?\\d+");
        bool b1 = 0, b2 = 0;
        while (!fs.eof())
        {
            fs >> buffer;
            stringstream ss;
            if (regex_match(buffer, reg1))
            {
                cmatch m;
                regex_search(buffer.c_str(), m, reg3);
                if (m.size() != 2)break;
                ss << m[0];
                ss >> ref_pos.x;
                ss << m[1];
                ss >> ref_pos.y;
                b1 = true;
            }
            if (regex_match(buffer, reg2))
            {
                cmatch m;
                regex_search(buffer.c_str(), m, reg3);
                if (m.size() != 2)break;
                ss << m[0];
                ss >> ref_size.x;
                ss << m[1];
                ss >> ref_size.y;
                b2 = true;
            }

        }
        if (b1 && b2)
        {
            this->ui->lineEdit_refPos->setText(QString::number(ref_pos.x) + " " + QString::number(ref_pos.y));
            this->ui->lineEdit_refSize->setText(QString::number(ref_size.x) + " " + QString::number(ref_size.y));
        }
    }
    ui->label_opened_dif_file->setText(QString::fromStdWString(L"已打开文件：") + dif_filename);
    return;
}
void SpecExtract::on_button_openDifFile_clicked()
{
    OPENFILENAME file;
    file = FileExplorer(folder_name);
    if (file.lpstrFile == NULL)
    {
        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"打开文件失败"), QMessageBox::Ok);
        return;
    }
    dif_filename = QString::fromStdWString(file.lpstrFile);
    this->readDifFile(file.lpstrFile);
    return;
}
void SpecExtract::readConfFile(TCHAR const* filename)
{
    fstream fs(filename, ios::in);
    if (!fs.is_open())
    {
        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"打开文件失败"), QMessageBox::Ok);
        return;
    }
    std::regex reg1("integration_time=\\d+");
    std::regex reg2("gain=\\d+");
    std::regex reg;
    string str;
    int i = 0;
    while (!fs.eof())
    {
        getline(fs, str);
        if (std::regex_match(str, reg1))
        {
            reg = std::regex("\\d+");
            std::sregex_token_iterator pos1(str.begin(), str.end(), reg, 0);
            conf_vec[i].x = atoi(pos1->str().c_str());
        }
        else {
            if (std::regex_match(str, reg2))
            {
                reg = std::regex("\\d+");
                std::sregex_token_iterator pos2(str.begin(), str.end(), reg, 0);
                conf_vec[i].y = atoi(pos2->str().c_str());
                i++;
            }
        }
    }
    for (i = 0; i < 36; i++)
    {
        coef_vec[i] = conf_vec[i].x * conf_vec[i].y / 10000;
    }
    ui->label_opened_conf_file->setText(QString::fromStdWString(L"已打开文件：") + conf_filename);
    return;
}
void SpecExtract::on_button_openConfFile_clicked()
{
    OPENFILENAME file;
    file = FileExplorer(folder_name);
    if (file.lpstrFile == NULL)
    {
        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"打开文件失败"), QMessageBox::Ok);
        return;
    }
    conf_filename = QString::fromStdWString(file.lpstrFile);
    this->readConfFile(file.lpstrFile);
    return;
}

void SpecExtract::on_button_cutout_clicked()
{
    if (!(specextract_flags && 0b00000001))
    {

        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"请先选择文件夹"), QMessageBox::Ok);
        return;
    }
    USES_CONVERSION;
    TCHAR file_name[MAX_PATH];
    char num2[10];
    time_pos = ui->spinBox->value();
    itoa(time_pos, num2, 10);
    wcscpy(file_name, folder_name);
    wcscat(file_name, L"\\bmp\\13\\out.");
    time_pos < 10 ? wcscat(file_name, L"000") : wcscat(file_name, L"00");
    wcscat(file_name, A2W(num2));
    wcscat(file_name, L".bmp");
    img_se = imread(W2A(file_name), IMREAD_COLOR);
    if (img_se.empty()) {
        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"打开图片失败"), QMessageBox::Ok);
        return;
    }
    imshow("select sample", img_se);
    setMouseCallback("select sample", OnMouseToRSpecExact, this);
    return;


}
void SpecExtract::on_button_getSpec_clicked()
{
    if (!(specextract_flags&&0b00000001))
    {

        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"请先选择文件夹"), QMessageBox::Ok);
        return;
    }
    if (conf_filename.isEmpty() || dif_filename.isEmpty()) {
        QMessageBox::warning(this, "Warning", QString::fromStdWString(L"请先打开配置文件"), QMessageBox::Ok);
        return;
    }
    USES_CONVERSION;
    int pic_order=ui->spinBox->value();
    time_pos = ui->spinBox->value();
    //String filename(folder_name);
    cv::Mat img;
    char num2[10];
    int x, y;

    //读取样本框位置

    bool toInt_isok1, toInt_isok2, toInt_isok3, toInt_isok4;
    ref_se_pos.x = ui->lineEdit_refPos->text().split(' ')[0].toInt(&toInt_isok1);
    ref_se_pos.y = ui->lineEdit_refPos->text().split(' ')[1].toInt(&toInt_isok2);
    ref_se_size.x = ui->lineEdit_refSize->text().split(' ')[0].toInt(&toInt_isok3);
    ref_se_size.y = ui->lineEdit_refSize->text().split(' ')[1].toInt(&toInt_isok4);
    
    //文件流
    fstream fs;
    stringstream ss;
    TCHAR spec_file_name[MAX_PATH],spec_info_file_name[MAX_PATH];
    wcscpy(spec_file_name, folder_name);
    wcscpy(spec_info_file_name, folder_name);
    wcscat(spec_file_name, L"\\spec_file.csv");
    wcscat(spec_info_file_name, L"\\spec_info_file.txt");
    fs.open(spec_file_name,ios::in|ios::out|ios::trunc);
    //char header_buf[100];
    //strcpy(header_buf, "time point=");
    //strcat(header_buf, num2);
    //strcat(header_buf, "\nsample position=");
    //itoa(ref_se_pos.x)
    //strcat(header_buf, num2);
    //fs.write(header_buf, 100);
    fstream fs_info;
    fs_info.open(spec_info_file_name, ios::in | ios::out | ios::trunc);
    fs_info << "Time point=" << time_pos << endl;
    fs_info << "Sample position x=" << ref_se_pos.x << endl;
    fs_info << "Sample position y=" << ref_se_pos.y << endl;
    fs_info << "Sample size x=" << ref_se_size.x << endl;
    fs_info << "Sample size y=" << ref_se_size.y << endl;
    for (int i = 0; i < 36; i++)
    {
        stringstream ss;
        char file_name[MAX_PATH];
        ss << W2A(folder_name)<<"\\bmp\\"<<setfill('0')<<setw(2)<<i<<"\\out."<<setw(4)<<time_pos<<".bmp";
        ss >> file_name;
        img = imread((file_name), IMREAD_COLOR);
        char* test = (file_name);
        //fs << "Channel " << i << ":";
        if (img.empty())
        {
            continue;
            fs << endl;
        }
        cvtColor(img, img, cv::COLOR_RGB2GRAY);
        vector<double>value_vec;
        for (int j = ref_se_pos.y; j < ref_se_size.y+ref_se_pos.y; j++)
        {
            for (int k = ref_se_pos.x; k < ref_se_size.x+ ref_se_pos.x; k++)
            {
                x = (k + dif_vec[i].x)>0? (k + dif_vec[i].x):0;
                x = x > 959 ? 959 : x;
                y = (j + dif_vec[i].y) > 0 ? (j + dif_vec[i].y) : 0;
                y = y > 539 ? 539 : y;
                fs << int(img.at<uchar>(y, x))*coef_vec[i] << ',';
            }
        }
        fs.seekp(-1,ios::cur);
        fs << endl;
    }
    QMessageBox::information(this, "完成", QString::fromStdWString(L"提取成功"), QMessageBox::Ok);
    return;
}
void SpecExtract::on_button_help_clicked()
{
    QMessageBox::information(this, "帮助", QString::fromStdWString(L"1.只支持bmp格式"
        "\n2.序号从0开始\n3.默认的通道数为34\n4.最多第99张照片\n5.计算完的光谱数据放在工程文件夹下的spec_file.csv文件中"), QMessageBox::Ok);
}


SpecExtract::~SpecExtract()
{
    delete ui;
}
