#include "CSocketThread.h"

CSocketThread::CSocketThread(QObject *parent)
	: QThread(parent)
{
}

void CSocketThread::Bin2Int(char * buf)
{

}

void CSocketThread::run()
{
	fd_set allSockets;
	WORD wdVersion = MAKEWORD(2, 2); //2.1  //22
	//int a = *((char*)&wdVersion);
	//int b = *((char*)&wdVersion+1);
	WSADATA wdScokMsg;
	//LPWSADATA lpw = malloc(sizeof(WSADATA));// WSADATA*
	int nRes = WSAStartup(wdVersion, &wdScokMsg);

	if (0 != nRes)
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("重启下电脑试试，或者检查网络库");
			break;
		case WSAVERNOTSUPPORTED:
			printf("请更新网络库");
			break;
		case WSAEINPROGRESS:
			printf("请重新启动");
			break;
		case WSAEPROCLIM:
			printf("请尝试关掉不必要的软件，以为当前网络运行提供充足资源");
			break;
		}
		return;
	}

	//校验版本
	if (2 != HIBYTE(wdScokMsg.wVersion) || 2 != LOBYTE(wdScokMsg.wVersion))
	{
		//说明版本不对
		//清理网络库
		WSACleanup();
		return;
	}

	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//int a = WSAGetLastError();
	if (INVALID_SOCKET == socketServer)
	{
		int a = WSAGetLastError();
		//清理网络库
		WSACleanup();
		return;
	}

	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	si.sin_addr.S_un.S_addr = inet_addr("10.206.152.172");
	//int a = ~0;
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*) & si, sizeof(si)))
	{
		//出错了
		int a = WSAGetLastError();
		//释放
		closesocket(socketServer);
		//清理网络库
		WSACleanup();
		return;
	}

	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		//出错了
		int a = WSAGetLastError();
		//释放
		closesocket(socketServer);
		//清理网络库
		WSACleanup();
		return;
	}


	//清零
	FD_ZERO(&allSockets);
	//服务器装进去
	FD_SET(socketServer, &allSockets);

	while (1)
	{
		//简单注释
		fd_set readSockets = allSockets;
		fd_set writeSockets = allSockets;
		//可以有也可以没有
		//FD_CLR(socketServer, &writeSockets);
		//fd_set errorSockets = allSockets;

		//时间段
		struct timeval st;
		st.tv_sec = 3;
		st.tv_usec = 0;

		//select
		int nRes = select(0, &readSockets, &writeSockets, nullptr, &st);

		if (0 == nRes) //没有响应的socket
		{
			continue;
		}
		else if (nRes > 0)
		{
			//处理错误
			//for (u_int i = 0; i < errorSockets.fd_count; i++)
			//{
			//	char str[100] = { 0 };
			//	int len = 99;
			//	if (SOCKET_ERROR == getsockopt(errorSockets.fd_array[i], SOL_SOCKET, SO_ERROR, str, &len))
			//	{
			//		printf("无法得到错误信息\n");
			//	}
			//	printf("%s\n", str);
			//}

			//for (u_int i = 0; i < writeSockets.fd_count; i++)
			//{
			//	//printf("服务器%d,%d:可写\n", socketServer, writeSockets.fd_array[i]);
			//	if (SOCKET_ERROR == send(writeSockets.fd_array[i], "ok", 2, 0))
			//	{
			//		int a = WSAGetLastError();
			//	}
			//}

			//有响应
			for (u_int i = 0; i < readSockets.fd_count; i++)
			{
				if (readSockets.fd_array[i] == socketServer)
				{
					//accept
					SOCKET socketClient = accept(socketServer, NULL, NULL);
					if (INVALID_SOCKET == socketClient)
					{
						//链接出错
						continue;
					}

					FD_SET(socketClient, &allSockets);
					//send
				}
				else
				{
					char strBuf[1024] = { '/' };
					//客户端吧
					int nRecv = recv(readSockets.fd_array[i], strBuf, 1024, 0);
					//send
					if (0 == nRecv)
					{
						//客户端下线了
						//从集合中拿掉
						SOCKET socketTemp = readSockets.fd_array[i];
						FD_CLR(readSockets.fd_array[i], &allSockets);
						//释放
						closesocket(socketTemp);
					}
					else if (0 < nRecv)
					{
						//接收到了消息
						char dir = strBuf[0];
						char* length = strBuf + 1;
						int len;
						std::string s;
						for (int i = 0; i < 1024; i++)
						{
							if (length[i] == '/')
								break;
							s += length[i];
						}
						len = std::atoi(s.c_str());
						switch (dir)
						{
						case '0':
							emit SendCmd(Z_AXIS, 100);
							break;
						case 'd':
							emit SendCmd(Z_AXIS, -len);
							break;
						case 'r':
							emit SendCmd(X_AXIS, len);
							break;
						case 'l':
							emit SendCmd(X_AXIS, -len);
							break;
						case 'f':
							emit SendCmd(Y_AXIS, len);
							break;
						case 'b':
							emit SendCmd(Y_AXIS, -len);
							break;
						case 'L':
							emit SendCmd(Light, len);
							break;
						case 'R':
							emit SendCmd(RESET, 0);

						}
						std::cout << strBuf << std::endl;
					}
					else //SOCK_ERROR
					{
						//强制下线也叫出错 10054
						int a = WSAGetLastError();
						switch (a)
						{
						case 10054:
						{
							SOCKET socketTemp = readSockets.fd_array[i];
							FD_CLR(readSockets.fd_array[i], &allSockets);
							//释放
							closesocket(socketTemp);
						}
						}
					}
				}
			}
		}
		else
		{
			//发生错误了
			break;
		}

	}

	////一定要close
	//FD_CLR(socketServer, &clientSockets);
	////closesocket(socketServer);

	//int a = FD_ISSET(socketServer, &clientSockets);
	//

	//释放所有socket
	for (u_int i = 0; i < allSockets.fd_count; i++)
	{
		closesocket(allSockets.fd_array[i]);
	}

	//清理网络库
	WSACleanup();
}

CSocketThread::~CSocketThread()
{
}
