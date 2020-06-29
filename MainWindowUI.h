#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QMutex>
#include "ui_MainWindowUI.h"
//#include "CGrabImage.h"
#include "CSystemControl.h"
#include <QString>
#include "CJai.h"
#include "CSocketThread.h"

class MainWindowUI : public QMainWindow
{
	Q_OBJECT

public:
	MainWindowUI(QWidget *parent = Q_NULLPTR);
	~MainWindowUI();
private:
	Ui::MainWindowUIClass ui;
	//QThread _read_thread;
	//CGrabImage* _grab_image;
	CSystemControl _system_control;
	CJai _jai;
	QMutex _mutex;
	CSocketThread _socket;
signals:
	//void StartReadThread();
	void SendStopSignal();
public slots:
	void On_Reset();
	void On_Left();
	void On_Right();
	void On_Up();
	void On_Down();
	void On_Front();
	void On_Back();
	void On_Light_Change(int value);
	void On_Save_Pic();
	void On_Focus();
	void On_ROI_Focus();
	void On_Fourer_Focus();
	void On_Scan_Sample();
	void On_Scan_Deep();
	void On_Stop_Scan();
	void UpdatePositionUI();
	void Receive_Cmd(int cmd, int length);
};
