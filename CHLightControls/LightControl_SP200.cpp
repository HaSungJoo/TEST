#include "StdAfx.h"
#include "LightControl_SP200.h"
#include "CommThread.h"

#define MIN_LIGHT_VALUE				0
#define MAX_LIGHT_VALUE				4095

#define MIN_MAPPING_LIGHT_VALUE		0
#define MAX_MAPPING_LIGHT_VALUE		255

CLightControl_SP200::CLightControl_SP200(int nIndex) : CLightControl(nIndex)
{
	m_nPortIndex	= 1;
	m_nBaudRate		= 9600;
	m_bConnected	= FALSE;

	m_dLightResolution	= 1.0;
	m_dLightResolution	= double(MAX_LIGHT_VALUE-MIN_LIGHT_VALUE) / double(MAX_MAPPING_LIGHT_VALUE-MIN_MAPPING_LIGHT_VALUE);
	m_nCurrentValue		= MIN_MAPPING_LIGHT_VALUE;

	// for sp200
	m_hComm = NULL;

	// initialize overlapped structure members to zero
	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;

	// create events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_szWriteBuffer = NULL;

	m_bThreadAlive = FALSE;

	m_ReadComplete = FALSE;
	m_ReadCount = 0;
}

CLightControl_SP200::~CLightControl_SP200(void)
{
	Disconnect();
}

void CLightControl_SP200::SetMappingValue(int nMin, int nMax, int nMinMap, int nMaxMap)
{
	m_dLightResolution	= 1.0;
	m_dLightResolution	= double(nMax-nMin) / double(nMaxMap-nMinMap);
	m_nCurrentValue		= nMinMap;
}

BOOL CLightControl_SP200::Connect(const CLightControlInfo& controlInfo)
{
	m_ControlInfo = controlInfo;

	int nPort = _ttoi(controlInfo.GetConnectionPort());
	if(nPort < 1) return FALSE;

	m_bConnected = InitPort(NULL, nPort, controlInfo.GetBaudRate(), 'N', 8, 1);

	if (!m_bConnected) return FALSE;

	m_nPortIndex = nPort;
	m_nBaudRate = controlInfo.GetBaudRate();

	StartMonitoring();

	unsigned char data[] = {0x55, 0x55, 0x55, 0x55, 0x5A, 0x5C, 0x49};

	WriteToPort(data, 7);

	return TRUE;
}

void CLightControl_SP200::Disconnect()
{
	if (!m_bConnected) return;

	StopMonitoring();

	m_bConnected = FALSE;
	
	delete [] m_szWriteBuffer;
}

BOOL CLightControl_SP200::GetLightLevel(int &nValue, int nChannel) const
{
	if (!m_bConnected) return FALSE;

	nValue = m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_SP200::GetLightLevel(double &dValue, int nChannel) const
{
	if (!m_bConnected) return FALSE;

	dValue = m_nCurrentValue;

	return TRUE;
}

BOOL CLightControl_SP200::GetLightStatus(int &nValue, int nChannel) const
{
	return TRUE;
}

BOOL CLightControl_SP200::SetLightLevel(int nValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	int	mappingValue = int( double(nValue) * m_dLightResolution );

	unsigned char Data[20];

	Data[0] = 0x2B;
	Data[1] = 0xA2;
	Data[2] = (unsigned char)(mappingValue >> 8);
	Data[3] = (unsigned char)mappingValue;

	WriteToPort(Data, 4);

	return TRUE;
}

BOOL CLightControl_SP200::SetLightLevel(double dValue, int nChannel)
{
	if (!m_bConnected) return FALSE;

	int	mappingValue = int( dValue * m_dLightResolution );

	unsigned char Data[20];

	Data[0] = 0x2B;
	Data[1] = 0xA2;
	Data[2] = (unsigned char)(mappingValue >> 8);
	Data[3] = (unsigned char)mappingValue;

	WriteToPort(Data, 4);

	return TRUE;
}

BOOL CLightControl_SP200::SetLightStatus(int nValue, int nChannel)
{
	return TRUE;
}

BOOL CLightControl_SP200::SetLightOn()
{
	return FALSE;
}

BOOL CLightControl_SP200::SetLightOff()
{
	return FALSE;
}

// Initialize the port. This can be port 1 to 4.
//
BOOL CLightControl_SP200::InitPort(CWnd* pPortOwner,	// the owner (CWnd) of the port (receives message)
						   UINT  portnr,		// portnumber (1..4)
						   UINT  baud,			// baudrate
						   char  parity,		// parity 
						   UINT  databits,		// databits 
						   UINT  stopbits,		// stopbits 
						   DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
						   UINT  writebuffersize)	// size to the writebuffer
{
	ASSERT(portnr > 0 && portnr < 4);
	//	assert(pPortOwner != NULL);

	// if the thread is alive: Kill
	if (m_bThreadAlive)
	{
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive);
		TRACE("Thread ended\n");
	}

	// create events
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// initialize critical section
	InitializeCriticalSection(&m_csCommunicationSync);

	if (m_szWriteBuffer != NULL)
		delete [] m_szWriteBuffer;
	m_szWriteBuffer = new unsigned char[writebuffersize];

	m_nPortNr = portnr;

	m_nWriteBufferSize = writebuffersize;
	m_dwCommEvents = dwCommEvents;

	BOOL bResult = FALSE;
//	char *szPort = new char[50];
//	char *szBaud = new char[50];

	CString szPort = _T("");
	CString szBaud = _T("");

	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: close it
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}

	// prepare port strings
