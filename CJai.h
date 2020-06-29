#pragma once

#include <QObject>
#include <jai_factory.h>
#include <iostream>
#include <QString>
#include <QMessageBox>
#include <opencv2/opencv.hpp>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

class CJai : public QObject
{
	Q_OBJECT

public:
	CJai(QObject* parent = nullptr);
	~CJai();
private:
	FACTORY_HANDLE  m_hFactory;     // Factory Handle
	CAM_HANDLE      m_hCam;         // Camera Handle
	THRD_HANDLE     m_hThread;
	int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;
	cv::Mat m_img;
	IplImage* m_pImg;
	cv::Mat m_img_roi;
signals:
	void Send_Image(cv::Mat);
	void Stop_Signal();
public:
	BOOL OpenFactoryAndCamera();
	void CloseFactoryAndCamera();
	void StreamCBFunc(J_tIMAGE_INFO* pAqImageInfo);
	void ShowErrorMsg(QString message, J_STATUS_TYPE error);
	void StartAcquisition();
	void StopAcquisition();
	void myRotateAntiClockWise90(cv::Mat &src);//ÄæÊ±Õë90¡ã
	int* Integral(cv::Mat& src);
	void CJai::AdaptiveThreshold(cv::Mat& SrcImage, cv::Mat& DstImage);
};
