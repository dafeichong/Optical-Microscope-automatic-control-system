#include "CPointImageFusion.h"

CPointImageFusion::CPointImageFusion()
{

}

CPointImageFusion::~CPointImageFusion()
{

}

void CPointImageFusion::PointGrad(const cv::Mat& src, cv::Mat& grad)
{
	int rows = src.rows;
	int cols = src.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
			{
				grad.at<uchar>(i, j) = 0;
			}
			else
			{
				uchar up = src.at<uchar>(i - 1, j);
				uchar down = src.at<uchar>(i + 1, j);
				uchar right = src.at<uchar>(i, j + 1);
				uchar left = src.at<uchar>(i, j - 1);
				uchar val = abs(up - down) + abs(right - left);
				//越界判断
				if (val >= 255)
					val = 255;
				if (val <= 0)
					val = 0;
				grad.at<uchar>(i, j) = val;
			}
		}
	}
}
/*
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
	else if (mat.channels() == 1)
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

*/
void CPointImageFusion::TenenGrad(const cv::Mat & src, cv::Mat & grad, int win_size)
{

}

void CPointImageFusion::Variance(const cv::Mat & src, cv::Mat & grad, int win_size)
{
	using namespace std;
	using namespace cv;
	int rows = src.rows;
	int cols = src.cols;
	int offset = win_size / 2;
	max_var = 0.0;
	vector<vector<float>> temp = vector<vector<float>>(rows, vector<float>(cols, 0.0));
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (i >= rows - offset || i < offset || j >= cols - offset || j < offset)
			{
				temp[i][j] = 0.0;
			}
			else
			{
				float var = GetVariance(src(Rect(j - offset, i - offset, win_size, win_size)));
				max_var = max(max_var, var);
				temp[i][j] = var;

			}
		}
	}
	//将temp中的数据归一化为0-255
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (i >= rows - offset || i < offset || j >= cols - offset || j < offset)
			{
				grad.at<uchar>(i, j) = 0;
			}
			else
			{
				uchar val = (uchar)(temp[i][j] / max_var * 255.0);
				//cout << (int)val << endl;
				grad.at<uchar>(i, j) = val;
			}
		}
	}
}

float CPointImageFusion::GetVariance(const cv::Mat & src, int win_size)
{
	using namespace std;
	double variance = 0.0;
	cv::Scalar mean = cv::mean(src);
	double average = mean.val[0];  //平均值
	int size = src.rows * src.cols;
	if (src.channels() != 1)
	{
		cout << "mat通道数不为1" << endl;
		return 0.0;
	}
	else
	{
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				variance += ((&src.data[i * src.step])[j] - average) * ((&src.data[i * src.step])[j] - average);   //平方
			}
		}
		//std::cout << " the variance is " << variance << std::endl;
	}
	return variance;
}


void CPointImageFusion::FindClearArea(const std::vector<cv::Mat>& images, std::vector<cv::Mat>& clear_images)
{
	int nums = images.size();
	for (int k = 0; k < nums; k++)
	{
		PointGrad(images[k], clear_images[k]);
	}
}

void CPointImageFusion::IndexSmooth(const cv::Mat & index_mat, cv::Mat & smooth_index_mat)
{
	cv::GaussianBlur(index_mat, smooth_index_mat, cv::Size(51, 51), 0);
}

