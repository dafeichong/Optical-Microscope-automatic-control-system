/*
	�������Խ��齫��΢��Z����˶���Χ����Ϊ-10000-8000֮��
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
/*����Ϊ�๫�ýӿڶ���*/
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
	QSerialPort _serial_port;  //������
	CPosition _position;  //ƽ̨��ǰλ��
	int _step_length;     //�ƶ�����
public:
	int Check();
	void Sleep(int msec);
};
