#include "StdAfx.h"
#include "LightControl_PlusTek_PLK2CHR.h"

CLightControl_PlusTek_PLK2CHR::CLightControl_PlusTek_PLK2CHR(int nIndex) : CLightControl(nIndex)
{
	m_nPortIndex	= 1;
	m_nBaudRate		= 38400;
	m_bConnected	= FALSE;
}

CLightControl_PlusTek_PLK2CHR::~CLightControl_PlusTek_PLK2CHR(void)
{
	Disconnect();
}

BOOL CLightControl_PlusTek_PLK2CHR::Connect(const CLightControlInfo& controlInfo)
{
	m_ControlInfo = controlInfo;

	int nPort = _ttoi(controlInfo.GetConnectionPort());
	if(nPort < 1) return FALSE;

	if(IsOpen()) Close();
	CString strProt;

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
	Setup(CSerial::EBaud38400,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	SetupHandshaking(CSerial::EHandshakeOff);

	m_nPortIndex = nPort;
	m_nBaudRate = controlInfo.GetBaudRate();
	m_bConnected = TRUE;

	//Turn On
	char Data[50];
	memset(&Data[0],0x00,sizeof(Data));
	sprintf_s(Data, "setonex ffffffff%c", 0x0D);
	Write(Data, strlen(Data));

	return TRUE;
}

void CLightControl_PlusTek_PLK2CHR::Disconnect()
{
	if (!m_bConnected) return;

	m_bConnected = FALSE;

	Close();
}

BOOL CLightControl_PlusTek_PLK2CHR::SetLightOn()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_PlusTek_PLK2CHR::SetLightOff()
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_PlusTek_PLK2CHR::GetLightLevel(int &nValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_PlusTek_PLK2CHR::GetLightLevel(double &dValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	dValue = (double)m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_PlusTek_PLK2CHR::SetLightLevel(int nValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	char Data[50];
	memset(&Data[0],0x00,sizeof(Data));
	sprintf_s(Data, "setbrightness %d %d%c", nChannel, nValue, 0x0D);
	Write(Data, strlen(Data));

	return TRUE;
}

BOOL CLightControl_PlusTek_PLK2CHR::SetLightLevel(double dValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	return TRUE;
}

BOOL CLightControl_PlusTek_PLK2CHR::GetLightStatus(int &nValue, int Channel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentStatus;

	return TRUE;
}

BOOL CLightControl_PlusTek_PLK2CHR::SetLightStatus(int nValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	

	return TRUE;
}