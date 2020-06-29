#pragma once

#include <QThread>
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

class CSocketThread : public QThread
{
	Q_OBJECT

public:
	CSocketThread(QObject* parent = nullptr);
	enum { Z_AXIS = 1, X_AXIS = 2, Y_AXIS = 3, Light = 4, SUCCESS, FAIL, RESET };
	void Bin2Int(char* buf);
	virtual void run();
	~CSocketThread();
signals:
	void SendCmd(int, int);
};
