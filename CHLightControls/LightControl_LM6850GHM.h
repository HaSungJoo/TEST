#pragma once

#include "Serial.h"
#include "LightControl.h"

//HEADER
#define HEADER_DLE				0x10
#define HEADER_STX				0x02

//IDENTIFIER
#define IDENTIFIER_OPCODE		0x03
#define IDENTIFIER_LENGTH		0x02

//DATA
#define DATA_ACK				0x06
#define DATA_NACK				0x15

//DATA STATUS
#define DATA_STATUS_NORMAL		0x00
#define DATA_STATUS_81H			0x81
#define DATA_STATUS_82H			0x82
#define DATA_STATUS_83H			0x83
#define DATA_STATUS_86H			0x86
#define DATA_STATUS_FFH			0xFF

//CONTROL
#define CONTROL_DIMMING			0x03
#define CONTROL_POWER			0x04
#define CONTROL_RESET			0x05

//TAIL
#define TAIL_DLE				0x10
#define TAIL_ETX				0x03

//CONTROL_DATA
#define CONTROL_DATA_ON			0x01
#define CONTROL_DATA_OFF		0x00
#define CONTROL_DATA_RESET		0x01

//PACKET LENGTH
#define PACKET_DATA_LENGTH		10

//CURRENT_STATUS
#define  CURRENT_STATUS_ON		1;
#define  CURRENT_STATUS_OFF		0;

#define CURR 0x00;
#define DEF	0x00;
#define lig	0x01;

class AFX_EXT_CLASS CLightControl_LM6850GHM : public CLightControl, 
											  public CSerial
{
public:
	CLightControl_LM6850GHM(int nIndex);
	virtual ~CLightControl_LM6850GHM(void);

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

	unsigned int CRC16(unsigned char* rdata, unsigned int len);
	static UINT  ReceiveThread(LPVOID pParam);
	BOOL CheckCRC(unsigned char* uOp1, unsigned char* uOp2);
	BOOL FactoryReset();

protected:
	int		m_nCurrentValue;
	int		m_nCurrentStatus;
};

