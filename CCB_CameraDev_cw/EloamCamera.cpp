#include"EloamCamera.h"
BOOL EloamCamera::m_hFirstOpen =FALSE;
int EloamCamera::m_hDevState = 0;
EloamCamera::EloamCamera()
{
	m_hImage1 = NULL;
	m_hImage2 = NULL;
	m_hScan1 = TRUE;
	m_hScan2 = TRUE;

	m_EventTask1 = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_EventTask2 = CreateEvent(NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection(&cs);
}

EloamCamera::~EloamCamera()
{
	DeleteCriticalSection(&cs);
	CloseHandle(m_EventTask1);
	CloseHandle(m_EventTask2);
}

//�����豸�Ļص�����
void EloamCamera::devCallback(int type, int idx, int dbt, void *param)
{
	LogOut.StatusOut(Info,_T("(%s) ����:type=%d,idx=%d,dbt=%d; %s\r\n"),__FUNCTION__,type,idx,dbt,_T("Start..."));
	EloamCamera *m_this = (EloamCamera *)param;
	if (EL_DEV_TYPE_VIDEO != type)
	{//������Ƶ�豸
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s %s\r\n"),__FUNCTION__,_T("������Ƶ�豸"),_T("End."));
		return;
	}	
	if(1 == dbt)
	{//�豸����
		m_hDevState = 1;
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("�豸����"));
		char* name = new char[256];
		//��ȡ�������豸����
		elGetDisplayName(type,idx,name);
		string str(name);				
		if (str.find("6684") == -1 && str.find("6685") == -1)
		{//�����ж�,ֻҪ6685��6684
			LogOut.StatusOut(Info,_T("(%s) ˵��:%s %s\r\n"),__FUNCTION__,_T("����6685��6684�豸"),_T("End."));
			return;
		}
		int *devSub=NULL;
		if( str.find("6684") != -1)
		{
			devSub=&(m_this->devSub1);
		}
		else if(str.find("6685") != -1)
		{
			devSub=&(m_this->devSub2);
		}
		//������Ƶ�豸
		ELdevice dev = elCreateDevice(type,idx);
		int subType  = elGetSubtype(dev);
		if(1 == subType)
		{
			*devSub = EL_DEV_SUBTYPE_YUY2;
		}
		else if(2 == subType)
		{
			*devSub = EL_DEV_SUBTYPE_MJPG;
		}
		else if(3 == subType)
		{
			*devSub = EL_DEV_SUBTYPE_YUY2;
		}
		m_this->m_hDevList.push_back(dev);
		delete name;
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("����豸���б�"));			
	}		
	else if(2 == dbt)
	{//�豸��ʧ
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("�豸��ʧ"));
		if(idx>0)
		{				
			elStopPreview(m_this->m_hDevList[0]);
			elReleaseDevice(m_this->m_hDevList[0]);
			m_this->m_hDevList[0] = NULL;
			m_this->m_hDevList.erase(m_this->m_hDevList.begin());
			if(0 == m_this->m_hDevList.size())
			{
				elDeinitDevs();
			}
			LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("�ͷ��豸"));
		}
	}
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
}

void  EloamCamera::viewCallback1(ELdevice dev, ELimage image, int frame, void *param)
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	EloamCamera *m_this = (EloamCamera *)param;
	if(m_this->m_hScan1 == TRUE)
	{
		EnterCriticalSection(&m_this->cs);
		if(m_this->m_hImage1)
		{
			elReleaseImage(m_this->m_hImage1);
			m_this->m_hImage1 = NULL;
		}
		m_this->m_hImage1 = elCloneImage(image);
		m_this->m_hScan1=FALSE;
		LeaveCriticalSection(&m_this->cs);
		SetEvent(m_this->m_EventTask1);
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("����ͼ��1�ɹ�"));
	}
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
}

void  EloamCamera::viewCallback2(ELdevice dev, ELimage image, int frame, void *param)
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	EloamCamera *m_this = (EloamCamera *)param;
	
	if(m_this->m_hScan2 == TRUE)
	{
		EnterCriticalSection(&m_this->cs);
		if(m_this->m_hImage2)
		{
			elReleaseImage(m_this->m_hImage2);
			m_this->m_hImage2 = NULL;
		}
		m_this->m_hImage2 = elCloneImage(image);
		m_this->m_hScan2=FALSE;
		LeaveCriticalSection(&m_this->cs);
		SetEvent(m_this->m_EventTask2);
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("����ͼ��2�ɹ�"));
	}
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
}

//���̱���
char* EloamCamera::GetCameraCode()
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	char* ch = "13";
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return ch;
}

