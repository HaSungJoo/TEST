
// SerialTestDlg.h : ��� ����
//

#pragma once
#include "LightControl.h"
#include "LightControl_LM6850GHM.h"

// CSerialTestDlg ��ȭ ����
class CSerialTestDlg : public CDialogEx,
					   public ILightControl2Parent
{
// �����Դϴ�.
public:
	CSerialTestDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	virtual ~CSerialTestDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SERIALTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CLightControlInfo				m_LightControlInfo;
	CLightControl_LM6850GHM*		m_pLightControl;

	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonSetlevel();
	afx_msg void OnBnClickedButtonOn();
	afx_msg void OnBnClickedButtonOff();
	afx_msg void OnBnClickedButtonReset();

	virtual void ILC2P_ResponseSignal(BYTE* nBuf, int nLength);
	virtual void ILC2P_ChangeLightLevel(int nIndex, int nlevel);
	virtual void ILC2P_DisplayMessage(int nIndex, const TCHAR* lpstrFormat, ...){};
};
