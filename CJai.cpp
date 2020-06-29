#include "CJai.h"

CJai::CJai(QObject *parent)
	: QObject(parent)
{
	m_hFactory = NULL;
	m_hCam = NULL;
	m_hThread = NULL;
	m_pImg = NULL;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;
}

CJai::~CJai()
{
}

BOOL CJai::OpenFactoryAndCamera()
{
	J_STATUS_TYPE   retval;
	uint32_t        iSize;
	uint32_t        iNumDev;
	bool8_t         bHasChange;

	m_bEnableStreaming = false;

	// Open factory
	retval = J_Factory_Open((const int8_t*)"", &m_hFactory);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(QString("Could not open factory!"), retval);
		return FALSE;
	}
	std::cout << "Opening factory succeeded\n" << std::endl;

	//Update camera list
	retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(QString("Could not update camera list!"), retval);
		return FALSE;
	}
	std::cout << "Updating camera list succeeded" << std::endl;

	// Get the number of Cameras
	retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(QString("Could not get the number of cameras!"), retval);
		return FALSE;
	}

	if (iNumDev == 0)
	{
		ShowErrorMsg(QString("Invalid number of cameras!"), retval);
		return FALSE;
	}
	std::cout << iNumDev << " cameras were found" << std::endl;

	// Get camera ID
	iSize = (uint32_t)sizeof(m_sCameraId);
	retval = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_sCameraId, &iSize);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(QString("Could not get the camera ID!"), retval);
		return FALSE;
	}
	std::cout << "Camera ID: " << m_sCameraId << std::endl;

	// Open camera
	retval = J_Camera_Open(m_hFactory, m_sCameraId, &m_hCam);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(QString("Could not open the camera!"), retval);
		return FALSE;
	}

	//Make sure streaming is supported!
	uint32_t numStreams = 0;
	retval = J_Camera_GetNumOfDataStreams(m_hCam, &numStreams);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(QString("Error with J_Camera_GetNumOfDataStreams."), retval);
		return FALSE;
	}

	if (0 == numStreams)
	{
		m_bEnableStreaming = false;
	}
	else
	{
		m_bEnableStreaming = true;
	}

	if (iNumDev > 0 && 0 != m_hCam)
	{
		std::cout << "Opening camera succeeded" << std::endl;
	}
	else
	{
		ShowErrorMsg(QString("Invalid number of Devices!"), iNumDev);
		return FALSE;
	}

	return TRUE;
}

void CJai::CloseFactoryAndCamera()
{
	J_STATUS_TYPE   retval;

	if (m_hCam)
	{
		// Close camera
		retval = J_Camera_Close(m_hCam);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(QString("Could not close the camera!"), retval);
		}
		m_hCam = NULL;
		std::cout << "Closed camera" << std::endl;
	}

	if (m_hFactory)
	{
		// Close factory
		retval = J_Factory_Close(m_hFactory);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(QString("Could not close the factory!"), retval);
		}
		m_hFactory = NULL;
		std::cout << "Closed factory" << std::endl;
	}
}

int* CJai::Integral(cv::Mat& src)
{
	//求积分图像
	uchar* pSrcData = src.data;
	int *Integral = new int[(src.cols + 1)*(src.rows + 1)];
	memset(Integral, 0, (src.cols + 1)*(src.rows + 1) * sizeof(int));
	int i, j;
	int x = 0;
	int y = 0;
	//int temp = 0;  // 调试参数
	for (i = 0; i < src.rows; i++)
	{
		y = i + 1;
		for (j = 0; j < src.cols; j++)
		{
			x = j + 1;
			//int temp1 = pSrcData[i*SrclineByte + j];
			//int temp2 = Integral[(y - 1)*(SrcWidth + 1) + x];
			//int temp3 = Integral[y*(SrcWidth + 1) + x - 1];
			//int temp4 = Integral[(y - 1)*(SrcWidth + 1) + x - 1];
			//Integral[y * (src.cols + 1) + x] = pSrcData[i*SrclineByte + j] + Integral[(y - 1)*(src.cols + 1) + x]
			//	+ Integral[y*(src.cols + 1) + x - 1] - Integral[(y - 1)*(src.cols + 1) + x - 1];
			Integral[y * (src.cols + 1) + x] = src.at<uchar>(i,j) + Integral[(y - 1)*(src.cols + 1) + x]
				+ Integral[y*(src.cols + 1) + x - 1] - Integral[(y - 1)*(src.cols + 1) + x - 1];
			//temp = Integral[y * (SrcWidth+1) + x]; //调试参数
		}
	}
	return Integral;
}

