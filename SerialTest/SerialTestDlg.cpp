
// SerialTestDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "SerialTest.h"
#include "SerialTestDlg.h"
#include "afxdialogex.h"
#include "Serial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSerialTestDlg ��ȭ ����




CSerialTestDlg::CSerialTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSerialTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSerialTestDlg::~CSerialTestDlg()
{
// 	if(m_pLightControl)
// 	{
// 		delete m_pLightControl;
// 		m_pLightControl = NULL;
// 	}	
}

void CSerialTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSerialTestDlg, CDialogEx)
	
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CSerialTestDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SETLEVEL, &CSerialTestDlg::OnBnClickedButtonSetlevel)
	ON_BN_CLICKED(IDC_BUTTON_ON, &CSerialTestDlg::OnBnClickedButtonOn)
	ON_BN_CLICKED(IDC_BUTTON_OFF, &CSerialTestDlg::OnBnClickedButtonOff)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CSerialTestDlg::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CSerialTestDlg �޽��� ó����

BOOL CSerialTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_pLightControl = new CLightControl_LM6850GHM(0);
	if(m_pLightControl==NULL)
	{
		AfxMessageBox(_T("Create Error!"));
		return TRUE;
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CSerialTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CSerialTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CSerialTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSerialTestDlg::OnBnClickedButtonConnect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_pLightControl==NULL)
		return;

	m_LightControlInfo.SetBaudRate(GetDlgItemInt(IDC_EDIT_BAUDRATE));
	CString strTemp;
	GetDlgItemText(IDC_EDIT_PORT, strTemp);
	m_LightControlInfo.SetConnectionPort(strTemp);

	if(!m_pLightControl->Connect(m_LightControlInfo))
	{
		AfxMessageBox(_T("Connect Error!"));
	}

	m_pLightControl->SetLC2P(static_cast<ILightControl2Parent*>(this));
}


void CSerialTestDlg::OnBnClickedButtonSetlevel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_pLightControl==NULL)
		return;
	CString strTemp; 
	GetDlgItemText(IDC_EDIT_VALUE, strTemp); 
	double data = _wtof(strTemp);
	
	if(!m_pLightControl->SetLightLevel(data))
	{
		AfxMessageBox(_T("Set Value Error!"));
	}
}


void CSerialTestDlg::OnBnClickedButtonOn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_pLightControl==NULL)
		return;
	
	if(!m_pLightControl->SetLightOn())
	{
		AfxMessageBox(_T("Power On Error!"));
	}

	return;
}


void CSerialTestDlg::OnBnClickedButtonOff()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_pLightControl==NULL)
		return;

	if(!m_pLightControl->SetLightOff())
	{
		AfxMessageBox(_T("Power Off Error!"));
	}

	return;
}


void CSerialTestDlg::OnBnClickedButtonReset()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_pLightControl==NULL)
		return;

	if(!m_pLightControl->FactoryReset())
	{
		AfxMessageBox(_T("Power Off Error!"));
	}

	return;
}

void CSerialTestDlg::ILC2P_ResponseSignal(BYTE* nBuf, int nLength)
{
	CString str1, str2;
	for(int i=0;i < nLength; i++)
	{
		str1.Format(_T("%x "), nBuf[i]);
		str2 += str1;
	}
	
	SetDlgItemText(IDC_EDIT_RECEIVE_DATA,str2);
}

void CSerialTestDlg::ILC2P_ChangeLightLevel(int nIndex, int nlevel)
{

}