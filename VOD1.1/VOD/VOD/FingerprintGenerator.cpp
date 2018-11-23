#include "stdafx.h"
#include "FingerprintGenerator.h"



FingerprintGenerator::FingerprintGenerator()
{
	FINGER_MASK_BYTE = 88;//+8;//10bit��ǰ����Ϊ���ڱ��ָ�����λ�ã�����4�����ڱ��ָ��������ĳ�������ϵ�λ����ʱԤ��
						  //��111010000101000011111110000101100001011011010110011111111100000010000101111100001110101100010000
						  //����Ϊ�ܵ����ɵ�ָ���뵫�ǽ��Ϊ��
						  //1110100001010000111111100001011000010110110101100111111111000000100001011111000011101011 0001 0000
						  //�����0001Ϊ��ǵĸ�ָ������ĳ�������ϵ�λ�ã������0000Ϊ��ָ���ڸ÷����ϵ�λ������Ϊ��0001�����ϣ������ƶ���
						  /************************************************************************/
						  /*                        ImgParameter������ʼ��                         */
						  /************************************************************************/
	//�ֿ��С7*7
	ImgParameter.ColNum = 7;
	ImgParameter.RowNum = 7;
	//ͼ���С140*140
	ImgParameter.ImageResizeHeight = 70;
	ImgParameter.ImageResizeWidth = 70;

	//ImgParameter.Imagepooling = 140;

	//ÿ��������в�����Ӧ�ñ�֤������
	ImgParameter.ColStep = ImgParameter.ImageResizeHeight / ImgParameter.ColNum;
	ImgParameter.RowStep = ImgParameter.ImageResizeWidth / ImgParameter.RowNum;//10

	//FINGER_MASK_BYTE2 = 64;
	//FINGER_MASK_BYTE3 = 16 * 16;
}

Mat FingerprintGenerator::pooling(Mat img, int overlap) 
{ 
	Mat pool_img = Mat((int)((img.rows - 1) / overlap) + 1, (int)((img.cols - 1) / overlap) + 1, CV_8UC1);	
	for (int col = 0, pool_col = 0; col < img.cols; col += overlap) 
	{ 
		for (int row = 0, pool_row = 0; row < img.rows; row += overlap) 
		{ 
			int minCol = min(col + overlap, img.cols);			
			int maxData = 0;			
			for (int poolX = col; poolX < minCol; poolX++)
			{ 
				int minRow = min(row + overlap, img.rows);			
				for (int poolY = row; poolY < minRow; poolY++) 
				{ 
					if (img.at<uchar>(poolY, poolX) > maxData)
					{ 
						maxData = img.at<uchar>(poolY, poolX);
					}
				} 
			}			
			pool_img.at<uchar>(pool_row, pool_col) = maxData;
			pool_row++; 
		}		
		pool_col++;
	}	
	return pool_img; 
}


std::vector<char> FingerprintGenerator::GenerateFingerprint(IplImage *src, char *FingerPosition_confirm)
{
	/*
	@param src:֡ͼ
	@param FingerPosition_confirm
	@return ָ����
	*/
	std::vector<char> fingerprint;
	if (NULL == src)
		return fingerprint;
	CHAR outinfo[512] = { 0 };
	//static int imgindex = 0;

	IplImage *ImgResize = cvCreateImage(cvSize(ImgParameter.ImageResizeWidth, ImgParameter.ImageResizeHeight), IPL_DEPTH_8U, 1);
	char *FingerOutput = new char[FINGER_MASK_BYTE];

	fingerprint.resize(FINGER_MASK_BYTE / 8);
	cvResize(src, ImgResize, CV_INTER_AREA);
	//cvResize(ImgResize, ImgResize, CV_INTER_LANCZOS4);
	//imgindex++;
	GenerateVideoFinger(ImgResize, FingerOutput, FingerPosition_confirm);
	TranslateFingerprint(fingerprint.begin(), fingerprint.end(), FingerOutput);

	cvReleaseImage(&ImgResize);
	delete[]FingerOutput;

	return fingerprint;

}


void FingerprintGenerator::GenerateVideoFinger(IplImage *src, char *FingerOutput, char *FingerPosition_confirm)
{
	/*
	@param src:֡ͼ
	@param FingerPosition_confirm
	@return NULL
	*/
	CvMat *sum = cvCreateMat(src->height + 1, src->width + 1, CV_32SC1);//���ڴ洢����ͼ
	cvIntegral(src, sum, NULL, NULL);

	
	Mat mat = cvarrToMat(sum);
	blur(mat, mat, Size(25,25));
	//bilateralFilter(mat, mat, 5, 5 , 5 );
	pooling(mat, 5);
	//normalize(mat,mat,0,255, NORM_MINMAX);  //��һ��
	CvMat b = mat;
	sum = &b;

	//���û���ͼ���γ�һ��ͼƬ��ָ����Ϣ
	int offset = 0;
	EightFingerCount(sum, FingerOutput + offset, 2, 2);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 2, 4);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 2, 6);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 4, 2);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 4, 6);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 6, 2);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 6, 4);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 6, 6);
	offset += 8;
	FourFingerCount(sum, FingerOutput + offset, 3, 3);
	offset += 4;
	FourFingerCount(sum, FingerOutput + offset, 3, 5);
	offset += 4;
	FourFingerCount(sum, FingerOutput + offset, 5, 3);
	offset += 4;
	FourFingerCount(sum, FingerOutput + offset, 5, 5);
	offset += 4;
	EightFingerCount(sum, FingerOutput + offset, 4, 4);
	//offset += 8;
	//AddFingerPosition(FingerOutput + offset,FingerPosition_confirm);
	cvReleaseMat(&sum);

}



