#include "CAbstractImageFusion.h"

CAbstractImageFusion::CAbstractImageFusion()
{

}

CAbstractImageFusion::~CAbstractImageFusion()
{
}

void CAbstractImageFusion::Fuse(const std::vector<cv::Mat>& images, cv::Mat& fusion_image, bool is_smooth)
{
	using namespace std;
	int nums = images.size();
	if (nums == 0)
	{
		cout << "error! clear_images.size() == 0 !" << endl;
		return;
	}
	int rows = images[0].rows;
	int cols = images[0].cols;
	vector<cv::Mat> clear_images;
	for (int i = 0; i < images.size(); i++)
	{
		cv::Mat tmp = cv::Mat::zeros(rows, cols, CV_8UC1);
		clear_images.push_back(tmp.clone());
	}
	//获得清晰度三维矩阵
	this->FindClearArea(images, clear_images);
	//获取索引图像
	cv::Mat index_mat = cv::Mat(rows, cols, CV_8UC1);
	this->GetIndex(clear_images, index_mat);
	cv::imwrite("未平滑索引图.jpg", index_mat);
	//如果需要平滑索引图像
	if (is_smooth)
	{
		cv::Mat smooth_index_mat = cv::Mat(rows, cols, CV_8UC1);
		//索引图像平滑
		this->IndexSmooth(index_mat, smooth_index_mat);
		//获取最后融合后的图像
		fusion_image = cv::Mat(rows, cols, CV_8UC1);
		this->IndexMapping2Images(smooth_index_mat, images, fusion_image);
		cv::imwrite("高斯平滑索引图_高斯.jpg", smooth_index_mat);
	}
	else
	{
		//获取最后融合后的图像
		fusion_image = cv::Mat(rows, cols, CV_8UC1);
		this->IndexMapping2Images(index_mat, images, fusion_image);
	}
	
}

void CAbstractImageFusion::GetIndex(const std::vector<cv::Mat>& clear_images, cv::Mat & index_mat)
{
	int nums = clear_images.size();
	int rows = clear_images[0].rows;
	int cols = clear_images[0].cols;
	//cv::imwrite("grad1.jpg", clear_images[50]);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//存放点(i,j)，清晰度值最大的图层序号
			int max_index = 0;
			//当前的最清晰度值
			int cur_clarity = clear_images[0].at<uchar>(i, j);
			for (int k = 1; k < nums; k++)
			{
				if (clear_images[k].at<uchar>(i, j) > cur_clarity)
				{
					cur_clarity = clear_images[k].at<uchar>(i, j);
					max_index = k;
				}
			}
			index_mat.at<uchar>(i, j) = max_index;
		}
	}
	//cv::imwrite("index.jpg", index_mat);
}

void CAbstractImageFusion::IndexMapping2Images(const cv::Mat & index_mat, const std::vector<cv::Mat>& images, cv::Mat & fusion_image)
{
	using namespace std;
	int rows = index_mat.rows;
	int cols = index_mat.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int index = index_mat.at<uchar>(i, j);
			fusion_image.at<uchar>(i, j) = images[index].at<uchar>(i, j);
		}
	}
}

void CAbstractImageFusion::IndexSmooth(const cv::Mat & index_mat, cv::Mat & smooth_index_mat)
{
	cv::GaussianBlur(index_mat, smooth_index_mat, cv::Size(7, 7), 0);
}

