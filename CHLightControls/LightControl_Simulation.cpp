#include "StdAfx.h"
#include "LightControl_Simulation.h"


CLightControl_Simulation::CLightControl_Simulation(int nIndex) : CLightControl(nIndex)
{
}

CLightControl_Simulation::~CLightControl_Simulation(void)
{
}

BOOL CLightControl_Simulation::Connect(const CLightControlInfo& controlInfo)
{
	m_ControlInfo = controlInfo;
	return TRUE;
}

void CLightControl_Simulation::Disconnect()
{

}

BOOL CLightControl_Simulation::GetLightLevel(int &nValue, int nChannel) const
{
	return TRUE;
}

BOOL CLightControl_Simulation::GetLightLevel(double &dValue, int nChannel) const
{
	return TRUE;
}

BOOL CLightControl_Simulation::GetLightStatus(int &nValue, int nChannel) const
{
	return TRUE;
}

BOOL CLightControl_Simulation::SetLightLevel(int nValue, int nChannel)
{
	return TRUE;
}

BOOL CLightControl_Simulation::SetLightLevel(double dValue, int nChannel)
{
	return TRUE;
}

BOOL CLightControl_Simulation::SetLightStatus(int nValue, int nChannel)
{
	return TRUE;
}

BOOL CLightControl_Simulation::SetLightOn()
{
	return TRUE;
}

BOOL CLightControl_Simulation::SetLightOff()
{
	return TRUE;
}