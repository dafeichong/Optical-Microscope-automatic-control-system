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
	Fuse：图像融合的算法步骤
	@parameter images: 多焦面图像序列
	@parameter is_smooth: 是否对索引图像进行平滑处理
	*/
	void Fuse(const std::vector<cv::Mat>& images, cv::Mat& fusion_image, bool is_smooth = false);
//--------------------------------------------------------------------------------------------------------------------

protected:
//--------------------------------------------------------------------------------------------------------------------
	/*
	FindClearArea: 找到多焦面序列图像每一层的清晰点
	@parameter images: 多焦面图像序列
	@parameter clear_images: 得到的清晰度图像序列
	*/
	virtual void FindClearArea(const std::vector<cv::Mat>& images, std::vector<cv::Mat>& clear_images) = 0;
//--------------------------------------------------------------------------------------------------------------------
	/*
	GetIndex: 获取图像序列中每个点最大清晰度值的图像索引号
	@parameter images: 清晰度图像序列
	@parameter index_mat: 得到的索引矩阵
	*/
	virtual void GetIndex(const std::vector<cv::Mat>& clear_images, cv::Mat& index_mat);
//--------------------------------------------------------------------------------------------------------------------
	/*
	IndexMapping2Images: 将index_mat矩阵每个点对应的图像序列号映射回原多焦面图像序列对应序号的对应点的灰度值
	@parameter index_mat: 索引矩阵
	@parameter images: 原多焦面图像序列
	@parameter fusion_image: 最后获得的融合图 
	*/
	virtual void IndexMapping2Images(const cv::Mat& index_mat, const std::vector<cv::Mat>& images, cv::Mat& fusion_image);
//--------------------------------------------------------------------------------------------------------------------
	/*
	IndexSmooth: 利用高斯函数或其他平滑函数，平滑索引矩阵
	@parameter index_mat: 索引矩阵
	@parameter smooth_index_mat: 平滑后的索引矩阵
	*/
	virtual void IndexSmooth(const cv::Mat& index_mat, cv::Mat& smooth_index_mat);
//--------------------------------------------------------------------------------------------------------------------

};

