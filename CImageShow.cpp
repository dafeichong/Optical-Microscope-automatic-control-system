#include "CImageShow.h"
#include <QPainter>

CImageShow::CImageShow(QWidget* parent):QOpenGLWidget(parent)
{
	_is_stop = false;
}



CImageShow::~CImageShow()
{
}

void CImageShow::paintEvent(QPaintEvent* e)
{
	QPainter paint;
	paint.begin(this);
	paint.drawImage(0, 0, _image);
	paint.end();
}

void CImageShow::SavePic(std::string file_path)
{
	cv::imwrite(file_path, _mat);
}

void CImageShow::isStop()
{
	_is_stop = true;
}

void CImageShow::ReceiveMat(cv::Mat mat)
{
	//cvtColor(mat, mat, cv::COLOR_BGR2RGB);
	if (!_is_stop)
	{
		receive_mat = mat;
		_mat = mat.clone();
		//cv::rotate(_mat, _mat, cv::ROTATE_90_CLOCKWISE);
		cv::resize(receive_mat, receive_mat, cv::Size(600, 800));
		_image = QImage(receive_mat.data, receive_mat.cols, receive_mat.rows, QImage::Format_Indexed8);
		update();
	}
	
}
