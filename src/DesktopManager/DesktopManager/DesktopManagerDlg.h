
// DesktopManagerDlg.h : header file
//

#pragma once

typedef enum ListCtrlStyleType {
    LCSTYPE_DETAIL = 0,
    LCSTYPE_TILE,
    LCSTYPE_ICON,
    LCSTYPE_LIST,
}ListCtrlStyleType;
class LinkItem {
public:
    LinkItem(INT _index, const TSTRING& _path, const TSTRING& _name) :index(_index), path(_path), name(_name) {}
public:
    INT index;
    TSTRING path;
    TSTRING name;
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
	DECLARE_MESSAGE_MAP()
public:
    void ResizeWindow()
    {
        CRect rect = {};
        CRect rectOk = {};
        CRect rectCancel = {};
        GetClientRect(rect);
        GetDlgItem(IDOK)->GetClientRect(rectOk);
        GetDlgItem(IDCANCEL)->GetClientRect(rectCancel);
        GetDlgItem(IDC_LIST_LINK)->MoveWindow(CRect(CPoint(0,0),CSize(rect.Width(), rect.Height() - rectOk.Height())));
        GetDlgItem(IDOK)->MoveWindow(CRect(CPoint(rect.Width() - rectOk.Width() - rectCancel.Width(), rect.Height() - rectOk.Height()), CSize(rectOk.Width(), rectOk.Height())));
        GetDlgItem(IDCANCEL)->MoveWindow(CRect(CPoint(rect.Width() - rectCancel.Width(), rect.Height() - rectCancel.Height()), CSize(rectCancel.Width(), rectCancel.Height())));
    }
public:
    INT m_nRow = 0;
    INT m_nCol = 0;
    TCHAR m_tParentPath[MAX_PATH] = TEXT("");
    TCHAR m_tDeskTopPath[MAX_PATH] = TEXT("");
    TCHAR m_tQuickLanchPath[MAX_PATH] = TEXT("");
    std::vector<LinkItem> m_strLinkList = {};
    CListCtrl* m_pListLink = NULL;
    CImageList m_NormalIconList = {};
    CImageList m_SmallIconList = {};
    IShellFolder* m_pIShellFolder = NULL;
    IShellFolder* m_pAppData = NULL;
    IEnumIDList* m_pIEnumFile = NULL;
    IEnumIDList* m_pFirstLayerFile = NULL;
    IEnumIDList* m_pIEnumFolder = NULL;
    IEnumIDList* m_pFirstLayerFolder = NULL;
public:

    //获取桌面文件夹的IShellFolder接口指针    
    BOOL GetDesktopIShellFolder()
    {
        m_pIShellFolder = GetIShellFolderByPath(m_tDeskTopPath);
        m_pAppData = GetIShellFolderByPath(m_tQuickLanchPath);
        return TRUE;
    }

