/*
2019-7-12 15：46 created by Chenxuxu

CGrabImage用于利用opencv的VideoCapture类打开摄像头，读取摄像头中的图像数据，并且通过信号Send_Image(cv::Mat)发出给其他类中。

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
	//判断摄像头是否打开
	bool isOpened();
	
private:
	cv::Mat _frame;        //存储的每一帧
	cv::VideoCapture _capture;
	int _image_height;  //抓取图像的高度
	int _image_width;   //抓取图像的宽度
public slots:
	void OpenCamera();  //打开摄像头
	void CloseCamera();  //关闭摄像头
	void ReadImage();  //读取当前摄像头数据并将数据存到_frame中后通过Send_Image信号发出
signals:
	void Send_Image(cv::Mat); //发送抓取到的图像数据
};
