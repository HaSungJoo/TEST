#pragma once

#include "LightControlInfo.h"

class AFX_EXT_CLASS CLightControl
{
public:
	CLightControl(int nIndex)
	{
		m_nIndex		= nIndex;
		m_pLC2P			= NULL;
		m_nPortIndex	= 1;
		m_nBaudRate		= CBR_9600;
		m_bConnected	= FALSE;
	}

	virtual ~CLightControl(void)
	{

	}

	// setter
	void	SetLC2P(ILightControl2Parent* pLC2P)			{ m_pLC2P = pLC2P; }
	void	SetDefaultValue(int nValue)	{ m_nDefaultValue = nValue; }

	// getter
	int		GetControlType() const		{ return m_ControlInfo.GetControllerType(); }
	int		GetPortIndex() const		{ return m_nPortIndex; }
	int		GetBaudRate() const			{ return m_nBaudRate; }
	BOOL	GetConnected() const		{ return m_bConnected; }
	int		GetDefaultValue() const		{ return m_nDefaultValue; }

	virtual BOOL Connect(const CLightControlInfo& controlInfo) = 0;
	virtual void Disconnect() = 0;

	virtual BOOL GetLightLevel(int &nValue, int nChannel = 0) const = 0;
	virtual BOOL GetLightLevel(double &dValue, int nChannel = 0) const = 0;
	virtual BOOL GetLightStatus(int &nValue, int nChannel = 0) const = 0;

	virtual BOOL SetLightLevel(int nValue, int nChannel = 0) = 0;
	virtual BOOL SetLightLevel(double dValue, int nChannel = 0) = 0;
	virtual BOOL SetLightStatus(int nValue, int nChannel = 0) = 0;

	virtual BOOL SetLightOn() = 0;
	virtual BOOL SetLightOff() = 0;
	
protected:
	ILightControl2Parent*	m_pLC2P;
	CLightControlInfo		m_ControlInfo;

	int						m_nIndex;
	int						m_nPortIndex;
	int						m_nBaudRate;
	BOOL					m_bConnected;
	int						m_nDefaultValue;
};