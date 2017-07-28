#include <iostream>
#include "UDPNet.h"
using namespace std;

void Run()
{
	int endnum = 0;
	while(1)
	{
		cin>>endnum;
		if(endnum == -1)
		{
			break;
		}
		cout<<"输入命令无效，输入-1关闭该服务器"<<endl;
	}
}

int main()
{
	CUDPNet udpnet;
	udpnet.InitialUDP();

	Run();

	udpnet.UnInitial();

	return 0;
}