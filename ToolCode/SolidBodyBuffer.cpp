#include "stdafx.h"
#include ".\solidbodybuffer.h"
#include ".\SolidBody.h"

#ifndef _DISABLE_DEBUG_NEW_	//TIDCORE_EXPORTS
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

const short CSolidBodyBuffer::RAWFACE_INDEXDATA = 1;
CSolidBodyBuffer::CSolidBodyBuffer(int nBlockSize/* = 1024*/)
{
	buffer = NULL;
	buffer_len = file_len = 0;
	log_mem_position = mem_cursor = 0;
	m_nBlockSize = nBlockSize;
	m_bExternalBuf=false;
}
CSolidBodyBuffer::CSolidBodyBuffer(char* srcBuf, DWORD buf_size)
{
	buffer = srcBuf;
	buffer_len = file_len = buf_size;
	log_mem_position = mem_cursor = 0;
	m_nBlockSize = buf_size;
	m_bExternalBuf=true;
}

CSolidBodyBuffer::~CSolidBodyBuffer(void)
{
}
bool CSolidBodyBuffer::AllocateMemory(DWORD buf_size)
{
	char* data=buf_size>0 ? new char[buf_size] : NULL;
	if(data==NULL)	//�ڴ����ʧ��
		return false;
	file_len=min(buf_size,file_len);
	if(file_len>0&&data!=NULL)
		memcpy(data,buffer,min(buf_size,file_len));
	if(!m_bExternalBuf&&file_len>0&&data!=NULL)
		delete []buffer;
	buffer=data;
	buffer_len=buf_size;
	mem_cursor=0;
	return true;
}
bool CSolidBodyBuffer::SeekToSection(short section)
{
	if(section==RAWFACE_INDEXDATA)
	{
		if(!SeekPosition(45))
		{
			SeekToEnd();
			Write(NULL,45-file_len);
		}
		return true;
	}
	return false;
}

WORD CSolidBodyBuffer::GetPolyFaceBuffLength(int i)
{
	SeekPosition(RawFaceIndexStartAddr+4*i);
	DWORD offset;
	ReadDword(&offset);
	SeekPosition(offset);
	WORD facebuf_len=0;
	ReadWord(&facebuf_len);
	return facebuf_len;
}
char* CSolidBodyBuffer::GetPolyFaceBuffer(int i)
{
	SeekPosition(RawFaceIndexStartAddr+4*i);
	DWORD offset;
	ReadDword(&offset);
	SeekPosition(offset+2);
	return GetCursorBuffer();
}
WORD CSolidBodyBuffer::GetRawEdgeBuffLength(int i)
{
	SeekPosition(EdgeIndexStartAddr+4*i);
	DWORD offset;
	ReadDword(&offset);
	SeekPosition(offset);
	WORD edgebuf_len=0;
	ReadWord(&edgebuf_len);
	return edgebuf_len;
}
char* CSolidBodyBuffer::GetRawEdgeBuffer(int i)
{
	SeekPosition(EdgeIndexStartAddr+4*i);
	DWORD offset;
	ReadDword(&offset);
	SeekPosition(offset+2);
	return GetCursorBuffer();
}
WORD CSolidBodyBuffer::GetBasicFaceBuffLength(int i)
{
	SeekPosition(BasicFaceIndexStartAddr+4*i);
	DWORD offset;
	ReadDword(&offset);
	SeekPosition(offset);
	WORD facebuf_len=0;
	ReadWord(&facebuf_len);
	return facebuf_len;
}
char* CSolidBodyBuffer::GetBasicFaceBuffer(int i)
{
	SeekPosition(BasicFaceIndexStartAddr+4*i);
	DWORD offset;
	ReadDword(&offset);
	SeekPosition(offset+2);
	return GetCursorBuffer();
}

