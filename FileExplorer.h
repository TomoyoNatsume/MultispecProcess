#pragma once
#include <iostream>
#include <windows.h>
#include<tchar.h>
#include<ShlObj.h>
using namespace std;
 static OPENFILENAME& FileExplorer(const TCHAR* str=_T("C:\\")) {

	//打开文件管理窗口
	static TCHAR szBuffer[MAX_PATH] = { 0 };
	OPENFILENAME file = { 0 };
	file.hwndOwner = NULL;
	file.lStructSize = sizeof(file);
	file.lpstrFilter = _T("图片文件\0*.bmp;*.jpg;*.jpeg;*.png;*.tiff;*.*\0");//要选择的文件后缀
	file.lpstrInitialDir = (str);//默认的文件路径
	file.lpstrFile = szBuffer;//存放文件的缓冲区
	file.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	file.nFilterIndex = 0;
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;//标志.如果是多选要加上OFN_ALLOWMULTISELECT
	BOOL bSel = GetOpenFileName(&file);
	if (!bSel)
	{
		file.lpstrFile = NULL;
	}
	return file;
}


 static BROWSEINFO& FolderExplorer() {
	 //打开文件夹管理窗口
	 static TCHAR szBuffer[MAX_PATH] = { 0 };
	 BROWSEINFO bi;
	 ZeroMemory(&bi, sizeof(BROWSEINFO));
	 bi.hwndOwner = NULL;
	 bi.pszDisplayName = szBuffer;
	 bi.lpszTitle = _T("从下面选文件夹目录:");
	 bi.ulFlags = BIF_RETURNFSANCESTORS;	 
	 LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	 if (NULL == idl)
	 {
		 return bi;
		 
	 }
	 SHGetPathFromIDList(idl, szBuffer);
	 return bi;
 }