#if !defined(__ENLARGE_FILE_DIALOG__)
#define __ENLARGE_FILE_DIALOG__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CEnlargeFileDialog
{
	TCHAR *m_pBufOld;
	DWORD m_dwMaxOld;
	CFileDialog *m_pFileDlg;
public:
	CEnlargeFileDialog(CFileDialog *pFileDlg,int nMaxFileNum=500)
	{
		m_pFileDlg=pFileDlg;
		if(m_pFileDlg&&nMaxFileNum>0)
		{
			m_pBufOld = m_pFileDlg->m_ofn.lpstrFile;   //����ɵ�ָ��
			m_dwMaxOld = m_pFileDlg->m_ofn.nMaxFile;    //����ɵ�����
			m_pFileDlg->m_ofn.lpstrFile = new TCHAR[ nMaxFileNum * MAX_PATH];
			ZeroMemory(m_pFileDlg->m_ofn.lpstrFile,sizeof(TCHAR) * nMaxFileNum * MAX_PATH);
			m_pFileDlg->m_ofn.nMaxFile = nMaxFileNum * MAX_PATH;    //�������,��MSDN����ΪfileDlg.m_ofn.lpstrFileָ��Ļ��������ַ���
		}
		else 
		{
			m_pBufOld=NULL;
			m_dwMaxOld=0;
		}
	}
	~CEnlargeFileDialog()
	{
		if(m_pFileDlg&&m_pBufOld)
		{
			delete [](m_pFileDlg->m_ofn.lpstrFile);        //������ڴ�
			m_pFileDlg->m_ofn.lpstrFile = m_pBufOld;        //��ԭ֮ǰ��ָ��,��Ϊû�������CFileDialogԴ�����,��������Ƿ�����,�����Ȼ�ԭ�ϰ�,
			m_pFileDlg->m_ofn.nMaxFile = m_dwMaxOld;        //��ԭ֮ǰ�������
		}
	}
};
#endif