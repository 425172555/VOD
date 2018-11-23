#include "stdafx.h"
#include "FingerprintGenerator.h"

FingerprintGenerator::FingerprintGenerator()
{
	FINGER_MASK_BYTE_7 = 88;//+8;//10bit中前面四为用于标记指纹码的位置，后面4个用于标记指纹码所在某个方向上的位置暂时预留
							  //如111010000101000011111110000101100001011011010110011111111100000010000101111100001110101100010000
							  //上面为总的生成的指纹码但是解读为：
							  //1110100001010000111111100001011000010110110101100111111111000000100001011111000011101011 0001 0000
							  //后面的0001为标记的该指纹所在某个方向上的位置，后面的0000为该指纹在该方向上的位置这里为在0001方向上（左上移动）
							  /************************************************************************/
							  /*                        ImgParameter参数初始化                         */
							  /************************************************************************/
	FINGER_MASK_BYTE_5 = 40;
	//分块大小7*7
	ImgParameter.ColNum_7 = 7;
	ImgParameter.RowNum_7 = 7;

	ImgParameter.ColNum_5 = 5;
	ImgParameter.RowNum_5 = 5;
	//图像大小140*140
	ImgParameter.ImageResizeHeight_7 = 140;
	ImgParameter.ImageResizeWidth_7 = 140;

	ImgParameter.ImageResizeHeight_5 = 140;
	ImgParameter.ImageResizeWidth_5 = 140;


	//每个块的行列步进，应该保证是整数
	ImgParameter.ColStep_7 = ImgParameter.ImageResizeHeight_7 / ImgParameter.ColNum_7;
	ImgParameter.RowStep_7 = ImgParameter.ImageResizeWidth_7 / ImgParameter.RowNum_7;

	ImgParameter.ColStep_5 = ImgParameter.ImageResizeHeight_5 / ImgParameter.ColNum_5;
	ImgParameter.RowStep_5 = ImgParameter.ImageResizeWidth_5 / ImgParameter.RowNum_5;

	//FINGER_MASK_BYTE2 = 64;
	//FINGER_MASK_BYTE3 = 16 * 16;
}

std::vector<char> FingerprintGenerator::GenerateFingerprint_7(IplImage *src, char *FingerPosition_confirm)
{
	/*
	@param src:帧图
	@param FingerPosition_confirm
	@return 指纹码
	*/
	std::vector<char> fingerprint;
	if (NULL == src)
		return fingerprint;
	CHAR outinfo[512] = { 0 };
	//static int imgindex = 0;

	IplImage *ImgResize = cvCreateImage(cvSize(ImgParameter.ImageResizeWidth_7, ImgParameter.ImageResizeHeight_7), IPL_DEPTH_8U, 1);
	char *FingerOutput = new char[FINGER_MASK_BYTE_7];

	fingerprint.resize(FINGER_MASK_BYTE_7 / 8);
	cvResize(src, ImgResize, CV_INTER_NN);
	//imgindex++;
	GenerateVideoFinger_7(ImgResize, FingerOutput, FingerPosition_confirm);
	TranslateFingerprint(fingerprint.begin(), fingerprint.end(), FingerOutput);

	cvReleaseImage(&ImgResize);
	delete[]FingerOutput;

	return fingerprint;

}

std::vector<char> FingerprintGenerator::GenerateFingerprint_5(IplImage * src, char * FingerPosition_confirm)
{
	/*
	@param src:帧图
	@param FingerPosition_confirm
	@return 指纹码
	*/
	std::vector<char> fingerprint;
	if (NULL == src)
		return fingerprint;
	CHAR outinfo[512] = { 0 };
	//static int imgindex = 0;

	IplImage *ImgResize = cvCreateImage(cvSize(ImgParameter.ImageResizeWidth_5, ImgParameter.ImageResizeHeight_5), IPL_DEPTH_8U, 1);
	char *FingerOutput = new char[FINGER_MASK_BYTE_5];

	fingerprint.resize(FINGER_MASK_BYTE_5 / 8);
	cvResize(src, ImgResize, CV_INTER_LINEAR);

	//imgindex++;
	GenerateVideoFinger_5(ImgResize, FingerOutput, FingerPosition_confirm);
	TranslateFingerprint(fingerprint.begin(), fingerprint.end(), FingerOutput);

	cvReleaseImage(&ImgResize);
	delete[]FingerOutput;

	return fingerprint;
}


