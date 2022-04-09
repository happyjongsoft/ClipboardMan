// ClipObject.cpp : implementation file
//

#include "stdafx.h"
#include "ClipMan.h"
#include "ClipObject.h"

#include <ctime>

IMPLEMENT_SERIAL(CClipObject, CObject, 1)

CClipObject::CClipObject()
{
	m_time = getCurrentTime();
}

CClipObject::~CClipObject()
{
}

void CClipObject::putCustomId(CString pid)
{
	POSITION pos;
	bool isExist = false;
	for (pos = m_customIds.GetHeadPosition(); pos != NULL;)
	{
		CString s = m_customIds.GetNext(pos);
		if (s == pid)
			isExist = true;
	}

	if (!isExist)
		m_customIds.AddTail(pid);
}

bool CClipObject::isChildOfCustomCategory(CString pid)
{
	POSITION pos;

	for (pos = m_customIds.GetHeadPosition(); pos != NULL;)
	{
		CString s = m_customIds.GetNext(pos);
		if (s == pid)
			return true;
	}

	return false;
}

CString CClipObject::getTitle()
{
	return m_title;
}

CString CClipObject::getFormat()
{
	return m_format;
}

CString CClipObject::getTime()
{
	return m_time;
}

CString CClipObject::getCurrentTime()
{
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);

	CString capturedTime;
	capturedTime.Format("%d-%.2d-%.2d %.2d:%.2d:%.2d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	return capturedTime;
}

void CClipObject::Serialize(CArchive& archive){
	CObject::Serialize(archive);
};