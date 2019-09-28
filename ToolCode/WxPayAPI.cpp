#include "stdafx.h"
#include "WxPayAPI.h"
#include "Markup.h"
#include "MD5.h"
#include "ArrayList.h"
#include "HttpClient.h"
#include "SortFunc.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
/* ΢�Ź��ں���Ϣ����
* APPID����֧����APPID���������ã�
* MCHID���̻��ţ��������ã�
* KEY���̻�֧����Կ���ο������ʼ����ã��������ã�
* APPSECRET�������ʺ�secert����JSAPI֧����ʱ����Ҫ���ã�
*/
const char* CWxPayConfig::APPID		= "wx2e0dfd49d40a80db";
const char* CWxPayConfig::MCHID		= "1493128472";
const char* CWxPayConfig::KEY		= "JVAx4v73wnvt3kv3u3mtt8D8tqGH7NsY";
const char* CWxPayConfig::APPSECRET	= "51c56b886b5be869567dd389b3e5d1d6";

//=======��֤��·�����á�===================================== 
/* ֤��·��,ע��Ӧ����д����·�������˿��������ʱ��Ҫ��
*/
const char* CWxPayConfig::SSLCERT_PATH		= "cert/apiclient_cert.p12";
const char* CWxPayConfig::SSLCERT_PASSWORD	= "1233410002";

//=======��֧�����֪ͨurl��===================================== 
/* ֧�����֪ͨ�ص�url�������̻�����֧�����
*/
const char* CWxPayConfig::NOTIFY_URL	= "http://www.xerofox.com/PayNotify.aspx";

//=======���̻�ϵͳ��̨����IP��===================================== 
/* �˲������ֶ�����Ҳ���ڳ������Զ���ȡ
*/
const char* CWxPayConfig::IP = "8.8.8.8";


//=======��������������á�===================================
/* Ĭ��IP�Ͷ˿ںŷֱ�Ϊ0.0.0.0��0����ʱ����������������Ҫ�����ã�
*/
const char* CWxPayConfig::PROXY_URL	= "";

//////////////////////////////////////////////////////////////////////////
// CWxPayData
//////////////////////////////////////////////////////////////////////////
void CWxPayData::SetValue(const char* key,const char* data)
{
	m_keyList.AttachNode(CXhChar100(key));
	CVariant var;
	var.SetGridString(data);
    m_values.SetValue(key,var);
}

void CWxPayData::SetValue(const char* key, int data)
{
	m_keyList.AttachNode(CXhChar100(key));
	CVariant var;
	var.vType=GRID_VT_I4;
	var.lVal=data;
	m_values.SetValue(key,var);
}

void CWxPayData::SetValue(const char* key,CVariant *pVar)
{
	if(pVar==NULL)
		return;
	m_keyList.AttachNode(CXhChar100(key));
	m_values.SetValue(key,*pVar);
}

CVariant* CWxPayData::GetValue(const char* key)
{
    return m_values.GetValue(key);
}

CString CWxPayData::GetValueStr(const char* key)
{
	CVariant *pVar=GetValue(key);
	if(pVar==NULL)
		return CString();
	CString str;
	if(pVar->vType==GRID_VT_STRING)
		str.Format(pVar->sVal);
	else if(pVar->vType==GRID_VT_I4)
		str.Format("%d",pVar->lVal);
	return str;
}

bool CWxPayData::IsSet(const char* key)
{
    if(m_values.GetValue(key)==NULL)
		return false;
	else
		return true;
}

static int compare_keystr(const CXhChar100& item1,const CXhChar100& item2)
{
	CXhChar100 str1(item1),str2(item2);
	int len1=str1.GetLength();
	int len2=str2.GetLength();
	int i = 0;  
	while(i<len1&&i<len2)
	{
		if(str1.At(i)>str2.At(i))
			return 1;
		else if(str1.At(i)<str2.At(i))
			return -1;
		i++;
	}
	if(len1<len2)
		return -1;
	else if(len1>len2)
		return 1;
	else
		return 0;
}

