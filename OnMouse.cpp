
#include"OnMouse.h"
//全局变量
// 存储原图像
Mat img;
// 存储原图像以及画点
Mat image;
string posOutput;

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
	{
		return cbsi.dwCursorPosition;
	}
	else
	{
		COORD invalid = { 0,0 };
		return invalid;
	}
}
//鼠标追踪回调函数
void OnMouse(int event, int x, int y, int, void*userdata)
{
	if (event == EVENT_LBUTTONUP)
	{
		QString current_pos, current_rgb;
		current_pos = QString::asprintf("%d,%d", x, y);
		current_rgb = QString::asprintf("%d,%d,%d", 
			(int)(image.data[y * image.step + x * 3 + 2]), 
			(int)(image.data[y * image.step + x * 3 + 1]), 
			(int)(image.data[y * image.step + x * 3])
		);
		MultispecProcess* qt_instance = (MultispecProcess*)(userdata);
		qt_instance->ui.label_pointedpos->setText(current_pos);
		qt_instance->ui.label_pointedrgb->setText(current_rgb);
		imshow("ImagePointer", image);
		return;
	}
	if (event == EVENT_MOUSEMOVE)
	{
		QString current_pos, current_rgb;
		current_pos = QString::asprintf("%d,%d", x, y);
		current_rgb = QString::asprintf("%d,%d,%d",
			(int)(image.data[y * image.step + x * 3 + 2]),
			(int)(image.data[y * image.step + x * 3 + 1]),
			(int)(image.data[y * image.step + x * 3])
		);
		MultispecProcess* qt_instance = (MultispecProcess*)(userdata);
		qt_instance->ui.label_currentpos->setText(current_pos);
		qt_instance->ui.label_currentrgb->setText(current_rgb);

		return;
	}
	return;
}
