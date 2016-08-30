#pragma once
//////////////////////////////////////////////////////////
//Brightness range : 29~255
//PWM Frequency : 83Khz
//Communication
//- RS232
//- 38400bps
//- 8bit / none parity/1 stop bit/none flow control
//Channel number : 2
//////////////////////////////////////////////////////////

#include "Serial.h"
#include "LightControl.h"

class AFX_EXT_CLASS  CLightControl_PlusTek_PLK2CHR: public CLightControl, public CSerial
{
public:
	CLightControl_PlusTek_PLK2CHR(int nIndex);
	virtual ~CLightControl_PlusTek_PLK2CHR(void);

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