CString CWxPayData::ToXml()
{
    CString sXML;
	//����Ϊ��ʱ����ת��Ϊxml��ʽ
    if (0 == m_values.GetNodeNum())
        return sXML;
	ARRAY_LIST<CXhChar100> keyArr;
	for(CXhChar100 *pKey=m_keyList.EnumObjectFirst();pKey;pKey=m_keyList.EnumObjectNext())
		keyArr.append(*pKey);
	CHeapSort<CXhChar100>::HeapSort(keyArr.m_pData,keyArr.Size(),compare_keystr);
	sXML.Append("<xml>");
	for(CXhChar100 *pKey=keyArr.GetFirst();pKey;pKey=keyArr.GetNext())
    {
        //�ֶ�ֵ����Ϊnull����Ӱ���������
		CVariant *pVar=m_values.GetValue(*pKey);
        if (pVar==NULL)
            return CString();
        if (pVar->vType == GRID_VT_I4)
			sXML.AppendFormat("<%s>%d</%s>",(char*)(*pKey),pVar->lVal,(char*)(*pKey));
        else if (pVar->vType==GRID_VT_STRING)
			sXML.AppendFormat("<%s><![CDATA[%s]]></%s>",(char*)(*pKey),pVar->sVal,(char*)(*pKey));
        else//����CString��int���Ͳ��ܺ���������������
            return CString();
    }
    sXML.Append("</xml>");
    return sXML;
}

bool CWxPayData::FromXml(CString xml)
{
    if (xml.GetLength()==0)
    {
        //Log.Error(this.GetType().ToString(), "���յ�xml��ת��ΪWxPayData���Ϸ�!");
        throw CString("���յ�xml��ת��ΪWxPayData���Ϸ�!");
		return false;
    }
	CMarkup xmlDoc(xml); 
    xmlDoc.ResetMainPos();
	CString sValue=_T("");
	while(xmlDoc.FindChildElem())   
	{
		xmlDoc.IntoElem();
		CString sTag=xmlDoc.GetTagName();
		CString sData=xmlDoc.GetData();
		SetValue(sTag,sData);
		xmlDoc.OutOfElem();
	}		
    try
    {
		//2015-06-29 ������û��ǩ��
		CVariant *pVar=m_values.GetValue("return_code");
		if(pVar==NULL || pVar->vType!=GRID_VT_STRING || strcmp(pVar->sVal,"SUCCESS")!=0)
		{
			return false;
		}
        CheckSign();//��֤ǩ��,��ͨ�������쳣
    }
    catch(CString sError)
    {
        throw sError;
    }
    return true;
}

CString CWxPayData::ToUrl(bool bIncSign)
{
    CString buff = "";
	if(m_keyList.Count<=0)
		return CString();
	ARRAY_LIST<CXhChar100> keyArr;
	for(CXhChar100 *pKey=m_keyList.EnumObjectFirst();pKey;pKey=m_keyList.EnumObjectNext())
		keyArr.append(*pKey);
	CHeapSort<CXhChar100>::HeapSort(keyArr.m_pData,keyArr.Size(),compare_keystr);
	for(CXhChar100 *pKey=keyArr.GetFirst();pKey;pKey=keyArr.GetNext())
	{	//�ֶ�ֵ����Ϊnull����Ӱ���������
		CVariant *pVar=m_values.GetValue(*pKey);
		if (pVar==NULL)
		{
			throw CString("WxPayData�ڲ�����ֵΪnull���ֶ�!");
			return CString();
		}
		if ((bIncSign||!pKey->EqualNoCase("sign")) && ((pVar->vType==GRID_VT_STRING && strlen(pVar->sVal)>0)||pVar->vType==GRID_VT_I4))
		{
			if(buff.GetLength()>0)
				buff.Append("&");
			if(pVar->vType==GRID_VT_STRING)
				buff.AppendFormat("%s=%s",(char*)(*pKey),(char*)pVar->sVal);
			else
				buff.AppendFormat("%s=%d",(char*)(*pKey),pVar->lVal);
		}
	}
    return buff;
}

CString CWxPayData::ToJson()
{
    CString jsonStr;// = JsonMapper.ToJson(m_values);
    return jsonStr;
}

CString CWxPayData::ToPrintStr()
{
    CString str = "";
	for(CXhChar100 *pKey=m_keyList.EnumObjectFirst();pKey;pKey=m_keyList.EnumObjectNext())
	{	//�ֶ�ֵ����Ϊnull����Ӱ���������
		CVariant *pVar=m_values.GetValue(*pKey);
		if (pVar==NULL)
		{
			throw CString("WxPayData�ڲ�����ֵΪnull���ֶ�!");
			return CString();
		}
		if(pVar->vType==GRID_VT_STRING)
			str.AppendFormat("%s=%s<br>",(char*)(*pKey),(char*)pVar->sVal);
		else
			str.AppendFormat("%s=%d<br>",(char*)(*pKey),pVar->lVal);
	}
    return str;
}
static BOOL StringToMemory(const char* str,BYTE* buf,UINT buf_length/*=16*/)
{
	UINT str_length=buf_length*2;
	if(strlen(str)<str_length)
	{
		memset(buf,0,buf_length);
		return FALSE;
	}
	for(UINT i=0;i<str_length;i+=2)
	{
		BYTE low=0,high=0;
		if(str[i]>='0'&&str[i]<='9')
			low=str[i]-'0';
		else if(str[i]>='A'&&str[i]<='F')
			low=str[i]-'A'+10;
		else if(str[i]>='a'&&str[i]<='f')
			low=str[i]-'a'+10;
		if(str[i+1]>='0'&&str[i+1]<='9')
			high=str[i+1]-'0';
		else if(str[i+1]>='A'&&str[i+1]<='F')
			high=str[i+1]-'A'+10;
		else if(str[i+1]>='a'&&str[i+1]<='f')
			high=str[i+1]-'a'+10;
		high<<=4;
		buf[i/2]=low+high;
	}
	return TRUE;
}
static BOOL MemoryToString(const BYTE* buf,char* str,UINT buf_length/*=16*/)
{
	UINT str_length=buf_length*2;
	for(UINT i=0;i<buf_length;i++)
	{
		BYTE low=buf[i]&0x0f;
		BYTE high=(buf[i]>>4)&0x0f;
		if(low<10)
			str[i*2]='0'+low;
		else
			str[i*2]='A'+low-10;
		if(high<10)
			str[i*2+1]='0'+high;
		else
			str[i*2+1]='A'+high-10;
	}
	return TRUE;
}

