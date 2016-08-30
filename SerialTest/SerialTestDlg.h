
// SerialTestDlg.h : 헤더 파일
//

#pragma once
#include "LightControl.h"
#include "LightControl_LM6850GHM.h"

// CSerialTestDlg 대화 상자
class CSerialTestDlg : public CDialogEx,
					   public ILightControl2Parent
{
// 생성입니다.
public:
	CSerialTestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	virtual ~CSerialTestDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERIALTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
