#pragma once
#include "Serial.h"
#include "LightControl.h"

class AFX_EXT_CLASS CLightControl_PSD1010 :	public CLightControl, public CSerial
{
public:
	CLightControl_PSD1010(int nIndex);
	virtual ~CLightControl_PSD1010(void);

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
	int		m_nCurrentValue;
	int		m_nCurrentStatus;
};
