// ClipTypeObject.cpp : implementation file
//

#include "stdafx.h"
#include "ClipMan.h"
#include "ClipTypeObject.h"

IMPLEMENT_SERIAL(CClipTypeObject, CObject, 1)
// CClipTypeObject

CClipTypeObject::CClipTypeObject()
{
	m_id = "";
	m_pid = "";
	m_name = "";
	generateId();
}

CClipTypeObject::~CClipTypeObject()
{
}

// CClipTypeObject member functions
void CClipTypeObject::generateId()
{
	int len = 20;
	// Set seed based on time
	srand(time(NULL));

	char* s = new char[len + 1];
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
	}

	s[len] = 0;

	m_id = CString(s);

	delete s;
}

CString CClipTypeObject::getId()
{
	return m_id;
}

void CClipTypeObject::setId(CString id)
{
	m_id = id;
}

CString CClipTypeObject::getParentId()
{
	return m_pid;
}

void CClipTypeObject::setParentId(CString pid)
{
	m_pid = pid;
}

void CClipTypeObject::Serialize(CArchive& archive)
{
	CObject::Serialize(archive);

	if (archive.IsStoring())
		archive << m_id << m_pid << m_name;			// Save item
	else
		archive >> m_id >> m_pid >> m_name;			// Open item

}

CString CClipTypeObject::getName()
{
	return m_name;
}

void CClipTypeObject::setName(CString name)
{
	m_name = name;
}