#include "stdafx.h"
#include "ServerSampleGenerator.h"

ServerSampleGenerator::ServerSampleGenerator(const int imgW, const int imgH, const float roi_h_step, const float roi_v_step)
{
	//输入图像和库中视频的 宽，高
	VIDEO_IMAGE_WIDTH = imgW;
	VIDEO_IMAGE_HEIGHT = imgH;

	//设置移动步长值
	ROI_H_step = 2;
	ROI_V_step = 2;

	ROI_H_step2 = 6;
	ROI_V_step2 = 4;

	imgRatio[0] = 1.0;   imgRatio[1] = 1.08;   imgRatio[2] = 0.92;

	//对于形成的指纹值进行位置的定义
	FingerPosition = new char*[21];
	for (int i = 0; i<21; i++)
	{
		FingerPosition[i] = new char[9];
	}

	strcpy(FingerPosition[0], "00000000");//表示原始图像
	strcpy(FingerPosition[1], "00010000");//表示	
	strcpy(FingerPosition[2], "00100000");//表示
	strcpy(FingerPosition[3], "00110000");//表示
	strcpy(FingerPosition[4], "01000000");//表示
	strcpy(FingerPosition[5], "01010000");//表示
	strcpy(FingerPosition[6], "01100000");//表示	
	strcpy(FingerPosition[7], "01110000");//表示
	strcpy(FingerPosition[8], "10000000");//表示
	strcpy(FingerPosition[9], "10010000");//表示
	strcpy(FingerPosition[10], "10100000");//表示
	strcpy(FingerPosition[11], "00000001");//表示
	strcpy(FingerPosition[12], "00000010");//表示
	strcpy(FingerPosition[13], "00000011");//表示
	strcpy(FingerPosition[14], "00000100");//表示
	strcpy(FingerPosition[15], "00000101");//表示
	strcpy(FingerPosition[16], "00000110");//表示
	strcpy(FingerPosition[17], "00000111");//表示
	strcpy(FingerPosition[18], "00001000");//表示
	strcpy(FingerPosition[19], "00001001");//表示
	strcpy(FingerPosition[20], "00001010");//表示


	//初始化Server_Channel_ERROR中的标记数组 用于解决采集端画面静止的情况
	memset(Server_Channel_ERROR.Same_PIC, 0, sizeof(int)*CHANNELNUMBER);
	memset(Server_Channel_ERROR.Each_Channel_Count, 0, sizeof(int)*CHANNELNUMBER);

}

ServerSampleGenerator::~ServerSampleGenerator()
{
	for (int i = 0; i<21; i++)
	{
		delete[]FingerPosition[i];
	}
	delete[]FingerPosition;
}


std::vector< std::vector<char> > ServerSampleGenerator::GenerateServerSample(IplImage *src, int imgCentrex, int imgCentrey, int imgCentrexWidth, int imgCentreHeigh,int mask)
{
	std::vector< std::vector<char> > fingerprintSamples;
	if (src->height != VIDEO_IMAGE_HEIGHT || src->width != VIDEO_IMAGE_WIDTH)
	{
		return fingerprintSamples;
	}
	//*对图像坐标进行转换*//
	//imgCentrex = cvRound(imgCentrex*360/960);
	//imgCentrey = cvRound(imgCentrey*288/540);
	//imgCentrexWidth = cvRound(imgCentrexWidth*720/1920);
	//imgCentreHeigh = cvRound(imgCentreHeigh*576/1080);
	//原始图像 FingerPosition[0],"00000000",表示原始图像

	//cvSetImageROI(src, cvRect(imgCentrex-cvRound(imgCentrex /2),imgCentrey-cvRound(imgCentrey /2),imgCentrexWidth,imgCentreHeigh) );
	switch (mask) {
	case 7:
		fingerprintSamples.push_back(fg.GenerateFingerprint_7(src, FingerPosition[0]));
		break;
	case 5:
		fingerprintSamples.push_back(fg.GenerateFingerprint_5(src, FingerPosition[0]));
		break;
	}
	//for(int m=1; m<=5; m++)
	//{
	//	cvSetImageROI( src, cvRect(imgCentrex-cvRound(imgCentrexWidth/2) + cvRound(m*ROI_H_step2),
	//		imgCentrey-cvRound(imgCentreHeigh/2) + cvRound(m*ROI_V_step2), 
	//		imgCentrexWidth - cvRound(m*ROI_H_step2*2), imgCentreHeigh - cvRound(m*ROI_V_step2*2)) );
	//	fingerprintSamples.push_back( fg.GenerateFingerprint(src,FingerPosition[m+10]) );
	//	
	//}
	//cvResetImageROI(src); 
	//返回的fingerprintSamples有2个元素，原始图像,放大形成的库
	return fingerprintSamples; 
}