//	sprintf(szPort, "COM%d", portnr);
//	sprintf(szBaud, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopbits);
	
	szPort.Format(_T("COM%d"), portnr);
	szBaud.Format(_T("baud=%d parity=%c data=%d stop=%d"), baud, parity, databits, stopbits);

	// get a handle to the port
	m_hComm = CreateFile(szPort,						// communication port string (COMX)
		GENERIC_READ | GENERIC_WRITE,	// read/write types
		0,								// comm devices must be opened with exclusive access
		NULL,							// no security attributes
		OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED,			// Async I/O
		0);							// template must be 0 for comm devices

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// port not found
//		delete [] szPort;
//		delete [] szBaud;

		return FALSE;
	}

	// set the timeout values
	m_CommTimeouts.ReadIntervalTimeout = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

	// configure
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{						   
		if (SetCommMask(m_hComm, dwCommEvents))
		{
			if (GetCommState(m_hComm, &m_dcb))
			{
				m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
				if (BuildCommDCB(szBaud, &m_dcb))
				{
					if (SetCommState(m_hComm, &m_dcb))
						; // normal operation... continue
					else
						ProcessErrorMessage(_T("SetCommState()"));
				}
				else
					ProcessErrorMessage(_T("BuildCommDCB()"));
			}
			else
				ProcessErrorMessage(_T("GetCommState()"));
		}
		else
			ProcessErrorMessage(_T("SetCommMask()"));
	}
	else
		ProcessErrorMessage(_T("SetCommTimeouts()"));

//	delete [] szPort;
//	delete [] szBaud;

	// flush the port
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// release critical section
	LeaveCriticalSection(&m_csCommunicationSync);

	TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);

	return TRUE;
}

