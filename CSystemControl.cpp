#include "CSystemControl.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <iterator>

CSystemControl::CSystemControl(QObject *parent)
	: CSerialCom(parent),_is_stop(false),_index_mat_stream_vector(0)
{
	
}

CSystemControl::~CSystemControl()
{
	
}

double CSystemControl::SV(cv::Mat mat)
{
	double sv = 0.0;
	cv::Mat sobel_mat;
	cv::Sobel(mat, sobel_mat, CV_16S, 1, 0, 3, 1, 1, cv::BORDER_DEFAULT);
	cv::Scalar mean = cv::mean(cv::abs(sobel_mat));
	double average = mean.val[0];  //x方向差分图的均值
	for (int i = 0; i < sobel_mat.rows; i++)
	{
		for (int j = 0; j < sobel_mat.cols; j++)
		{
			short* ptr = sobel_mat.ptr<short>(i, j);
			short value = *ptr;
			sv += (value - average) * (value - average); //平方
		}
	}
	std::cout << "the SV is " << sv << std::endl;
	return sv;
}

double CSystemControl::Variance(cv::Mat mat)
{
	double variance = 0.0;
	cv::Scalar mean = cv::mean(mat);
	double average = mean.val[0];  //平均值
	int size = mat.rows * mat.cols * mat.elemSize();
	//mat中行与行之间连续存储
	if (mat.isContinuous() && mat.channels() == 1)
	{
		for (int i = 0; i < size; i++)
		{
			variance += (mat.data[i] - average) * (mat.data[i] - average);  //平方
		}
		std::cout << "is Continous" << " the variance is " << variance << std::endl;
	}
	else if(mat.channels() == 1)
	{
		for (int i = 0; i < mat.rows; i++)
		{
			for (int j = 0; j < mat.cols; j++)
			{
				variance += ((&mat.data[i * mat.step])[j] - average) * ((&mat.data[i * mat.step])[j] - average);   //平方
			}
		}
		std::cout << "is not Continous" << " the variance is " << variance << std::endl;
	}
	else 
		return 0.0;
	return variance;
}

float CSystemControl::Fourier(cv::Mat mat)
{
	float fourier = 0.0;
	int rows = cv::getOptimalDFTSize(mat.rows);
	int cols = cv::getOptimalDFTSize(mat.cols);
	cv::Mat padded;
	cv::copyMakeBorder(mat, padded, 0, rows - mat.rows, 0, cols - mat.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	cv::Mat planes[] = { cv::Mat_<float>(padded),cv::Mat::zeros(padded.size(),CV_32F) };
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);
	cv::dft(complexI, complexI);
	cv::split(complexI, planes);
	split(complexI, planes);
	magnitude(planes[0], planes[1], planes[0]);
	cv::Mat magnitudeImage = planes[0];
	fourier = cv::sum(magnitudeImage)[0];
	std::cout << "the Fourier is " << fourier << std::endl;
	return fourier;
}

void CSystemControl::AutoFocus()
{
	std::unordered_map<int, float> value;
	int large_sleep_time = 1500;
	int small_sleep_time = 1000;

	int large_focus_step = 1000;
	int small_focus_step = 500;


	while(this->GetPosition()._z < 60000)
	{
		this->MovePlatform(large_focus_step, Z_AXIS);
		emit updateUI();
		this->Sleep(1000);
		value.insert(std::make_pair(this->GetPosition()._z, Variance(_mat)));
	}
	float max_value = 0.0;
	int max_location = 0;
	std::unordered_map<int, float>::iterator it;
	for (std::unordered_map<int, float>::iterator iter = value.begin(); iter != value.end(); iter++)
	{
		float cur_value = iter->second;
		int cur_location = iter->first;		
		max_location = (cur_value > max_value) ? cur_location : max_location;
		max_value = (cur_value > max_value) ? cur_value : max_value;
	}
	std::cout << max_value << std::endl;
	std::cout << max_location << std::endl;
	this->MovePlatform(max_location - this->GetPosition()._z, Z_AXIS);
	emit updateUI();
}

void CSystemControl::SmallRegionAutoFocus()
{
	std::unordered_map<int, float> value;
	int large_sleep_time = 1500;
	int small_sleep_time = 1000;
	_mat_roi = _mat(cv::Rect(54, 53, 5, 5));
	//设置大聚焦步长与小聚焦步长
	int large_focus_step = 1000;
	int small_focus_step = 500;

	while (this->GetPosition()._z < 60000)
	{
		this->MovePlatform(large_focus_step, Z_AXIS);
		emit updateUI();
		this->Sleep(1000);
		_mat_roi = _mat(cv::Rect(54, 53, 5, 5));
		value.insert(std::make_pair(this->GetPosition()._z, Variance(_mat_roi)));
	}
	float max_value = 0.0;
	int max_location = 0;
	std::unordered_map<int, float>::iterator it;
	for (auto iter = value.begin(); iter != value.end(); iter++)
	{
		float cur_value = iter->second;
		int cur_location = iter->first;
		max_location = (cur_value > max_value) ? cur_location : max_location;
		max_value = (cur_value > max_value) ? cur_value : max_value;
	}
	std::cout << max_value << std::endl;
	std::cout << max_location << std::endl;
	this->MovePlatform(max_location - this->GetPosition()._z, Z_AXIS);
	emit updateUI();
}

