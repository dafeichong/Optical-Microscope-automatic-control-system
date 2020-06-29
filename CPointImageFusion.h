/**************************************************************************

Copyright:CXX

Author: cxx

Date:2020-06-08

Description: Concrete class for image fusion inherited by CAbstractImageFusion 
			 override FindClearArea method using point grad

**************************************************************************/

#pragma once
#include "CAbstractImageFusion.h"

class CPointImageFusion : public CAbstractImageFusion
{
public:
	CPointImageFusion();
	virtual ~CPointImageFusion();
//--------------------------------------------------------------------------------------------------------------------
	/*
	PointGrad: �ҵ��ཹ������ͼ��ÿһ���������, grad(x, y) = abs(src(x + 1, y) - src(x - 1, y)) + abs(src(x, y + 1) - src(x, y - 1))
	@parameter src: ԭʼ����ͼ��
	@parameter grad: �õ����ݶ�ͼ��
	*/
	void PointGrad(const cv::Mat& src, cv::Mat& grad);
//--------------------------------------------------------------------------------------------------------------------
	/*
	TenenGrad: �ҵ��ཹ������ͼ��ÿһ���������
	@parameter src: ԭʼ����ͼ��
	@parameter grad: �õ����ݶ�ͼ��
	*/
	void TenenGrad(const cv::Mat& src, cv::Mat& grad, int win_size = 3);
//--------------------------------------------------------------------------------------------------------------------
	/*
	TenenGrad: �ҵ��ཹ������ͼ��ÿһ���������
	@parameter src: ԭʼ����ͼ��
	@parameter grad: �õ����ݶ�ͼ��
	*/
	void Variance(const cv::Mat& src, cv::Mat& grad, int win_size = 3);
//--------------------------------------------------------------------------------------------------------------------
	/*
	GetVariance: �ҵ��ཹ������ͼ��ÿһ���������
	@parameter src: ԭʼ����ͼ��
	@return : ����õ��ķ���ֵ
	*/
	float GetVariance(const cv::Mat& src, int win_size = 3);
//--------------------------------------------------------------------------------------------------------------------
	float max_var;
protected:
	//override functions
//--------------------------------------------------------------------------------------------------------------------
	/*
	FindClearArea: �ҵ��ཹ������ͼ��ÿһ���������
	@parameter images: �ཹ��ͼ������
	@parameter clear_images: �õ���������ͼ������
	*/
	virtual void FindClearArea(const std::vector<cv::Mat>& images, std::vector<cv::Mat>& clear_images);
//--------------------------------------------------------------------------------------------------------------------
	/*
	IndexSmooth: ���ø�˹����������ƽ��������ƽ����������
	@parameter index_mat: ��������
	@parameter smooth_index_mat: ƽ�������������
	*/
	virtual void IndexSmooth(const cv::Mat& index_mat, cv::Mat& smooth_index_mat);
//--------------------------------------------------------------------------------------------------------------------
};