    //获取桌面文件夹的IEnumIDList接口    
    BOOL GetIEunmIDList(IShellFolder* m_pShellFolder, BOOL isRecur, BOOL isQuickLanch)
    {
        if (!m_pShellFolder) return FALSE;

        HRESULT hr = m_pShellFolder->EnumObjects(0, SHCONTF_NONFOLDERS, &m_pIEnumFile);
        if (FAILED(hr))
        {
            return FALSE;
        }
        if (!isRecur)
        {
            m_pFirstLayerFile = m_pIEnumFile;//第一层文件夹的遍历接口指针    
        }
        EnumAllItems(m_pIEnumFile, FALSE, isQuickLanch);

        if (!isQuickLanch)
        {
            HRESULT hr = m_pShellFolder->EnumObjects(0, SHCONTF_FOLDERS, &m_pIEnumFolder);
            if (FAILED(hr))
            {
                return FALSE;
            }

            if (!isRecur)
            {
                m_pFirstLayerFolder = m_pIEnumFolder;//第一层文件夹的遍历接口指针    
            }
            EnumAllItems(m_pIEnumFolder, TRUE, isQuickLanch);
        }
        return TRUE;
    }
    void SetListCtrlStyle(ListCtrlStyleType lcstype)
    {
        if (m_pListLink == NULL)
        {
            m_pListLink = ((CListCtrl*)(GetDlgItem(IDC_LIST_LINK)));
        }
        switch (lcstype)
        {
        case LCSTYPE_DETAIL:
            m_pListLink->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_ICON, LVS_REPORT, TRUE);
            break;
        case LCSTYPE_TILE:
            m_pListLink->ModifyStyle(LVS_SMALLICON | LVS_LIST | LVS_REPORT, LVS_ICON, TRUE);
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
    //遍历桌面文件夹第一层里的所有项    
    BOOL EnumAllItems(IEnumIDList* m_pEnum, BOOL isFolder, BOOL isQuickLanch)
    {
        LPITEMIDLIST m_pItem = NULL;
        ULONG   m_ulwork = 0;
        while (m_pEnum->Next(1, &m_pItem, &m_ulwork) == S_OK)
        {
            //如果是第一层，重置路径    
            if (!isQuickLanch)
            {
                if ((m_pFirstLayerFolder == m_pEnum) && (isFolder))
                {
                    lstrcpy(m_tParentPath, m_tDeskTopPath);
                }

                if ((m_pFirstLayerFile == m_pEnum) && (!isFolder))
                {
                    lstrcpy(m_tParentPath, m_tDeskTopPath);
                }
            }
            else
            {
                if ((m_pFirstLayerFile == m_pEnum) && (!isFolder))
                {
                    lstrcpy(m_tParentPath, m_tQuickLanchPath);
                }
            }

            WIN32_FIND_DATA ffd;
            SHGetDataFromIDList(m_pIShellFolder, m_pItem, SHGDFIL_FINDDATA, &ffd, sizeof(WIN32_FIND_DATA));
            if (!isFolder)
            {
                CString m_strTempPath = m_tParentPath;
                m_strTempPath += TEXT("\\");
                m_strTempPath += ffd.cFileName;
                CString m_strCmp = TEXT(".lnk");
                m_strTempPath.MakeUpper();
                m_strCmp.MakeUpper();
                if (m_strTempPath.Right(4) == m_strCmp)
                {
                    ReadShortCut((LPTSTR)(LPCTSTR)m_strTempPath);

                    SHFILEINFO sfi = { 0 };
                    SHGetFileInfo((LPTSTR)(LPCTSTR)m_strTempPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_DISPLAYNAME | SHGFI_TYPENAME);
                    m_NormalIconList.Add(sfi.hIcon);
                    SHGetFileInfo((LPTSTR)(LPCTSTR)m_strTempPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_DISPLAYNAME | SHGFI_TYPENAME);
                    m_SmallIconList.Add(sfi.hIcon);
                    m_pListLink->SetImageList(&m_NormalIconList, LVSIL_NORMAL);
                    m_pListLink->SetImageList(&m_SmallIconList, LVSIL_SMALL);
                    m_strLinkList.emplace_back(m_strLinkList.size(), (LPTSTR)(LPCTSTR)m_strTempPath, sfi.szDisplayName);
                    LV_ITEM lvi = { 0 };
                    memset(&lvi, 0, sizeof(lvi));
                    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
                    lvi.lParam = m_pListLink->GetItemCount();
                    lvi.iItem = m_nRow;
                    lvi.iSubItem = m_nCol;
                    {
                        /*CRect rect = {};
                        GetClientRect(rect);
                        if ((m_nRow + 1) * 32 >= rect.Width())
                        {
                            m_nCol++;
                        }
                        else*/
                        {
                            m_nRow++;
                        }
                    }
                    lvi.stateMask = 0;
                    lvi.iImage = m_NormalIconList.GetImageCount() - 1;          //显示不同的图标时，可以把一个int 变量赋给这个属性值
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
            }
            else
            {
                lstrcat(m_tParentPath, TEXT("\\"));
                lstrcat(m_tParentPath, ffd.cFileName);
                IShellFolder* m_pITemp = GetIShellFolderByPath(m_tParentPath);
                GetIEunmIDList(m_pITemp, TRUE, isQuickLanch);
            }
        }
        return TRUE;
    }

    //获取桌面文件夹和快速启动文件夹的路径    
    int GetPath(LPTSTR DeskTop, LPTSTR AppData)
    {
        CRegKey m_reg;
        if (ERROR_SUCCESS == m_reg.Open(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), KEY_READ))
        {
            ULONG m_ulsize1 = 1000;
            ULONG m_ulsize2 = 1000;
            m_reg.QueryStringValue(TEXT("Desktop"), DeskTop, &m_ulsize1);
            m_reg.QueryStringValue(TEXT("AppData"), AppData, &m_ulsize2);
            lstrcat(AppData, TEXT("\\Microsoft\\Internet Explorer\\Quick Launch"));
        }
        return 0;
    }

    //获取指定目录的IShellFolder接口    
    IShellFolder* GetIShellFolderByPath(LPTSTR path)
    {
        IShellFolder* m_ShellFolderTopMost = NULL;
        HRESULT hr = SHGetDesktopFolder(&m_ShellFolderTopMost);
        if (FAILED(hr))
        {
            return NULL;
        }
        IShellFolder* m_pFolder;
        LPITEMIDLIST pidlWorkDir = NULL;
        hr = m_ShellFolderTopMost->ParseDisplayName(NULL, NULL, path, NULL, &pidlWorkDir, NULL);
        if (FAILED(hr))
        {
            return   NULL;
        }
        hr = m_ShellFolderTopMost->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&m_pFolder);
        if (m_ShellFolderTopMost)m_ShellFolderTopMost->Release();
        return m_pFolder;
    }

