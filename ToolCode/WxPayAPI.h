#pragma once

#include "stdafx.h"
#include "WxPayAPI.h"
#include "list.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "Variant.h"

/**
* 	�����˺���Ϣ
*/
class CWxPayConfig
{
public:
    //=======��������Ϣ���á�=====================================
    /* ΢�Ź��ں���Ϣ����
    * APPID����֧����APPID���������ã�
    * MCHID���̻��ţ��������ã�
    * KEY���̻�֧����Կ���ο������ʼ����ã��������ã�
    * APPSECRET�������ʺ�secert����JSAPI֧����ʱ����Ҫ���ã�
    */
    static const char* APPID;//     = "wx2e0dfd49d40a80db";
    static const char* MCHID;//     = "1493128472";
    static const char* KEY;//		= "JVAx4v73wnvt3kv3u3mtt8D8tqGH7NsY";
    static const char* APPSECRET;// = "51c56b886b5be869567dd389b3e5d1d6";

    //=======��֤��·�����á�===================================== 
    /* ֤��·��,ע��Ӧ����д����·�������˿��������ʱ��Ҫ��
    */
    static const char* SSLCERT_PATH;//		= "cert/apiclient_cert.p12";
    static const char* SSLCERT_PASSWORD;//	= "1233410002";

    //=======��֧�����֪ͨurl��===================================== 
    /* ֧�����֪ͨ�ص�url�������̻�����֧�����
    */
    static const char* NOTIFY_URL;

    //=======���̻�ϵͳ��̨����IP��===================================== 
    /* �˲������ֶ�����Ҳ���ڳ������Զ���ȡ
    */
    static const char* IP;// = "8.8.8.8";


    //=======��������������á�===================================
    /* Ĭ��IP�Ͷ˿ںŷֱ�Ϊ0.0.0.0��0����ʱ����������������Ҫ�����ã�
    */
    static const char* PROXY_URL;	//

    //=======���ϱ���Ϣ���á�===================================
    /* �����ϱ��ȼ���0.�ر��ϱ�; 1.������ʱ�ϱ�; 2.ȫ���ϱ�
    */
    static const int REPORT_LEVENL = 1;

    //=======����־����===================================
    /* ��־�ȼ���0.�������־��1.ֻ���������Ϣ; 2.��������������Ϣ; 3.���������Ϣ��������Ϣ�͵�����Ϣ
    */
    static const int LOG_LEVENL = 0;
};

/// <summary>
/// ΢��֧��Э��ӿ������࣬���е�API�ӿ�ͨ�Ŷ�����������ݽṹ��
/// �ڵ��ýӿ�֮ǰ���������ֶε�ֵ��Ȼ����нӿ�ͨ�ţ�
/// ������Ƶĺô��ǿ���չ��ǿ���û��������Э����и��Ķ���������������ݽṹ��
/// ������������ϳ���ͬ��Э�����ݰ�������Ϊÿ��Э�����һ�����ݰ��ṹ
/// </summary>
class CWxPayData
{
private:
    //���������Dictionary�ĺô��Ƿ�������ݰ�����ǩ����������ǩ��֮ǰ����һ������
	CXhSimpleList<CXhChar100> m_keyList;	//���������key
	CHashStrList<CVariant> m_values;
public:
    /**
    * ����ĳ���ֶε�ֵ
    * @param key �ֶ���
    * @param value �ֶ�ֵ
    */
    void SetValue(const char* key, const char* data);
	void SetValue(const char* key, int data);
	void SetValue(const char* key,CVariant *pVar);

    /**
    * �����ֶ�����ȡĳ���ֶε�ֵ
    * @param key �ֶ���
    * @return key��Ӧ���ֶ�ֵ
    */
    CVariant* GetValue(const char* key);
	CString GetValueStr(const char* key);
    /**
    * �ж�ĳ���ֶ��Ƿ�������
    * @param key �ֶ���
    * @return ���ֶ�key�ѱ����ã��򷵻�true�����򷵻�false
    */
    bool IsSet(const char* key);

    /**
    * @��Dictionaryת��xml
    * @return ��ת���õ���xml��
    * @throws WxPayException
    **/
    CString ToXml();
    
	/**
    * @��xmlתΪWxPayData���󲢷��ض����ڲ�������
    * @param string ��ת����xml��
    * @return ��ת���õ���Dictionary
    * @throws WxPayException
    */
    bool FromXml(CString xml);
    
	/**
    * @Dictionary��ʽת����url������ʽ
    * @ return url��ʽ��, �ô�������sign�ֶ�ֵ
    */
    CString ToUrl(bool bIncSign);
    
    /**
    * @Dictionary��ʽ����Json
    * @return json������
    */
    CString ToJson();
    
    /**
    * @values��ʽ��������Webҳ������ʾ�Ľ������Ϊwebҳ���ϲ���ֱ�����xml��ʽ���ַ�����
    */
    CString ToPrintStr();
    
    /**
    * @����ǩ�������ǩ�������㷨
    * @return ǩ��, sign�ֶβ��μ�ǩ��
    */
    CString MakeSign();
    
    /**
    * 
    * ���ǩ���Ƿ���ȷ
    * ��ȷ����true���������쳣
    */
    bool CheckSign();

