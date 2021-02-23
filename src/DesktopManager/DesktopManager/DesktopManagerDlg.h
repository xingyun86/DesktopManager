
// DesktopManagerDlg.h : header file
//

#pragma once

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
// CDesktopManagerDlg dialog
class CDesktopManagerDlg : public CDialogEx
{
// Construction
public:
	CDesktopManagerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DESKTOPMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedOk();
    afx_msg void OnNMDblclkListLink(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP()
public:
    void ResizeWindow(INT cx = 0, INT cy = 0)
    {
        if (m_pListLink != NULL)
        {
            CRect rect = { 0,0, cx, cy };
            CRect rectOk = {};
            CRect rectCancel = {};
            if (rect.Width() <= 0)
            {
                GetClientRect(rect);
            }
            GetDlgItem(IDOK)->GetClientRect(rectOk);
            GetDlgItem(IDCANCEL)->GetClientRect(rectCancel);
            GetDlgItem(IDOK)->MoveWindow(CRect(CPoint(rect.Width() - rectOk.Width() - rectCancel.Width(), 0), CSize(rectOk.Width(), rectOk.Height())), FALSE);
            GetDlgItem(IDCANCEL)->MoveWindow(CRect(CPoint(rect.Width() - rectCancel.Width(), 0), CSize(rectCancel.Width(), rectCancel.Height())), FALSE);
            GetDlgItem(IDC_EDIT_NAME)->MoveWindow(CRect(CPoint(rectOk.Width() + rectCancel.Width(), 0), CSize(rect.Width() - rectOk.Width() - rectCancel.Width() - rectOk.Width() - rectCancel.Width(), rectOk.Height())), FALSE);
            GetDlgItem(IDC_LIST_LINK)->MoveWindow(CRect(CPoint(0, rectOk.Height()), CSize(rect.Width(), rect.Height() - rectOk.Height())), FALSE);
            m_nRow = 0;
            m_nCol = 0;
            for (size_t i = 0; i < m_strLinkList.size(); i++)
            {
                LV_ITEM lvi = { 0 };
                memset(&lvi, 0, sizeof(lvi));
                lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
                lvi.lParam = MAKELPARAM(i, 0);
                lvi.iItem = m_nRow;
                lvi.iSubItem = m_nCol;
                m_nRow++;
                lvi.stateMask = 0;
                lvi.iImage = i;          //显示不同的图标时，可以把一个int 变量赋给这个属性值
                lvi.pszText = (LPTSTR)m_strLinkList.at(lvi.iImage).name.c_str();
                if (m_nCol == 0)
                {
                    m_pListLink->InsertItem(&lvi);
                }
                else
                {
                    m_pListLink->SetItem(&lvi);
                }
            }
            Invalidate();
        }
    }
public:
    INT m_nRow = 0;
    INT m_nCol = 0;
    TCHAR m_tParentPath[MAX_PATH] = TEXT("");
    TCHAR m_tLogonDesktopPath[MAX_PATH] = TEXT("");
    TCHAR m_tPublicDesktopPath[MAX_PATH] = TEXT("");
    TCHAR m_tQuickLanchPath[MAX_PATH] = TEXT("");
    std::vector<LinkItem> m_strLinkList = {};
    CListCtrl* m_pListLink = NULL;
    CImageList m_NormalIconList = {};
    CImageList m_SmallIconList = {};
    IShellFolder* m_pIShellFolderDesktopLogon = NULL;
    IShellFolder* m_pIShellFolderDesktopPublic = NULL;
    IShellFolder* m_pIShellFolderQuickLaunch = NULL;
    IEnumIDList* m_pIEnumFile = NULL;
    IEnumIDList* m_pFirstLayerFile = NULL;
    IEnumIDList* m_pIEnumFolder = NULL;
    IEnumIDList* m_pFirstLayerFolder = NULL;
public:

    void SetListCtrlStyle(ListCtrlStyleType lcstype)
    {
        m_pListLink->ModifyStyleEx(0, LVS_EX_DOUBLEBUFFER, TRUE);
        switch (lcstype)
        {
        case LCSTYPE_DETAIL:
            m_pListLink->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_ICON, LVS_REPORT, TRUE);
            break;
        case LCSTYPE_TILE:
            m_pListLink->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_REPORT, LVS_ICON | LVS_AUTOARRANGE, TRUE);
            break;
        case LCSTYPE_ICON:
            m_pListLink->ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_OWNERDRAWFIXED, LVS_SMALLICON, TRUE);
            break;
        case LCSTYPE_LIST:
            m_pListLink->ModifyStyle(LVS_ICON | LVS_SMALLICON | LVS_REPORT | LVS_OWNERDRAWFIXED, LVS_LIST, TRUE);
            break;
        default:
            m_pListLink->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_ICON, LVS_REPORT, TRUE);
            break;
        }
    }
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
            m_pListLink->SetImageList(&m_NormalIconList, LVSIL_NORMAL);
            m_pListLink->SetImageList(&m_SmallIconList, LVSIL_SMALL);
            m_strLinkList.emplace_back(m_strLinkList.size(), sfi.dwAttributes, tTempFile, sfi.szDisplayName, sfi.szTypeName);
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
                pIShellLink->GetPath(tstrPath, sizeof(tstrPath)/sizeof(*tstrPath), NULL, SLGP_UNCPRIORITY);
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
};
