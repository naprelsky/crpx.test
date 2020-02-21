#include "framework.h"
#include "CropixMFC.h"
#include "CropixMFCDlg.h"
#include "afxdialogex.h"
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_UPDATE_REPORT (WM_USER + 100)

extern CCropixMFCApp mfcApp;

CListCtrl lstReport;
CComboBox cmbCompany;

ReportTreeNode m_reportTree;

CCropixMFCDlg::CCropixMFCDlg(CWnd* pParent): CDialogEx(IDD_CROPIXMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_emulator = new DbEmulator();
}

void CCropixMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_REPORT, lstReport);
	DDX_Control(pDX, IDC_COMBO_COMPANY, cmbCompany);
}

BEGIN_MESSAGE_MAP(CCropixMFCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UPDATE_REPORT, &CCropixMFCDlg::OnUpdateReport)
	ON_BN_CLICKED(IDOK, &CCropixMFCDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_REPORT, &CCropixMFCDlg::OnBnClickedBtnReport)
END_MESSAGE_MAP()

BOOL CCropixMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	lstReport.InsertColumn(0, _T("Лицо"), LVCFMT_LEFT, 200);
	lstReport.InsertColumn(1, _T("Связь между лицами"), LVCFMT_LEFT, 450);
	lstReport.InsertColumn(2, _T("Основание"), LVCFMT_LEFT, 200);

	auto persons = m_emulator->getAllPersonsName();
	for each (std::string person in persons)
	{
		cmbCompany.AddString(encoding::cp1251_to_utf16(person).c_str());
	}

	return TRUE;
}

void CCropixMFCDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void CCropixMFCDlg::OnBnClickedBtnReport()
{
	int i = cmbCompany.GetCurSel();

	if (CB_ERR == cmbCompany.GetCurSel()) {
		MessageBoxW(L"Не выбрана организация для отчета!", L"Отчет");
		return;
	}
	lstReport.DeleteAllItems();

	GetDlgItem(IDC_BTN_REPORT)->EnableWindow(FALSE);
	AfxBeginThread((AFX_THREADPROC)GenerateReportThread, this);
}


LRESULT CCropixMFCDlg::OnUpdateReport(WPARAM wParam, LPARAM lParam)
{
	int index = 0;
	GenerateReportNode(&m_reportTree, &index);
	GetDlgItem(IDC_BTN_REPORT)->EnableWindow(TRUE);
	return 0;
}

void CCropixMFCDlg::GenerateReportNode(ReportTreeNode* node, int* index)
{
	for each (ReportTreeNode child in node->childs)
	{
		int nIndex = lstReport.InsertItem((*index)++, encoding::cp1251_to_utf16(child.data.сompany.Name).c_str());
		lstReport.SetItemText(nIndex, 1, encoding::cp1251_to_utf16(child.data.relation).c_str());
		lstReport.SetItemText(nIndex, 2, encoding::cp1251_to_utf16(child.data.basis).c_str());

		if (child.childs.size() > 0) {
			for each (ReportTreeNode var in child.childs)
			{
				int nIndex = lstReport.InsertItem((*index)++, L"");
				lstReport.SetItemText(nIndex, 1, encoding::cp1251_to_utf16(var.data.relation).c_str());
				lstReport.SetItemText(nIndex, 2, encoding::cp1251_to_utf16(var.data.basis).c_str());
			}
			GenerateReportNode(&child, index);
		}
	}
}

UINT CCropixMFCDlg::GenerateReportThread(LPVOID pParam)
{
	CCropixMFCDlg* pDlg = (CCropixMFCDlg*)pParam;
	CComboBox* pCombo = (CComboBox*)pDlg->GetDlgItem(IDC_COMBO_COMPANY);

	CString value;
	pCombo->GetLBText(pCombo->GetCurSel(), value);

	std::string companyName = CStringA(value);
	m_reportTree = pDlg->m_emulator->getReport(companyName);
	::PostMessage(pDlg->m_hWnd, WM_UPDATE_REPORT, 1, 0);

	return 0;
}
