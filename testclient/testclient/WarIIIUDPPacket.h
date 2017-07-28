#ifndef WARIIIUDPPACKET_H__
#define WARIIIUDPPACKET_H__

#include <windows.h>
using namespace std;

enum UDP_PACKET_OPERATION_TYPE
{
	UDP_PACKET_INITIALIZE = 0x1,		//这是游戏初始化坏时候发送的
	UDP_PACKET_QUERY_HOST = 0x2F,		//查询本地游戏主机
	UDP_PACKET_ECHO_HOST,				//应答其他客户机
	UDP_PACKET_CREATE_HOST,				//主机端创建地图时
	UDP_PACKET_PEOPLECHANGE,			//房间内玩家发生变更
	UDP_PACKET_CANCELHOST,				//主机端取消地图创建
};

//UDP数据头部
typedef struct _WAR3_UDP_HEAD_PACKET
{
	BYTE SignTrue;				//标志码
	BYTE Operation;				//操作吗
	WORD PacketLen;				//封包长度
}WAR3_UDP_HEAD_PACKET;

//UDP数据类型

//	操作码0x2F
//  功能:广播本地局域网查询可加入的游戏主机
typedef struct _WAR3_UDP_QUERY_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;		//头部数据
	DWORD					GameType;	//游戏类型
	DWORD					GameVersion;//游戏版本
	DWORD					GameID;		//游戏ID<广播时置零>
}WAR3_UDP_QUERY_PACKET;

//	操作码0x31
//	功能:当主机端建立好房间后，会将此数据进行广播，告知其他客户有主机已经建立了
typedef struct _WAR3_UDP_CREATEHOST_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;			//头
	DWORD					GameType;		//游戏类型
	DWORD					GameVersion;	//游戏版本，不同版本无视此封包
	DWORD					GameID;			//游戏ID（初始为1，随着逐个游戏的创建而自增）
}WAR3_UDP_CREATEHOST_PACKET;

//	操作码0x32
//	功能:当主机端建立好房间后，游戏内的玩家数发生变更后会广播此封包
typedef struct _WAR3_UDP_PEOPLECHANGE_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;			//头
	DWORD					GameID;			//游戏ID（初始为1，随着逐个游戏的创建而自增）
	DWORD					PeopleNumber;	//当前玩家数量
	DWORD					PeopleTotal;	//房间可加入的玩家总数
}WAR3_UDP_PEOPLECHANGE_PACKET;

//	操作码0x33
//	功能:当主机端取消建立好的房间后，会广播此数据包
typedef struct _WAR3_UDP_CANCELHOST_PACKET
{
	WAR3_UDP_HEAD_PACKET	Head;			//头
	DWORD					GameID;			//游戏ID（初始为1，随着逐个游戏的创建而自增）
}WAR3_UDP_CANCELHOST_PACKET;

#endif