#include "MainWindowUI.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

MainWindowUI::MainWindowUI(QWidget *parent)
	: QMainWindow(parent),_system_control(this)
{
	ui.setupUi(this);
	qRegisterMetaType<cv::Mat>("cv::Mat");
	this->setWindowState(Qt::WindowMaximized);
	ui.tabWidget->setCurrentIndex(0);
	//读取摄像头数据线程初始化
	//_grab_image = new CGrabImage();
	//_grab_image->moveToThread(&_read_thread);

	//connect(&_read_thread, &QThread::finished, _grab_image, &QObject::deleteLater);
	//connect(this, &MainWindowUI::StartReadThread, _grab_image, &CGrabImage::ReadImage);
	//connect(_grab_image, &CGrabImage::Send_Image, ui.image_show, &CImageShow::ReceiveMat);
	connect(&_jai, &CJai::Send_Image, ui.image_show, &CImageShow::ReceiveMat);
	//_read_thread.start();
	//_grab_image->OpenCamera();
	//emit StartReadThread();
	//std::cout << "Main Thread is " << QThread::currentThreadId() << std::endl;
	//串口初始化
	_system_control.OpenSerialPort();
	_system_control.SetStepLength(ui.input_long_step->text().toInt());
	_system_control.ResetPlatform();
	//将CGrabImage类抓的图发送到CSystemControl中
	//connect(_grab_image, &CGrabImage::Send_Image, &_system_control, &CSystemControl::ReceiveMat);
	connect(&_jai, &CJai::Send_Image, &_system_control, &CSystemControl::ReceiveMat);
	connect(&_system_control, &CSystemControl::updateUI, this, &MainWindowUI::UpdatePositionUI);
	connect(&_system_control, &CSystemControl::SendProcessedImage, ui.dst_image_show, &CImageShow::ReceiveMat);
	//Jai code
	connect(&_jai, &CJai::Stop_Signal, ui.image_show, &CImageShow::isStop);
	_jai.OpenFactoryAndCamera();
	_jai.StartAcquisition();
	//set stop signal
	connect(this, &MainWindowUI::SendStopSignal, &_system_control, &CSystemControl::ReceiveStopSignal);
	//connect(this, &MainWindowUI::destroyed, &_system_control, &CSystemControl::ReceiveStopSignal);

	//网络socket线程
	_socket.start();
	connect(&_socket, &CSocketThread::SendCmd, this, &MainWindowUI::Receive_Cmd);

}

MainWindowUI::~MainWindowUI()
{
	_mutex.lock();
	_jai.StopAcquisition();
	_mutex.unlock();
	_jai.CloseFactoryAndCamera();
	_socket.terminate();
	//_grab_image->CloseCamera();
	//_read_thread.quit();
	//_read_thread.wait();
}


void MainWindowUI::UpdatePositionUI()
{
	int x, y, z;
	z = _system_control.GetPosition()._z;
	y = _system_control.GetPosition()._y;
	x = _system_control.GetPosition()._x;
	ui.z_position_show->setText(QString::number(z));
	ui.y_position_show->setText(QString::number(y));
	ui.x_position_show->setText(QString::number(x));
}

void MainWindowUI::Receive_Cmd(int cmd, int length)
{
	if(cmd == CSystemControl::Light)
		_system_control.ChangeLight(length);
	else if (cmd == CSocketThread::RESET)
	{
		_system_control.ResetPlatform();
	}
	else
	{
		_system_control.MovePlatform(length, cmd);
		UpdatePositionUI();
	}
}

void MainWindowUI::On_Left()
{
	_system_control.SetStepLength(ui.input_long_step->text().toInt());
	int step_length = _system_control.GetStepLength();
	_system_control.MovePlatform(-step_length, CSystemControl::X_AXIS);
	UpdatePositionUI();
}