int EloamCamera::OpenCamera()
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	int ret = 0;
	if(elInitDevs(devCallback, this) != 0)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s %s\r\n"),__FUNCTION__,_T("�豸��ʼ��ʧ��"),_T("End."));
		return -1;
	}
	if(0 == m_hDevState)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s %s\r\n"),__FUNCTION__,_T("�豸δ����"),_T("End."));
		return -1;
	}
	if(m_hDevList.size()<2)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s %s\r\n"),__FUNCTION__,_T("ͼ���豸����С��2"),_T("End."));
 		return -1;
	}
	for(int i =0;i<m_hDevList.size();i++)
	{
		int type = elGetDevEloamType(m_hDevList[i]);
		LogOut.StatusOut(Info,_T("(%s) EloamType:%d\r\n"),__FUNCTION__,type);		
		if(1 == type)
		{//��ɫ����ͷ
			LogOut.StatusOut(Info,_T("(%s) subtype:%d\r\n"),__FUNCTION__,devSub1);
			if(0 != elStartPreview(m_hDevList[i], viewCallback1, this, NULL, NULL,6, devSub1))
			{
				LogOut.StatusOut(Info,_T("(%s) ˵��:%d\r\n"),__FUNCTION__,_T("����Ƶһʧ��"));
			}
		}		
		else if(2 == type||3 == type)
		{//�ڰ�
			LogOut.StatusOut(Info,_T("(%s) subtype:%d\r\n"),__FUNCTION__,devSub2);
			if(0 != elStartPreview(m_hDevList[i], viewCallback2, this, NULL, NULL,4, EL_DEV_SUBTYPE_MJPG))
			{
				LogOut.StatusOut(Info,_T("(%s) ˵��:%d\r\n"),__FUNCTION__,_T("����Ƶ��ʧ��"));
			}
		}
	}
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return 2;
}

int EloamCamera::GetCameraRatio(int* w,int* h)
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	if(m_hDevList.size()<2)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("ͼ���豸����С��2"));
		return 0;
	}
	if(!m_hDevList[0] || !m_hDevList[1])
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("�豸��������"));
		return 0;
	}
	
	*h  = elGetResolutionHeightEx(m_hDevList[0],devSub1,6);
	*w = elGetResolutionWidthEx(m_hDevList[0],devSub1,6);
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return 0;
}

int EloamCamera::GetCameraFrame(BYTE* pBuf_Color,BYTE* pBuf_BW)
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	if(m_hDevList.size()<2)
		return -1;
	if(!m_hDevList[0] || !m_hDevList[1])
		return -1;
	this->m_hScan1 = TRUE;
	this->m_hScan2 = TRUE;
	if(WAIT_OBJECT_0  == WaitForSingleObject(m_EventTask1, 10*1000) && WAIT_OBJECT_0  == WaitForSingleObject(m_EventTask2, 10*1000))
	{
		EnterCriticalSection(&this->cs);
		if(this->m_hImage1 && this->m_hImage2)
		{
			if(this->m_hImage1)
			{
				int widthStep = elGetImageProperty(this->m_hImage1,EL_PROP_WIDTHSTEP);
				int width = elGetImageProperty(this->m_hImage1, EL_PROP_WIDTH);
				int height = elGetImageProperty(this->m_hImage1, EL_PROP_HEIGHT);
				int channel = elGetImageProperty(this->m_hImage1, EL_PROP_CHANNELS);

				int lenData = widthStep * height;
				LogOut.StatusOut(Info,_T("(%s) width:%d,height:%d,widthStep:%d,channel:%d\r\n"),__FUNCTION__,width,height,widthStep,channel);
				int len = 0;
				if (1 == channel)
				{
					len =  256 * sizeof(RGBQUAD) + lenData;
				}
				else if (3 == channel)
				{
					len =  lenData;
				}
				if(NULL != pBuf_Color)
				{
					memcpy(pBuf_Color,(unsigned char*)elGetImageData(this->m_hImage1),len);
				}
				elReleaseImage(this->m_hImage1);
				this->m_hImage1 = NULL;
				LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("��ȡͼ��һ���ݳɹ�"));
			}
			if(this->m_hImage2)
			{
				int widthStep = elGetImageProperty(this->m_hImage2,EL_PROP_WIDTHSTEP);
				int width = elGetImageProperty(this->m_hImage2, EL_PROP_WIDTH);
				int height = elGetImageProperty(this->m_hImage2, EL_PROP_HEIGHT);
				int channel = elGetImageProperty(this->m_hImage2, EL_PROP_CHANNELS);
				LogOut.StatusOut(Info,_T("(%s) width:%d,height:%d,widthStep:%d,channel:%d\r\n"),__FUNCTION__,width,height,widthStep,channel);
				int lenData = widthStep * height;

				int len = 0;
				if (1 == channel)
				{
					len =  256 * sizeof(RGBQUAD) + lenData;
				}
				else if (3 == channel)
				{
					len = lenData;
				}
				if(NULL != pBuf_BW)
				{
					memcpy(pBuf_BW,(unsigned char*)elGetImageData(this->m_hImage2),len);
				}
				elReleaseImage(this->m_hImage2);
				this->m_hImage2 = NULL;		
				LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("��ȡͼ������ݳɹ�"));
			}
			LeaveCriticalSection(&this->cs);
			return 0;
		}
		LeaveCriticalSection(&this->cs);
	}
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return -1;
}

void EloamCamera::CameraClose()
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	if(m_hDevList.size()<2)
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("ͼ���豸����С��2"));
		return;
	}
	if(!m_hDevList[0] || !m_hDevList[1])
	{
		LogOut.StatusOut(Info,_T("(%s) ˵��:%s\r\n"),__FUNCTION__,_T("�豸��������"));
		return;
	}
	for(int i =0 ;i < (int)m_hDevList.size();i++)
	{
		elStopPreview(m_hDevList[i]);
		elReleaseDevice(m_hDevList[i]);
		m_hDevList[i] = NULL;
	}
	if(0 != elDeinitDevs())
	{
		LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ʼ��ʧ��"));
	}
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
}