void FingerprintGenerator::GenerateVideoFinger_7(IplImage *src, char *FingerOutput, char *FingerPosition_confirm)
{
	/*
	@param src:帧图
	@param FingerPosition_confirm
	@return NULL
	*/
	CvMat *sum = cvCreateMat(src->height + 1, src->width + 1, CV_32SC1);//用于存储积分图
	cvIntegral(src, sum, NULL, NULL);
	//利用积分图来形成一张图片的指纹信息
	int offset = 0;
	EightFingerCount(sum, FingerOutput + offset, 2, 2,7);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 2, 4,7);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 2, 6,7);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 4, 2,7);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 4, 6,7);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 6, 2,7);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 6, 4,7);
	offset += 8;
	EightFingerCount(sum, FingerOutput + offset, 6, 6,7);
	offset += 8;
	FourFingerCount(sum, FingerOutput + offset, 3, 3);
	offset += 4;
	FourFingerCount(sum, FingerOutput + offset, 3, 5);
	offset += 4;
	FourFingerCount(sum, FingerOutput + offset, 5, 3);
	offset += 4;
	FourFingerCount(sum, FingerOutput + offset, 5, 5);
	offset += 4;
	EightFingerCount(sum, FingerOutput + offset, 4, 4,7);
	offset += 8;
	//AddFingerPosition(FingerOutput + offset,FingerPosition_confirm);
	cvReleaseMat(&sum);

}


void FingerprintGenerator::GenerateVideoFinger_5(IplImage *src, char * FingerOutput, char *FingerPosition_confirm)
{
	/*
	@param src:帧图
	@param FingerPosition_confirm
	@return NULL
	*/

	CvMat *sum = cvCreateMat(src->height + 1, src->width + 1, CV_32SC1);//用于存储积分图
	cvIntegral(src, sum, NULL, NULL);

	//利用积分图来形成一张图片的指纹信息
	int offset = 0;
	SevenFingerCount(sum, FingerOutput + offset, 2, 2);
	offset += 7;
	SevenFingerCount(sum, FingerOutput + offset, 2, 4);
	offset += 7;
	SevenFingerCount(sum, FingerOutput + offset, 4, 2);
	offset += 7;
	SevenFingerCount(sum, FingerOutput + offset, 4, 4);
	offset += 7;
	EightFingerCount(sum, FingerOutput + offset, 3, 3, 5);
	offset += 8;
	OneFingerCount(sum, FingerOutput + offset, 2, 3, 1);
	offset += 1;
	OneFingerCount(sum, FingerOutput + offset, 4, 3, 2);
	offset += 1;
	OneFingerCount(sum, FingerOutput + offset, 3, 2, 3);
	offset += 1;
	OneFingerCount(sum, FingerOutput + offset, 3, 4, 4);
	//offset += 1;
	//AddFingerPosition(FingerOutput + offset,FingerPosition_confirm);
	cvReleaseMat(&sum);
}

void FingerprintGenerator::OneFingerCount(CvMat * ImgIntegral, char * FingerOutput, int rCenter, int cCenter, int position)
{
	/*
	@param ImgIntegral:帧图矩阵
	@param FingerOutput:二进制指纹码数组
	@param rCenter:算法中心点x
	@param cCenter:算法中心点y
	@param position:位置
	@return NULL
	*/
	int IntegralArray[1][2] = { 0,0 };
	int IntegralUpLeft = 0, IntegralUp = 0, IntegralLeft = 0, IntegralBottomRight = 0;

	//上
	if (position == 1) {
		IntegralUpLeft = 0;
		IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralUp = 0;
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralArray[0][0] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralArray[0][1] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		int FingerOutputCount = 0;
		if (IntegralArray[0][0] - IntegralArray[0][1] >= 0) {
			FingerOutput[FingerOutputCount] = '1';
		}
		else
		{
			FingerOutput[FingerOutputCount] = '0';
		}
	}
	//下
	if (position == 2) {
		IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter  * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter + 1) * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, (rCenter + 1) * ImgParameter.RowStep_5, (cCenter + 1) * ImgParameter.ColStep_5);
		IntegralArray[0][0] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralArray[0][1] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		int FingerOutputCount = 0;
		if (IntegralArray[0][0] - IntegralArray[0][1] >= 0) {
			FingerOutput[FingerOutputCount] = '1';
		}
		else
		{
			FingerOutput[FingerOutputCount] = '0';
		}
	}
	//左
	if (position == 3) {
		IntegralUpLeft = 0;
		IntegralLeft = 0;
		IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1)  * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter  * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralArray[0][0] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, cCenter  * ImgParameter.ColStep_5);
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralArray[0][1] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		int FingerOutputCount = 0;
		if (IntegralArray[0][0] - IntegralArray[0][1] >= 0) {
			FingerOutput[FingerOutputCount] = '1';
		}
		else
		{
			FingerOutput[FingerOutputCount] = '0';
		}
	}
	//右
	if (position == 4) {
		IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1)  * ImgParameter.RowStep_5, (cCenter + 1) * ImgParameter.ColStep_5);
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, (cCenter + 1) * ImgParameter.ColStep_5);
		IntegralArray[0][0] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, (cCenter - 1) * ImgParameter.ColStep_5);
		IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_5, cCenter * ImgParameter.ColStep_5);
		IntegralArray[0][1] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

		int FingerOutputCount = 0;
		if (IntegralArray[0][0] - IntegralArray[0][1] >= 0) {
			FingerOutput[FingerOutputCount] = '1';
		}
		else
		{
			FingerOutput[FingerOutputCount] = '0';
		}
	}
}

