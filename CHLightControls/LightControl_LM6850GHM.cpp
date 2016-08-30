#include "StdAfx.h"
#include "LightControl_LM6850GHM.h"


CLightControl_LM6850GHM::CLightControl_LM6850GHM(int nIndex) : CLightControl(nIndex)
{
	m_nPortIndex	= 1;
	m_nBaudRate		= 9600;
	m_bConnected	= FALSE;
	m_nCurrentStatus = 0;
	m_nCurrentValue = 0;
}


CLightControl_LM6850GHM::~CLightControl_LM6850GHM(void)
{

}

BOOL CLightControl_LM6850GHM::Connect(const CLightControlInfo& controlInfo)
{

	m_ControlInfo = controlInfo;
	int nPort = _ttoi(controlInfo.GetConnectionPort());
	if(nPort < 1) return FALSE;

	CString strProt;
	strProt.Format(_T("COM%d"), nPort);

	if(Open(strProt, NULL) != ERROR_SUCCESS)
	{
		m_bConnected = FALSE;
		return FALSE;
	}
	Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	SetupHandshaking(CSerial::EHandshakeOff);
	SetupReadTimeouts(CSerial::EReadTimeoutBlocking);

	m_bConnected = TRUE;
	m_nPortIndex = nPort;

	return TRUE;
}

void CLightControl_LM6850GHM::Disconnect()
{
	m_bConnected = FALSE;
	Close();
	return;
}

BOOL CLightControl_LM6850GHM::GetLightLevel(int &nValue, int nChannel /* = 0 */) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_LM6850GHM::GetLightLevel(double &dValue, int nChannel /* = 0 */) const
{
	if (!m_bConnected) return FALSE;

	dValue = (double)m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_LM6850GHM::GetLightStatus(int &nValue, int nChannel /* = 0 */) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentStatus;

	return TRUE;
}

BOOL CLightControl_LM6850GHM::SetLightLevel(int nValue, int nChannel /* = 0 */) 
{
	if (!m_bConnected) return FALSE;
	unsigned char uOp1 = CONTROL_DIMMING;
	unsigned char uOp2 = (unsigned char)nValue;
	if(!CheckCRC(&uOp1, &uOp2))
	{
		return FALSE;
	}
	m_nCurrentValue = nValue;
	return TRUE;
}

BOOL CLightControl_LM6850GHM::SetLightLevel(double dValue, int nChannel /* = 0 */) 
{
	if (!m_bConnected) return FALSE;
	unsigned char uOp1 = CONTROL_DIMMING;
	unsigned char uOp2 = (unsigned char)dValue;
	if(!CheckCRC(&uOp1, &uOp2))
	{
		return FALSE;
	}

	m_nCurrentValue = (int)dValue;

	return TRUE;
}

BOOL CLightControl_LM6850GHM::SetLightStatus(int nValue, int nChannel /* = 0 */) 
{
	//NOT USE

	return 1;
}

BOOL CLightControl_LM6850GHM::SetLightOn()
{
	if (!m_bConnected) return FALSE;

	unsigned char uOp1 = CONTROL_POWER;
	unsigned char uOp2 = CONTROL_DATA_ON;
	if(!CheckCRC(&uOp1, &uOp2))
	{
		return FALSE;
	}

	m_nCurrentStatus = CURRENT_STATUS_ON;

	return TRUE;
}

BOOL CLightControl_LM6850GHM::SetLightOff()
{
	if (!m_bConnected) return FALSE;

	unsigned char uOp1 = CONTROL_POWER;
	unsigned char uOp2 = CONTROL_DATA_OFF;
	if(!CheckCRC(&uOp1, &uOp2))
	{
		return FALSE;
	}

	m_nCurrentStatus = CURRENT_STATUS_OFF;

	return TRUE;
}

BOOL CLightControl_LM6850GHM::FactoryReset()
{
	if (!m_bConnected) return FALSE;

	unsigned char uOp1 = CONTROL_RESET;
	unsigned char uOp2 = CONTROL_DATA_RESET;
	if(!CheckCRC(&uOp1, &uOp2))
	{
		return FALSE;
	}

	return TRUE;
}

unsigned int CLightControl_LM6850GHM::CRC16(unsigned char* rdata, unsigned int len)
{
	int i,n;
	unsigned int wCrc = 0xFFFF;
	unsigned int wCh;

	for(i=0;i<len;i++)
	{
		wCh = (unsigned int)*(rdata+i);
		for(n=0;n<8;n++)
		{
			if((wCh^wCrc) & 0x0001) 
				wCrc = (wCrc >> 1) ^ 0xA001;
			else
				wCrc >>= 1;
			wCh >>= 1;
		}
	}
	return wCrc;
}

BOOL CLightControl_LM6850GHM::CheckCRC(unsigned char* uOp1, unsigned char* uOp2)
{
	unsigned int uRes = 0;
	char Data[50];
	memset(&Data[0],0x00,sizeof(Data));

	// make CRC Code
	unsigned char uInputCode[4];
	unsigned char uOutputCode[2];
	unsigned int  uReturnCode;

	memset(&uInputCode[0],0x00,sizeof(uInputCode));
	memset(&uOutputCode[0],0x00,sizeof(uOutputCode));

	//CHECK CRC
	uInputCode[0] = IDENTIFIER_OPCODE;
	uInputCode[1] = IDENTIFIER_LENGTH;
	uInputCode[2] = *uOp1;
	uInputCode[3] = *uOp2;
	uReturnCode = CRC16(uInputCode, sizeof(uInputCode));

	uOutputCode[0] = uReturnCode & 0xFF;
	uOutputCode[1] = uReturnCode >> 8 & 0xFF;

	// Send
	sprintf_s(Data, "%c%c%c%c%c%c%c%c%c%c", 
		HEADER_DLE, HEADER_STX, IDENTIFIER_OPCODE, IDENTIFIER_LENGTH, uInputCode[2], uInputCode[3], TAIL_DLE, TAIL_ETX, uOutputCode[1], uOutputCode[0]);
	Write(Data, PACKET_DATA_LENGTH);

	CWinThread* pThread = NULL;
	pThread = AfxBeginThread(ReceiveThread,this);
	if(pThread == NULL)
	{
		return FALSE;
	}	

	return TRUE;
}

UINT CLightControl_LM6850GHM::ReceiveThread(LPVOID pParam)
{

	CLightControl_LM6850GHM* pCtrl = (CLightControl_LM6850GHM*) pParam;

	// Read
	DWORD dwBytesRead = 0;
	BYTE  abBuffer[10];
	//Sleep(100);
	do 
	{
		if(pCtrl->Read(abBuffer,sizeof(abBuffer),&dwBytesRead))
		{
			continue;
		}		
		if(dwBytesRead == sizeof(abBuffer))
		{
			if(pCtrl->m_pLC2P)
			{
				pCtrl->m_pLC2P->ILC2P_ResponseSignal(abBuffer, dwBytesRead);
			}
		}
	} while (dwBytesRead != sizeof(abBuffer));	

	return 0;
}