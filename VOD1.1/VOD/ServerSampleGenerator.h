#ifndef SERVERSAMPLEGENERATOR_INCLUDE_H
#define SERVERSAMPLEGENERATOR_INCLUDE_H

#include "FingerprintGenerator.h"

#define CHANNELNUMBER 32   //�趨��ǰ��32·�ɼ�
#define SERVERCHANNELERRORTHRESHOLD 3000 //�����趨������⵽300֡���ҵ�ʱ�������趨����ֵ

//ServerSampleGenerator�������ڷ������˶�ͼ����ȡ��Ƶָ�ƣ�ÿ��ͼ������7����Ƶָ�ƣ���Ӧ�ڲ�ͬ��ͼ��߶ȣ�ÿ��ָ��Ϊ11byte
class ServerSampleGenerator
{
public:
	//imgW��imgH�ֱ�Ϊ�ɼ���ͼ��Ŀ�Ⱥ͸߶ȣ�roi_h_step��roi_v_step�ֱ�ΪROI����ˮƽƽ�ƵĲ����ʹ�ֱƽ�ƵĲ���
	ServerSampleGenerator(const int imgW = 720, const int imgH = 576, const float roi_h_step = 12, const float roi_v_step = 12);
	//GenerateServerSample���ڶԴ����ͼ����ȡ�������˵�ָ�ƣ�����ȡ��25��ָ�ƣ�ÿ��ָ��Ϊ11Byte�������ص�std::vector< std::vector<char> >
	//��25��Ԫ�أ�ÿ��Ԫ��std::vector<char>����11��charԪ��

	//imgCentrex��imgCentreyΪ��ͼ��src������ȡͼ�������ĵ����꣬
	//imgCentreWidth,imgCentreHeightΪ��ͼ��src������ȡͼ���Ŀ�͸�
	std::vector< std::vector<char> > GenerateServerSample(IplImage *src, int imgCentrex = 960, int imgCentrey = 540, int imgCentrexWidth = 800, int imgCentreHeigh = 600,int mask = 7);
	std::vector< std::vector<char> > GenerateServerSample2(IplImage *src, int imgCentrex = 960, int imgCentrey = 540, int imgCentrexWidth = 800, int imgCentreHeigh = 600);

	//���ڼ��ɼ��˻��治������
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

	char **FingerPosition;//���ڱ������ָ�����ڵĸ���λ��

						  //���ڴ���ɼ��Ļ��澲ֹ������
	struct
	{
		int Same_PIC[CHANNELNUMBER];
		int Each_Channel_Count[CHANNELNUMBER];
		std::vector<char> fingerprintfirst;
		std::vector<char> fingerprintsecond;

	}Server_Channel_ERROR;

};


//CompareFingerprint()�������ڽ�TV�����ɵ�22Bytesָ�ƣ�fp_TV����Server�����ɵ�11Bytesָ�ƣ�fp_Server�����бȶԡ�
//����������0�����fp_TV��fp_Serverû��ƥ���ϣ�
//����1�����fp_TV��fp_Serverƥ�����ˣ�����-1��Ϊ����
int CompareFingerprint(const std::vector<char> &fp_TV, const std::vector<char> &fp_Server);

int CompareFingerprint_Server_Server(const std::vector<char> &fp_Server1920, const std::vector<char> &fp_Server720, int *num);

int BLack_Fingerprint_Det(std::vector< std::vector<char> > fp_Server);//�����ж�һ��ָ�������ж��ٸ�1��0

#endif