int CompareFingerprint(const std::vector<char> &fp_TV, const std::vector<char> &fp_Server)
{
	if (fp_TV.size() != 22 || fp_Server.size() != 11)
		return -1;

	const int thresh = 80;   //这里这个阈值用于判断两个视频指纹是否一致，当相同的bit数大于该阈值则认为两个视频指纹相一致
	int sum_4_3 = 0, sum_16_9 = 0;
	for (int i = 0; i<fp_Server.size(); i++)
	{
		unsigned char temp_4_3 = ~(fp_TV[i] ^ fp_Server[i]);
		unsigned char temp_16_9 = ~(fp_TV[i + 11] ^ fp_Server[i]);
		unsigned char bitCode = 0x80;
		for (int j = 0; j<8; j++)
		{
			if (temp_4_3 & bitCode)
				sum_4_3++;

			if (temp_16_9 & bitCode)
				sum_16_9++;

			bitCode = bitCode >> 1;

		}

	}

	if (sum_4_3>thresh || sum_16_9>thresh)
		return 1;
	else
		return 0;

}

int CompareFingerprint_Server_Server(const std::vector<char> &fp_Server1920, const std::vector<char> &fp_Server720, int *num)
{
	if (fp_Server1920.size() != 12 || fp_Server720.size() != 12)
	{
		return -1;
	}
	int sum_server = 0;
	for (int i = 0; i<fp_Server1920.size() - 1; i++) //只比较前11个字节，最后一个字节是对server指纹的位置标记不用比对
	{
		unsigned char temp_server = ~(fp_Server1920[i] ^ fp_Server720[i]);
		unsigned char bitCode = 0x80;
		for (int j = 0; j<8; j++)
		{
			if (temp_server & bitCode)
			{
				sum_server++;
			}

			bitCode = bitCode >> 1;
		}
	}
	printf("the different bit is:%d \n", 88 - sum_server);
	if (88 - sum_server>4)
	{
		(*num)++;
		int tt = 0;
	}

	return 0;
}


int ServerSampleGenerator::ServerErrorDetect(const std::vector<char> &fp_Server_first, int index)
{
	Server_Channel_ERROR.fingerprintfirst = fp_Server_first;

	if (Server_Channel_ERROR.fingerprintsecond.size() == 12)
	{
		int sum = 0;
		for (int i = 0; i<fp_Server_first.size() - 1; i++)//去掉最后8bit的位置编码
		{
			unsigned char temp = ~(Server_Channel_ERROR.fingerprintfirst[i] ^ Server_Channel_ERROR.fingerprintsecond[i]);
			unsigned char bitCode = 0x80;
			for (int j = 0; j<8; j++)
			{
				if (temp & bitCode)
				{
					sum++;
				}

				bitCode = bitCode >> 1;

			}

		}

		if (Server_Channel_ERROR.Each_Channel_Count[index]>0)
		{
			Server_Channel_ERROR.Each_Channel_Count[index]++;
		}

		if (sum >= 86)//fingerprintfirst = fingerprintsecond 若前后两帧的图像相同
					  //则指纹一样，那么就对该情况做一个标记
		{
			if (Server_Channel_ERROR.Each_Channel_Count[index] == 0)
			{
				Server_Channel_ERROR.Each_Channel_Count[index]++;
			}

			Server_Channel_ERROR.Same_PIC[index]++;
			if ((Server_Channel_ERROR.Same_PIC[index] == SERVERCHANNELERRORTHRESHOLD) &&
				(Server_Channel_ERROR.Each_Channel_Count[index] == SERVERCHANNELERRORTHRESHOLD))
			{
				Server_Channel_ERROR.Same_PIC[index] = SERVERCHANNELERRORTHRESHOLD - 1;
				Server_Channel_ERROR.Each_Channel_Count[index] = SERVERCHANNELERRORTHRESHOLD - 1;
				return -2;//不把当前的指纹值上传
			}
			else if (Server_Channel_ERROR.Each_Channel_Count[index] == SERVERCHANNELERRORTHRESHOLD)
			{
				Server_Channel_ERROR.Same_PIC[index] = 0;
				Server_Channel_ERROR.Each_Channel_Count[index] = 0;
			}
		}
		else
		{
			Server_Channel_ERROR.Same_PIC[index] = 0;
			Server_Channel_ERROR.Each_Channel_Count[index] = 0;
		}
	}
	Server_Channel_ERROR.fingerprintsecond = Server_Channel_ERROR.fingerprintfirst;

	return 1;
}

int BLack_Fingerprint_Det(std::vector< std::vector<char> > fp_Server)
{
	//if( fp_Server.size() != 25 ) //server端25个指纹码和
	//{
	//	return -1;
	//}

	//用于判断0101黑屏 只判断旧指纹码88bits
	for (int Server_num = 0; Server_num<fp_Server.size(); Server_num++)
	{
		int sum_1 = 0;
		for (int i = 0; i<fp_Server[Server_num].size() - 1; i++) //比对11字节的指纹码和1字节的位置码
		{
			unsigned char temp = fp_Server[Server_num][i];
			unsigned char bitCode = 0x80;
			for (int j = 0; j<8; j++)
			{
				if (temp & bitCode)
				{
					sum_1++;
				}
				bitCode = bitCode >> 1;

			}
		}

		if (sum_1 >= 80) //zls
		{
			return Server_num;
		}
	}

	return -1;
}
