#include "stdafx.h"
#include "GetTestImage.h"
#include<iostream>
#include<windows.h>
#include "afxdialogex.h"

#include "ServerSampleGenerator.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


std::vector<std::vector<char>> GetTestImage::get_ImageFingerprint(std::string image_path,int mask)
{
	Mat src, result;
	IplImage* test_Image = NULL;
	std::vector< std::vector<char> > v;
	
	try {
		src = imread(image_path);
		cvtColor(src, result, CV_BGR2GRAY);//转灰度图
		test_Image = &IplImage(result);
		ServerSampleGenerator *s = new ServerSampleGenerator(test_Image->width, test_Image->height);
		if (mask == 7) {
			v = s->GenerateServerSample(test_Image, test_Image->width / 2, test_Image->height / 2, test_Image->width / 2, test_Image->height / 2, 7);
		}
		else if (mask == 5) {
			v = s->GenerateServerSample(test_Image, test_Image->width / 2, test_Image->height / 2, test_Image->width / 2, test_Image->height / 2, 5);
		}
		//cvReleaseImage(&test_Image);
	}
	catch (Exception exception) {
		MessageBox(NULL,_T("未选择文件"), _T("提示"),MB_ICONASTERISK);
	}
	
	return v;
}

CString GetTestImage::vector_to_CString(std::vector<std::vector<char>> u)
{
	CString m = _T("");
	CString tmpstr = _T("");
	//std::vector<char>a;
	vector<vector<char>>::iterator interator = u.begin();
	for (;interator != u.end();++interator) {
		vector<char> fingerprint = *interator;
		vector<char>::iterator inner = fingerprint.begin();
		while (inner != fingerprint.end()) {
			//a.push_back(*inner);
			tmpstr.Format(_T("%02x"), (unsigned char)*inner);
			m += tmpstr;
			inner++;
		}
	}
	return m;
}

