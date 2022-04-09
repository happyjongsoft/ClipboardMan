#pragma once

// CClipTypeObject command target

class CClipTypeObject : public CObject
{
public:
	DECLARE_SERIAL(CClipTypeObject)

	CClipTypeObject();
	virtual ~CClipTypeObject();

	void generateId();
	CString getId();
	void setId(CString id);
	CString getParentId();
	void setParentId(CString pid);
	CString getName();
	void setName(CString name);

	virtual		void Serialize(CArchive& archive);
private:
	CString m_id;
	CString m_pid;
	CString m_name;

};