    /**
    * @��ȡDictionary
    */
    CHashStrList<CVariant>* GetValues();
};

/// <summary>
/// �ص��������
/// ��Ҫ�������΢��֧����̨���͹��������ݣ������ݽ���ǩ����֤
/// �����ڴ�������Ͻ�����������д�Լ��Ļص��������
/// </summary>
class CWxPayNotify
{
public:
	CWxPayNotify(){;}
	/// <summary>
	/// ���մ�΢��֧����̨���͹��������ݲ���֤ǩ��
	/// </summary>
	/// <returns>΢��֧����̨���ص�����</returns>
	bool GetNotifyData(CWxPayData &data);
	//��������Ҫ��д������������в�ͬ�Ļص�����
	virtual void ProcessNotify(){;}
};

class CWxPayApi
{
public:
	/**
    * �ύ��ɨ֧��API
    * ����Աʹ��ɨ���豸��ȡ΢���û�ˢ����Ȩ���Ժ󣬶�ά���������Ϣ�������̻�����̨��
    * ���̻�����̨�����̻���̨���øýӿڷ���֧����
    * @param WxPayData inputObj �ύ����ɨ֧��API�Ĳ���
    * @param int timeOut ��ʱʱ��
    * @throws WxPayException
    * @return �ɹ�ʱ���ص��ý�����������쳣
    */
    static bool Micropay(CWxPayData &inputObj, CWxPayData &outputObj,int timeOut = 10);
        
    /**
    *    
    * ��ѯ����
    * @param WxPayData inputObj �ύ����ѯ����API�Ĳ���
    * @param int timeOut ��ʱʱ��
    * @throws WxPayException
    * @return �ɹ�ʱ���ض�����ѯ������������쳣
    */
    static bool OrderQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);

    /**
    * 
    * ��������API�ӿ�
    * @param WxPayData inputObj �ύ����������API�ӿڵĲ�����out_trade_no��transaction_id����һ��
    * @param int timeOut �ӿڳ�ʱʱ��
    * @throws WxPayException
    * @return �ɹ�ʱ����API���ý�����������쳣
    */
    static bool Reverse(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);

    /**
    * 
    * �����˿�
    * @param WxPayData inputObj �ύ�������˿�API�Ĳ���
    * @param int timeOut ��ʱʱ��
    * @throws WxPayException
    * @return �ɹ�ʱ���ؽӿڵ��ý�����������쳣
    */
    static bool Refund(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
    
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
	static bool RefundQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
	
    /**
    * ���ض��˵�
    * @param WxPayData inputObj �ύ�����ض��˵�API�Ĳ���
    * @param int timeOut �ӿڳ�ʱʱ��
    * @throws WxPayException
    * @return �ɹ�ʱ���أ��������쳣
    */
	static bool DownloadBill(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
    
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
	static bool ShortUrl(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
	
    /**
    * 
    * ͳһ�µ�
    * @param WxPaydata inputObj �ύ��ͳһ�µ�API�Ĳ���
    * @param int timeOut ��ʱʱ��
    * @throws WxPayException
    * @return �ɹ�ʱ���أ��������쳣
    */
    static bool UnifiedOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
	
    /**
	* 
	* �رն���
	* @param WxPayData inputObj �ύ���رն���API�Ĳ���
	* @param int timeOut �ӿڳ�ʱʱ��
	* @throws WxPayException
	* @return �ɹ�ʱ���أ��������쳣
	*/
	static bool CloseOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);

    /**
	* 
	* �����ϱ�
	* @param string interface_url �ӿ�URL
	* @param int timeCost �ӿں�ʱ
	* @param WxPayData inputObj��������
	*/
    static void ReportCostTime(CString interface_url, int timeCost, CWxPayData &inputObj);


    /**
	* 
	* �����ϱ��ӿ�ʵ��
	* @param WxPayData inputObj �ύ�������ϱ��ӿڵĲ���
	* @param int timeOut �����ϱ��ӿڳ�ʱʱ��
	* @throws WxPayException
	* @return �ɹ�ʱ���ز����ϱ��ӿڷ��صĽ�����������쳣
	*/
	static bool Report(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 1);

    /**
    * ���ݵ�ǰϵͳʱ���������������ɶ�����
    * @return ������
    */
    static CString GenerateOutTradeNo(const char* hardId,const char* applyNonceCode);
    
    /**
    * ����ʱ�������׼����ʱ�䣬ʱ��Ϊ����������1970��1��1�� 0��0��0������������
        * @return ʱ���
    */
    static CString GenerateTimeStamp();
    /**
    * ����������������������ĸ������
    * @return �����
    */
    static CString GenerateNonceStr();
};

//
class CNativePay
{
public:
    /**
    * ����ɨ��֧��ģʽһURL
    * @param productId ��ƷID
    * @return ģʽһURL
    */
    static CString GetPrePayUrl(const char* productId);
    /**
    * ����ֱ��֧��url��֧��url��Ч��Ϊ2Сʱ,ģʽ��
    * @param productId ��ƷID
    * @return ģʽ��URL
    */
	static CString GetPayUrl(int money,const char* productId,const char* nonce_str,const char* out_trade_no,
							 const char* description,const char* attachData,const char* tag);
};