CString GBK2UTF8(const CString& strGBK)
{
	wchar_t* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, nullptr, 0);
	str1 = new wchar_t[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, nullptr, 0, nullptr, nullptr);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, nullptr, nullptr);
	CString strUTF8(str2);
	delete[]str1;
	delete[]str2;
	return strUTF8;
}

CString CWxPayData::MakeSign()
{
    //תurl��ʽ
    CString str = ToUrl(false);
    //��CString�����API KEY
    str.AppendFormat("&key=%s",CWxPayConfig::KEY);
	//����str������תΪUTF-8����
	CString ss=GBK2UTF8(str);
	//MD5����
	CString sMD5=MD5Str((BYTE*)ss.GetBuffer(),ss.GetLength());
	/*CString sSrc="appid=wx2428e34e0e7dc6ef&attach=test&body=TMA&goods_tag=test&key=e10adc3849ba56abbe56e056f20f883e&mch_id=1233410002&nonce_str=10B93C9C7871467F9A4BDDE164EFEEFB&notify_url=http://paysdk.weixin.qq.com/example/ResultNotifyPage.aspx&out_trade_no=2636002815101189630&product_id=123456789&spbill_create_ip=8.8.8.8&time_expire=20171108133923&time_start=20171108132923&total_fee=1&trade_type=NATIVE&key=e10adc3849ba56abbe56e056f20f883e";
	sMD5=MD5Str((BYTE*)sSrc.GetBuffer(),sSrc.GetLength());
	sSrc="appid=wx2428e34e0e7dc6ef&attach=test&body=TMA&goods_tag=test&key=e10adc3849ba56abbe56e056f20f883e&mch_id=1233410002&nonce_str=10B93C9C7871467F9A4BDDE164EFEEFB&notify_url=http://paysdk.weixin.qq.com/example/ResultNotifyPage.aspx&out_trade_no=2636002815101189630&product_id=123456789&spbill_create_ip=8.8.8.8&time_expire=20171108133923&time_start=20171108132923&total_fee=1&trade_type=NATIVE&key=e10adc3849ba56abbe56e056f20f883e";
	CString sM=MD5Str((BYTE*)sSrc.GetBuffer(),sSrc.GetLength());*/
	//�����ַ�תΪ��д
    return sMD5.MakeUpper();
}

bool CWxPayData::CheckSign()
{
    //���û������ǩ�������������
    if (!IsSet("sign"))
    {
        throw CString("WxPayDataǩ�����ڵ����Ϸ�!");
    }
	CVariant *pVar=GetValue("sign");
    //���������ǩ������ǩ��Ϊ�գ������쳣
    if(pVar==NULL||pVar->vType!=GRID_VT_STRING||strlen(pVar->sVal)==0)
    {
        throw CString("WxPayDataǩ�����ڵ����Ϸ�!");
    }

    //��ȡ���յ���ǩ��
    CString return_sign = pVar->sVal;
    //�ڱ��ؼ����µ�ǩ��
    CString cal_sign = MakeSign();
    if (cal_sign == return_sign)
    {
        return true;
    }
    throw CString("WxPayDataǩ����֤����!");
}

CHashStrList<CVariant>* CWxPayData::GetValues()
{
    return &m_values;
}

