#include "StdAfx.h"
#include "LightControl_KSVision_PDSeries.h"


CLightControl_KSVision_PDSeries::CLightControl_KSVision_PDSeries(int nIndex) : CLightControl(nIndex)
{
	m_bConnected	= FALSE;
	m_nPortIndex	= 1;
	m_nBaudRate		= 9600;
	m_nCurrentValue = 100;
	m_nCurrentStatus = 0;
}

CLightControl_KSVision_PDSeries::~CLightControl_KSVision_PDSeries(void)
{
	Disconnect();
}

BOOL CLightControl_KSVision_PDSeries::Connect(const CLightControlInfo& controlInfo)
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
	sprintf_s(Data, "CTON%c%c", ASCII_CR, ASCII_LF);
	Write(Data, strlen(Data));

	return TRUE;
}

void CLightControl_KSVision_PDSeries::Disconnect()
{
	if (!m_bConnected) return;

	m_bConnected = FALSE;

	Close();
}

BOOL CLightControl_KSVision_PDSeries::SetLightOn()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_KSVision_PDSeries::SetLightOff()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_KSVision_PDSeries::SetLightLevel(int nValue, int Channel)
{
	if (!m_bConnected) return FALSE;

	int nHigh = nValue / 16;
	int nLow = nValue % 16;

	CString hex("0123456789ABCDEF");

	char Data[50];
	memset(&Data[0],0x00,sizeof(Data));
	//sprintf_s(Data, "CT%02x%c%c", nValue, ASCII_CR, ASCII_LF);
	//sprintf_s(Data, "CT%c%c%c%c", hex.GetAt(nHigh), hex.GetAt(nLow), ASCII_CR, ASCII_LF);

	sprintf_s(Data, "C%d%c%c%c%c", (Channel+1), hex.GetAt(nHigh), hex.GetAt(nLow), ASCII_CR, ASCII_LF);

	Write(Data, strlen(Data));

	m_nCurrentValue = nValue;

	return TRUE;
}

BOOL CLightControl_KSVision_PDSeries::SetLightLevel(double dValue, int Channel)
{
	int nValue = int(dValue+0.5);

	return SetLightLevel(nValue, Channel);
}

BOOL CLightControl_KSVision_PDSeries::GetLightLevel(int &nValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_KSVision_PDSeries::GetLightLevel(double &dValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	dValue = (double)m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_KSVision_PDSeries::SetLightStatus(int nValue, int Channel)
{
	if (!m_bConnected) return FALSE;

	int nHigh = nValue / 16;
	int nLow = nValue % 16;

	CString hex("0123456789ABCDEF");

	char Data[50];
	memset(&Data[0],0x00,sizeof(Data));
	//sprintf_s(Data, "CT%02x%c%c", nValue, ASCII_CR, ASCII_LF);
	//sprintf_s(Data, "CT%c%c%c%c", hex.GetAt(nHigh), hex.GetAt(nLow), ASCII_CR, ASCII_LF);

	if (nValue==0)
		sprintf_s(Data, "H%dOF%c%c", (Channel+1), hex.GetAt(nHigh), hex.GetAt(nLow), ASCII_CR, ASCII_LF);
	else
		sprintf_s(Data, "H%dON%c%c", (Channel+1), hex.GetAt(nHigh), hex.GetAt(nLow), ASCII_CR, ASCII_LF);

	Write(Data, strlen(Data));

	m_nCurrentValue = nValue;

	return TRUE;
}

BOOL CLightControl_KSVision_PDSeries::GetLightStatus(int &nValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentStatus;

	return TRUE;
}