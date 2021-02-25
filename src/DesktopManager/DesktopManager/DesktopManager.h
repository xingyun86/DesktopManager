
// DesktopManager.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDesktopManagerApp:
// See DesktopManager.cpp for the implementation of this class
//
#define SECTION_POSITION_NAME "POSITION"
typedef enum ListCtrlStyleType {
    LCSTYPE_DETAIL = 0,
    LCSTYPE_TILE,
    LCSTYPE_ICON,
    LCSTYPE_LIST,
}ListCtrlStyleType;
typedef enum EnumIDListType {
    EIDLTYPE_DESKTOP_LOGON = 0,
    EIDLTYPE_DESKTOP_PUBLIC,
    EIDLTYPE_QUICKLAUNCH,
}EnumIDListType;
class LinkItem {
public:
    LinkItem(INT _index, DWORD _attr, const TSTRING& _path, const TSTRING& _name, const TSTRING& _type_name) :index(_index), attr(_attr), path(_path), name(_name), type_name(_type_name) {}
public:
    INT index;
    DWORD attr;
    TSTRING path;
    TSTRING name;
    TSTRING type_name;
};

typedef enum ListDataType {
    LDTYPE_NULLPTR = 0,
    LDTYPE_SHORTCUT,
    LDTYPE_FOLDER,
    LDTYPE_OTHERS,
}ListDataType;
class CDesktopManagerApp : public CWinApp
{
public:
	CDesktopManagerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:
    INT m_nFlags = (-1);
    HWND m_hProgman = NULL;
    HWND m_hShellDllDefView = NULL;
    HWND m_hSysListView32 = NULL;
    UINT_PTR m_uTimerID = 0;
	TCHAR m_tParentPath[MAX_PATH] = TEXT("");
	TCHAR m_tLogonDesktopPath[MAX_PATH] = TEXT("");
	TCHAR m_tPublicDesktopPath[MAX_PATH] = TEXT("");
	TCHAR m_tQuickLanchPath[MAX_PATH] = TEXT("");
	IShellFolder* m_pIShellFolderDesktopLogon = NULL;
	IShellFolder* m_pIShellFolderDesktopPublic = NULL;
	IShellFolder* m_pIShellFolderQuickLaunch = NULL;
	IEnumIDList* m_pIEnumFile = NULL;
	IEnumIDList* m_pFirstLayerFile = NULL;
	IEnumIDList* m_pIEnumFolder = NULL;
	IEnumIDList* m_pFirstLayerFolder = NULL;
    CImageList m_NormalIconList = {};
    CImageList m_SmallIconList = {};
	std::vector<LinkItem> m_LinkDataList = {};
    std::unordered_map<ListDataType, CRect> m_LinkDataRect = {
        {LDTYPE_SHORTCUT,{0,0,0,0}},
        {LDTYPE_FOLDER,{0,0,0,0}},
        {LDTYPE_OTHERS,{0,0,0,0}},
    };
    std::unordered_map<ListDataType, TSTRING> m_LinkDataType = {
        {LDTYPE_NULLPTR,TEXT("NULLPTR")},
        {LDTYPE_SHORTCUT,TEXT("快捷方式")},
        {LDTYPE_FOLDER,TEXT("文件夹")},
        {LDTYPE_OTHERS,TEXT("其它")},
    };
public:
    //获取登录用户桌面文件夹的IShellFolder接口指针    
    BOOL GetLogonDesktopIShellFolder()
    {
        m_pIShellFolderDesktopLogon = GetIShellFolderByPath(m_tLogonDesktopPath);
        return TRUE;
    }
    //获取公共桌面文件夹的IShellFolder接口指针    
    BOOL GetPublicDesktopIShellFolder()
    {
        m_pIShellFolderDesktopPublic = GetIShellFolderByPath(m_tPublicDesktopPath);
        return TRUE;
    }
    //获取快速启动文件夹的IShellFolder接口指针    
    BOOL GetQuickLaunchIShellFolder()
    {
        m_pIShellFolderQuickLaunch = GetIShellFolderByPath(m_tQuickLanchPath);
        return TRUE;
    }

