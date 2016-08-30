#pragma once

#include "LightControl.h"

class AFX_EXT_CLASS CLightControl_SP200 : public CLightControl
{
public:
	CLightControl_SP200(int nIndex);
	virtual ~CLightControl_SP200(void);

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

	void SetMappingValue(int nMin, int nMax, int nMinMap, int nMaxMap);

protected:
	double	m_dLightResolution;
	int		m_nCurrentValue;

	// for sp200
private:

	// port initialisation											
	BOOL		InitPort(CWnd* pPortOwner, UINT portnr = 1, UINT baud = 19200, char parity = 'N', UINT databits = 8, UINT stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR | EV_CTS, UINT nBufferSize = 512);

	// start/stop comm watching
	BOOL		StartMonitoring();
	BOOL		RestartMonitoring();
	BOOL		StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB			GetDCB();

	void		WriteToPort(unsigned char* Data, UINT size);
	void		ClearReadBuff(void);

	// protected memberfunctions
	void		ProcessErrorMessage(CString ErrorText);
	static UINT	CommThread(LPVOID pParam);
	static void	ReceiveChar(CLightControl_SP200* port, COMSTAT comstat);
	static void	WriteChar(CLightControl_SP200* port);

	BOOL			m_ReadComplete;
	unsigned char	m_ReadBuff[256];
	int				m_ReadCount;
	BOOL			m_bThreadRun;

	// thread
	CWinThread*			m_Thread;

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;
	BOOL				m_bThreadAlive;

	// handles
	HANDLE				m_hShutdownEvent;
	HANDLE				m_hComm;
	HANDLE				m_hWriteEvent;

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;

	// misc
	UINT				m_nPortNr;
	unsigned char*		m_szWriteBuffer;
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;
	UINT				m_WriteDataSize;

};