//
//  The CommThread Function.
//
UINT CLightControl_SP200::CommThread(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPort class
	CLightControl_SP200 *port = (CLightControl_SP200*)pParam;

	// Set the status variable in the dialog class to
	// TRUE to indicate the thread is running.
	port->m_bThreadAlive = TRUE;	

	// Misc. variables
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;
	BOOL  bResult = TRUE;

	// Clear comm buffers at startup
	if (port->m_hComm)		// check if the port is opened
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// begin forever loop.  This loop will run as long as the thread is alive.
	while (port->m_bThreadRun == TRUE) 
	{ 

		// Make a call to WaitCommEvent().  This call will return immediatly
		// because our port was created as an async port (FILE_FLAG_OVERLAPPED
		// and an m_OverlappedStructerlapped structure specified).  This call will cause the 
		// m_OverlappedStructerlapped element m_OverlappedStruct.hEvent, which is part of the m_hEventArray to 
		// be placed in a non-signeled state if there are no bytes available to be read,
		// or to a signeled state if there are bytes available.  If this event handle 
		// is set to the non-signeled state, it will be set to signeled when a 
		// character arrives at the port.

		// we do this for each port!

		bResult = WaitCommEvent(port->m_hComm, &Event, &port->m_ov);

		if (!bResult)  
		{ 
			// If WaitCommEvent() returns FALSE, process the last error to determin
			// the reason..
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// This is a normal return value if there are no bytes
					// to read at the port.
					// Do nothing and continue
					break;
				}
			case 87:
				{
					// Under Windows NT, this value is returned for some reason.
					// I have not investigated why, but it is also a valid reply
					// Also do nothing and continue.
					break;
				}
			default:
				{
					// All other error codes indicate a serious error has
					// occured.  Process this error.
					//port->ProcessErrorMessage("WaitCommEvent()");
					break;
				}
			}
		}
		else
		{
			// If WaitCommEvent() returns TRUE, check to be sure there are
			// actually bytes in the buffer to read.  
			//
			// If you are reading more than one byte at a time from the buffer 
			// (which this program does not do) you will have the situation occur 
			// where the first byte to arrive will cause the WaitForMultipleObjects() 
			// function to stop waiting.  The WaitForMultipleObjects() function 
			// resets the event handle in m_OverlappedStruct.hEvent to the non-signelead state
			// as it returns.  
			//
			// If in the time between the reset of this event and the call to 
			// ReadFile() more bytes arrive, the m_OverlappedStruct.hEvent handle will be set again
			// to the signeled state. When the call to ReadFile() occurs, it will 
			// read all of the bytes from the buffer, and the program will
			// loop back around to WaitCommEvent().
			// 
			// At this point you will be in the situation where m_OverlappedStruct.hEvent is set,
			// but there are no bytes available to read.  If you proceed and call
			// ReadFile(), it will return immediatly due to the async port setup, but
			// GetOverlappedResults() will not return until the next character arrives.
			//
			// It is not desirable for the GetOverlappedResults() function to be in 
			// this state.  The thread shutdown event (event 0) and the WriteFile()
			// event (Event2) will not work if the thread is blocked by GetOverlappedResults().
			//
			// The solution to this is to check the buffer with a call to ClearCommError().
			// This call will reset the event handle, and if there are no bytes to read
			// we can loop back through WaitCommEvent() again, then proceed.
			// If there are really bytes to read, do nothing and proceed.

			bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

			if (comstat.cbInQue == 0)
				continue;
		}	// end if bResult

		// Main wait function.  This function will normally block the thread
		// until one of nine events occur that require action.
		Event = WaitForMultipleObjects(3, port->m_hEventArray, FALSE, INFINITE);

		switch (Event)
		{
		case 0:
			{
				// Shutdown event.  This is event zero so it will be
				// the higest priority and be serviced first.

				port->m_bThreadAlive = FALSE;

				// Kill this thread.  break is not needed, but makes me feel better.
				AfxEndThread(100);
				break;
			}
		case 1:	// read event
			{
				GetCommMask(port->m_hComm, &CommEvent);

				if (CommEvent & EV_RXCHAR)
					// Receive character event from port.
					ReceiveChar(port, comstat);

				break;
			}  
		case 2: // write event
			{
				// Write character event from port
				WriteChar(port);
				break;
			}

		} // end switch

	} // close forever loop

	port->m_bThreadAlive = FALSE;

	return 0;
}

//
// start comm watching
//
BOOL CLightControl_SP200::StartMonitoring()
{
	m_bThreadRun = TRUE;
	if (!(m_Thread = AfxBeginThread(CommThread, this)))
		return FALSE;
	TRACE("Thread started\n");
	return TRUE;	
}

//
// Restart the comm thread
//
BOOL CLightControl_SP200::RestartMonitoring()
{
	TRACE("Thread resumed\n");
	m_Thread->ResumeThread();
	return TRUE;	
}


//
// Suspend the comm thread
//
BOOL CLightControl_SP200::StopMonitoring()
{
	m_bThreadRun = FALSE;

	do
	{
		SetEvent(m_hShutdownEvent);
	} while (m_bThreadAlive);

	CloseHandle(m_hComm);

	return TRUE;
}


