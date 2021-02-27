
// DesktopManagerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "DesktopManager.h"
#include "DesktopManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDesktopManagerDlg dialog



CDesktopManagerDlg::CDesktopManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DESKTOPMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDesktopManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDesktopManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDOK, &CDesktopManagerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDesktopManagerDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LINK, &CDesktopManagerDlg::OnNMDblclkListLink)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CDesktopManagerDlg message handlers

BOOL CDesktopManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	//theApp.m_hDesktopIconParentWnd = theApp.FindDesktopIconParentWnd();
	//SetParent(FromHandle(theApp.m_hDesktopIconParentWnd));

	CRect rect = {};
	theApp.LoadItemPostion(rect, m_listDataType);
	CString strBtnText = (theApp.m_nHideFlag == SW_HIDE) ? TEXT("显示桌面") : TEXT("隐藏桌面");
	SetDlgItemText(IDOK, strBtnText);
	SetDlgItemText(IDCANCEL, TEXT("退出"));
	SetDlgItemText(IDC_EDIT_NAME, theApp.m_LinkDataType.at(m_listDataType).c_str());// TEXT("桌面快捷方式"));
	GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);

	m_pListLink = ((CListCtrl*)(GetDlgItem(IDC_LIST_LINK)));
	/*{
		SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE,	GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0X80000);
		HMODULE hMod = GetModuleHandle(TEXT("USER32.DLL")); //显式加载DLL
		if (hMod != NULL)
		{
			typedef BOOL(WINAPI* PFN_SetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
			PFN_SetLayeredWindowAttributes fnSetLayeredWindowAttributes = (PFN_SetLayeredWindowAttributes)GetProcAddress(hMod, "SetLayeredWindowAttributes");//取得SetLayeredWindowAttributes函数指针
			if (fnSetLayeredWindowAttributes != NULL)
			{
				fnSetLayeredWindowAttributes(this->GetSafeHwnd(), 0, 222, 2);      //这里使用的是透明度150，fnSetLayeredWindowAttributes是函数指针，指向了SetLayeredWindowAttributes函数。
			}
		}
	}*/
	theApp.SetListCtrlStyle(m_pListLink, LCSTYPE_TILE);
	//m_pListLink->ModifyStyle(0, LVS_NOSCROLL);
	//m_pListLink->ShowScrollBar(SB_HORZ, FALSE);
	//m_pListLink->ShowScrollBar(SB_VERT, TRUE);
	m_pListLink->SetIconSpacing(CSize(64, 64));     //set pictures spacing
	theApp.BindListCtrl(m_pListLink);

	MoveWindow(rect.left, rect.top, (rect.Width() == 0) ? 640: rect.Width(), (rect.Height() == 0) ? 480 : rect.Height());
	ResizeWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
LRESULT CDesktopManagerDlg::DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (WM_TASKBARCREATED_MSG == uMsg)
	{
		// Explorer.exe重新启动,仅当顶层父窗口才可以接收到该消息
		theApp.m_hShellDllDefView = NULL;
		return TRUE;
	}
	// TODO: Add your specialized code here and/or call the base class
	return CDialogEx::DefWindowProc(uMsg, wParam, lParam);
}

void CDesktopManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDesktopManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDesktopManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDesktopManagerDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: Add your message handler code here
	if (m_pListLink != NULL)
	{
		CRect rect = {};
		GetWindowRect(rect);
		theApp.SaveItemPostion(rect, m_listDataType);
	}
}


void CDesktopManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	ResizeWindow();
}

void CDesktopManagerDlg::OnNMDblclkListLink(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (pNMItemActivate->iItem != (-1) && pNMItemActivate->iSubItem != (-1))
	{
		LV_ITEM lvi = { 0 };
		lvi.mask = LVIF_PARAM;
		lvi.iItem = pNMItemActivate->iItem;
		lvi.iSubItem = pNMItemActivate->iSubItem;
		if (m_pListLink->GetItem(&lvi) && lvi.lParam < theApp.m_LinkDataList.size())
		{
			ShellExecute(NULL, TEXT("OPEN"), theApp.m_LinkDataList[lvi.lParam].path.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

LRESULT CDesktopManagerDlg::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CRect rect;
	GetWindowRect(&rect);
	CRect rect1 = rect;
	rect1.DeflateRect(6, 6, -6, -6);
	rect1.NormalizeRect();
	if (point.x <= rect.left + 2)
		return HTLEFT;
	else if (point.x >= rect.right - 2)
		return HTRIGHT;
	else if (point.y <= rect.top + 2)
		return HTTOP;
	else if (point.y >= rect.bottom - 2)
		return HTBOTTOM;
	else if (point.x <= rect.left + 6 && point.y <= rect.top + 6)
		return HTTOPLEFT;
	else if (point.x >= rect.right - 6 && point.y <= rect.top + 6)
		return HTTOPRIGHT;
	else if (point.x <= rect.left + 6 && point.y >= rect.bottom - 6)
		return HTBOTTOMLEFT;
	else if (point.x >= rect.right - 6 && point.y >= rect.bottom - 6)
		return HTBOTTOMRIGHT;
	else if (!rect.IsRectEmpty())
	{
		LRESULT uRet = CWnd::OnNcHitTest(point);
		uRet = (uRet == HTCLIENT) ? HTCAPTION : uRet;
		return uRet;
	}
	else
	{
		{
			CRect rect = {};
			CRect rectOk = {};
			CRect rectCancel = {};
			GetClientRect(&rect);
			GetDlgItem(IDOK)->GetClientRect(rectOk);
			GetDlgItem(IDCANCEL)->GetClientRect(rectCancel);
			rect.right = rect.Width() - (rectOk.Width() + rectCancel.Width());
			rect.bottom = rectOk.Height();
			ClientToScreen(&rect);
			return rect.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);   //鼠标如果在客户区，将其当作标题栏
		}
		return CWnd::OnNcHitTest(point);
	}
	return 0;
	//return CDialogEx::OnNcHitTest(point);
}

void CDesktopManagerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();	
	theApp.HideOrShowDeskTopIcons();
}

void CDesktopManagerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	SetParent(NULL);
	CDialogEx::OnCancel();
	m_bClosed = TRUE;
}
