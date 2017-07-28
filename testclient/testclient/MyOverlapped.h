#pragma once
#include <list>
#include "GlobalInformation.h"
using namespace std;

class CMyOverlapped
{
public:
	CMyOverlapped(void);
	~CMyOverlapped(void);
public:
	list<MYOVERINFO *> m_lstUsingOverInfo;
	list<MYOVERINFO *> m_lstUsedOverInfo;
	CRITICAL_SECTION m_cs;
public:
	//申请一块MYOVERINFO 并添加到链表中
	MYOVERINFO * NewOverInfoAndAddList();
	//从链表中删除指定的OVERINFO
	void DelOverInfoFromList(MYOVERINFO * pDel);
	//删除链表中的全部
	void DeleteAllInList();
public:
	list<MYOVERGAMEINFO *> m_lstUsingGameOverInfo;
	list<MYOVERGAMEINFO *> m_lstUsedGameOverInfo;
	CRITICAL_SECTION m_Gamecs;
public:
	//申请一块MYOVERINFO 并添加到链表中
	MYOVERGAMEINFO * NewOverGameInfoAndAddList();
	//从链表中删除指定的OVERINFO
	void DelOverGameInfoFromList(MYOVERGAMEINFO * pDel);
};