void CSystemControl::FourierFocus()
{
	std::unordered_map<int, float> value;
	int large_sleep_time = 1500;
	int small_sleep_time = 1000;
	//先将平台设置在z轴较低位置
	//this->MovePlatform(-60000, Z_AXIS);
	//emit updateUI();   //更新界面坐标位置显示
	//this->Sleep(12000);
	int large_focus_step = 1000;
	int small_focus_step = 500;

	while (this->GetPosition()._z < 60000)
	{
		this->MovePlatform(large_focus_step, Z_AXIS);
		emit updateUI();
		this->Sleep(1000);
		value.insert(std::make_pair(this->GetPosition()._z, Fourier(_mat)));
	}
	float max_value = 0.0;
	int max_location = 0;
	std::unordered_map<int, float>::iterator it;
	for (std::unordered_map<int, float>::iterator iter = value.begin(); iter != value.end(); iter++)
	{
		float cur_value = iter->second;
		int cur_location = iter->first;
		max_location = (cur_value > max_value) ? cur_location : max_location;
		max_value = (cur_value > max_value) ? cur_value : max_value;
	}
	std::cout << max_value << std::endl;
	std::cout << max_location << std::endl;
	this->MovePlatform(max_location - this->GetPosition()._z, Z_AXIS);
	emit updateUI();
}
//move platform to scan sample for image stitching
void CSystemControl::ScanSample(int start_x, int start_y, int end_x, int end_y,int step)
{
	int scan_width = end_x - start_x;
	int scan_height = start_y - end_y;
	int x_move_num = std::ceil(scan_width / step);
	int y_move_num = std::ceil(scan_height / (step * 4)) + 1;
	_is_stop = false;
	_direction = 0;
	for (int j = 0; j < y_move_num; ++j)
	{
		for (int i = 0; i < x_move_num; ++i)
		{
			if (_is_stop)
			{
				break;
			}
			if (_direction == 0)
			{
				//向←扫描x变大
				this->ScanDeep(this->GetPosition()._z, 1, 100);
				this->MovePlatform(step, CSystemControl::X_AXIS);
				emit updateUI();
				this->Sleep(500);
			}
			else
			{
				//向→扫描x变小
				this->ScanDeep(this->GetPosition()._z, 1, 100);
				this->MovePlatform(-step, CSystemControl::X_AXIS);
				emit updateUI();
				this->Sleep(500);
			}
		}
		if (_is_stop)
		{
			break;
		}
		this->ScanDeep(this->GetPosition()._z, 1, 100);
		this->MovePlatform(-4 * step, CSystemControl::Y_AXIS);
		emit updateUI();
		this->Sleep(500);
		_direction = !_direction;
	}
	QMessageBox::information(nullptr, QString::fromLocal8Bit("提示信息"), QString::fromLocal8Bit("已完成扫描"));

}

void CSystemControl::ScanDeep(int start_z, int num, int step)
{
	//read _mat to a vector called _mat_stream
	_is_stop = false;
	for (int i = 0; i < num; ++i)
	{
		if (_is_stop)
		{
			break;
		}
		this->MovePlatform(step, CSystemControl::Z_AXIS);
		emit updateUI();
		this->Sleep(500);
		_mat_stream.push_back(_mat.clone());
		_images.push_back(_mat.clone());
	}
	//start image fusion
	this->ImageFusion(_images, _fusion_image, true);
	//save _mat_stream
	int count = 0;
	QString root_dir_str = QString("..\\..\\pic\\");
	for (int i = 0; i < _mat_stream.size(); ++i)
	{
		QDir dir;
		QString dir_path = root_dir_str +
			QString("X%1_Y%2_Z%3_%4").arg(this->GetPosition()._x).arg(this->GetPosition()._y).arg(start_z).arg(this->GetPosition()._z);
		if (!dir.exists(dir_path))
		{
			dir.mkpath(dir_path);
		}
		QString file_name = QString(dir_path+"\\%1.jpg").arg(count);
		std::string file = file_name.toStdString();
		cv::imwrite(file, _mat_stream[i]);
		count++;
	}
	_index_mat_stream_vector++;
	_mat_stream.clear();
	this->MovePlatform(start_z - this->GetPosition()._z, CSystemControl::Z_AXIS);
	this->Sleep(1000);
	emit updateUI();

}

void CSystemControl::ImageFusion(std::vector<cv::Mat>& images, cv::Mat & fusion_image, bool is_smooth)
{
	CAbstractImageFusion* pimage_fusion = new CPointImageFusion();
	pimage_fusion->Fuse(images, fusion_image, is_smooth);
	emit SendProcessedImage(fusion_image);
	cv::imwrite("梯度点融合_高斯.jpg", fusion_image);
	cv::Mat binary_image;
	cv::threshold(fusion_image, binary_image, 128, 255, CV_THRESH_OTSU);
	cv::imwrite("二值图_高斯.jpg", binary_image);
	delete pimage_fusion;
	_images.clear();
}

void CSystemControl::calMultiFocusClarity(std::vector<cv::Mat> mat_stream, int cal_type)
{
	std::vector<float> result;
	switch (cal_type)
	{
	case SOBEL_VARIANCE:
		for (int i = 0; i < mat_stream.size(); ++i)
		{
			result.push_back(SV(mat_stream[i]));
		}
		break;
	case VARIANCE:
		for (int i = 0; i < mat_stream.size(); ++i)
		{
			result.push_back(Variance(mat_stream[i]));
		}
		break;
	case FOURIER:
		for (int i = 0; i < mat_stream.size(); ++i)
		{
			result.push_back(Fourier(mat_stream[i]));
		}
		break;
	default:
		break;
		for (int i = 0; i < result.size(); ++i)
		{
			std::cout << result[i] << std::endl;
		}
	}

}

void CSystemControl::ReceiveStopSignal()
{
	_is_stop = true;
}

void CSystemControl::ReceiveMat(cv::Mat mat)
{
	_mat = mat;
}
