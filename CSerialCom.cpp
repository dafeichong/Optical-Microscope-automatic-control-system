#include "CSerialCom.h"
#include <iostream>

CSerialCom::CSerialCom(QObject *parent)
	: QObject(parent),_position(0,0,0),_serial_port(this),_step_length(0)
{
}

CSerialCom::~CSerialCom()
{
	this->ResetPlatform();
	this->CloseSerialPort();
}

void CSerialCom::OpenSerialPort()
{
	_serial_port.setPortName("COM1");
	_serial_port.setBaudRate(QSerialPort::Baud19200);
	_serial_port.setDataBits(QSerialPort::Data8);
	_serial_port.setParity(QSerialPort::NoParity);
	_serial_port.setStopBits(QSerialPort::OneStop);
	_serial_port.setFlowControl(QSerialPort::NoFlowControl);
	if (_serial_port.open(QIODevice::ReadWrite))
		std::cout << "serialport is opened" << std::endl;
}

void CSerialCom::CloseSerialPort()
{
	if (_serial_port.isOpen())
	{
		_serial_port.close();
		std::cout << "serialport is closed" << std::endl;
	}
}

void CSerialCom::ResetPlatform()
{
	unsigned char Buffer[8];
	QByteArray byArr;
	int ok;
	//Z轴方向reset（显微镜上下移动）
	Buffer[0] = 0x03;
	Buffer[1] = 0x55;
	Buffer[2] = 0x01;
	Buffer[3] = 0x00;
	Buffer[4] = 0x00;
	Buffer[5] = 0x00;
	Buffer[6] = 0x00;
	Buffer[7] = (Buffer[0] + (Buffer[1] << 1) + (Buffer[2] << 2) + (Buffer[3] << 3) + (Buffer[4] << 4) + (Buffer[5] << 5)
		+ (Buffer[6] << 6)) & 0xFF;
	byArr.resize(8);
	for (int i = 0; i < 8; i++)
	{
		byArr[i] = Buffer[i];
	}
	_serial_port.write(byArr);
	//确认回复命令
	ok = Check();
	if (ok == SUCCESS)
	{
	}
	else
	{
		std::cout << "Z error" << std::endl;
	}
	_position._z = 0;

	//X轴方向reset（显微镜左右移动）
	Buffer[0] = 0x03;
	Buffer[1] = 0x55;
	Buffer[2] = 0x02;
	Buffer[3] = 0x00;
	Buffer[4] = 0x00;
	Buffer[5] = 0x00;
	Buffer[6] = 0x00;
	Buffer[7] = (Buffer[0] + (Buffer[1] << 1) + (Buffer[2] << 2) + (Buffer[3] << 3) + (Buffer[4] << 4) + (Buffer[5] << 5)
		+ (Buffer[6] << 6)) & 0xFF;
	byArr.resize(8);
	for (int i = 0; i < 8; i++)
	{
		byArr[i] = Buffer[i];
	}
	_serial_port.write(byArr);
	//确认回复命令
	ok = Check();
	if (ok == SUCCESS)
	{
	}
	else
	{
		std::cout << "X error" << std::endl;
	}
	_position._x = 0;
	//Y轴方向reset（显微镜前后移动）
	Buffer[0] = 0x03;
	Buffer[1] = 0x55;
	Buffer[2] = 0x03;
	Buffer[3] = 0x00;
	Buffer[4] = 0x00;
	Buffer[5] = 0x00;
	Buffer[6] = 0x00;
	Buffer[7] = (Buffer[0] + (Buffer[1] << 1) + (Buffer[2] << 2) + (Buffer[3] << 3) + (Buffer[4] << 4) + (Buffer[5] << 5)
		+ (Buffer[6] << 6)) & 0xFF;
	byArr.resize(8);
	for (int i = 0; i < 8; i++)
	{
		byArr[i] = Buffer[i];
	}
	_serial_port.write(byArr);
	//确认回复命令
	ok = Check();
	if (ok == SUCCESS)
	{
	}
	else
	{
		std::cout << "Y error" << std::endl;
	}
	_position._y = 0;
}