//////////////////////////////////////////////////////////////////////////
// CWxPayNotify
//////////////////////////////////////////////////////////////////////////
bool CWxPayNotify::GetNotifyData(CWxPayData &data)
{
	//���մ�΢�ź�̨POST����������
	/*System.IO.Stream s = page.Request.InputStream;
	int count = 0;
	byte[] buffer = new byte[1024];
	StringBuilder builder = new StringBuilder();
	while ((count = s.Read(buffer, 0, 1024)) > 0)
	{
		builder.Append(Encoding.UTF8.GetString(buffer, 0, count));
	}
	s.Flush();
	s.Close();
	s.Dispose();

	Log.Info(this.GetType().ToString(), "Receive data from WeChat : " + builder.ToString());

	//ת�����ݸ�ʽ����֤ǩ��
	WxPayData data = new WxPayData();
	try
	{
		data.FromXml(builder.ToString());
	}
	catch(WxPayException ex)
	{
		//��ǩ���������������ؽ����΢��֧����̨
		WxPayData res = new WxPayData();
		res.SetValue("return_code", "FAIL");
		res.SetValue("return_msg", ex.Message);
		Log.Error(this.GetType().ToString(), "Sign check error : " + res.ToXml());
		page.Response.Write(res.ToXml());
		page.Response.End();
	}

	Log.Info(this.GetType().ToString(), "Check sign success");*/
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CWxPayApi
//////////////////////////////////////////////////////////////////////////
bool CWxPayApi::Micropay(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 10*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/micropay";
    //���������
    if (!inputObj.IsSet("body"))
    {
        throw CString("�ύ��ɨ֧��API�ӿ��У�ȱ�ٱ������body��");
    }
    else if (!inputObj.IsSet("out_trade_no"))
    {
        throw CString("�ύ��ɨ֧��API�ӿ��У�ȱ�ٱ������out_trade_no��");
    }
    else if (!inputObj.IsSet("total_fee"))
    {
        throw CString("�ύ��ɨ֧��API�ӿ��У�ȱ�ٱ������total_fee��");
    }
    else if (!inputObj.IsSet("auth_code"))
    {
        throw CString("�ύ��ɨ֧��API�ӿ��У�ȱ�ٱ������auth_code��");
    }
       
    inputObj.SetValue("spbill_create_ip", CWxPayConfig::IP);//�ն�ip
    inputObj.SetValue("appid", CWxPayConfig::APPID);//�����˺�ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//�̻���
    inputObj.SetValue("nonce_str", GenerateNonceStr());//����ַ���
    inputObj.SetValue("sign", inputObj.MakeSign());//ǩ��
    CString xml = inputObj.ToXml();

	DWORD start=GetTickCount();	//����ʼʱ��

    //Log.Debug("WxPayApi", "MicroPay request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//����HTTPͨ�Žӿ����ύ���ݵ�API
    //Log.Debug("WxPayApi", "MicroPay response : " + response);

	DWORD end = GetTickCount();
    DWORD timeCost = end-start;//��ýӿں�ʱ

    //��xml��ʽ�Ľ��ת��Ϊ�����Է���
    outputObj.FromXml(response);
    ReportCostTime(url, timeCost, outputObj);//�����ϱ�

    return true;
}

        
/**
*    
* ��ѯ����
* @param WxPayData inputObj �ύ����ѯ����API�Ĳ���
* @param int timeOut ��ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���ض�����ѯ������������쳣
*/
bool CWxPayApi::OrderQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/orderquery";
    //���������
    if (!inputObj.IsSet("out_trade_no") && !inputObj.IsSet("transaction_id"))
    {
        throw CString("������ѯ�ӿ��У�out_trade_no��transaction_id������һ����");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//�����˺�ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//�̻���
    inputObj.SetValue("nonce_str", CWxPayApi::GenerateNonceStr());//����ַ���
    inputObj.SetValue("sign", inputObj.MakeSign());//ǩ��

    CString xml = inputObj.ToXml();

    DWORD start = GetTickCount();

    //Log.Debug("WxPayApi", "OrderQuery request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//����HTTPͨ�Žӿ��ύ����
    //Log.Debug("WxPayApi", "OrderQuery response : " + response);

	DWORD end = GetTickCount();
    int timeCost = end-start;//��ýӿں�ʱ

    //��xml��ʽ������ת��Ϊ�����Է���
    outputObj.FromXml(response);
    ReportCostTime(url, timeCost, outputObj);//�����ϱ�

    return true;
}


/**
* 
* ��������API�ӿ�
* @param WxPayData inputObj �ύ����������API�ӿڵĲ�����out_trade_no��transaction_id����һ��
* @param int timeOut �ӿڳ�ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ����API���ý�����������쳣
*/
bool CWxPayApi::Reverse(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/secapi/pay/reverse";
    //���������
    if (!inputObj.IsSet("out_trade_no") && !inputObj.IsSet("transaction_id"))
    {
        throw CString("��������API�ӿ��У�����out_trade_no��transaction_id������дһ����");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//�����˺�ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//�̻���
    inputObj.SetValue("nonce_str", GenerateNonceStr());//����ַ���
    inputObj.SetValue("sign", inputObj.MakeSign());//ǩ��
    CString xml = inputObj.ToXml();

    DWORD start = GetTickCount();//����ʼʱ��

    //Log.Debug("WxPayApi", "Reverse request : " + xml);

    CString response = CHttpClient::Post(xml, url, true, timeOut);

    //Log.Debug("WxPayApi", "Reverse response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;

    outputObj.FromXml(response);

    ReportCostTime(url, timeCost, outputObj);//�����ϱ�

    return true;
}


/**
* 
* �����˿�
* @param WxPayData inputObj �ύ�������˿�API�Ĳ���
* @param int timeOut ��ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���ؽӿڵ��ý�����������쳣
*/
bool CWxPayApi::Refund(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/secapi/pay/refund";
    //���������
    if (!inputObj.IsSet("out_trade_no") && !inputObj.IsSet("transaction_id"))
    {
        throw CString("�˿�����ӿ��У�out_trade_no��transaction_id������һ����");
    }
    else if (!inputObj.IsSet("out_refund_no"))
    {
        throw CString("�˿�����ӿ��У�ȱ�ٱ������out_refund_no��");
    }
    else if (!inputObj.IsSet("total_fee"))
    {
        throw CString("�˿�����ӿ��У�ȱ�ٱ������total_fee��");
    }
    else if (!inputObj.IsSet("refund_fee"))
    {
        throw CString("�˿�����ӿ��У�ȱ�ٱ������refund_fee��");
    }
    else if (!inputObj.IsSet("op_user_id"))
    {
        throw CString("�˿�����ӿ��У�ȱ�ٱ������op_user_id��");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//�����˺�ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//�̻���
    inputObj.SetValue("nonce_str", GenerateNonceStr());//����ַ���
    inputObj.SetValue("sign", inputObj.MakeSign());//ǩ��
            
    CString xml = inputObj.ToXml();
    DWORD start = GetTickCount();

    //Log.Debug("WxPayApi", "Refund request : " + xml);
    CString response = CHttpClient::Post(xml, url, true, timeOut);//����HTTPͨ�Žӿ��ύ���ݵ�API
    //Log.Debug("WxPayApi", "Refund response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;//��ýӿں�ʱ

    //��xml��ʽ�Ľ��ת��Ϊ�����Է���
    outputObj.FromXml(response);
    ReportCostTime(url, timeCost, outputObj);//�����ϱ�

    return true;
}


/**
* 
* ��ѯ�˿�
* �ύ�˿������ͨ���ýӿڲ�ѯ�˿�״̬���˿���һ����ʱ��
* ����Ǯ֧�����˿�20�����ڵ��ˣ����п�֧�����˿�3�������պ����²�ѯ�˿�״̬��
* out_refund_no��out_trade_no��transaction_id��refund_id�ĸ���������һ��
* @param WxPayData inputObj �ύ����ѯ�˿�API�Ĳ���
* @param int timeOut �ӿڳ�ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���أ��������쳣
*/
bool CWxPayApi::RefundQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
	CString url = "https://api.mch.weixin.qq.com/pay/refundquery";
	//���������
	if(!inputObj.IsSet("out_refund_no") && !inputObj.IsSet("out_trade_no") &&
		!inputObj.IsSet("transaction_id") && !inputObj.IsSet("refund_id"))
    {
		throw CString("�˿��ѯ�ӿ��У�out_refund_no��out_trade_no��transaction_id��refund_id�ĸ���������һ����");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//�����˺�ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//�̻���
	inputObj.SetValue("nonce_str",GenerateNonceStr());//����ַ���
	inputObj.SetValue("sign",inputObj.MakeSign());//ǩ��

	CString xml = inputObj.ToXml();
		
	DWORD start = GetTickCount();//����ʼʱ��

    //Log.Debug("WxPayApi", "RefundQuery request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//����HTTPͨ�Žӿ����ύ���ݵ�API
    //Log.Debug("WxPayApi", "RefundQuery response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;//��ýӿں�ʱ

    //��xml��ʽ�Ľ��ת��Ϊ�����Է���
    outputObj.FromXml(response);

	ReportCostTime(url, timeCost, outputObj);//�����ϱ�
		
	return true;
}


/**
* ���ض��˵�
* @param WxPayData inputObj �ύ�����ض��˵�API�Ĳ���
* @param int timeOut �ӿڳ�ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���أ��������쳣
*/
bool CWxPayApi::DownloadBill(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/downloadbill";
    //���������
    if (!inputObj.IsSet("bill_date"))
    {
        throw CString("���˵��ӿ��У�ȱ�ٱ������bill_date��");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//�����˺�ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//�̻���
    inputObj.SetValue("nonce_str", GenerateNonceStr());//����ַ���
    inputObj.SetValue("sign", inputObj.MakeSign());//ǩ��

    CString xml = inputObj.ToXml();

    //Log.Debug("WxPayApi", "DownloadBill request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//����HTTPͨ�Žӿ����ύ���ݵ�API
    //Log.Debug("WxPayApi", "DownloadBill result : " + response);

    //���ӿڵ���ʧ�ܻ᷵��xml��ʽ�Ľ��
	CString head=response.Mid(0,5);
    if (head.CompareNoCase("<xml>")==0)
    {
        outputObj.FromXml(response);
    }
    //�ӿڵ��óɹ��򷵻ط�xml��ʽ������
    else
        outputObj.SetValue("result", response);

    return true;
}


/**
* 
* ת��������
* �ýӿ���Ҫ����ɨ��ԭ��֧��ģʽһ�еĶ�ά������ת�ɶ�����(weixin://wxpay/s/XXXXXX)��
* ��С��ά��������������ɨ���ٶȺ;�ȷ�ȡ�
* @param WxPayData inputObj �ύ��ת��������API�Ĳ���
* @param int timeOut �ӿڳ�ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���أ��������쳣
*/
bool CWxPayApi::ShortUrl(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
	CString url = "https://api.mch.weixin.qq.com/tools/shorturl";
	//���������
	if(!inputObj.IsSet("long_url"))
    {
		throw CString("��Ҫת����URL��ǩ����ԭ����������URL encode��");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//�����˺�ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//�̻���
	inputObj.SetValue("nonce_str",GenerateNonceStr());//����ַ���	
	inputObj.SetValue("sign",inputObj.MakeSign());//ǩ��
	CString xml = inputObj.ToXml();
		
	DWORD start = GetTickCount();//����ʼʱ��

    //Log.Debug("WxPayApi", "ShortUrl request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);
    //Log.Debug("WxPayApi", "ShortUrl response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;

    outputObj.FromXml(response);
	ReportCostTime(url, timeCost, outputObj);//�����ϱ�
		
	return true;
}


/**
* 
* ͳһ�µ�
* @param WxPaydata inputObj �ύ��ͳһ�µ�API�Ĳ���
* @param int timeOut ��ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���أ��������쳣
*/
bool CWxPayApi::UnifiedOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/unifiedorder";
    //���������
    if (!inputObj.IsSet("out_trade_no"))
    {
        throw CString("ȱ��ͳһ֧���ӿڱ������out_trade_no��");
    }
    else if (!inputObj.IsSet("body"))
    {
        throw CString("ȱ��ͳһ֧���ӿڱ������body��");
    }
    else if (!inputObj.IsSet("total_fee"))
    {
        throw CString("ȱ��ͳһ֧���ӿڱ������total_fee��");
    }
    else if (!inputObj.IsSet("trade_type"))
    {
        throw CString("ȱ��ͳһ֧���ӿڱ������trade_type��");
    }

    //��������
    if (inputObj.GetValueStr("trade_type").CompareNoCase("JSAPI")==0 && !inputObj.IsSet("openid"))
    {
        throw CString("ͳһ֧���ӿ��У�ȱ�ٱ������openid��trade_typeΪJSAPIʱ��openidΪ���������");
    }
    if (inputObj.GetValueStr("trade_type").CompareNoCase("NATIVE")==0 && !inputObj.IsSet("product_id"))
    {
        throw CString("ͳһ֧���ӿ��У�ȱ�ٱ������product_id��trade_typeΪJSAPIʱ��product_idΪ���������");
    }
    //ǩ��
    inputObj.SetValue("sign", inputObj.MakeSign());
    CString xml = inputObj.ToXml();

    DWORD start = GetTickCount();

    //Log.Debug("WxPayApi", "UnfiedOrder request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);
    //Log.Debug("WxPayApi", "UnfiedOrder response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;
    outputObj.FromXml(response);

    ReportCostTime(url, timeCost, outputObj);//�����ϱ�

    return true;
}

 
/**
* 
* �رն���
* @param WxPayData inputObj �ύ���رն���API�Ĳ���
* @param int timeOut �ӿڳ�ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���أ��������쳣
*/
bool CWxPayApi::CloseOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
	CString url = "https://api.mch.weixin.qq.com/pay/closeorder";
	//���������
	if(!inputObj.IsSet("out_trade_no"))
    {
		throw CString("�رն����ӿ��У�out_trade_no���");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//�����˺�ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//�̻���
	inputObj.SetValue("nonce_str",GenerateNonceStr());//����ַ���		
	inputObj.SetValue("sign",inputObj.MakeSign());//ǩ��
	CString xml = inputObj.ToXml();
		
	DWORD start = GetTickCount();//����ʼʱ��

    CString response = CHttpClient::Post(xml, url, false, timeOut);

    DWORD end = GetTickCount();
    int timeCost = end-start;
    outputObj.FromXml(response);

	ReportCostTime(url, timeCost, outputObj);//�����ϱ�
		
	return true;
}


/**
* 
* �����ϱ�
* @param CString interface_url �ӿ�URL
* @param int timeCost �ӿں�ʱ
* @param WxPayData inputObj��������
*/
void CWxPayApi::ReportCostTime(CString interface_url, int timeCost, CWxPayData &inputObj)
{
	//�������
	if(inputObj.IsSet("err_code")&&inputObj.IsSet("err_code_des"))
	{
		CString ss=inputObj.GetValueStr("err_code");
		ss+=inputObj.GetValueStr("err_code_des");
		AfxMessageBox(ss);
		return;
	}
	//�������Ҫ�����ϱ�
	if(CWxPayConfig::REPORT_LEVENL == 0)
    {
		return;
	} 

	//�����ʧ���ϱ�
	if(CWxPayConfig::REPORT_LEVENL == 1 && inputObj.IsSet("return_code") && inputObj.GetValueStr("return_code").CompareNoCase("SUCCESS")==0 &&
		inputObj.IsSet("result_code") && inputObj.GetValueStr("result_code").CompareNoCase("SUCCESS")==0)
    {
		return;
	}
		 
	//�ϱ��߼�
	CWxPayData data;
    data.SetValue("interface_url",interface_url);
	data.SetValue("execute_time_",timeCost);
	//����״̬��
	if(inputObj.IsSet("return_code"))
    {
		data.SetValue("return_code",inputObj.GetValue("return_code"));
	}
	//������Ϣ
    if(inputObj.IsSet("return_msg"))
    {
		data.SetValue("return_msg",inputObj.GetValue("return_msg"));
	}
	//ҵ����
    if(inputObj.IsSet("result_code"))
    {
		data.SetValue("result_code",inputObj.GetValue("result_code"));
	}
	//�������
    if(inputObj.IsSet("err_code"))
    {
		data.SetValue("err_code",inputObj.GetValue("err_code"));
	}
	//�����������
    if(inputObj.IsSet("err_code_des"))
    {
		data.SetValue("err_code_des",inputObj.GetValue("err_code_des"));
	}
	//�̻�������
    if(inputObj.IsSet("out_trade_no"))
    {
		data.SetValue("out_trade_no",inputObj.GetValue("out_trade_no"));
	}
	//�豸��
    if(inputObj.IsSet("device_info"))
    {
		data.SetValue("device_info",inputObj.GetValue("device_info"));
	}
		
	try
    {
		CWxPayData outputData;
		Report(data,outputData);
	}
    catch (CString ex)
    {	//�����κδ���
		CString sMessage=inputObj.GetValueStr("return_msg");
		if(sMessage.GetLength()>0)
		{
			CString ss=sMessage+ex;
			AfxMessageBox(ss);
		}
		else
			AfxMessageBox(ex);
	}
}


/**
* 
* �����ϱ��ӿ�ʵ��
* @param WxPayData inputObj �ύ�������ϱ��ӿڵĲ���
* @param int timeOut �����ϱ��ӿڳ�ʱʱ��
* @throws WxPayException
* @return �ɹ�ʱ���ز����ϱ��ӿڷ��صĽ�����������쳣
*/
bool CWxPayApi::Report(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 1*/)
{
	CString url = "https://api.mch.weixin.qq.com/payitil/report";
	//���������
	if(!inputObj.IsSet("interface_url"))
    {
		throw CString("�ӿ�URL��ȱ�ٱ������interface_url��");
	} 
    if(!inputObj.IsSet("return_code"))
    {
		throw CString("����״̬�룬ȱ�ٱ������return_code��");
	} 
    if(!inputObj.IsSet("result_code"))
    {
		throw CString("ҵ������ȱ�ٱ������result_code��");
	} 
    if(!inputObj.IsSet("user_ip"))
    {
		throw CString("���ʽӿ�IP��ȱ�ٱ������user_ip��");
	} 
    if(!inputObj.IsSet("execute_time_"))
    {
		throw CString("�ӿں�ʱ��ȱ�ٱ������execute_time_��");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//�����˺�ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//�̻���
	inputObj.SetValue("user_ip",CWxPayConfig::IP);//�ն�ip
	CTime t=CTime::GetCurrentTime();
	inputObj.SetValue("time",t.Format("%Y%m%d%H%M%S"));//�̻��ϱ�ʱ��	 
	inputObj.SetValue("nonce_str",GenerateNonceStr());//����ַ���
	inputObj.SetValue("sign",inputObj.MakeSign());//ǩ��
	CString xml = inputObj.ToXml();

    //Log.Info("WxPayApi", "Report request : " + xml);

    CString response = CHttpClient::Post(xml, url, false, timeOut);

    //Log.Info("WxPayApi", "Report response : " + response);
    outputObj.FromXml(response);
	return true;
}

/**
* ���ݵ�ǰϵͳʱ���������������ɶ�����
* @return ������
*/
CString CWxPayApi::GenerateOutTradeNo(const char* hardId,const char* applyNonceCode)
{
	CString str;
	//str.Format("%d%d%d",CWxPayConfig::MCHID,time(NULL),rand());
	str.Format("%s%s",hardId,applyNonceCode);
	return MD5Str((BYTE*)str.GetBuffer(),str.GetLength());
}

/**
* ����ʱ�������׼����ʱ�䣬ʱ��Ϊ����������1970��1��1�� 0��0��0������������
* @return ʱ���
*/
CString CWxPayApi::GenerateTimeStamp()
{
	DWORD timestamp=(DWORD)time(NULL);
	CString str;
	str.Format("%ld",timestamp);
	return str;
}

/**
* ����������������������ĸ������
* @return �����
*/
CString CWxPayApi::GenerateNonceStr()
{
	GUID guid;
	CoCreateGuid(&guid);
	CString strGuid;
	strGuid.Format("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",guid.Data1,guid.Data2 ,guid.Data3 ,guid.Data4[0],
		guid.Data4[1],guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
	return strGuid;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
/**
* ����ɨ��֧��ģʽһURL
* @param productId ��ƷID
* @return ģʽһURL
*/
CString CNativePay::GetPrePayUrl(const char* productId)
{
    CWxPayData data;
    data.SetValue("appid", CWxPayConfig::APPID);//�����ʺ�id
    data.SetValue("mch_id", CWxPayConfig::MCHID);//�̻���
    data.SetValue("nonce_str", CWxPayApi::GenerateNonceStr());//����ַ���
    data.SetValue("product_id", productId);//��ƷID
	data.SetValue("time_stamp", CWxPayApi::GenerateTimeStamp());//ʱ���
    data.SetValue("sign", data.MakeSign());//ǩ��
    CString url = "weixin://wxpay/bizpayurl?" + data.ToUrl(true);
    return url;
}

/**
* ����ֱ��֧��url��֧��url��Ч��Ϊ2Сʱ,ģʽ��
* @param productId ��ƷID
* @return ģʽ��URL
*/
CString CNativePay::GetPayUrl(int money,const char* productId,const char* nonce_str,const char* out_trade_no,
							  const char* description/*=NULL*/,const char* attachData/*=NULL*/,const char* tag/*=NULL*/)
{
    CWxPayData data;
    data.SetValue("appid", CWxPayConfig::APPID);//�����˺�ID
	data.SetValue("attach", attachData!=NULL?attachData:"");//��������CString(127)
	data.SetValue("body", description!=NULL?description:"");//��Ʒ����CString(128)
    data.SetValue("goods_tag", tag!=NULL?tag:"");//��Ʒ���
	data.SetValue("mch_id", CWxPayConfig::MCHID);//�̻���
	//�첽֪ͨurlδ���ã���ʹ�������ļ��е�url
	if (!data.IsSet("notify_url"))
	{
		data.SetValue("notify_url", CWxPayConfig::NOTIFY_URL);//�첽֪ͨurl
	}
	data.SetValue("out_trade_no", out_trade_no);//����ַ���	CString(32)
	data.SetValue("product_id", productId);		//��ƷID	CString(32)
	data.SetValue("nonce_str", nonce_str);		//����ַ���	CString(32)
    data.SetValue("spbill_create_ip", CWxPayConfig::IP);//�ն�ip
	CTime startTime=CTime::GetCurrentTime();
	CTime endTim=startTime+CTimeSpan(0,0,10,0);
	data.SetValue("time_expire", endTim.Format("%Y%m%d%H%M%S"));//���׽���ʱ��
	data.SetValue("time_start", startTime.Format("%Y%m%d%H%M%S"));//������ʼʱ��
	data.SetValue("total_fee", money);//�ܽ��
	data.SetValue("trade_type", "NATIVE");//��������
    
	CWxPayData result;
    CWxPayApi::UnifiedOrder(data,result);//����ͳһ�µ��ӿ�
    CString url = result.GetValueStr("code_url");//���ͳһ�µ��ӿڷ��صĶ�ά������
    return url;
}