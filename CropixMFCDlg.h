#pragma once
#include "DbEmulator.h"

class CCropixMFCDlg : public CDialogEx
{
public:
	DbEmulator* m_emulator;

	CCropixMFCDlg(CWnd* pParent = nullptr);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnReport();
	afx_msg LRESULT OnUpdateReport(WPARAM wParam, LPARAM lParam);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

private:
	static UINT GenerateReportThread(LPVOID pParam);
	void GenerateReportNode(ReportTreeNode* node, int* index);
};
