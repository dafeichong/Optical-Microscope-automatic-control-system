/*
2019-7-19 15：59 created by Chenxuxu
继承与CSerialCom，用于控制显微镜平台的移动以及自动聚焦、拼接等功能
*/
#pragma once

#include "CSerialCom.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <QDir>
#include <QMessageBox>
#include "CPointImageFusion.h"
#include "CAbstractImageFusion.h"

class CSystemControl : public CSerialCom
{
	Q_OBJECT

public:
	enum { SOBEL_VARIANCE = 0, VARIANCE = 1, FOURIER = 2 };
	CSystemControl(QObject *parent);
	~CSystemControl();
	double SV(cv::Mat mat);
	double Variance(cv::Mat mat);
	float Fourier(cv::Mat mat);
	void AutoFocus();
	void SmallRegionAutoFocus();
	void FourierFocus();
	void ScanSample(int start_x,int start_y,int end_x,int end_y,int step = 500);
	void ScanDeep(int start_z, int num = 50, int step = 100);
	void ImageFusion(std::vector<cv::Mat>& images, cv::Mat& fusion_image, bool is_smooth);
	void calMultiFocusClarity(std::vector<cv::Mat> mat_stream, int cal_type = VARIANCE);
public slots:
	void ReceiveMat(cv::Mat mat);
	void ReceiveStopSignal();
private:
	cv::Mat _mat;
	cv::Mat _mat_roi;
	std::vector<cv::Mat> _mat_stream;
	std::vector <cv::Mat> _images;
	cv::Mat _fusion_image;
	std::vector<std::vector<cv::Mat>> _mat_stream_vector;
	CPointImageFusion* m_pimage_fusion;
	int _index_mat_stream_vector;
	bool _is_stop;
	bool _direction;
signals:
	void updateUI();  //更新界面坐标位置信号
	void SendProcessedImage(cv::Mat);    //发送处理完的图像到Cimageshow
};