void CSerialCom::ChangeLight(int value)
{
	unsigned char Buffer[8];
	Buffer[0] = 0x08;
	Buffer[1] = 0x55;
	Buffer[2] = 0;

	*(long*)(Buffer + 3) = value;    //buffer[3]~buffer[6]

	unsigned char b3 = Buffer[3];


	Buffer[6] = 0;
	Buffer[5] = 0;
	Buffer[4] = 0;
	Buffer[3] = b3;


	Buffer[7] = (Buffer[0] + (Buffer[1] << 1) + (Buffer[2] << 2) + (Buffer[3] << 3) + (Buffer[4] << 4) + (Buffer[5] << 5) + (Buffer[6] << 6)) & 0xFF;

	QByteArray byArr;
	byArr.resize(8);
	for (int i = 0; i < 8; i++)
	{
		byArr[i] = Buffer[i];
	}
	_serial_port.write(byArr);
}

int CSerialCom::Check()
{
	unsigned char ok;
	unsigned char GetBuffer[8];
	Sleep(300);

	QByteArray receiveData = _serial_port.readAll();
	for (int i = 0; i < 8; i++)
	{
		GetBuffer[i] = *(receiveData.data()+i);
	}
	unsigned char getb0 = GetBuffer[0];
	unsigned char getb1 = GetBuffer[1];
	unsigned char getb2 = GetBuffer[2];
	unsigned char getb3 = GetBuffer[3];
	unsigned char getb4 = GetBuffer[4];
	unsigned char getb5 = GetBuffer[5];
	unsigned char getb6 = GetBuffer[6];
	unsigned char getb7 = GetBuffer[7];
	ok = *(unsigned char*)(GetBuffer + 1);
	GetBuffer[6] = getb3;
	GetBuffer[5] = getb4;
	GetBuffer[4] = getb5;
	GetBuffer[3] = getb6;
	int NumOfAxis = 0;
	NumOfAxis = *(unsigned char*)(GetBuffer + 2);
	if (ok == 170)                //检验B1中的值是否为0xAA
	{
		if (NumOfAxis == Z_AXIS)
		{
			_position._z = *(long*)(GetBuffer + 3);

		}
		else if (NumOfAxis == X_AXIS)
		{
			_position._x = *(long*)(GetBuffer + 3);
		}
		else if (NumOfAxis == Y_AXIS)
		{
			_position._y = *(long*)(GetBuffer + 3);
		}
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}
}

void CSerialCom::Sleep(int msec)
{
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void CSerialCom::MovePlatform(int step_length, int axis)
{
	unsigned char Buffer[8];
	Buffer[0] = 0x01;
	Buffer[1] = 0x55;
	Buffer[2] = axis;
	if (axis == Z_AXIS)                         //Z轴
	{
		*(long*)(Buffer + 3) = _position._z + step_length;    //buffer[3]~buffer[6]
		_position._z += step_length;

	}
	if (axis == X_AXIS)                         //X轴
	{
		*(long*)(Buffer + 3) = _position._x + step_length;    //buffer[3]~buffer[6]
		_position._x += step_length;
	}
	if (axis == Y_AXIS)                         //Y轴
	{
		*(long*)(Buffer + 3) = _position._y + step_length;    //buffer[3]~buffer[6]
		_position._y += step_length;
	}
	unsigned char b3 = Buffer[3];
	unsigned char b4 = Buffer[4];
	unsigned char b5 = Buffer[5];
	unsigned char b6 = Buffer[6];
	Buffer[6] = b3;
	Buffer[5] = b4;
	Buffer[4] = b5;
	Buffer[3] = b6;
	Buffer[7] = (Buffer[0] + (Buffer[1] << 1) + (Buffer[2] << 2) + (Buffer[3] << 3) +
		(Buffer[4] << 4) + (Buffer[5] << 5) + (Buffer[6] << 6)) & 0xFF;
	QByteArray byArr;
	byArr.resize(8);
	for (int i = 0; i < 8; i++)
	{
		byArr[i] = Buffer[i];
	}
	_serial_port.write(byArr);
}

void CSerialCom::SetPosition(CPosition position)
{
	_position = position;
}

CPosition CSerialCom::GetPosition()
{
	return _position;
}

void CSerialCom::SetStepLength(int step_length)
{
	_step_length = step_length;
}

int CSerialCom::GetStepLength()
{
	return _step_length;
}