void FingerprintGenerator::SevenFingerCount(CvMat * ImgIntegral, char * FingerOutput, int rCenter, int cCenter)
{
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
		rUpLeftPix = (rCount - 1) * ImgParameter.RowStep_5;
		for (cCount = (cCenter - 1); cCount <= cCenter + 1; ++cCount)
		{
			cUpLeftPix = (cCount - 1) * ImgParameter.ColStep_5;
			IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix, cUpLeftPix);
			IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix + ImgParameter.RowStep_5, cUpLeftPix);
			IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix, cUpLeftPix + ImgParameter.ColStep_5);
			IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix + ImgParameter.RowStep_5, cUpLeftPix + ImgParameter.ColStep_5);
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
				if (IntegralArray[rArrayCount][cArrayCount] - IntegralArray[1][1] >= 0)
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

void FingerprintGenerator::EightFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter,int mask)
{
	/*
@param ImgIntegral:帧图矩阵
@param FingerOutput:二进制指纹码数组
@param rCenter:算法中心点x
@param cCenter:算法中心点y
@return NULL
*/
	int RowStep = 0;
	int ColStep = 0;
	if (mask == 5) {
		RowStep = ImgParameter.RowStep_5;
		ColStep = ImgParameter.ColStep_5;
	}
	else if (mask == 7) {
		RowStep = ImgParameter.RowStep_7;
		ColStep = ImgParameter.ColStep_7;
	}

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
		rUpLeftPix = (rCount - 1) * ImgParameter.RowStep_7;
		for (cCount = (cCenter - 1); cCount <= cCenter + 1; ++cCount)
		{
			cUpLeftPix = (cCount - 1) * ColStep;
			IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix, cUpLeftPix);
			IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix + RowStep, cUpLeftPix);
			IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix, cUpLeftPix + ColStep);
			IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rUpLeftPix + RowStep, cUpLeftPix + ColStep);
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
				if (IntegralArray[rArrayCount][cArrayCount] - IntegralArray[1][1] >= 0)
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
	@param ImgIntegral:帧图矩阵
	@param FingerOutput:二进制指纹码数组
	@param rCenter:算法中心点x
	@param cCenter:算法中心点y
	@return NULL
	*/
	int IntegralUpLeft = 0, IntegralUp = 0, IntegralLeft = 0, IntegralBottomRight = 0;
	int IntegralArray[5] = { 0 };
	//上
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 2) * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 2) * ImgParameter.RowStep_7, cCenter * ImgParameter.ColStep_7);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, cCenter * ImgParameter.ColStep_7);
	IntegralArray[0] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//左
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, (cCenter - 2) * ImgParameter.ColStep_7);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, (cCenter - 2) * ImgParameter.ColStep_7);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralArray[1] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//右
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, cCenter * ImgParameter.ColStep_7);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, cCenter * ImgParameter.ColStep_7);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, (cCenter + 1) * ImgParameter.ColStep_7);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, (cCenter + 1) * ImgParameter.ColStep_7);
	IntegralArray[2] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//下
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter + 1) * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, cCenter * ImgParameter.ColStep_7);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, (rCenter + 1) * ImgParameter.RowStep_7, cCenter * ImgParameter.ColStep_7);
	IntegralArray[3] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;
	//中间
	IntegralUpLeft = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralLeft = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, (cCenter - 1) * ImgParameter.ColStep_7);
	IntegralUp = CV_MAT_ELEM(*ImgIntegral, int, (rCenter - 1) * ImgParameter.RowStep_7, cCenter  * ImgParameter.ColStep_7);
	IntegralBottomRight = CV_MAT_ELEM(*ImgIntegral, int, rCenter * ImgParameter.RowStep_7, cCenter * ImgParameter.ColStep_7);
	IntegralArray[4] = IntegralBottomRight + IntegralUpLeft - IntegralLeft - IntegralUp;

	for (int FingerOutputCount = 0; FingerOutputCount < 5; ++FingerOutputCount)
	{
		if (IntegralArray[FingerOutputCount] - IntegralArray[4] >= 0)
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
	指纹码二进制转16进制（88位->22位）
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



