/*
2019-7-12 15��46 created by Chenxuxu

CGrabImage��������opencv��VideoCapture�������ͷ����ȡ����ͷ�е�ͼ�����ݣ�����ͨ���ź�Send_Image(cv::Mat)�������������С�

*/

#pragma once

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QThread>

class CGrabImage : public QObject
{

	Q_OBJECT

public:
	CGrabImage(QObject* parent = nullptr);
	~CGrabImage();
	//�ж�����ͷ�Ƿ��
	bool isOpened();
	
private:
	cv::Mat _frame;        //�洢��ÿһ֡
	cv::VideoCapture _capture;
	int _image_height;  //ץȡͼ��ĸ߶�
	int _image_width;   //ץȡͼ��Ŀ��
public slots:
	void OpenCamera();  //������ͷ
	void CloseCamera();  //�ر�����ͷ
	void ReadImage();  //��ȡ��ǰ����ͷ���ݲ������ݴ浽_frame�к�ͨ��Send_Image�źŷ���
signals:
	void Send_Image(cv::Mat); //����ץȡ����ͼ������
};
