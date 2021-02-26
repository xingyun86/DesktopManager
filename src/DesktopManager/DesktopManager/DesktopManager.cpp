
// DesktopManager.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "DesktopManager.h"
#include "DesktopManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDesktopManagerApp

BEGIN_MESSAGE_MAP(CDesktopManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDesktopManagerApp construction

CDesktopManagerApp::CDesktopManagerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDesktopManagerApp object

CDesktopManagerApp theApp;
UINT const WM_TASKBARCREATED_MSG = RegisterWindowMessage(TEXT("TaskbarCreated"));


// CDesktopManagerApp initialization

BOOL CDesktopManagerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	(VOID)(::CoInitializeEx(NULL, COINIT_MULTITHREADED));

#ifdef _DEBUG
	AllocConsole();
	_tfopen(TEXT("CONIN$"), TEXT("rb"));
	_tfopen(TEXT("CONOUT$"), TEXT("wb"));
	_tfopen(TEXT("CONOUT$"), TEXT("wb"));
#endif // _DEBUG

	{
		//初始化
		LoadHideShowFlag(m_nHideFlag);
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

		GetLogonDesktopPath(m_tLogonDesktopPath);
		GetPublicDesktopPath(m_tPublicDesktopPath);
	}
__RESTARTS_EXPLORER__:
	{
		if (m_NormalIconList.m_hImageList != NULL)
		{
			while (m_NormalIconList.Remove(0));
			m_NormalIconList.DeleteImageList();
		}
		if (m_SmallIconList.m_hImageList != NULL)
		{
			while (m_SmallIconList.Remove(0));
			m_SmallIconList.DeleteImageList();
		}

		m_LinkDataList.clear();

		m_NormalIconList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 1);
		m_SmallIconList.Create(24, 24, ILC_COLOR24 | ILC_MASK, 1, 1);

		GetLogonDesktopIShellFolder();
		GetPublicDesktopIShellFolder();
		if (m_pIShellFolderDesktopLogon != NULL)
		{
			GetIEnumIDList(m_pIShellFolderDesktopLogon, FALSE, EIDLTYPE_DESKTOP_LOGON);
		}
		if (m_pIShellFolderDesktopPublic != NULL)
		{
			GetIEnumIDList(m_pIShellFolderDesktopPublic, FALSE, EIDLTYPE_DESKTOP_PUBLIC);
		}
	}
	HideOrShowDeskTopIcons(m_nHideFlag);
	for (auto &it : m_LinkWnd)
	{
		it.second = new CDesktopManagerDlg();
		((CDesktopManagerDlg*)it.second)->SetListDataType(it.first);
		((CDesktopManagerDlg*)it.second)->Create(IDD_DESKTOPMANAGER_DIALOG);
		((CDesktopManagerDlg*)it.second)->ShowWindow(SW_SHOWNORMAL);
	}
	
	m_bRunning = TRUE;
	while (m_bRunning)
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
		else
		{
			BOOL bClosed = FALSE;
			BOOL bNormal = FALSE;
			if (m_hShellDllDefView == NULL)
			{
				// 完成某些工作的其他行程式
				m_hShellDllDefView = theApp.FindShellDllDefViewWnd();
				if (m_hShellDllDefView != NULL)
				{
					for (auto& it : m_LinkWnd)
					{
						if (it.second != NULL && it.second->GetSafeHwnd() != NULL)
						{
							it.second->ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
							it.second->SetParent(it.second->FromHandle(m_hShellDllDefView));
							it.second->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
						}
					}
				}
			}
			for (auto& it : m_LinkWnd)
			{
				bNormal |= (!(it.second->GetSafeHwnd() == NULL));
				bClosed |= (!((CDesktopManagerDlg*)it.second)->GetClosed());
			}
			m_bRunning &= bClosed;
			if (bNormal == FALSE)
			{
				for (auto& it : m_LinkWnd)
				{
					if (it.second != NULL)
					{
						((CDesktopManagerDlg*)it.second)->DestroyWindow();
						delete ((CDesktopManagerDlg*)it.second);
						it.second = NULL;
					}
				}
				
				m_hShellDllDefView = NULL;
				printf("explorer exit");
				goto __RESTARTS_EXPLORER__;
			}
			//释放CPU,避免卡顿
			Sleep(16);
		}
	}
	for (auto& it : m_LinkWnd)
	{
		if (it.second != NULL)
		{
			((CDesktopManagerDlg*)it.second)->DestroyWindow();
			delete ((CDesktopManagerDlg*)it.second);
			it.second = NULL;
		}
	}

	HideOrShowDeskTopIcons(SW_SHOWNORMAL);

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

#ifdef _DEBUG
	FreeConsole();
#endif // _DEBUG

	::CoUninitialize();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