void FingerprintGenerator::EightFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter)
{
	/*
	@param ImgIntegral:֡ͼ����
	@param FingerOutput:������ָ��������
	@param rCenter:�㷨���ĵ�x
	@param cCenter:�㷨���ĵ�y
	@return NULL
	*/
	int rCount = rCenter - 1;
	int cCount = cCenter - 1;
	int IntegralArray[3][3] = { { 0,0,0 },{ 0,0,0 },{ 0,0,0 } };
	int IntegralUpLeft = 0, IntegralUp = 0, IntegralLeft = 0, IntegralBottomRight = 0;
	int rUpLeftPix = 0;
	int cUpLeftPix = 0;
	int rArrayCount = 0;
	int cArrayCount = 0;
	for (; rCount <= rCenter + 1; ++rCount)
	{
		rUpLeftPix = (rCount - 1) * ImgParameter.RowStep;
		for (cCount = (cCenter - 1); cCount <= cCenter + 1; ++cCount)
		{
			cUpLeftPix = (cCount - 1) * ImgParameter.ColStep;
			IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix, cUpLeftPix);
			IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix + ImgParameter.RowStep, cUpLeftPix);
			IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix, cUpLeftPix + ImgParameter.ColStep);
			IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix + ImgParameter.RowStep, cUpLeftPix + ImgParameter.ColStep);
			IntegralArray[rArrayCount][cArrayCount] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
			++cArrayCount;
		}
		cArrayCount = 0;
		++rArrayCount;
	}
	int FingerOutputCount = 0;
	for (rArrayCount = 0; rArrayCount < 3; ++rArrayCount)
	{
		for (cArrayCount = 0; cArrayCount < 3; ++cArrayCount)
		{
			if ((rArrayCount == 1) && (cArrayCount == 1))
			{
				continue;
			}
			else
			{
				if (IntegralArray[1][1] - IntegralArray[rArrayCount][cArrayCount] >= 0)
				{
					FingerOutput[FingerOutputCount] = '1';
				}
				else
				{
					FingerOutput[FingerOutputCount] = '0';
				}
				++FingerOutputCount;
			}
		}
	}

}


void FingerprintGenerator::AddFingerPosition(char *FingerOutput, char *FingerPosition_confirm)
{
	for (int Finger_i = 0; Finger_i<8; Finger_i++)
	{
		FingerOutput[Finger_i] = FingerPosition_confirm[Finger_i];
	}
}


void FingerprintGenerator::FourFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter)
{
	/*
	@param ImgIntegral:֡ͼ����
	@param FingerOutput:������ָ��������
	@param rCenter:�㷨���ĵ�x
	@param cCenter:�㷨���ĵ�y
	@return NULL
	*/
	int IntegralUpLeft = 0, IntegralUp = 0, IntegralLeft = 0, IntegralBottomRight = 0;
	int IntegralArray[5] = { 0 };
	//��
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 2) * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 2) * ImgParameter.RowStep, cCenter * ImgParameter.ColStep);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, cCenter * ImgParameter.ColStep);
	IntegralArray[0] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//��
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, (cCenter - 2) * ImgParameter.ColStep);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, (cCenter - 2) * ImgParameter.ColStep);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralArray[1] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//��
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, cCenter * ImgParameter.ColStep);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, cCenter * ImgParameter.ColStep);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, (cCenter + 1) * ImgParameter.ColStep);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, (cCenter + 1) * ImgParameter.ColStep);
	IntegralArray[2] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//��
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter + 1) * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, cCenter * ImgParameter.ColStep);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, (rCenter + 1) * ImgParameter.RowStep, cCenter * ImgParameter.ColStep);
	IntegralArray[3] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//�м�
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, (cCenter - 1) * ImgParameter.ColStep);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep, cCenter  * ImgParameter.ColStep);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep, cCenter * ImgParameter.ColStep);
	IntegralArray[4] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	for (int FingerOutputCount = 0; FingerOutputCount < 4; ++FingerOutputCount)
	{
		if (IntegralArray[4] - IntegralArray[FingerOutputCount] >= 0)
		{
			FingerOutput[FingerOutputCount] = '1';
		}
		else
		{
			FingerOutput[FingerOutputCount] = '0';
		}
	}
}

/*
	ָ���������ת16���ƣ�88λ->22λ��
*/
int FingerprintGenerator::TranslateFingerprint(std::vector<char>::iterator start, std::vector<char>::iterator end, const char *fingerOutput)
{
	//if( end-start != FINGER_MASK_BYTE/8 )
	//	return -1;

	for (; start != end; start++)
	{
		unsigned char temp = 0;
		unsigned char bitCode = 0x80;

		for (int i = 0; i<8; i++)
		{
			if (*fingerOutput == '1')
				temp = temp | bitCode;

			bitCode = bitCode >> 1;
			fingerOutput++;

		}
		*start = (char)temp;
	}
	return 0;
}



