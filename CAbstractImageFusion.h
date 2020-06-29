/**************************************************************************

Copyright:CXX

Author: cxx

Date:2020-06-08

Description: Abstract base class for image fusion using template method

**************************************************************************/

#include <iostream>
#include <assert.h>
#include <opencv2/opencv.hpp>

#pragma once
class CAbstractImageFusion
{
public:
	CAbstractImageFusion();
	virtual ~CAbstractImageFusion();
//--------------------------------------------------------------------------------------------------------------------
	/*
	Fuse��ͼ���ںϵ��㷨����
	@parameter images: �ཹ��ͼ������
	@parameter is_smooth: �Ƿ������ͼ�����ƽ������
	*/
	void Fuse(const std::vector<cv::Mat>& images, cv::Mat& fusion_image, bool is_smooth = false);
//--------------------------------------------------------------------------------------------------------------------

protected:
//--------------------------------------------------------------------------------------------------------------------
	/*
	FindClearArea: �ҵ��ཹ������ͼ��ÿһ���������
	@parameter images: �ཹ��ͼ������
	@parameter clear_images: �õ���������ͼ������
	*/
	virtual void FindClearArea(const std::vector<cv::Mat>& images, std::vector<cv::Mat>& clear_images) = 0;
//--------------------------------------------------------------------------------------------------------------------
	/*
	GetIndex: ��ȡͼ��������ÿ�������������ֵ��ͼ��������
	@parameter images: ������ͼ������
	@parameter index_mat: �õ�����������
	*/
	virtual void GetIndex(const std::vector<cv::Mat>& clear_images, cv::Mat& index_mat);
//--------------------------------------------------------------------------------------------------------------------
	/*
	IndexMapping2Images: ��index_mat����ÿ�����Ӧ��ͼ�����к�ӳ���ԭ�ཹ��ͼ�����ж�Ӧ��ŵĶ�Ӧ��ĻҶ�ֵ
	@parameter index_mat: ��������
	@parameter images: ԭ�ཹ��ͼ������
	@parameter fusion_image: ����õ��ں�ͼ 
	*/
	virtual void IndexMapping2Images(const cv::Mat& index_mat, const std::vector<cv::Mat>& images, cv::Mat& fusion_image);
//--------------------------------------------------------------------------------------------------------------------
	/*
	IndexSmooth: ���ø�˹����������ƽ��������ƽ����������
	@parameter index_mat: ��������
	@parameter smooth_index_mat: ƽ�������������
	*/
	virtual void IndexSmooth(const cv::Mat& index_mat, cv::Mat& smooth_index_mat);
//--------------------------------------------------------------------------------------------------------------------

};