//
// If there is a error, give the right message
//
void CLightControl_SP200::ProcessErrorMessage(CString ErrorText)
{
	//char *Temp = new char[200];
	
	CString Temp = _T("");

	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

//	sprintf(Temp, "WARNING:  %s Failed with the following error: \n%s\nPort: %d\n", (char*)ErrorText, lpMsgBuf, m_nPortNr); 

	ErrorText = _T("WARNING:") + ErrorText;

	Temp.Format(_T(" Failed with the following error: \n%s\nPort: %d\n"), lpMsgBuf, m_nPortNr);
	Temp = ErrorText + Temp;

	MessageBox(NULL, Temp, _T("Application Error"), MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}

//
// Write a character.
//
void CLightControl_SP200::WriteChar(CLightControl_SP200* port)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;

	DWORD BytesSent = 0;

	ResetEvent(port->m_hWriteEvent);

	// Gain ownership of the critical section
	EnterCriticalSection(&port->m_csCommunicationSync);

	if (bWrite)
	{
		// Initailize variables
		port->m_ov.Offset = 0;
		port->m_ov.OffsetHigh = 0;

		// Clear buffer
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

		bResult = WriteFile(port->m_hComm,							// Handle to COMM Port
			port->m_szWriteBuffer,					// Pointer to message buffer in calling finction
			port->m_WriteDataSize,					// Length of message to send
			&BytesSent,								// Where to store the number of bytes sent
			&port->m_ov);							// Overlapped structure

		// deal with any error codes
		if (!bResult)  
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
				{
					// continue to GetOverlappedResults()
					BytesSent = 0;
					bWrite = FALSE;
					break;
				}
			default:
				{
					// all other error codes
					port->ProcessErrorMessage(_T("WriteFile()"));
				}
			}
		} 
		else
		{
			LeaveCriticalSection(&port->m_csCommunicationSync);
		}
	} // end if(bWrite)

	if (!bWrite)
	{
		bWrite = TRUE;

		bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
			&port->m_ov,		// Overlapped structure
			&BytesSent,		// Stores number of bytes sent
			TRUE); 			// Wait flag

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// deal with the error code 
		if (!bResult)  
		{
			port->ProcessErrorMessage(_T("GetOverlappedResults() in WriteFile()"));
		}	
	} // end if (!bWrite)

	// Verify that the data size send equals what we tried to send
	if (BytesSent != strlen((char*)port->m_szWriteBuffer))
	{
		TRACE("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, strlen((char*)port->m_szWriteBuffer));
	}
}

//
// Character received. Inform the owner
//
void CLightControl_SP200::ReceiveChar(CLightControl_SP200* port, COMSTAT comstat)
{
	BOOL  bRead = TRUE; 
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	unsigned char RXBuff;

	for (;;) 
	{ 
		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 

		EnterCriticalSection(&port->m_csCommunicationSync);

		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.

		bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solutiion to be the most efficient way to do this.

		if (comstat.cbInQue == 0)
		{
			// break out when all bytes have been read
			break;
		}

		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bRead)
		{
			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
				&RXBuff,				// RX Buffer Pointer
				1,					// Read one byte
				&BytesRead,			// Stores number of bytes read
				&port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 
			if (!bResult)  
			{ 
				switch (dwError = GetLastError()) 
				{ 
				case ERROR_IO_PENDING: 	
					{ 
						// asynchronous i/o is still in progress 
						// Proceed on to GetOverlappedResults();
						bRead = FALSE;
						break;
					}
				default:
					{
						// Another error has occured.  Process this error.
						port->ProcessErrorMessage(_T("ReadFile()"));
						break;
					} 
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}  // close if (bRead)

		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
				&port->m_ov,		// Overlapped structure
				&BytesRead,		// Stores number of bytes read
				TRUE); 			// Wait flag

			// deal with the error code 
			if (!bResult)  
			{
				port->ProcessErrorMessage(_T("GetOverlappedResults() in ReadFile()"));
			}	
		}  // close if (!bRead)

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// notify parent that a byte was received
		//::SendMessage((port->m_pOwner)->m_hWnd, WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
		port->m_ReadBuff[port->m_ReadCount++] = RXBuff;
		if (RXBuff == 0x0d)
			port->m_ReadComplete = TRUE;
	} // end forever loop

}

//
// Write a string to the port
//
void CLightControl_SP200::WriteToPort(unsigned char* Data, UINT Size)
{		
	ASSERT(m_hComm != 0);

	ClearReadBuff();
	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	memcpy(m_szWriteBuffer, Data, Size);
	m_WriteDataSize = Size;

	// set event for write
	SetEvent(m_hWriteEvent);
}

void CLightControl_SP200::ClearReadBuff(void)
{
	//memset(m_ReadBuff, 0, 256);
	m_ReadComplete = FALSE;
	m_ReadCount = 0;
}

//
// Return the device control block
//
DCB CLightControl_SP200::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD CLightControl_SP200::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD CLightControl_SP200::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}