    //获取桌面文件夹的IEnumIDList接口    
    BOOL GetIEnumIDList(IShellFolder* pShellFolder, BOOL isRecur, EnumIDListType type)
    {
        if (!pShellFolder) return FALSE;

        HRESULT hr = pShellFolder->EnumObjects(0, SHCONTF_NONFOLDERS, &m_pIEnumFile);
        if (FAILED(hr))
        {
            return FALSE;
        }
        if (!isRecur)
        {
            m_pFirstLayerFile = m_pIEnumFile;//第一层文件夹的遍历接口指针    
        }
        EnumAllItems(m_pIEnumFile, FALSE, type);

        switch (type)
        {
        case EIDLTYPE_DESKTOP_LOGON:
        case EIDLTYPE_DESKTOP_PUBLIC:
        {
            HRESULT hr = pShellFolder->EnumObjects(0, SHCONTF_FOLDERS, &m_pIEnumFolder);
            if (FAILED(hr))
            {
                return FALSE;
            }

            if (!isRecur)
            {
                m_pFirstLayerFolder = m_pIEnumFolder;//第一层文件夹的遍历接口指针    
            }
            EnumAllItems(m_pIEnumFolder, TRUE, type);
        }
        break;
        case EIDLTYPE_QUICKLAUNCH:
            break;
        default:
            break;
        }
        return TRUE;
    }
    //遍历桌面文件夹第一层里的所有项    
    BOOL EnumAllItems(IEnumIDList* pEnum, BOOL bFolder, EnumIDListType type)
    {
        LPITEMIDLIST m_pItem = NULL;
        ULONG   m_ulwork = 0;
        while (pEnum->Next(1, &m_pItem, &m_ulwork) == S_OK)
        {
            //如果是第一层，重置路径
            switch (type)
            {
            case EIDLTYPE_DESKTOP_LOGON:
            {
                if ((m_pFirstLayerFolder == pEnum) && (bFolder))
                {
                    lstrcpy(m_tParentPath, m_tLogonDesktopPath);
                }

                if ((m_pFirstLayerFile == pEnum) && (!bFolder))
                {
                    lstrcpy(m_tParentPath, m_tLogonDesktopPath);
                }
            }
            break;
            case EIDLTYPE_DESKTOP_PUBLIC:
            {
                if ((m_pFirstLayerFolder == pEnum) && (bFolder))
                {
                    lstrcpy(m_tParentPath, m_tPublicDesktopPath);
                }

                if ((m_pFirstLayerFile == pEnum) && (!bFolder))
                {
                    lstrcpy(m_tParentPath, m_tPublicDesktopPath);
                }
            }
            break;
            case EIDLTYPE_QUICKLAUNCH:
            {
                if ((m_pFirstLayerFile == pEnum) && (!bFolder))
                {
                    lstrcpy(m_tParentPath, m_tQuickLanchPath);
                }
            }
            break;
            default:
                break;
            }

            TCHAR tTempFile[MAX_PATH] = { 0 };
            DWORD dwFileAttribute = FILE_ATTRIBUTE_NORMAL;
            UINT uFlags = SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_DISPLAYNAME | SHGFI_TYPENAME;
            WIN32_FIND_DATA wfd = { 0 };
            SHGetDataFromIDList(m_pIShellFolderDesktopLogon, m_pItem, SHGDFIL_FINDDATA, &wfd, sizeof(WIN32_FIND_DATA));
            if (!bFolder)
            {
                lstrcpy(tTempFile, m_tParentPath);
                lstrcat(tTempFile, TEXT("\\"));
                lstrcat(tTempFile, wfd.cFileName);
            }
            else
            {
                lstrcat(m_tParentPath, TEXT("\\"));
                lstrcat(m_tParentPath, wfd.cFileName);
                lstrcpy(tTempFile, m_tParentPath);
                uFlags |= SHGFI_SYSICONINDEX;
                dwFileAttribute = FILE_ATTRIBUTE_DIRECTORY;
            }

            SHFILEINFO sfi = { 0 };
            SHGetFileInfo(tTempFile, dwFileAttribute, &sfi, sizeof(sfi), uFlags | SHGFI_LARGEICON);
            m_NormalIconList.Add(sfi.hIcon);
            SHGetFileInfo(tTempFile, dwFileAttribute, &sfi, sizeof(sfi), uFlags | SHGFI_SMALLICON);
            m_SmallIconList.Add(sfi.hIcon);

            m_LinkDataList.emplace_back(m_LinkDataList.size(), sfi.dwAttributes, tTempFile, sfi.szDisplayName, sfi.szTypeName);
            /*LV_ITEM lvi = { 0 };
            memset(&lvi, 0, sizeof(lvi));
            lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
            lvi.lParam = MAKELPARAM(m_pListLink->GetItemCount(), 0);
            lvi.iItem = m_nRow;
            lvi.iSubItem = m_nCol;
            m_nRow++;
            lvi.stateMask = 0;
            lvi.iImage = m_NormalIconList.GetImageCount() - 1;          //显示不同的图标时，可以把一个int 变量赋给这个属性值
            if (m_strLinkList.size() > lvi.iImage)
            {
                lvi.pszText = (LPTSTR)m_strLinkList.at(lvi.iImage).name.c_str();
            }
            if (m_nCol == 0)
            {
                m_pListLink->InsertItem(&lvi);
            }
            else
            {
                m_pListLink->SetItem(&lvi);
            }*/
        }
        return TRUE;
    }

