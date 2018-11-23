#ifndef SERVERSAMPLEGENERATOR_INCLUDE_H
#define SERVERSAMPLEGENERATOR_INCLUDE_H

#include "FingerprintGenerator.h"

#define CHANNELNUMBER 32   //设定当前的32路采集
#define SERVERCHANNELERRORTHRESHOLD 3000 //用于设定连续监测到300帧左右的时间内所设定的阈值

//ServerSampleGenerator类用于在服务器端对图像提取视频指纹，每幅图像生成7个视频指纹，对应于不同的图像尺度，每个指纹为11byte
class ServerSampleGenerator
{
public:
	//imgW和imgH分别为采集的图像的宽度和高度，roi_h_step和roi_v_step分别为ROI区域水平平移的步长和垂直平移的步长
	ServerSampleGenerator(const int imgW = 720, const int imgH = 576, const float roi_h_step = 12, const float roi_v_step = 12);
	//GenerateServerSample用于对传入的图像提取服务器端的指纹，共提取出25个指纹，每个指纹为11Byte，即返回的std::vector< std::vector<char> >
	//有25个元素，每个元素std::vector<char>又有11个char元素

	//imgCentrex，imgCentrey为在图像src中所截取图像块的中心点坐标，
	//imgCentreWidth,imgCentreHeight为在图像src中所截取图像块的宽和高
	std::vector< std::vector<char> > GenerateServerSample(IplImage *src, int imgCentrex = 960, int imgCentrey = 540, int imgCentrexWidth = 800, int imgCentreHeigh = 600,int mask = 7);
	std::vector< std::vector<char> > GenerateServerSample2(IplImage *src, int imgCentrex = 960, int imgCentrey = 540, int imgCentrexWidth = 800, int imgCentreHeigh = 600);

	//用于检测采集端画面不变的情况
	int ServerErrorDetect(const std::vector<char> &fp_Server_first, int index);

	~ServerSampleGenerator();

private:
	FingerprintGenerator fg;

	int VIDEO_IMAGE_WIDTH;
	int VIDEO_IMAGE_HEIGHT;
	int IMAGE_CUT_WIDTH;
	int IMAGE_CUT_HEIGHT;

	float ROI_H_step;
	float ROI_V_step;

	float ROI_H_step2;
	float ROI_V_step2;

	double imgRatio[3];
	CvRect ImageRoiFourThree[3];

	char **FingerPosition;//用于标记生成指纹所在的各个位置

						  //用于处理采集的画面静止的问题
	struct
	{
		int Same_PIC[CHANNELNUMBER];
		int Each_Channel_Count[CHANNELNUMBER];
		std::vector<char> fingerprintfirst;
		std::vector<char> fingerprintsecond;

	}Server_Channel_ERROR;

};


//CompareFingerprint()函数用于将TV端生成的22Bytes指纹（fp_TV）与Server端生成的11Bytes指纹（fp_Server）进行比对。
//若函数返回0则表明fp_TV与fp_Server没有匹配上；
//返回1则表明fp_TV与fp_Server匹配上了；返回-1则为错误。
int CompareFingerprint(const std::vector<char> &fp_TV, const std::vector<char> &fp_Server);

int CompareFingerprint_Server_Server(const std::vector<char> &fp_Server1920, const std::vector<char> &fp_Server720, int *num);

int BLack_Fingerprint_Det(std::vector< std::vector<char> > fp_Server);//用于判断一个指纹里面有多少个1或0

#endif


