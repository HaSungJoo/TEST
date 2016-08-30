#include "StdAfx.h"
#include "LightControl_PSD1010.h"

CLightControl_PSD1010::CLightControl_PSD1010(int nIndex) : CLightControl(nIndex)
{
	m_nPortIndex	= 1;
	m_nBaudRate		= 9600;
	m_bConnected	= FALSE;

	m_nCurrentValue = 0;
	m_nCurrentStatus = 0;
}

CLightControl_PSD1010::~CLightControl_PSD1010(void)
{
	Disconnect();
}


BOOL CLightControl_PSD1010::Connect(const CLightControlInfo& controlInfo)
{
	m_ControlInfo = controlInfo;

	int nPort = _ttoi(controlInfo.GetConnectionPort());
	if(nPort < 1) return FALSE;

	CString strProt;
	strProt.Format(_T("COM%d"), nPort);

	if(Open(strProt, controlInfo.GetBaudRate()) != ERROR_SUCCESS)
	{
		m_bConnected = FALSE;
		return FALSE;
	}

	m_bConnected = TRUE;
	m_nPortIndex = nPort;
	m_nBaudRate = controlInfo.GetBaudRate();

	return TRUE;
}

void CLightControl_PSD1010::Disconnect()
{
	if (!m_bConnected) return;

	m_bConnected = FALSE;

	Close();
}

BOOL CLightControl_PSD1010::SetLightOn()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_PSD1010::SetLightOff()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_PSD1010::GetLightStatus(int &nValue, int nChannel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentStatus;

	return TRUE;
}

BOOL CLightControl_PSD1010::GetLightLevel(int &nValue, int nChannel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_PSD1010::GetLightLevel(double &dValue, int nChannel) const
{
	if (!m_bConnected) return FALSE;

	dValue = (double)m_nCurrentValue;

	return TRUE;
}


BOOL CLightControl_PSD1010::SetLightLevel(int nValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	char Data[10];

	sprintf_s(Data, "w%03d%c", nValue, ASCII_CR);

	if(Write(Data, strlen(Data)) != ERROR_SUCCESS) return FALSE;

	m_nCurrentValue = atoi(Data);

	return TRUE;
// 	if(SendData(Data, 5))
// 	{
// 		m_nCurrentValue = atoi(Data);
// 		return TRUE;
// 	}
// 	return FALSE;
}

BOOL CLightControl_PSD1010::SetLightLevel(double dValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	char Data[10];

	sprintf_s(Data, "w%03d%c", (int)dValue, ASCII_CR);

	if(Write(Data, strlen(Data)) != ERROR_SUCCESS) return FALSE;

	m_nCurrentValue = atoi(Data);

	return TRUE;
// 	if(SendData(Data, 5))
// 	{
// 		m_nCurrentValue = atoi(Data);
// 		return TRUE;
// 	}
// 	return FALSE;
}

BOOL CLightControl_PSD1010::SetLightStatus(int nValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	m_nCurrentStatus = nValue;

	return TRUE;
}
