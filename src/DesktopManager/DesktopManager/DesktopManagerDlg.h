
// DesktopManagerDlg.h : header file
//

#pragma once

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
    /*virtual void PostNcDestroy() {
        delete this;
        CDialogEx::PostNcDestroy();
    }*/
    virtual LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNMDblclkListLink(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
	DECLARE_MESSAGE_MAP()
public:
    void ResizeWindow(INT cx = 0, INT cy = 0)
    {
        if (m_pListLink != NULL)
        {
            CRect rect = { 0, 0, cx, cy };
            CRect rectOk = {};
            CRect rectCancel = {};
            if (rect.Width() <= 0)
            {
                GetClientRect(rect);
            }
            GetDlgItem(IDOK)->GetClientRect(rectOk);
            GetDlgItem(IDCANCEL)->GetClientRect(rectCancel);
            GetDlgItem(IDOK)->MoveWindow(CRect(CPoint(rect.Width() - rectOk.Width() - rectCancel.Width(), 1), CSize(rectOk.Width(), rectOk.Height())), FALSE);
            GetDlgItem(IDCANCEL)->MoveWindow(CRect(CPoint(rect.Width() - rectCancel.Width(), 1), CSize(rectCancel.Width(), rectCancel.Height())), FALSE);
            GetDlgItem(IDC_EDIT_NAME)->MoveWindow(CRect(CPoint(1, 1), CSize(rect.Width() - rectOk.Width() - rectCancel.Width(), rectOk.Height())), FALSE);
            GetDlgItem(IDC_LIST_LINK)->MoveWindow(CRect(CPoint(1, rectOk.Height()), CSize(rect.Width() - 2, rect.Height() - rectOk.Height() - 2)), FALSE);
            m_nRow = 0;
            m_nCol = 0;
            m_pListLink->DeleteAllItems();
            for (size_t i = 0; i < theApp.m_LinkDataList.size(); i++)
            {
                if ((m_listDataType == LDTYPE_OTHERS
                    && theApp.m_LinkDataList.at(i).type_name.compare(theApp.m_LinkDataType.at(LDTYPE_SHORTCUT)) != 0
                    && theApp.m_LinkDataList.at(i).type_name.compare(theApp.m_LinkDataType.at(LDTYPE_FOLDER)) != 0)
                    || (theApp.m_LinkDataList.at(i).type_name.compare(theApp.m_LinkDataType.at(m_listDataType)) == 0))
                {
                    LV_ITEM lvi = { 0 };
                    memset(&lvi, 0, sizeof(lvi));
                    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
                    lvi.lParam = MAKELPARAM(i, 0);
                    lvi.iItem = m_nRow++;
                    lvi.iSubItem = m_nCol;
                    lvi.stateMask = 0;
                    lvi.iImage = i;          //显示不同的图标时，可以把一个int 变量赋给这个属性值
                    lvi.pszText = (LPTSTR)theApp.m_LinkDataList.at(lvi.iImage).name.c_str();
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
            Invalidate();
        }
    }
private:
    INT m_nRow = 0;
    INT m_nCol = 0;
    BOOL m_bClosed = FALSE;
    CListCtrl* m_pListLink = NULL;
    ListDataType m_listDataType = LDTYPE_NULLPTR;
public:
    void SetListDataType(ListDataType listDataType)
    {
        m_listDataType = listDataType;
    }
    BOOL GetClosed()
    {
        return m_bClosed;
    }
};
