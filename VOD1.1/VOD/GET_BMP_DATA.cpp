#include "stdafx.h"

#include "GET_BMP_DATA.h"

#define cvQueryHistValue_1D( hist, idx0 )	((float)cvGetReal1D((hist)->bins, (idx0)))


#define PICBLACKTHRESHOLD 0.91 //zls
#define GRTHISTTHRESHPLD 22

int Get_BMP_data(IplImage *pImg, IplImage *pImg_temp, IplImage *pImgBlackDecide, BYTE *lpBmpData, RECT &captureRect)
{
	BYTE *lpBmpData_Position = lpBmpData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	int nChanel_total_num = 3;

	for (int height = 0; height<pImg_temp->height; height++)
	{
		for (int width = 0; width<pImg_temp->width; width++)
		{
			for (int nChanel = 0; nChanel < nChanel_total_num; nChanel++)
			{
				pImg_temp->imageData[(pImg_temp->height - 1 - height)*pImg_temp->width*nChanel_total_num + width * nChanel_total_num + nChanel] =
					lpBmpData_Position[height*pImg_temp->width*nChanel_total_num + width * nChanel_total_num + nChanel];//???????
			}
		}
	}
	cvCvtColor(pImg_temp, pImg, CV_BGR2GRAY);
	cvSetImageROI(pImg, cvRect(cvRound(captureRect.left * 720 / 1920 - pImgBlackDecide->width / 2),
		cvRound(captureRect.top * 576 / 1080 - pImgBlackDecide->height / 2), pImgBlackDecide->width, pImgBlackDecide->height));
	cvCopy(pImg, pImgBlackDecide);
	cvResetImageROI(pImg);
	if (ImageBlackDecide(&pImgBlackDecide)<0)
	{
		return -2;
	}
	else
	{
		return 1;
	}
}
//ÅÐ¶ÏÖ¡Í¼ÊÇ·ñÊÇºÚÆÁ
int ImageBlackDecide(IplImage **ppImage)
{
	int nHistSize = 256;
	float fRange[] = { 0, 256 };
	float *pfRanges[] = { fRange };
	CvHistogram *pcvHistogram = cvCreateHist(1, &nHistSize, CV_HIST_ARRAY, pfRanges);
	cvCalcHist(ppImage,pcvHistogram);

	float HistValue_sum = 0;
	for (int i = 0; i < GRTHISTTHRESHPLD; i++)
	{
		HistValue_sum = HistValue_sum + cvQueryHistValue_1D(pcvHistogram, i); ////ÏñËØÖ±·½¿é´óÐ¡µþ¼Ó
	}
	cvReleaseHist(&pcvHistogram);

	if (HistValue_sum >= ((*ppImage)->width*(*ppImage)->height*PICBLACKTHRESHOLD))
	{
		return -1;
	}
	else
	{
		return 1;
	}
}