void MainWindowUI::On_Right()
{
	_system_control.SetStepLength(ui.input_long_step->text().toInt());
	int step_length = _system_control.GetStepLength();
	_system_control.MovePlatform(step_length, CSystemControl::X_AXIS);
	UpdatePositionUI();
}

void MainWindowUI::On_Up()
{
	_system_control.SetStepLength(ui.input_long_step->text().toInt());
	int step_length = _system_control.GetStepLength();
	_system_control.MovePlatform(step_length, CSystemControl::Z_AXIS);
	UpdatePositionUI();
	if (_system_control.GetPosition()._z >= 80000)
	{
		ui.btn_up->setEnabled(false);
		QMessageBox::information(nullptr, QString::fromLocal8Bit("提示信息"), QString::fromLocal8Bit("已达到Z轴最高点"));
	}
	if (_system_control.GetPosition()._z >= -100000)
	{
		ui.btn_down->setEnabled(true);
	}
}

void MainWindowUI::On_Down()
{
	_system_control.SetStepLength(ui.input_long_step->text().toInt());
	int step_length = _system_control.GetStepLength();
	_system_control.MovePlatform(-step_length, CSystemControl::Z_AXIS);
	UpdatePositionUI();
	if (_system_control.GetPosition()._z <= -100000)
	{
		ui.btn_down->setEnabled(false);
		QMessageBox::information(nullptr, QString::fromLocal8Bit("提示信息"), QString::fromLocal8Bit("已达到Z轴最低点"));
	}
	if (_system_control.GetPosition()._z <= 80000)
	{
		ui.btn_up->setEnabled(true);
	}
}

void MainWindowUI::On_Front()
{
	_system_control.SetStepLength(ui.input_long_step->text().toInt());
	int step_length = _system_control.GetStepLength();
	_system_control.MovePlatform(-step_length, CSystemControl::Y_AXIS);
	UpdatePositionUI();
}

void MainWindowUI::On_Back()
{
	_system_control.SetStepLength(ui.input_long_step->text().toInt());
	int step_length = _system_control.GetStepLength();
	_system_control.MovePlatform(step_length, CSystemControl::Y_AXIS);
	UpdatePositionUI();
}

void MainWindowUI::On_Light_Change(int value)
{
	_system_control.ChangeLight(value);
}

void MainWindowUI::On_Save_Pic()
{
	QString file_name = QFileDialog::getSaveFileName(this, tr("Save Image"), tr("..\\..\\pic\\"), tr("Images (*.jpg *.png *.bmp)"));
	if (!file_name.isNull() && ui.tabWidget->currentIndex() == 0)
		ui.image_show->SavePic(file_name.toStdString());
	else if (!file_name.isNull() && ui.tabWidget->currentIndex() == 1)
		ui.dst_image_show->SavePic(file_name.toStdString());

}

void MainWindowUI::On_Reset()
{
	_system_control.ResetPlatform();
	UpdatePositionUI();
}

void MainWindowUI::On_Focus() 
{
	_system_control.AutoFocus();
}

void MainWindowUI::On_ROI_Focus()
{
	_system_control.SmallRegionAutoFocus();
}

void MainWindowUI::On_Fourer_Focus()
{
	_system_control.FourierFocus();
}

void MainWindowUI::On_Scan_Sample()
{
	_system_control.ScanSample(_system_control.GetPosition()._x, _system_control.GetPosition()._y, ui.x_position_scan->text().toInt(), ui.y_position_scan->text().toInt());
}

void MainWindowUI::On_Scan_Deep()
{
	int step = ui.input_short_step->text().toInt();
	int num = ui.input_short_num->text().toInt();
	_system_control.ScanDeep(_system_control.GetPosition()._z, num, step);
	ui.tabWidget->setCurrentIndex(1);
	QMessageBox::information(nullptr, QString::fromLocal8Bit("提示信息"), QString::fromLocal8Bit("图像融合已完成"));
}

void MainWindowUI::On_Stop_Scan()
{
	emit SendStopSignal();
}
