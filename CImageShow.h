/*
2019-7-12 15:57 created by Chenxuxu
CImageShow类为UI控件类，继承与QOpenGLWidget，用于接收外部的cv::Mat信号，之后将Mat转化为QImage后显示
*/


#pragma once
#include <QOpenGLWidget>
#include <QPaintEvent>
#include <opencv2/opencv.hpp>
#include <QImage>

class CImageShow:public QOpenGLWidget
{
	Q_OBJECT

public:
	CImageShow(QWidget* parent);
	~CImageShow();
	void paintEvent(QPaintEvent* e);
	void SavePic(std::string file_path);
public slots:
	void ReceiveMat(cv::Mat mat);
	void isStop();
private:
	QImage _image;
	cv::Mat _mat;
	cv::Mat receive_mat;
	bool _is_stop;
};

