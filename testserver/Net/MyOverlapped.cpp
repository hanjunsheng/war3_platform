#include "MyOverlapped.h"

CMyOverlapped::CMyOverlapped(void)
{
	InitializeCriticalSection(&this->m_cs);
	InitializeCriticalSection(&this->m_Gamecs);
}

CMyOverlapped::~CMyOverlapped(void)
{
	DeleteAllInList();
	DeleteCriticalSection(&this->m_cs);
	DeleteCriticalSection(&this->m_Gamecs);
}

//申请一块MYOVERINFO 并添加到链表中
MYOVERINFO * CMyOverlapped::NewOverInfoAndAddList()
{
	MYOVERINFO *pmyoverinfo = NULL;
	EnterCriticalSection(&this->m_cs);
	if(this->m_lstUsedOverInfo.empty() == false)
	{
		pmyoverinfo = this->m_lstUsedOverInfo.front();
		this->m_lstUsedOverInfo.pop_front();
	}
	else
	{
		pmyoverinfo = new MYOVERINFO;
	}
	this->m_lstUsingOverInfo.push_back(pmyoverinfo);
	LeaveCriticalSection(&this->m_cs);
	ZeroMemory(pmyoverinfo,sizeof(MYOVERINFO));

	return pmyoverinfo;
}

//从链表中删除指定的OVERINFO
void CMyOverlapped::DelOverInfoFromList(MYOVERINFO * pDel)
{
	EnterCriticalSection(&this->m_cs);
	list<MYOVERINFO *>::iterator ite = this->m_lstUsingOverInfo.begin();
	while(ite != this->m_lstUsingOverInfo.end())
	{
		if(*ite == pDel)
		{
			MYOVERINFO * ptemp = *ite;
			this->m_lstUsingOverInfo.erase(ite);
			this->m_lstUsedOverInfo.push_back(ptemp);
			break;
		}
		++ite;
	}
	LeaveCriticalSection(&this->m_cs);
}

//删除链表中的全部
void CMyOverlapped::DeleteAllInList()
{
	//MYOVERINFO
	EnterCriticalSection(&this->m_cs);
	list<MYOVERINFO *>::iterator iteusing = this->m_lstUsingOverInfo.begin();
	while(iteusing != this->m_lstUsingOverInfo.end())
	{
		delete *iteusing;
		*iteusing = NULL;
		++iteusing;
	}

	list<MYOVERINFO *>::iterator iteuded = this->m_lstUsedOverInfo.begin();
	while(iteuded != this->m_lstUsedOverInfo.end())
	{
		delete *iteuded;
		*iteuded = NULL;
		++iteuded;
	}
	LeaveCriticalSection(&this->m_cs);

	//MYOVERGAMEINFO
	EnterCriticalSection(&this->m_Gamecs);
	list<MYOVERGAMEINFO *>::iterator iteGameusing = this->m_lstUsingGameOverInfo.begin();
	while(iteGameusing != this->m_lstUsingGameOverInfo.end())
	{
		delete *iteGameusing;
		*iteGameusing = NULL;
		++iteGameusing;
	}

	list<MYOVERGAMEINFO *>::iterator iteGameuded = this->m_lstUsedGameOverInfo.begin();
	while(iteGameuded != this->m_lstUsedGameOverInfo.end())
	{
		delete *iteGameuded;
		*iteGameuded = NULL;
		++iteGameuded;
	}
	LeaveCriticalSection(&this->m_Gamecs);
}

MYOVERGAMEINFO * CMyOverlapped::NewOverGameInfoAndAddList()
{
	MYOVERGAMEINFO *pmyoverinfo = NULL;
	EnterCriticalSection(&this->m_Gamecs);
	if(this->m_lstUsedGameOverInfo.empty() == false)
	{
		pmyoverinfo = this->m_lstUsedGameOverInfo.front();
		this->m_lstUsedGameOverInfo.pop_front();
	}
	else
	{
		pmyoverinfo = new MYOVERGAMEINFO;
	}

	this->m_lstUsingGameOverInfo.push_back(pmyoverinfo);
	LeaveCriticalSection(&this->m_Gamecs);
	ZeroMemory(pmyoverinfo,sizeof(MYOVERGAMEINFO));

	return pmyoverinfo;
}

void CMyOverlapped::DelOverGameInfoFromList(MYOVERGAMEINFO * pDel)
{
	EnterCriticalSection(&this->m_Gamecs);
	list<MYOVERGAMEINFO *>::iterator ite = this->m_lstUsingGameOverInfo.begin();
	while(ite != this->m_lstUsingGameOverInfo.end())
	{
		if(*ite == pDel)
		{
			MYOVERGAMEINFO * ptemp = *ite;
			this->m_lstUsingGameOverInfo.erase(ite);
			this->m_lstUsedGameOverInfo.push_back(ptemp);
			break;
		}
		++ite;
	}
	LeaveCriticalSection(&this->m_Gamecs);
}