bool CSolidBodyBuffer::MergeBodyBuffer(char *pExterBuffer, DWORD size,CSolidBodyBuffer* pTurboBuf/*=NULL*/)
{
	CSolidBodyBuffer extersolid(pExterBuffer,size);
	if(m_bExternalBuf)
		return false;	//��ǰ�����е��ڴ�����������ģ��ⲿ���õĻ����޷����䷵��false
	if(GetLength()==0)
	{	//ԭʼ����Ϊ��ʱ��ֱ�ӽ��п���
		if(buffer&&!m_bExternalBuf)
			delete []buffer;
		if(size>0)
			buffer=new char[size];
		buffer_len = file_len = size;
		log_mem_position = mem_cursor = 0;
		if(size>0)
			memcpy(buffer,pExterBuffer,size);
		return true;
	}
	CSolidBodyBuffer legacy;
	if(pTurboBuf==NULL)
		pTurboBuf=&legacy;
	pTurboBuf->ClearContents();
	pTurboBuf->Write(buffer,buffer_len);	//���ݳ�ʼʵ�����ݻ���
	VertexNumber+=extersolid.VertexNumber;
	EdgeNumber+=extersolid.EdgeNumber;
	RawFaceNumber+=extersolid.RawFaceNumber;
	BasicFaceNumber+=extersolid.BasicFaceNumber;
	if(VertexRecordLength==0)
		VertexRecordLength=25;	//Ĭ��25Byte
	if(VertexRecordLength!=extersolid.VertexRecordLength)
		return false;
	DWORD position;
	//�ϲ�����������
	RawFaceIndexStartAddr=45;
	SeekToSection(RAWFACE_INDEXDATA);
	SeekPosition(45);
	pTurboBuf->SeekPosition(45);
	Write(pTurboBuf->GetCursorBuffer(),pTurboBuf->RawFaceNumber*4);
	extersolid.SeekPosition(45);
	Write(extersolid.GetCursorBuffer(),extersolid.RawFaceNumber*4);
	//�ϲ�����������
	position=GetCursorPosition();	//ԭʼ��������������ַƫ����
	EdgeIndexStartAddr=position;
	pTurboBuf->SeekPosition(pTurboBuf->EdgeIndexStartAddr);
	WriteAt(position,pTurboBuf->GetCursorBuffer(),pTurboBuf->EdgeNumber*4);
	extersolid.SeekPosition(extersolid.EdgeIndexStartAddr);
	Write(extersolid.GetCursorBuffer(),extersolid.EdgeNumber*4);
	//�ϲ�ʵ����ʾ������Ƭ����������
	position=GetCursorPosition();	//ԭʼ��������������ַƫ����
	BasicFaceIndexStartAddr=position;
	pTurboBuf->SeekPosition(pTurboBuf->BasicFaceIndexStartAddr);
	WriteAt(position,pTurboBuf->GetCursorBuffer(),pTurboBuf->BasicFaceNumber*4);
	extersolid.SeekPosition(extersolid.BasicFaceIndexStartAddr);
	Write(extersolid.GetCursorBuffer(),extersolid.BasicFaceNumber*4);
	//�ϲ�����������
	position=GetCursorPosition();
	VertexDataStartAddr=position;
	pTurboBuf->SeekPosition(pTurboBuf->VertexDataStartAddr);
	WriteAt(position,pTurboBuf->GetCursorBuffer(),pTurboBuf->VertexNumber*VertexRecordLength);
	extersolid.SeekPosition(extersolid.VertexDataStartAddr);
	Write(extersolid.GetCursorBuffer(),extersolid.VertexNumber*VertexRecordLength);
	//�ϲ�ԭʼ������������
	position=GetCursorPosition();
	RawFaceDataStartAddr=position;
	SeekPosition(position);
	DWORD iFace;
	pTurboBuf->SeekPosition(pTurboBuf->RawFaceDataStartAddr);
	for(iFace=0;iFace<pTurboBuf->RawFaceNumber;iFace++)
	{
		WORD facebuf_len=pTurboBuf->GetPolyFaceBuffLength(iFace);
		WriteWord(facebuf_len);
		Write(pTurboBuf->GetPolyFaceBuffer(iFace),facebuf_len);
	}
	for(iFace=0;iFace<extersolid.RawFaceNumber;iFace++)
	{
		WORD facebuf_len=extersolid.GetPolyFaceBuffLength(iFace);
		WriteWord(facebuf_len);
		Write(extersolid.GetPolyFaceBuffer(iFace),facebuf_len);
	}
	//�ϲ�ԭʼ�����������
	position=GetCursorPosition();
	EdgeDataStartAddr=position;
	SeekPosition(position);
	pTurboBuf->SeekPosition(pTurboBuf->EdgeDataStartAddr);
	DWORD iEdge;
	for(iEdge=0;iEdge<pTurboBuf->EdgeNumber;iEdge++)
	{
		WORD edgebuf_len=pTurboBuf->GetRawEdgeBuffLength(iEdge);
		WriteWord(edgebuf_len);
		Write(pTurboBuf->GetRawEdgeBuffer(iEdge),edgebuf_len);
	}
	for(iEdge=0;iEdge<extersolid.EdgeNumber;iEdge++)
	{
		WORD edgebuf_len=extersolid.GetRawEdgeBuffLength(iEdge);
		WriteWord(edgebuf_len);
		Write(extersolid.GetRawEdgeBuffer(iEdge),edgebuf_len);
	}
	//�ϲ�ʵ����ʾ������Ƭ��������
	position=GetCursorPosition();
	BasicFaceDataStartAddr=position;
	SeekPosition(position);
	pTurboBuf->SeekPosition(pTurboBuf->BasicFaceDataStartAddr);
	for(iFace=0;iFace<pTurboBuf->BasicFaceNumber;iFace++)
	{
		WORD facebuf_len=pTurboBuf->GetBasicFaceBuffLength(iFace);
		WriteWord(facebuf_len);
		Write(pTurboBuf->GetBasicFaceBuffer(iFace),facebuf_len);
	}
	for(iFace=0;iFace<extersolid.BasicFaceNumber;iFace++)
	{
		WORD facebuf_len=extersolid.GetBasicFaceBuffLength(iFace);
		WriteWord(facebuf_len);
		Write(extersolid.GetBasicFaceBuffer(iFace),facebuf_len);
	}
	//��������������
	//���±���������
	//����ʵ����ʾ������Ƭ����������
	//����ԭʼ�����������
	SeekPosition(EdgeDataStartAddr);
	for(iEdge=0;iEdge<EdgeNumber;iEdge++)
	{
		DWORD position=GetCursorPosition();
			//1.���±�������ַ
		memcpy(buffer+EdgeIndexStartAddr+iEdge*4,&position,4);
			//2.���±�ʼ�ն˶����ʶ������
		WORD edge_len=0;
		ReadWord(&edge_len);
		CRawSolidEdge edge;
		edge.InitBuffer(GetCursorBuffer(),edge_len,true);
		if(iEdge>=pTurboBuf->EdgeNumber)
		{	//�ⲿ����ıߣ�ʼ�ն˶����Ӧ����ԭʼʵ�嶥������
			edge.LineStartId+=pTurboBuf->VertexNumber;
			edge.LineEndId+=pTurboBuf->VertexNumber;
		}
		Offset(edge_len);
	}
	//����ԭʼ������������
	SeekPosition(RawFaceDataStartAddr);
	for(iFace=0;iFace<RawFaceNumber;iFace++)
	{
		DWORD position=GetCursorPosition();
			//1.���±�������ַ
		memcpy(buffer+RawFaceIndexStartAddr+iFace*4,&position,4);
			//2.���±�ʼ�ն˶����ʶ������
		WORD face_len=0;
		ReadWord(&face_len);
		CRawSolidFace face;
		face.InitBuffer(GetCursorBuffer(),face_len);
		if(iFace>=pTurboBuf->RawFaceNumber)
		{	//�ⲿ������棬���¸����ڵ���ɱ߱�ʶ�����ţ�����ԭʼʵ���������
			if(face.BasicFaceId>0)
				face.BasicFaceId+=pTurboBuf->BasicFaceNumber;
			CFaceLoop outerloop=face.GetOutterLoop();
			for(iEdge=0;iEdge<outerloop.LoopEdgeLineNum();iEdge++)
				*((DWORD*)(outerloop.GetBuffer()+2+iEdge*4))+=pTurboBuf->EdgeNumber;
			for(WORD iLoop=0;iLoop<face.InnerLoopNum();iLoop++)
			{
				CFaceLoop innerloop=face.GetInnerLoopAt(iLoop);
				for(iEdge=0;iEdge<innerloop.LoopEdgeLineNum();iEdge++)
					*((DWORD*)(innerloop.GetBuffer()+2+iEdge*4))+=pTurboBuf->EdgeNumber;
			}
		}
		Offset(face_len);
	}
	//����ʵ����ʾ������Ƭ��������
	return true;
}
