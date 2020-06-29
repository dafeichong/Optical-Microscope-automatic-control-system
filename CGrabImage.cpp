#include "CGrabImage.h"
#include <QMessageBox>
#include <iostream>
#include <qcoreapplication.h>

CGrabImage::CGrabImage(QObject *parent)
	: QObject(parent),_image_height(0),_image_width(0)
{

}

CGrabImage::~CGrabImage()
{

}

void CGrabImage::ReadImage()
{
	std::cout << "New Thread is " << QThread::currentThreadId() << std::endl;
	while (1)
	{
		if (isOpened())
		{
			//QThread::msleep(20);       //停止20ms
			_capture.read(_frame);
			if (_frame.dims == 2 && _frame.channels() == 1)
			{
				emit Send_Image(_frame);
			}
		}
		else
			break;
	}
}

void CGrabImage::OpenCamera()
{
	if (!isOpened())
	{
		_capture.open(0);
		_image_height = _capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		_image_width = _capture.get(CV_CAP_PROP_FRAME_WIDTH);
		std::cout << "camera is opened!" << std::endl;
		std::cout << "Image Height = " << _image_height << std::endl;
		std::cout << "Image Width = " << _image_width << std::endl;
	}
	else
	{
		QMessageBox::information(nullptr, "提示信息", "摄像头已打开");
	}
}

void CGrabImage::CloseCamera()
{
	_capture.release();
}

bool CGrabImage::isOpened()
{
	return _capture.isOpened();
}