void CJai::AdaptiveThreshold(cv::Mat& SrcImage, cv::Mat& DstImage)
{
	//阈值分割
	int SrcHeight = SrcImage.rows;
	int SrcWidth = SrcImage.cols;
	int *integral = Integral(SrcImage);
	int s = SrcHeight / 32;
	int t = 0;
	int x1, x2, y1, y2, sum, dy, dx;
	for (int i = 0; i < SrcHeight; i++)
	{
		dy = i + 1;
		for (int j = 0; j < SrcWidth; j++)
		{
			dx = j + 1;
			x1 = j - s / 2;
			x2 = j + s / 2;
			y1 = i - s / 2;
			y2 = i + s / 2;
			if (x1 < 0) x1 = 0;//边界判断
			if (y1 < 0) y1 = 0;//边界判断
			if (x2 >= SrcWidth) x2 = SrcWidth - 1;//边界判断
			if (y2 >= SrcHeight) y2 = SrcHeight - 1;//边界判断
			int count = (x2 - x1 + 1)*(y2 - y1 + 1);
			sum = integral[(SrcWidth + 1)*(y2 + 1) + (x2 + 1)]
				+ integral[(SrcWidth + 1)*(y1 + 1) + (x1 + 1)]
				- integral[(SrcWidth + 1)*(y2 + 1) + (x1 + 1)]
				- integral[(SrcWidth + 1)*(y1 + 1) + (x2 + 1)];
			//double temp1 = pSrcData[i*SrclineByte + j] * count;
			double temp1 = SrcImage.at<uchar>(i, j)*count;
			double temp2 = (sum*(100 - t) / 100.0);
			if (temp1 <= temp2)
			{
				DstImage.at<uchar>(i,j) = 0;
			}
			else
			{
				DstImage.at<uchar>(i, j) = 255;
			}
		}
	}
	delete[]integral;
}

void CJai::StreamCBFunc(J_tIMAGE_INFO* pAqImageInfo)
{
	if (m_pImg == NULL)
	{
		// Create the Image:
		// We assume this is a 8-bit monochrome image in this sample
		m_pImg = cvCreateImage(cvSize(pAqImageInfo->iSizeX, pAqImageInfo->iSizeY), IPL_DEPTH_8U, 1);
	}
	memcpy(m_pImg->imageData, pAqImageInfo->pImageBuffer, m_pImg->imageSize);
	m_img = cv::cvarrToMat(m_pImg);
	//cv::rotate(m_img, m_img, cv::ROTATE_90_COUNTERCLOCKWISE);
	this->myRotateAntiClockWise90(m_img);
	//cv::GaussianBlur(m_img, m_img, cv::Size(9, 9), 0);
	//m_img_roi = m_img(cv::Rect(60, 670, 30, 60));
	//cv::Mat grad_x, grad_y;
	//cv::Mat abs_grad_x, abs_grad_y;
	//cv::Sobel(m_img, grad_x, CV_16S, 1, 0, 3, 1, 1, cv::BORDER_DEFAULT);
	//cv::convertScaleAbs(grad_x, abs_grad_x);
	//cv::Sobel(m_img, grad_y, CV_16S, 0, 1, 3, 1, 1, cv::BORDER_DEFAULT);
	//cv::convertScaleAbs(grad_y, abs_grad_y);
	//cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, m_img);
	//cv::threshold(m_img, m_img, 60, 255, cv::THRESH_OTSU);
	//cv::Mat bw(m_img.size(), m_img.type());
	//AdaptiveThreshold(m_img, bw);
	//cv::Canny(m_img, m_img, 10, 30);
	if(m_bEnableStreaming)
		emit Send_Image(m_img);
}

