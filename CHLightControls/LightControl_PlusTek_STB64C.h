#pragma once

#include "Serial.h"
#include "lightcontrol.h"

class CLightTransmitPlustech;

class AFX_EXT_CLASS CLightControl_PlusTek_STB64C : public CLightControl
{
public:
	CLightControl_PlusTek_STB64C(int nIndex);
	virtual ~CLightControl_PlusTek_STB64C(void);

	virtual BOOL Connect(const CLightControlInfo& controlInfo);
	virtual void Disconnect();

	virtual BOOL GetLightLevel(int &nValue, int nChannel = 0) const;
	virtual BOOL GetLightLevel(double &dValue, int nChannel = 0) const;
	virtual BOOL GetLightStatus(int &nValue, int nChannel = 0) const;

	virtual BOOL SetLightLevel(int nValue, int nChannel = 0);
	virtual BOOL SetLightLevel(double dValue, int nChannel = 0);
	virtual BOOL SetLightStatus(int nValue, int nChannel = 0);

	virtual BOOL SetLightOn();
	virtual BOOL SetLightOff();

protected:
	CLightTransmitPlustech* m_pLightControl;

	int		m_nLightLevel;
	int		m_nLightStatus;
};

