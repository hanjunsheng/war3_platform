#pragma once

//WAR3端口
#define GAME_PORT 6112
//客户端端口号
#define CLIENT_PORT 4200
//客户端与游戏通信端口
#define CLIENT_TCP_GAME 4201
//地图包最大大小
#define MAX_MAP_SIZE 300
//event
#define MY_QUERY_EVENT_NAME "MyQueryEventName"
//continue event
#define MY_CONTINUE_ENENT_NAME "MyContinueEventName"
//map
#define MY_MAPFILE_NAME "MyMapFileName"

//协议
enum TRANSMIT_PROTOCOL_TYPE
{
	TRANSMIT_PROTOCOL_USERLOGIN,	  //这是一个用户登陆的包
	TRANSMIT_PROTOCOL_GAMEUDP,		  //这是一个游戏UDP数据包
	TRANSMIT_PROTOCOL_FILEVERSION,	  //这是游戏文件的版本号
	TRANSMIT_PROTOCOL_USERQUIT,	      //这是一个用户退出的包
	TRANSMIT_PROTOCOL_HOUSECANCEL,	  //指定的游戏主机已经被关闭
	TRANSMIT_PROTOCOL_HOUSEMAPINFO,	  //指定的游戏主机地图包信息
	TRANSMIT_PROTOCOL_CONNECTHOUSE,   //客户端请求连接指定的游戏主机
	TRANSMIT_PROTOCOL_VIRTUALIPADDR,  //分配虚拟IP地址
	TRANSMIT_PROTOCOL_PEOPLECHANGE,   //房间人数变更
	TRANSMIT_PROTOCOL_CONNECTINFO,    //通知服务器连接信息
	TRANSMIT_PROTOCOL_CLIENTINFO,     //服务器发来的客户机信息
	TRANSMIT_PROTOCOL_GAMEINFO       //tcp游戏包
};

//HOOK信息
typedef struct _HOOKADDRINFO
{
	DWORD OldFuncAddr;
	DWORD NewFuncAddr;
	BOOL  bIsHook;
}HOOKADDRINFO;

typedef struct _ASSOCIATE_VIRTUAL_IP_ADDR
{
	in_addr ClientVirtualIp;
}ASSOCIATE_VIRTUAL_IP_ADDR;

//传输的类型
typedef struct _TRANSMIT_PROTOCOL_METHOD
{
	TRANSMIT_PROTOCOL_TYPE Protocol;
}TRANSMIT_PROTOCOL_METHOD;

//获取指定房间的主机地图包的封包结构
typedef struct _TRANSMIT_GAMEHOUSE_MAPINFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIp;
	ULONG uMapLen;												//地图 len
	ULONG uHouseId;												//房间ID
	BYTE lpszMapInfo;										    //实际的地图包data	
}TRANSMIT_GAMEHOUSE_MAPINFO;

//告知客户端游戏要连接的地址
typedef struct _TRANSMIT_CONNECT_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIpAddr;
}TRANSMIT_CONNECT_INFO;

//服务器分配的虚拟ip返回客户端
typedef struct _TRANSMIT_VIRTUAL_IP
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr VirtualIpAddr;
}TRANSMIT_VIRTUAL_IP;
