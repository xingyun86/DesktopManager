
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
	m_pIEnumFolder = NULL;
	m_pIShellFolderDesktopLogon = NULL;
	m_pIShellFolderDesktopPublic = NULL;
	m_pIShellFolderQuickLaunch = NULL;
	m_pIEnumFile = NULL;
	m_pFirstLayerFolder = NULL;
	m_pFirstLayerFile = NULL;
	memset(m_tLogonDesktopPath, 0, sizeof(m_tLogonDesktopPath) / sizeof(*m_tLogonDesktopPath));
	memset(m_tPublicDesktopPath, 0, sizeof(m_tPublicDesktopPath) / sizeof(*m_tPublicDesktopPath));
	memset(m_tQuickLanchPath, 0, sizeof(m_tQuickLanchPath) / sizeof(*m_tQuickLanchPath));
	memset(m_tParentPath, 0, sizeof(m_tParentPath) / sizeof(*m_tParentPath));
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
	ON_BN_CLICKED(IDOK, &CDesktopManagerDlg::OnBnClickedOk)
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
				fnSetLayeredWindowAttributes(this->GetSafeHwnd(), 0, 180, 2);      //这里使用的是透明度150，fnSetLayeredWindowAttributes是函数指针，指向了SetLayeredWindowAttributes函数。
			}
		}
	}*/
	SetListCtrlStyle(LCSTYPE_TILE);
	//m_pListLink->ModifyStyle(0, LVS_NOSCROLL);
	//m_pListLink->ShowScrollBar(SB_HORZ, FALSE);
	//m_pListLink->ShowScrollBar(SB_VERT, TRUE);
	m_pListLink->SetIconSpacing(CSize(64, 64));     //set pictures spacing
	m_NormalIconList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 1);
	m_SmallIconList.Create(24, 24, ILC_COLOR24 | ILC_MASK, 1, 1);
	GetLogonDesktopPath(m_tLogonDesktopPath);
	GetPublicDesktopPath(m_tPublicDesktopPath);
	GetLogonDesktopIShellFolder();
	GetPublicDesktopIShellFolder();

	ResizeWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
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



void CDesktopManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	ResizeWindow();
}


void CDesktopManagerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
	m_pListLink->DeleteAllItems();
	m_strLinkList.clear();
	while (m_NormalIconList.Remove(0));
	while (m_SmallIconList.Remove(0));
	if (m_pIShellFolderDesktopLogon != NULL)
	{
		GetIEnumIDList(m_pIShellFolderDesktopLogon, FALSE, EIDLTYPE_DESKTOP_LOGON);
	}
	if (m_pIShellFolderDesktopPublic != NULL)
	{
		GetIEnumIDList(m_pIShellFolderDesktopPublic, FALSE, EIDLTYPE_DESKTOP_PUBLIC);
	}
	static INT nFlags = SW_SHOWNORMAL;
	nFlags = (nFlags == SW_SHOWNORMAL) ? SW_HIDE : SW_SHOWNORMAL;

	CRect rect = {};
	GetWindowRect(rect);
	MoveWindow(rect.left, rect.top, 480 + ((nFlags == SW_HIDE) ? -SW_SHOWNORMAL : SW_SHOWNORMAL), (m_strLinkList.size() / 6 + 1) * 80 > 480 ? 480 : (m_strLinkList.size() / 8 + 1) * 80);

	//HWND hDesktopWnd = ::FindWindowEx(::GetDesktopWindow(), NULL, WC_LISTVIEW, TEXT(""));
	//::ShowWindow(hDesktopWnd, nFlags);

	HWND hTaskBar = ::FindWindow(TEXT("Shell_TrayWnd"), NULL);//find taskbar handle
	::ShowWindow(hTaskBar, nFlags);
	HWND hDeskIcon = ::FindWindow(TEXT("Progman"), NULL); //find desktop icons
	::ShowWindow(hDeskIcon, nFlags);
}


void CDesktopManagerDlg::OnNMDblclkListLink(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (pNMItemActivate->iItem != (-1))
	{
		ShellExecute(NULL, TEXT("OPEN"), m_strLinkList[pNMItemActivate->iItem].path.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}


LRESULT CDesktopManagerDlg::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect = {};
	CRect rectOk = {};
	GetDlgItem(IDOK)->GetClientRect(rectOk);
	rect.left = rect.top = 0;
	rect.bottom = rectOk.Height();
	rect.right = rectOk.Width() + rectOk.Width();
	ClientToScreen(&rect);
	return rect.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);   //鼠标如果在客户区，将其当作标题栏
	//return CDialogEx::OnNcHitTest(point);
}
