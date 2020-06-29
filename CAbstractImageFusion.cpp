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
	//�����������ά����
	this->FindClearArea(images, clear_images);
	//��ȡ����ͼ��
	cv::Mat index_mat = cv::Mat(rows, cols, CV_8UC1);
	this->GetIndex(clear_images, index_mat);
	cv::imwrite("δƽ������ͼ.jpg", index_mat);
	//�����Ҫƽ������ͼ��
	if (is_smooth)
	{
		cv::Mat smooth_index_mat = cv::Mat(rows, cols, CV_8UC1);
		//����ͼ��ƽ��
		this->IndexSmooth(index_mat, smooth_index_mat);
		//��ȡ����ںϺ��ͼ��
		fusion_image = cv::Mat(rows, cols, CV_8UC1);
		this->IndexMapping2Images(smooth_index_mat, images, fusion_image);
		cv::imwrite("��˹ƽ������ͼ_��˹.jpg", smooth_index_mat);
	}
	else
	{
		//��ȡ����ںϺ��ͼ��
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
			//��ŵ�(i,j)��������ֵ����ͼ�����
			int max_index = 0;
			//��ǰ����������ֵ
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

