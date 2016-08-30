#include "StdAfx.h"
#include "LightControl_PlusTek_STB64C.h"
#include "LightTransmitPlustech.h"

CLightControl_PlusTek_STB64C::CLightControl_PlusTek_STB64C(int nIndex) : CLightControl(nIndex)
{
	m_nLightLevel = 0;
	m_nLightStatus = 0;
	m_pLightControl = new CLightTransmitPlustech();
}

CLightControl_PlusTek_STB64C::~CLightControl_PlusTek_STB64C(void)
{
	Disconnect();

	if (m_pLightControl)
	{
		delete m_pLightControl;
		m_pLightControl = NULL;
	}
}

BOOL CLightControl_PlusTek_STB64C::Connect(const CLightControlInfo& controlInfo)
{
	m_ControlInfo = controlInfo;

	if (NULL == m_pLightControl) return FALSE;

	if (TRUE == m_pLightControl->IsOpened()) 
	{
		return FALSE;
	}

	int nPort = _ttoi(controlInfo.GetConnectionPort());
	if(nPort < 1) return FALSE;

	int nBaud = controlInfo.GetBaudRate();
	if(FALSE == m_pLightControl->OpenControl(nPort, nBaud))
	{
		return FALSE;
	}	

	return m_pLightControl->IsTurnOnLamp(FALSE);
}

void CLightControl_PlusTek_STB64C::Disconnect()
{
	if (NULL == m_pLightControl) return;

	if (TRUE == m_pLightControl->IsOpened()) return;

	m_pLightControl->CloseControl();
}

BOOL CLightControl_PlusTek_STB64C::SetLightOn()
{
	if (NULL == m_pLightControl) return FALSE;

	if (FALSE == m_pLightControl->IsOpened()) return FALSE;

	return m_pLightControl->IsTurnOnLamp(TRUE);
}

BOOL CLightControl_PlusTek_STB64C::SetLightOff()
{
	if (NULL == m_pLightControl) return FALSE;

	if (FALSE == m_pLightControl->IsOpened()) return FALSE;

	return m_pLightControl->IsTurnOnLamp(FALSE);
}

BOOL CLightControl_PlusTek_STB64C::SetLightStatus(int nValue, int nChannel)
{
	if (NULL == m_pLightControl) return FALSE;

	if (FALSE == m_pLightControl->IsOpened()) return FALSE;

	if (nChannel<0 || nChannel>=MAX_TRANSMIT_CH) return FALSE;

	m_nLightStatus = nValue;

	return TRUE;
}

BOOL CLightControl_PlusTek_STB64C::GetLightStatus(int& nValue, int nChannel) const
{
	if (NULL == m_pLightControl) return FALSE;

	if (FALSE == m_pLightControl->IsOpened()) return FALSE;

	if (nChannel<0 || nChannel>=MAX_TRANSMIT_CH) return FALSE;

	nValue = m_nLightStatus;

	return TRUE;
}

BOOL CLightControl_PlusTek_STB64C::SetLightLevel(int nValue, int nChannel)
{
	if (NULL == m_pLightControl) return FALSE;

	if (FALSE == m_pLightControl->IsOpened()) return FALSE;

	if (nChannel<0 || nChannel>=MAX_TRANSMIT_CH) return FALSE;

	int nOnTime = STROBE_DEFAULT_ONTIME;

	DATA_SubConfig_type *pCF;
	DATA_SubControl_type *pLV;

	for(int iDrv=0; iDrv<MAX_TRANSMIT_ID; iDrv++)
	{
		for(int iCh=0; iCh<MAX_TRANSMIT_CH; iCh++)
		{

			pCF = m_pLightControl->GetTransmitLightConfig(iDrv, iCh);
			pLV = m_pLightControl->GetTransmitLightValue(iDrv, iCh);
			if(pCF == NULL || pLV == NULL)
				continue;

			pLV->volt = nValue;
			pLV->time = nOnTime;

			if(pLV->volt > TRANSMIT_MAX_VOLT_VALUE)
				pLV->volt = TRANSMIT_MAX_VOLT_VALUE;
			if(pLV->time > STROBE_DEFAULT_ONTIME)
				pLV->time = STROBE_DEFAULT_ONTIME;
		}
	}
	
	if (FALSE==m_pLightControl->SetLightControlValue())
	{
		return FALSE;
	}

	m_nLightLevel = nValue;

	return TRUE;
	
}

BOOL CLightControl_PlusTek_STB64C::SetLightLevel(double dValue, int nChannel)
{
	int nValue = int(dValue+0.5);

	return SetLightLevel(nValue, nChannel);
}

BOOL CLightControl_PlusTek_STB64C::GetLightLevel(int &nValue, int Channel) const
{
	if (NULL == m_pLightControl) return FALSE;

	if (FALSE == m_pLightControl->IsOpened()) return FALSE;

	nValue = m_nLightLevel;

	return TRUE;	
}

BOOL CLightControl_PlusTek_STB64C::GetLightLevel(double &dValue, int Channel) const
{
	int nValue = 0;
	
	if (FALSE == GetLightLevel(nValue))
	{
		return FALSE;
	}

	dValue = nValue;

	return TRUE;
}