#include "StdAfx.h"
#include "LightControl_ShinHan_FLS2CHR.h"

CLightControl_ShinHan_FLS2CHR::CLightControl_ShinHan_FLS2CHR(int nIndex) : CLightControl(nIndex)
{
	m_bConnected	= FALSE;
	m_nPortIndex	= 1;
	m_nBaudRate		= 38400;
	m_nCurrentValue = 1;
}

CLightControl_ShinHan_FLS2CHR::~CLightControl_ShinHan_FLS2CHR(void)
{
	Disconnect();
}

BOOL CLightControl_ShinHan_FLS2CHR::Connect(const CLightControlInfo& controlInfo)
{
	m_ControlInfo = controlInfo;

	int nPort = _ttoi(controlInfo.GetConnectionPort());
	if(nPort < 1) return FALSE;

	if(IsOpen()) Close();
	CString strProt = _T("");

	if(nPort > 9)
	{
		strProt.Format(_T("\\\\.\\\\COM%d"), nPort);
	}
	else
	{
		strProt.Format(_T("COM%d"), nPort);
	}

	if (Open(strProt,NULL) != ERROR_SUCCESS)
	{
		m_bConnected = FALSE;
		return false;
	}
	Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	SetupHandshaking(CSerial::EHandshakeOff);

	m_nPortIndex = nPort;
	m_nBaudRate = controlInfo.GetBaudRate();
	m_bConnected = TRUE;

	//Turn On
	char Data[50];
	memset(&Data[0],0x00,sizeof(Data));
	sprintf_s(Data, "C%c", 0x0D);
	Write(Data, strlen(Data));

	return TRUE;
}

void CLightControl_ShinHan_FLS2CHR::Disconnect()
{
	if (!m_bConnected) return;

	m_bConnected = FALSE;

	Close();
}


BOOL CLightControl_ShinHan_FLS2CHR::SetLightStatus(int nValue, int Channel)
{
	if (!m_bConnected) return FALSE;

	m_nCurrentStatus = nValue;

	return TRUE;
}

BOOL CLightControl_ShinHan_FLS2CHR::GetLightStatus(int &nValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentStatus;

	return TRUE;
}


BOOL CLightControl_ShinHan_FLS2CHR::GetLightLevel(int &nValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_ShinHan_FLS2CHR::GetLightLevel(double &dValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	dValue = (double)m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_ShinHan_FLS2CHR::SetLightLevel(int nValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	char Data[50];
	memset(&Data[0],0x00,sizeof(Data));
	sprintf_s(Data, "A%03d%c", nValue, 0x0D);
	Write(Data, strlen(Data));

	m_nCurrentValue = nValue;

	return TRUE;
}

BOOL CLightControl_ShinHan_FLS2CHR::SetLightLevel(double dValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_ShinHan_FLS2CHR::SetLightOn()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_ShinHan_FLS2CHR::SetLightOff()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}