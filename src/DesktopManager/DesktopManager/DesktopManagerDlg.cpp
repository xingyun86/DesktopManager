
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
	m_pIShellFolder = NULL;
	m_pIEnumFolder = NULL;
	m_pAppData = NULL;
	m_pIEnumFile = NULL;
	m_pFirstLayerFolder = NULL;
	m_pFirstLayerFile = NULL;
	memset(m_tDeskTopPath, 0, sizeof(m_tDeskTopPath) / sizeof(*m_tDeskTopPath));
	memset(m_tQuickLanchPath, 0, sizeof(m_tQuickLanchPath) / sizeof(*m_tQuickLanchPath));
	memset(m_tParentPath, 0, sizeof(m_tParentPath) / sizeof(*m_tParentPath));
	/*m_strBrowserList = {
		LinkItem(0,TEXT("360SE.EXE"),{0}),//360浏览器    
		LinkItem(0,TEXT("IEXPLORE.EXE"),{0}),//IE浏览器    
		LinkItem(0,TEXT("FIREFOX.EXE"),{0}),//火狐    
		LinkItem(0,TEXT("OPERA.EXE"),{0}),//Opera    
		LinkItem(0,TEXT("MAXTHON.EXE"),{0}),//遨游    
		LinkItem(0,TEXT("LOCAL SET"),{0}),//Google    
		LinkItem(0,TEXT("THEWORLD.EXE"),{0}),//世界之窗    
		LinkItem(0,TEXT("TTRAVELER.EXE"),{0}),//腾讯TT    
	};*/
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
	{
		SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE,
			GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0X80000);
		HMODULE hMod = GetModuleHandle(TEXT("USER32.DLL")); //显式加载DLL
		if (hMod != NULL)
		{
			typedef BOOL(WINAPI* PFN_SetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
			PFN_SetLayeredWindowAttributes fnSetLayeredWindowAttributes = (PFN_SetLayeredWindowAttributes)GetProcAddress(hMod, "SetLayeredWindowAttributes");//取得SetLayeredWindowAttributes函数指针
			if (fnSetLayeredWindowAttributes != NULL)
			{
				fnSetLayeredWindowAttributes(this->GetSafeHwnd(), 0, 150, 2);      //这里使用的是透明度150，fnSetLayeredWindowAttributes是函数指针，指向了SetLayeredWindowAttributes函数。
			}
		}
	}
	SetListCtrlStyle(LCSTYPE_TILE);
	m_NormalIconList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 1);
	m_SmallIconList.Create(24, 24, ILC_COLOR24 | ILC_MASK, 1, 1);
	GetPath(m_tDeskTopPath, m_tQuickLanchPath);
	
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
	CDialogEx::OnOK();
}
