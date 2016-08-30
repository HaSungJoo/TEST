#pragma once

#include "MacroFile.h"

interface ILightControl2Parent
{
	virtual void ILC2P_DisplayMessage(int nIndex, const TCHAR* lpstrFormat, ...) = 0;
	virtual void ILC2P_ChangeLightLevel(int nIndex, int nLevel) = 0;
	virtual void ILC2P_ResponseSignal(BYTE* nBuf, int nLength) = 0;
};

class CLightControlInfo
{
public:
	CLightControlInfo(int nIndex=0) : m_nIndex(nIndex)		{ Reset(); }
	~CLightControlInfo(void)								{ Reset(); }
	void Reset()
	{
		m_strName			= _T("");
		m_nControllerType	= 0;
		m_strConnectionPort	= _T("");
		m_nBaudRate			= CBR_9600;
		m_nDefaultValue		= 0;
		m_nCurrentValue		= 0;
		m_nMaxValue			= 255;
		m_nMinValue			= 0;
	}

	BOOL LoadInfo(CMacroFile* pFile, const CString& strItemName)
	{
		if (pFile==NULL) return FALSE;

		CString strValue = _T("");

		strValue = strItemName + _T("_INDEX");
		pFile->GetItem(strValue, m_nIndex, 0);

		strValue = strItemName + _T("_NAME");
		pFile->GetItem(strValue, m_strName, 0);

		strValue = strItemName + _T("_CONTROL_TYPE");
		pFile->GetItem(strValue, m_nControllerType, 0);

		strValue = strItemName + _T("_CONNECT_PORT");
		pFile->GetItem(strValue, m_strConnectionPort, _T(""));

		strValue = strItemName + _T("_BAUD_RATE");
		pFile->GetItem(strValue, m_nBaudRate, CBR_9600);

		strValue = strItemName + _T("_DEFAULT_VALUE");
		pFile->GetItem(strValue, m_nDefaultValue, 0);

		strValue = strItemName + _T("_CURRENT_VALUE");
		pFile->GetItem(strValue, m_nCurrentValue, 0);

		strValue = strItemName + _T("_MAX_VALUE");
		pFile->GetItem(strValue, m_nMaxValue, 1);

		strValue = strItemName + _T("_MIN_VALUE");
		pFile->GetItem(strValue, m_nMinValue, 0);

		return TRUE;
	}

	BOOL SaveInfo(CMacroFile* pFile, const CString& strItemName)
	{
		if (pFile==NULL) return FALSE;

		CString strValue = _T("");

		strValue = strItemName + _T("_INDEX");
		pFile->SetItem(strValue, m_nIndex);

		strValue = strItemName + _T("_NAME");
		pFile->SetItem(strValue, m_strName);

		strValue = strItemName + _T("_CONTROL_TYPE");
		pFile->SetItem(strValue, m_nControllerType);

		strValue = strItemName + _T("_CONNECT_PORT");
		pFile->SetItem(strValue, m_strConnectionPort);

		strValue = strItemName + _T("_BAUD_RATE");
		pFile->SetItem(strValue, m_nBaudRate);

		strValue = strItemName + _T("_DEFAULT_VALUE");
		pFile->SetItem(strValue, m_nDefaultValue);

		strValue = strItemName + _T("_CURRENT_VALUE");
		pFile->SetItem(strValue, m_nCurrentValue);

		strValue = strItemName + _T("_MAX_VALUE");
		pFile->SetItem(strValue, m_nMaxValue);

		strValue = strItemName + _T("_MIN_VALUE");
		pFile->SetItem(strValue, m_nMinValue);

		return TRUE;
	}

	int			GetIndex() const							{ return m_nIndex; }
	CString		GetName() const								{ return m_strName; }
	int			GetControllerType() const					{ return m_nControllerType; }
	CString		GetConnectionPort() const					{ return m_strConnectionPort; }
	int			GetBaudRate() const							{ return m_nBaudRate; }
	int			GetDefaultValue() const						{ return m_nDefaultValue; }
	int			GetCurrentValue() const						{ return m_nCurrentValue; }
	int			GetMaxValue() const							{ return m_nMaxValue; }
	int			GetMinValue() const							{ return m_nMinValue; }

	void		SetIndex(int nIndex)						{ m_nIndex = nIndex; }
	void		SetName(const CString& strName)				{ m_strName = strName; }
	void		SetControllerType(int nType)				{ m_nControllerType = nType; }
	void		SetConnectionPort(const CString& strPort) 	{ m_strConnectionPort = strPort; }
	void		SetBaudRate(int nValue)						{ m_nBaudRate = nValue; }
	void		SetDefaultValue(int nValue)					{ m_nDefaultValue = nValue; }
	void		SetCurrentValue(int nValue) 				{ m_nCurrentValue = nValue; }
	void		SetMaxValue(int nValue)						{ m_nMaxValue = nValue; }
	void		SetMinValue(int nValue)						{ m_nMinValue = nValue; }

protected:
	int			m_nIndex;					// 인덱스
	CString		m_strName;					// 이름
	int			m_nControllerType;			// 컨트롤러 타입
	CString		m_strConnectionPort;		// 연결 포트
	int			m_nBaudRate;				// 연결 속도

	int			m_nDefaultValue;			// 기본 값
	int			m_nCurrentValue;			// 현재 값
	int			m_nMaxValue;				// 최대 값
	int			m_nMinValue;				// 최소 값
};