    //获取登录桌面文件夹的路径    
    int GetLogonDesktopPath(TCHAR(&DesktopPath)[MAX_PATH])
    {
        CRegKey regKey = {};
        if (ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), KEY_READ))
        {
            ULONG ulSize = MAX_PATH;
            regKey.QueryStringValue(TEXT("Desktop"), DesktopPath, &ulSize);
        }
        return 0;
    }
    //获取公共桌面文件夹的路径  
    int GetPublicDesktopPath(TCHAR(&PublicDeskTop)[MAX_PATH])
    {
        CRegKey regKey = {};
        if (ERROR_SUCCESS == regKey.Open(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), KEY_READ))
        {
            ULONG ulSize = MAX_PATH;
            regKey.QueryStringValue(TEXT("Common Desktop"), PublicDeskTop, &ulSize);
        }
        return 0;
    }
    //获取快速启动文件夹的路径    
    int GetQuickLaunchPath(TCHAR(&QuickLaunchPath)[MAX_PATH])
    {
        CRegKey regKey = {};
        if (ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), KEY_READ))
        {
            ULONG ulSize = MAX_PATH;
            regKey.QueryStringValue(TEXT("AppData"), QuickLaunchPath, &ulSize);
            lstrcat(QuickLaunchPath, TEXT("\\Microsoft\\Internet Explorer\\Quick Launch"));
        }
        return 0;
    }

    //获取登录桌面文件夹的路径    
    int SetDesktopIconsState(BOOL bHideIcons)
    {
        CRegKey regKey = {};
        if (ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"), KEY_READ))
        {
            regKey.SetDWORDValue(TEXT("HideIcons"), bHideIcons);
        }
        return 0;
    }
    //获取指定目录的IShellFolder接口    
    IShellFolder* GetIShellFolderByPath(LPTSTR path)
    {
        IShellFolder* pShellFolderTopMost = NULL;
        HRESULT hr = SHGetDesktopFolder(&pShellFolderTopMost);
        if (FAILED(hr))
        {
            return NULL;
        }
        IShellFolder* pFolder = NULL;
        LPITEMIDLIST pidlWorkDir = NULL;
        hr = pShellFolderTopMost->ParseDisplayName(NULL, NULL, path, NULL, &pidlWorkDir, NULL);
        if (FAILED(hr))
        {
            return NULL;
        }
        hr = pShellFolderTopMost->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&pFolder);
        if (pShellFolderTopMost)pShellFolderTopMost->Release();
        return pFolder;
    }

    //读取快捷方式的信息    
    BOOL ReadShortCut(LPTSTR linkName)
    {
        IShellLink* pIShellLink = NULL;
        IPersistFile* pIPersistFile = NULL;
        HRESULT hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pIShellLink);
        if (hr == S_OK)
        {
            hr = pIShellLink->QueryInterface(IID_IPersistFile, (void**)&pIPersistFile);
            if (hr == S_OK)
            {
                USES_CONVERSION;
                pIPersistFile->Load(T2COLE(linkName), STGM_READWRITE);
                TCHAR tstrPath[MAX_PATH] = { 0 };

                //读取快捷方式的目标路径    
                pIShellLink->GetPath(tstrPath, sizeof(tstrPath) / sizeof(*tstrPath), NULL, SLGP_UNCPRIORITY);
                /*CString temp = tstrPath;
                temp.MakeUpper();
                for (auto it : m_strLinkList)
                {
                    if (i == 5)
                    {
                        if (temp.Right(9).Compare(it.c_str()) == 0)
                        {
                            pIShellLink->SetArguments(TEXT(""));
                            pIPersistFile->Save(T2COLE(linkName), TRUE);
                        }
                    }
                    else
                    {
                        if (temp.Find(it.path.c_str()) != -1)
                        {
                            pIShellLink->SetArguments(TEXT(""));
                            pIPersistFile->Save(T2COLE(linkName), TRUE);
                        }
                    }
                }*/
            }
            else
            {
                if (pIShellLink) pIShellLink->Release();
                if (pIPersistFile) pIPersistFile->Release();
                return FALSE;
            }
        }
        else
        {
            if (pIShellLink) pIShellLink->Release();
            if (pIPersistFile) pIPersistFile->Release();
            return FALSE;
        }
        if (pIShellLink) pIShellLink->Release();
        if (pIPersistFile) pIPersistFile->Release();
        return TRUE;
    }

    //绑定列表框
    void BindListCtrl(CListCtrl * pListCtrl)
    {
        pListCtrl->SetImageList(&m_NormalIconList, LVSIL_NORMAL);
        pListCtrl->SetImageList(&m_SmallIconList, LVSIL_SMALL);
    }

    //设置列表框属性
    void SetListCtrlStyle(CListCtrl * pListCtrl, ListCtrlStyleType lcstype)
    {
        pListCtrl->ModifyStyleEx(0, LVS_EX_DOUBLEBUFFER, TRUE);
        switch (lcstype)
        {
        case LCSTYPE_DETAIL:
            pListCtrl->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_ICON, LVS_REPORT, TRUE);
            break;
        case LCSTYPE_TILE:
            pListCtrl->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_REPORT, LVS_ICON | LVS_AUTOARRANGE, TRUE);
            break;
        case LCSTYPE_ICON:
            pListCtrl->ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_OWNERDRAWFIXED, LVS_SMALLICON, TRUE);
            break;
        case LCSTYPE_LIST:
            pListCtrl->ModifyStyle(LVS_ICON | LVS_SMALLICON | LVS_REPORT | LVS_OWNERDRAWFIXED, LVS_LIST, TRUE);
            break;
        default:
            pListCtrl->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_ICON, LVS_REPORT, TRUE);
            break;
        }
    }

    //隐藏显示桌面快捷方式图标
    void HideOrShowDeskTopIcons(INT nFlags)
    {
        SHELLFLAGSTATE sfs = { 0 };
        SHGetSettings(&sfs, SSF_HIDEICONS);
        if (sfs.fHideIcons == FALSE)
        {
            HWND hProgman = NULL;
            HWND hShellDllDefView = NULL;
            hProgman = ::FindWindow(TEXT("Progman"), TEXT("Program Manager")); //find desktop icons
            if (hProgman != NULL)
            {
                if (hProgman != m_hProgman)
                {
                    m_hSysListView32 = NULL;
                }
                
                hShellDllDefView = ::FindWindowEx(hProgman, NULL, TEXT("SHELLDLL_DefView"), NULL);
                if (hShellDllDefView == NULL)
                {
                    HWND hWorkerW = NULL;
                __AGAIN__:
                    hWorkerW = ::FindWindowEx(NULL, hWorkerW, TEXT("WorkerW"), NULL);
                    if (hWorkerW != NULL)
                    {
                        hShellDllDefView = ::FindWindowEx(hWorkerW, NULL, TEXT("SHELLDLL_DefView"), NULL);
                        if (hShellDllDefView == NULL)
                        {
                            goto __AGAIN__;
                        }
                    }
                }
                if (hShellDllDefView != NULL)
                {
                    if (hShellDllDefView != m_hShellDllDefView)
                    {
                        m_hSysListView32 = NULL;
                    }
                    if (hProgman != m_hProgman || hShellDllDefView != m_hShellDllDefView)
                    {
                        m_hProgman = hProgman;
                        m_hShellDllDefView = hShellDllDefView;
                        m_hSysListView32 = ::FindWindowEx(hShellDllDefView, NULL, TEXT("SysListView32"), NULL);
                    }
                }
            }
        }
        if (m_hSysListView32 != NULL)
        {
            ::ShowWindow(m_hSysListView32, nFlags);
        }
        if (nFlags == SW_SHOWNORMAL)
        {
            KillTimer(NULL, m_uTimerID);
            m_uTimerID = 0;
        }
    }

    //隐藏显示桌面快捷方式图标
    void HideOrShowDeskTopIcons()
    {
        if (m_nFlags == (-1))
        {
            SHELLFLAGSTATE sfs = { 0 };
            SHGetSettings(&sfs, SSF_HIDEICONS);
            m_nFlags = sfs.fHideIcons ? SW_HIDE : SW_SHOWNORMAL;
        }
        m_nFlags = (m_nFlags == SW_SHOWNORMAL) ? SW_HIDE : SW_SHOWNORMAL;

        if (m_nFlags == SW_HIDE)
        {
            if (m_uTimerID == 0)
            {
                m_uTimerID = SetTimer(NULL, NULL, 300, [](HWND hWnd, UINT uMsg, UINT_PTR uTimeID, DWORD dwTime)
                    {
                        extern CDesktopManagerApp theApp;
                        theApp.HideOrShowDeskTopIcons(theApp.m_nFlags);
                    });
            }
        }
        HWND hTaskBar = ::FindWindow(TEXT("Shell_TrayWnd"), NULL);//find taskbar handle
        //::ShowWindow(hTaskBar, nFlags);

        //::ShowWindow(hDeskIcon, nFlags);
        //SHELLSTATE ss = { 0 };
        //SecureZeroMemory(&ss, sizeof(ss));
        //sfs.fHideIcons = !sfs.fHideIcons;
        //SHGetSettings(&sfs, SSF_HIDEICONS);	
        //SHGetSetSettings(&ss, SSF_HIDEICONS, FALSE);
        //ss.fHideIcons = !ss.fHideIcons;
        //SHGetSetSettings(&ss, SSF_HIDEICONS, TRUE);
        //SetDesktopIconsState(ss.fHideIcons);
        //PDWORD_PTR lpdwResult = 0;
        //SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, NULL, NULL, SMTO_ABORTIFHUNG, 3, lpdwResult);
        //SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSHNOWAIT, NULL, NULL);
        //SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    }

    void LoadItemPostion(CRect & rect, ListDataType listDataType)
    {
        CString strPosDefault = TEXT("");
        switch (listDataType)
        {
        case LDTYPE_NULLPTR:strPosDefault = TEXT("(0,0,0,0)"); break;
        case LDTYPE_SHORTCUT:strPosDefault = TEXT("(1,1,641,481)"); break;
        case LDTYPE_FOLDER:strPosDefault = TEXT("(641,1,1281,481)"); break;
        case LDTYPE_OTHERS:strPosDefault = TEXT("(1,481,641,961)"); break;
        default: break;
        }
        CString strPos = GetProfileString(TEXT(SECTION_POSITION_NAME), m_LinkDataType.at(listDataType).c_str(), strPosDefault);
        _stscanf(strPos, TEXT("(%d,%d,%d,%d)"), &rect.left, &rect.top, &rect.right, &rect.bottom);
    }
    void SaveItemPostion(CRect& rect, ListDataType listDataType)
    {
        CString strPos = TEXT("");
        strPos.Format(TEXT("(%d,%d,%d,%d)"), rect.left, rect.top, rect.right, rect.bottom);
        WriteProfileString(TEXT(SECTION_POSITION_NAME), m_LinkDataType.at(listDataType).c_str(), strPos);
    }

};

extern CDesktopManagerApp theApp;
