#pragma once
#include<fstream>
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
#define ROW_CHAR_LENGTH 300
//使用csv存储光谱数据文件
//将图片的每个波长的亮度数据存在某一列中，用逗号分开


//读取整行,row从0开始
bool csvReadRow(fstream& fs, const unsigned& row, vector<int>& arr)
{
	if (!fs.is_open())
	{
		
		return false;
	}
	char buf[ROW_CHAR_LENGTH];
	char value_char[10];
	stringstream ss;
	for (int i = 0; i < row; i++)
	{
		fs.getline(buf, ROW_CHAR_LENGTH);
	}
	fs.getline(buf, ROW_CHAR_LENGTH);
	ss << buf;
	while (!ss.eof())
	{
		ss.getline(value_char, 10, ',');
		arr.push_back(atoi(value_char));
	}
	return true;
}

//读取整列

bool csvReadCol(fstream& fs, const unsigned& col, vector<int> &arr)
{
	if (!fs.is_open())
	{
		return false;
	}
	char value_char[10];
	char buf[ROW_CHAR_LENGTH];
	while (!fs.eof())
	{
		fs.getline(value_char, 10, ',');
		fs.getline(buf, ROW_CHAR_LENGTH);
		arr.push_back(atoi(value_char));
	}
	return true;
}


//读取某坐标处值,pos的x为行序数，y为列
bool csvReadPos(fstream& fs, cv::Point pos, int& value)
{
	if (!fs.is_open())
	{
		return false;
	}
	char buf[ROW_CHAR_LENGTH];
	char value_char[10];
	stringstream ss;
	for (int i = 0; i < pos.x; i++)
	{
		fs.getline(buf, ROW_CHAR_LENGTH);
	}
	for (int i = 0; i < pos.y; i++)
	{
		fs.getline(value_char, 10, ',');
	}
	fs.getline(value_char, 10, ',');
	ss << value_char;
	ss.getline(value_char, 10);
	value = atoi(value_char);
	return 1;
}
//在文件末尾添加一行数据（自动在开头添加换行）
bool csvWriteRow(fstream& fs, unsigned const &row, vector<double>& arr)
{

	if (!fs.is_open())
	{
		return false;
	}
	fs.write("\n", 1);
	char value_char[10];
	for (vector<double>::iterator i = arr.begin(); i != arr.end(); i++)
	{
		
		itoa(int(*i),value_char,10);
		fs.write(value_char, strlen(value_char));
		fs.write(",", 1);
	}
	return true;
}
//修改某坐标处值（暂时没有高效方式实现！！并且也用不到）
bool csvWritePos(fstream& fs, cv::Point pos, int& value)
{

	if (!fs.is_open())
	{
		return false;
	}
	char buf[ROW_CHAR_LENGTH];
	char value_char[10];
	
	for (int i = 0; i < pos.x; i++)
	{
		fs.getline(buf, ROW_CHAR_LENGTH);
	}
	for (int i = 0; i < pos.y; i++)
	{
		fs.getline(value_char, 10, ',');
	}

}
//在某坐标后插入某值
bool csvInsertPos(fstream& fs, cv::Point pos, int& value)
{
	


}
//在某行后插入某行
bool csvInsertRow(fstream& fs, unsigned const& row, int* arr)
{

}

//在某列后插入某列
bool csvInsertCol(fstream& fs, unsigned const& col, int* arr)
{

}