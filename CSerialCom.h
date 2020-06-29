/*
	经过测试建议将显微镜Z轴的运动范围限制为-10000-8000之间
*/
#pragma once

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QSerialPortInfo>
#include "CPosition.h"
#include <QThread>
#include <QCoreApplication>
#include <QTime>

class CSerialCom : public QObject
{
	Q_OBJECT

public:
	CSerialCom(QObject *parent);
	virtual ~CSerialCom();
	enum { Z_AXIS = 1, X_AXIS = 2, Y_AXIS = 3, Light = 4, SUCCESS, FAIL };
/*以下为类公用接口定义*/
	void OpenSerialPort();
	void CloseSerialPort();
	void ResetPlatform();
	void ChangeLight(int value);
	void MovePlatform(int step_length, int axis);
	void SetPosition(CPosition position);
	CPosition GetPosition();
	void SetStepLength(int step_length);
	int GetStepLength();
private:
	QSerialPort _serial_port;  //串口类
	CPosition _position;  //平台当前位置
	int _step_length;     //移动步长
public:
	int Check();
	void Sleep(int msec);
};