void CJai::ShowErrorMsg(QString message, J_STATUS_TYPE error)
{
	QString errorMsg;
	//errorMsg.Format(_T("%s: Error = %d: "), message, error);
	errorMsg = message;
	switch (error)
	{
	case J_ST_INVALID_BUFFER_SIZE:	errorMsg += "Invalid buffer size ";	                break;
	case J_ST_INVALID_HANDLE:		errorMsg += "Invalid handle ";		                break;
	case J_ST_INVALID_ID:			errorMsg += "Invalid ID ";			                break;
	case J_ST_ACCESS_DENIED:		errorMsg += "Access denied ";		                break;
	case J_ST_NO_DATA:				errorMsg += "No data ";				                break;
	case J_ST_ERROR:				errorMsg += "Generic error ";		                break;
	case J_ST_INVALID_PARAMETER:	errorMsg += "Invalid parameter ";	                break;
	case J_ST_TIMEOUT:				errorMsg += "Timeout ";				                break;
	case J_ST_INVALID_FILENAME:		errorMsg += "Invalid file name ";	                break;
	case J_ST_INVALID_ADDRESS:		errorMsg += "Invalid address ";		                break;
	case J_ST_FILE_IO:				errorMsg += "File IO error ";		                break;
	case J_ST_GC_ERROR:				errorMsg += "GenICam error ";		                break;
	case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
	case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
	}
	QMessageBox::information(NULL, "ErrorMessage", errorMsg);
	//AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);
}

void CJai::StartAcquisition()
{
	J_STATUS_TYPE   retval;
	int64_t int64Val;
	int64_t pixelFormat;

	SIZE	ViewSize;
	POINT	TopLeft;

	if (!m_bEnableStreaming)
	{
		ShowErrorMsg(QString("Streaming not enabled on this device."), 0);
		return;
	}
	// Get Width from the camera
	//retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_WIDTH, 750);
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
	ViewSize.cx = (LONG)int64Val;     // Set window size cx

	// Get Height from the camera
	//retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_HEIGHT, 1000);
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
	ViewSize.cy = (LONG)int64Val;     // Set window size cy

	// Get pixelformat from the camera
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &int64Val);
	pixelFormat = int64Val;

	// Calculate number of bits (not bytes) per pixel using macro
	int bpp = J_BitsPerPixel(pixelFormat);
	std::cout << "Image Width is " << ViewSize.cx << std::endl;
	std::cout << "Image height is " << ViewSize.cy << std::endl;
	// Open stream
	retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CJai::StreamCBFunc), &m_hThread, (ViewSize.cx * ViewSize.cy * bpp) / 8);
	if (retval != J_ST_SUCCESS) {
		//AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
		QMessageBox::information(NULL, "ErrorMessage", "Could not open stream!");
		return;
	}
	std::cout << "Opening stream succeeded" << std::endl;

	// Start Acquisition
	retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
}

void CJai::StopAcquisition()
{
	J_STATUS_TYPE retval;
	emit Stop_Signal();
	if (!m_bEnableStreaming)
	{
		return;
	}

	// Stop Acquisition
	if (m_hCam) {
		retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
	}

	if (m_hThread)
	{
		// Close stream
		retval = J_Image_CloseStream(m_hThread);
		m_hThread = NULL;
		std::cout << "Closed stream" << std::endl;
	}

	if (m_pImg != NULL)
	{
		cvReleaseImage(&m_pImg);
		m_pImg = NULL;
	}
	
}

void CJai::myRotateAntiClockWise90(cv::Mat& src)
{
	if (src.empty())
	{
		std::cout << "mat is empty!" << std::endl;
	}
	transpose(src, src);
	flip(src, src, 0);
}








