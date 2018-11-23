#include "stdafx.h"
#include "afxdialogex.h"

#include <time.h>
#include <string>
#include <sstream>
#include <iostream>

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\highgui\highgui_c.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "ServerSampleGenerator.h"

using namespace std;
using namespace cv;

string int2str(int i) {
	string s;
	stringstream ss;//(s);   
	ss << i;
	return ss.str();
}
//void test(string aviPath, string picPath, int interval)
//{
//	IplImage* pFrame = NULL;
//	CvCapture* pCapture = NULL;
//	int k;
//	VideoCapture vc = VideoCapture(aviPath);
//	pCapture = cvCaptureFromAVI(aviPath.c_str());
//
//	k = vc.get(CAP_PROP_FRAME_COUNT);
//	//���֡��
//	k = cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_COUNT);
//	cout << k << endl;
//	for (int i = 0; i<k; i++)
//	{
//		pFrame = cvQueryFrame(pCapture); //ץȡһ֡   
//		if (i % interval == 0)
//		{
//			string sss = picPath;
//			string a = int2str((i - i % interval) / interval);
//			sss.append(a);
//			sss.append(".jpg");
//			const char *e = sss.c_str();
//			cvSaveImage(e, pFrame);
//		}
//	}
//	cvReleaseCapture(&pCapture);
//}


vector<char> generateCode(string aviPath, string picPath, int interval, int imgCentrex, int imgCentrey, int imgCentreWidth, int imgCentreHeigh)
{
	/*
		@param aviPath:��Ƶ·��
		@param picPath:֡ͼ����·��
		@param interval:��Ƶ���
		@param imgCentrex:��Ƶ��X����
		@param imgCentrey:��Ƶ��Y����
		��Ƶ������ֵ
		@param imgCentreWidth
		@param imgCentreHeight
		@return ָ����
	*/
	IplImage* pFrame = NULL;
	IplImage* result = NULL;
	CvCapture* pCapture = NULL;
	std::vector< std::vector<char> > v;
	vector<char> contactedFingerprints;
	ServerSampleGenerator *s = new ServerSampleGenerator(imgCentreWidth * 2, imgCentreHeigh * 2);
	pCapture = cvCaptureFromAVI(aviPath.c_str());
	//���֡��
	int k = cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_COUNT);
	//cout << "frame number:" << k << endl;
	int fps = cvGetCaptureProperty(pCapture, CV_CAP_PROP_FPS);

	for (int i = 0; i < k; i++)
	{
		pFrame = cvQueryFrame(pCapture); //ץȡһ֡   
		if (i % interval == 0 && (pFrame != NULL))
		{
			result = cvCreateImage(cvGetSize(pFrame), pFrame->depth, 1);//����image
			cvCvtColor(pFrame, result, CV_BGR2GRAY);//ת���ɻҶ�ͼ
			v = s->GenerateServerSample(result, imgCentrex, imgCentrey, imgCentreWidth, imgCentreHeigh);
			cvReleaseImage(&result);
			vector<vector<char>>::iterator interator = v.begin();
			while (interator != v.end()) {
				vector<char> fingerprint = *interator;
				vector<char>::iterator inner = fingerprint.begin();
				while (inner != fingerprint.end()) {
					contactedFingerprints.push_back(*inner);
					inner++;
				}
				interator++;
			}
		}
		if (i%fps == 0)
		{
			cout << "process secondes :" << i / fps << endl;
			//CClientDC dc(this);
			//CHAR outinfo[256] = { 0 };

			//CRect rect(10, 10, 10 + 800, 10 + 30);
			//sprintf(outinfo, "process secondes :%ld", i / fps);
			//dc.DrawText(outinfo, -1, rect, DT_LEFT | DT_SINGLELINE);

		}
	}
	cvReleaseCapture(&pCapture);
	delete s;
	return contactedFingerprints;
}