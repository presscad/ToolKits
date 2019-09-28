/* soapTMSServiceSoap12Proxy.h
   Generated by gSOAP 2.8.19 from d:\Structure\!ClientServTemp\ClientServer.h

Copyright(C) 2000-2014, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapTMSServiceSoap12Proxy_H
#define soapTMSServiceSoap12Proxy_H
#include "soapH.h"

class SOAP_CMAC TMSServiceSoap12Proxy : public soap
{ public:
	/// Endpoint URL of service 'TMSServiceSoap12Proxy' (change as needed)
	const char *soap_endpoint;
	/// Variables globally declared in d:\Structure\!ClientServTemp\ClientServer.h (non-static)
	/// Constructor
	TMSServiceSoap12Proxy();
	/// Construct from another engine state
	TMSServiceSoap12Proxy(const struct soap&);
	/// Constructor with endpoint URL
	TMSServiceSoap12Proxy(const char *url);
	/// Constructor with engine input+output mode control
	TMSServiceSoap12Proxy(soap_mode iomode);
	/// Constructor with URL and input+output mode control
	TMSServiceSoap12Proxy(const char *url, soap_mode iomode);
	/// Constructor with engine input and output mode control
	TMSServiceSoap12Proxy(soap_mode imode, soap_mode omode);
	/// Destructor frees deserialized data
	virtual	~TMSServiceSoap12Proxy();
	/// Initializer used by constructors
	virtual	void TMSServiceSoap12Proxy_init(soap_mode imode, soap_mode omode);
	/// Delete all deserialized data (with soap_destroy and soap_end)
	virtual	void destroy();
	/// Delete all deserialized data and reset to default
	virtual	void reset();
	/// Disables and removes SOAP Header from message
	virtual	void soap_noheader();
	/// Get SOAP Header structure (NULL when absent)
	virtual	const SOAP_ENV__Header *soap_header();
	/// Get SOAP Fault structure (NULL when absent)
	virtual	const SOAP_ENV__Fault *soap_fault();
	/// Get SOAP Fault string (NULL when absent)
	virtual	const char *soap_fault_string();
	/// Get SOAP Fault detail as string (NULL when absent)
	virtual	const char *soap_fault_detail();
	/// Close connection (normally automatic, except for send_X ops)
	virtual	int soap_close_socket();
	/// Force close connection (can kill a thread blocked on IO)
	virtual	int soap_force_close_socket();
	/// Print fault
	virtual	void soap_print_fault(FILE*);
#ifndef WITH_LEAN
	/// Print fault to stream
#ifndef WITH_COMPAT
	virtual	void soap_stream_fault(std::ostream&);
#endif

	/// Put fault into buffer
	virtual	char *soap_sprint_fault(char *buf, size_t len);
#endif

	/// Web service operation 'loginUser' (returns error code or SOAP_OK)
	virtual	int loginUser(_TMSServ__loginUser *TMSServ__loginUser, _TMSServ__loginUserResponse &TMSServ__loginUserResponse) { return this->loginUser(NULL, NULL, TMSServ__loginUser, TMSServ__loginUserResponse); }
	virtual	int loginUser(const char *endpoint, const char *soap_action, _TMSServ__loginUser *TMSServ__loginUser, _TMSServ__loginUserResponse &TMSServ__loginUserResponse);

	/// Web service operation 'loginUserByByteArr' (returns error code or SOAP_OK)
	virtual	int loginUserByByteArr(_TMSServ__loginUserByByteArr *TMSServ__loginUserByByteArr, _TMSServ__loginUserByByteArrResponse &TMSServ__loginUserByByteArrResponse) { return this->loginUserByByteArr(NULL, NULL, TMSServ__loginUserByByteArr, TMSServ__loginUserByByteArrResponse); }
	virtual	int loginUserByByteArr(const char *endpoint, const char *soap_action, _TMSServ__loginUserByByteArr *TMSServ__loginUserByByteArr, _TMSServ__loginUserByByteArrResponse &TMSServ__loginUserByByteArrResponse);

	/// Web service operation 'logoutUser' (returns error code or SOAP_OK)
	virtual	int logoutUser(_TMSServ__logoutUser *TMSServ__logoutUser, _TMSServ__logoutUserResponse &TMSServ__logoutUserResponse) { return this->logoutUser(NULL, NULL, TMSServ__logoutUser, TMSServ__logoutUserResponse); }
	virtual	int logoutUser(const char *endpoint, const char *soap_action, _TMSServ__logoutUser *TMSServ__logoutUser, _TMSServ__logoutUserResponse &TMSServ__logoutUserResponse);

	/// Web service operation 'QueryObjects' (returns error code or SOAP_OK)
	virtual	int QueryObjects(_TMSServ__QueryObjects *TMSServ__QueryObjects, _TMSServ__QueryObjectsResponse &TMSServ__QueryObjectsResponse) { return this->QueryObjects(NULL, NULL, TMSServ__QueryObjects, TMSServ__QueryObjectsResponse); }
	virtual	int QueryObjects(const char *endpoint, const char *soap_action, _TMSServ__QueryObjects *TMSServ__QueryObjects, _TMSServ__QueryObjectsResponse &TMSServ__QueryObjectsResponse);

	/// Web service operation 'DownloadObjects' (returns error code or SOAP_OK)
	virtual	int DownloadObjects(_TMSServ__DownloadObjects *TMSServ__DownloadObjects, _TMSServ__DownloadObjectsResponse &TMSServ__DownloadObjectsResponse) { return this->DownloadObjects(NULL, NULL, TMSServ__DownloadObjects, TMSServ__DownloadObjectsResponse); }
	virtual	int DownloadObjects(const char *endpoint, const char *soap_action, _TMSServ__DownloadObjects *TMSServ__DownloadObjects, _TMSServ__DownloadObjectsResponse &TMSServ__DownloadObjectsResponse);

	/// Web service operation 'GetObjectsVersion' (returns error code or SOAP_OK)
	virtual	int GetObjectsVersion(_TMSServ__GetObjectsVersion *TMSServ__GetObjectsVersion, _TMSServ__GetObjectsVersionResponse &TMSServ__GetObjectsVersionResponse) { return this->GetObjectsVersion(NULL, NULL, TMSServ__GetObjectsVersion, TMSServ__GetObjectsVersionResponse); }
	virtual	int GetObjectsVersion(const char *endpoint, const char *soap_action, _TMSServ__GetObjectsVersion *TMSServ__GetObjectsVersion, _TMSServ__GetObjectsVersionResponse &TMSServ__GetObjectsVersionResponse);

	/// Web service operation 'GetTowerPartsLabelAndVersionSerial' (returns error code or SOAP_OK)
	virtual	int GetTowerPartsLabelAndVersionSerial(_TMSServ__GetTowerPartsLabelAndVersionSerial *TMSServ__GetTowerPartsLabelAndVersionSerial, _TMSServ__GetTowerPartsLabelAndVersionSerialResponse &TMSServ__GetTowerPartsLabelAndVersionSerialResponse) { return this->GetTowerPartsLabelAndVersionSerial(NULL, NULL, TMSServ__GetTowerPartsLabelAndVersionSerial, TMSServ__GetTowerPartsLabelAndVersionSerialResponse); }
	virtual	int GetTowerPartsLabelAndVersionSerial(const char *endpoint, const char *soap_action, _TMSServ__GetTowerPartsLabelAndVersionSerial *TMSServ__GetTowerPartsLabelAndVersionSerial, _TMSServ__GetTowerPartsLabelAndVersionSerialResponse &TMSServ__GetTowerPartsLabelAndVersionSerialResponse);

	/// Web service operation 'MatchTowerParts' (returns error code or SOAP_OK)
	virtual	int MatchTowerParts(_TMSServ__MatchTowerParts *TMSServ__MatchTowerParts, _TMSServ__MatchTowerPartsResponse &TMSServ__MatchTowerPartsResponse) { return this->MatchTowerParts(NULL, NULL, TMSServ__MatchTowerParts, TMSServ__MatchTowerPartsResponse); }
	virtual	int MatchTowerParts(const char *endpoint, const char *soap_action, _TMSServ__MatchTowerParts *TMSServ__MatchTowerParts, _TMSServ__MatchTowerPartsResponse &TMSServ__MatchTowerPartsResponse);

	/// Web service operation 'SaveOrUpdateObjects' (returns error code or SOAP_OK)
	virtual	int SaveOrUpdateObjects(_TMSServ__SaveOrUpdateObjects *TMSServ__SaveOrUpdateObjects, _TMSServ__SaveOrUpdateObjectsResponse &TMSServ__SaveOrUpdateObjectsResponse) { return this->SaveOrUpdateObjects(NULL, NULL, TMSServ__SaveOrUpdateObjects, TMSServ__SaveOrUpdateObjectsResponse); }
	virtual	int SaveOrUpdateObjects(const char *endpoint, const char *soap_action, _TMSServ__SaveOrUpdateObjects *TMSServ__SaveOrUpdateObjects, _TMSServ__SaveOrUpdateObjectsResponse &TMSServ__SaveOrUpdateObjectsResponse);

	/// Web service operation 'DeleteObject' (returns error code or SOAP_OK)
	virtual	int DeleteObject(_TMSServ__DeleteObject *TMSServ__DeleteObject, _TMSServ__DeleteObjectResponse &TMSServ__DeleteObjectResponse) { return this->DeleteObject(NULL, NULL, TMSServ__DeleteObject, TMSServ__DeleteObjectResponse); }
	virtual	int DeleteObject(const char *endpoint, const char *soap_action, _TMSServ__DeleteObject *TMSServ__DeleteObject, _TMSServ__DeleteObjectResponse &TMSServ__DeleteObjectResponse);

	/// Web service operation 'DownloadFileById' (returns error code or SOAP_OK)
	virtual	int DownloadFileById(_TMSServ__DownloadFileById *TMSServ__DownloadFileById, _TMSServ__DownloadFileByIdResponse &TMSServ__DownloadFileByIdResponse) { return this->DownloadFileById(NULL, NULL, TMSServ__DownloadFileById, TMSServ__DownloadFileByIdResponse); }
	virtual	int DownloadFileById(const char *endpoint, const char *soap_action, _TMSServ__DownloadFileById *TMSServ__DownloadFileById, _TMSServ__DownloadFileByIdResponse &TMSServ__DownloadFileByIdResponse);

	/// Web service operation 'UploadFileObject' (returns error code or SOAP_OK)
	virtual	int UploadFileObject(_TMSServ__UploadFileObject *TMSServ__UploadFileObject, _TMSServ__UploadFileObjectResponse &TMSServ__UploadFileObjectResponse) { return this->UploadFileObject(NULL, NULL, TMSServ__UploadFileObject, TMSServ__UploadFileObjectResponse); }
	virtual	int UploadFileObject(const char *endpoint, const char *soap_action, _TMSServ__UploadFileObject *TMSServ__UploadFileObject, _TMSServ__UploadFileObjectResponse &TMSServ__UploadFileObjectResponse);

	/// Web service operation 'FinalizeFileObjectData' (returns error code or SOAP_OK)
	virtual	int FinalizeFileObjectData(_TMSServ__FinalizeFileObjectData *TMSServ__FinalizeFileObjectData, _TMSServ__FinalizeFileObjectDataResponse &TMSServ__FinalizeFileObjectDataResponse) { return this->FinalizeFileObjectData(NULL, NULL, TMSServ__FinalizeFileObjectData, TMSServ__FinalizeFileObjectDataResponse); }
	virtual	int FinalizeFileObjectData(const char *endpoint, const char *soap_action, _TMSServ__FinalizeFileObjectData *TMSServ__FinalizeFileObjectData, _TMSServ__FinalizeFileObjectDataResponse &TMSServ__FinalizeFileObjectDataResponse);

	/// Web service operation 'OpenFileObjectDataProvider' (returns error code or SOAP_OK)
	virtual	int OpenFileObjectDataProvider(_TMSServ__OpenFileObjectDataProvider *TMSServ__OpenFileObjectDataProvider, _TMSServ__OpenFileObjectDataProviderResponse &TMSServ__OpenFileObjectDataProviderResponse) { return this->OpenFileObjectDataProvider(NULL, NULL, TMSServ__OpenFileObjectDataProvider, TMSServ__OpenFileObjectDataProviderResponse); }
	virtual	int OpenFileObjectDataProvider(const char *endpoint, const char *soap_action, _TMSServ__OpenFileObjectDataProvider *TMSServ__OpenFileObjectDataProvider, _TMSServ__OpenFileObjectDataProviderResponse &TMSServ__OpenFileObjectDataProviderResponse);

	/// Web service operation 'OpenTapModelDataProvider' (returns error code or SOAP_OK)
	virtual	int OpenTapModelDataProvider(_TMSServ__OpenTapModelDataProvider *TMSServ__OpenTapModelDataProvider, _TMSServ__OpenTapModelDataProviderResponse &TMSServ__OpenTapModelDataProviderResponse) { return this->OpenTapModelDataProvider(NULL, NULL, TMSServ__OpenTapModelDataProvider, TMSServ__OpenTapModelDataProviderResponse); }
	virtual	int OpenTapModelDataProvider(const char *endpoint, const char *soap_action, _TMSServ__OpenTapModelDataProvider *TMSServ__OpenTapModelDataProvider, _TMSServ__OpenTapModelDataProviderResponse &TMSServ__OpenTapModelDataProviderResponse);

	/// Web service operation 'OpenServerObjectDataProvider' (returns error code or SOAP_OK)
	virtual	int OpenServerObjectDataProvider(_TMSServ__OpenServerObjectDataProvider *TMSServ__OpenServerObjectDataProvider, _TMSServ__OpenServerObjectDataProviderResponse &TMSServ__OpenServerObjectDataProviderResponse) { return this->OpenServerObjectDataProvider(NULL, NULL, TMSServ__OpenServerObjectDataProvider, TMSServ__OpenServerObjectDataProviderResponse); }
	virtual	int OpenServerObjectDataProvider(const char *endpoint, const char *soap_action, _TMSServ__OpenServerObjectDataProvider *TMSServ__OpenServerObjectDataProvider, _TMSServ__OpenServerObjectDataProviderResponse &TMSServ__OpenServerObjectDataProviderResponse);

	/// Web service operation 'DownloadFileObject' (returns error code or SOAP_OK)
	virtual	int DownloadFileObject(_TMSServ__DownloadFileObject *TMSServ__DownloadFileObject, _TMSServ__DownloadFileObjectResponse &TMSServ__DownloadFileObjectResponse) { return this->DownloadFileObject(NULL, NULL, TMSServ__DownloadFileObject, TMSServ__DownloadFileObjectResponse); }
	virtual	int DownloadFileObject(const char *endpoint, const char *soap_action, _TMSServ__DownloadFileObject *TMSServ__DownloadFileObject, _TMSServ__DownloadFileObjectResponse &TMSServ__DownloadFileObjectResponse);

	/// Web service operation 'CloseFileObjectDataProvider' (returns error code or SOAP_OK)
	virtual	int CloseFileObjectDataProvider(_TMSServ__CloseFileObjectDataProvider *TMSServ__CloseFileObjectDataProvider, _TMSServ__CloseFileObjectDataProviderResponse &TMSServ__CloseFileObjectDataProviderResponse) { return this->CloseFileObjectDataProvider(NULL, NULL, TMSServ__CloseFileObjectDataProvider, TMSServ__CloseFileObjectDataProviderResponse); }
	virtual	int CloseFileObjectDataProvider(const char *endpoint, const char *soap_action, _TMSServ__CloseFileObjectDataProvider *TMSServ__CloseFileObjectDataProvider, _TMSServ__CloseFileObjectDataProviderResponse &TMSServ__CloseFileObjectDataProviderResponse);
};
#endif