    //读取快捷方式的信息    
    BOOL ReadShortCut(LPTSTR linkName)
    {
        IShellLink* m_pIShellLink = NULL;
        IPersistFile* m_pIPersistFile = NULL;
        HRESULT hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&m_pIShellLink);
        if (hr == S_OK)
        {
            hr = m_pIShellLink->QueryInterface(IID_IPersistFile, (void**)&m_pIPersistFile);
            if (hr == S_OK)
            {
                USES_CONVERSION;
                m_pIPersistFile->Load(T2COLE(linkName), STGM_READWRITE);
                TCHAR tstrPath[MAX_PATH] = { 0 };

                //读取快捷方式的目标路径    
                m_pIShellLink->GetPath(tstrPath, sizeof(tstrPath)/sizeof(*tstrPath), NULL, SLGP_UNCPRIORITY);
                /*CString temp = tstrPath;
                temp.MakeUpper();
                for (auto it : m_strLinkList)
                {
                    if (i == 5)
                    {
                        if (temp.Right(9).Compare(it.c_str()) == 0)
                        {
                            m_pIShellLink->SetArguments(TEXT(""));
                            m_pIPersistFile->Save(T2COLE(linkName), TRUE);
                        }
                    }
                    else
                    {
                        if (temp.Find(it.path.c_str()) != -1)
                        {
                            m_pIShellLink->SetArguments(TEXT(""));
                            m_pIPersistFile->Save(T2COLE(linkName), TRUE);
                        }
                    }
                }*/
            }
            else
            {
                if (m_pIShellLink) m_pIShellLink->Release();
                if (m_pIPersistFile) m_pIPersistFile->Release();
                return FALSE;
            }
        }
        else
        {
            if (m_pIShellLink) m_pIShellLink->Release();
            if (m_pIPersistFile) m_pIPersistFile->Release();
            return FALSE;
        }
        if (m_pIShellLink) m_pIShellLink->Release();
        if (m_pIPersistFile) m_pIPersistFile->Release();
        return TRUE;
    }
};
