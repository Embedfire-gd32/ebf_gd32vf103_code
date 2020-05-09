//#include "sys.h"

#include "bsp.h"
//#include "usart.h"

#include "sys.h"
#include "delay.h"	
#include "exti.h"
#include "dma.h"
#include "spi.h"
#include "hw_config.h"


u8 InstructType = 0;	//ָ������ at���߾���
u8 TransferType = NEED_RETRANS; //�Ƿ���Ҫת��

#define TCP_CON		0			//tcp ����
#define UDP_CON		1			//udp����

//spi
extern u8 SPI_Data_Out[MAX_IN_OUT_DATA_SIZE];	//����洢
extern u8 SPI_Data_In[MAX_IN_OUT_DATA_SIZE];	//����洢
int Out_Data_Len = 0;
//uart
extern u16 USART_RX_STA;
extern u8 USART_RX_BUF[MAX_IN_OUT_DATA_SIZE];

extern void uart3_init(u32 pclk2,u32 bound);
extern void TestSPI_JJ(void);

#define POOL_RCV	0		//��ѯ����

u8 frameno =0;	//֡���
u8 tcporudp = TCP_CON;	//0��ʾtcp��1��ʾudp
u8 ifbroadcast = 0;		//udp���Ըĳ�1����ʾ���㲥��
u8 socketno = 0;	
u8 currentstate = 0;

u8 ssid[] = {"WX_ThingsTurn"};
u8 pwd[] = {"thingsturn2018"};
u8 remoteip[4] = {192,168,2,221};
u16 remoteport = 2000;
u16 localport = 1000;

#if POOL_RCV
u16 remoteport_2 = 1030;
u16 localport_2 = 1001;
u8 socketno_2 = 0;

u16 remoteport_3 = 1040;	//udp
u16 localport_3 = 1002;
u8 socketno_3 = 0;
#endif

//buf:	�������ͷ
//type:	0x01:����ָ������ 0x00:�û�����
//len:	payload���ȣ��ֽ����Ǵ�ˣ���λ�ڵص�ַ����λ�ڸߵ�ַ
void MergeJJHead(u8 *buf, u8 type, u16 len)
{
	buf[0] = 0xAA;	//ͬ���ֽڣ��̶�Ϊaa
	buf[1] = (u8)type;
	buf[2] = (u8)(len/256);
	buf[3] = (u8)(len%256);
	buf[4] = frameno;
	buf[5] = 0;
	if(0 == type)
	{
		if(UDP_CON == tcporudp)
		{
			buf[6] = 0x40;	//����ucp
		}
		else
		{
			buf[6] = socketno&0x3f;	//����tcp����ʾsocket ��
		}
	}
	else
	{
		buf[6] = 0;
	}
	buf[7] = buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6];	//֡ͷУ���
	frameno ++;
	if(UDP_CON == tcporudp && 0 == type)
	{
		if(ifbroadcast)
		{
			buf[8] = 0xff;
			buf[9] = 0xff;
			buf[10] = 0xff;
			buf[11] = 0xff;
		}
		else
		{
			buf[8] = remoteip[0];
			buf[9] = remoteip[1];
			buf[10] = remoteip[2];
			buf[11] = remoteip[3];
		}
		buf[12] = (u8)(remoteport/256);
		buf[13] = (u8)(remoteport%256);
		buf[14] = (u8)(localport/256);
		buf[15] = (u8)(localport%256);
	}
}


//ext: �Ƿ��������1��ʾ�в���
//cmd: ������
//param :�����������п�����0,���Բ�����strcpy
//paramlen: ��������
u16 MergeJJData(u8 *buf,u8 cmd, u8 ext, u8 *param, u16 paramlen)
{
	u16 i = 8;
	u16 j= i;
	
	if(cmd)
	{
		MergeJJHead(buf, 0x01, 4 + paramlen);	//����
	}
	else
	{
		MergeJJHead(buf, 0x00, paramlen);	//����
	}
	if(UDP_CON == tcporudp && 0 == cmd)
	{
		j = i = 16;	//udp������֡ͷ��16���ֽ�
	}
	if(cmd)
	{
		buf[i++] = 0x01;	//����
		buf[i++] = (u8)cmd;
		buf[i++] = 0;
	}
	if(ext)		//
	{
		if(cmd)
			buf[i++] = ext;
		if(param)
		{
			memcpy(buf+i, param, paramlen);
			i += paramlen;
		}
	}
	else
	{
		buf[i++] = 0;
	}
	for(;j < i;j ++)
	{
		buf[i] += buf[j];		//����֡��У��� 
	}
	i ++;
	j = i%4;
	if(j != 0)	//˵����Ҫ��0
	{
		memset(buf+i,0,(4-j));
		return (i+4-j);	//�������ݳ���
	}
	else
		return i;	//�������ݳ���
	
}

void setssid(char *ssid,u8 len)
{
	u8 param[64];
	u16 datalen = 0;
	u16 cnt = 0;
	int i = 0;
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	memset(param, 0, sizeof(param));
	strcpy(param+1,ssid);
	param[0] = len;
	datalen = MergeJJData(SPI_Data_Out , 0x41, 1, param,len+1);	//����ssid
	SPIx_TransferCmd(SPI_Data_Out, datalen);
	
	while(1)
	{
		
		delay_ms(100);
		cnt ++;
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);
			printf("\nReceiveData:");		
			for(i=0; i< 20; i++)
			{
				printf("%02x ", SPI_Data_In[i]);
			}
			printf("\r\n");
			if(SPI_Data_In[8] == 0x02 && SPI_Data_In[9] == 0x41 && SPI_Data_In[10] == 0x00)	//����ssid�ɹ�
				break;			
		}
		if(cnt>50)	//5����û���յ�Ӧ�𣬼�����������
		{
			memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
			datalen = MergeJJData(SPI_Data_Out , 0x41, 1, param,len+1);	//����ssid
			SPIx_TransferCmd(SPI_Data_Out, datalen);
			cnt = 0;
		}
	}

}

void setkey(char *key, u8 len)
{
	u8 param[64];
	u16 datalen = 0;
	u16 cnt = 0;
	int i = 0;
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	memset(param, 0, sizeof(param));
	param[0] = 0x01;	//ascii�ַ�
	param[1] = 0;
	if(len > 0)
	{
		param[2] = len;		//��Կ����
		strcpy(param+3, key);
		datalen = MergeJJData(SPI_Data_Out , 0x42, 1, param, len+3);	//����key��
	}
	else
	{
		datalen = MergeJJData(SPI_Data_Out , 0x42, 1, param, 2);	//����key��
	}
	
	SPIx_TransferCmd(SPI_Data_Out, datalen);
	
	while(1)
	{		
		delay_ms(100);
		cnt ++;
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);
			printf("\nReceiveData:");		
			for(i=0; i< 20; i++)
			{
				printf("%02x ", SPI_Data_In[i]);
			}			
			printf("\r\n");
			if(SPI_Data_In[8] == 0x02 && SPI_Data_In[9] == 0x42 && SPI_Data_In[10] == 0x00)	//����key�ɹ�
				break;			
		}
		if(cnt>50)	//5����û���յ�Ӧ�𣬼�����������
		{
			memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
			if(len > 0)
			{
				param[2] = len;		//��Կ����
				strcpy(param+3, key);
				datalen = MergeJJData(SPI_Data_Out , 0x42, 1, param, len+3);	//����key��
			}
			else
			{
				datalen = MergeJJData(SPI_Data_Out , 0x42, 1, param, 2);	//����key��
			}
			SPIx_TransferCmd(SPI_Data_Out, datalen);
			cnt = 0;
		}
	}	
}

void joinnet(void)
{	
	u16 datalen = 0;
	u16 cnt = 0;
	int i = 0;
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	datalen = MergeJJData(SPI_Data_Out , 0x20, 0, NULL, 0);	//����
	SPIx_TransferCmd(SPI_Data_Out, datalen);
	
	while(1)
	{			
		delay_ms(100);
		cnt ++;
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);
			printf("\nReceiveData:");		
			for(i=0; i< 20; i++)
			{
				printf("%02x ", SPI_Data_In[i]);
			}				
			printf("\r\n");			
#if 0
			if(SPI_Data_In[9] == 0xe3)
			{
				SPIx_TransferCmd(SPI_Data_Out, datalen);	
				cnt = 0;
			}
#endif			
			if(SPI_Data_In[8] == 0x02 && SPI_Data_In[9] == 0x20 && SPI_Data_In[10] == 0x00 && SPI_Data_In[11] == 0x00)	//˵���Ѿ�����
				break;
			if(SPI_Data_In[9] == 0xe6)	//�����ɹ�
				break;
		}
		if(cnt>100)	//10����û���յ�Ӧ�𣬼�����������
		{
			memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
			datalen = MergeJJData(SPI_Data_Out , 0x20, 0, NULL, 0);	//����
			SPIx_TransferCmd(SPI_Data_Out, datalen);
			cnt = 0;
		}
	}
	
}

void createclient(void)
{
	u16 datalen = 0;
	u8 param[64];
	u16 cnt = 0;
	int i = 0;
	u16 delay = 0;
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	memset(param, 0, sizeof(param));
	param[0] = tcporudp;	//tcp or udp
	param[1] = 0;		//client
	param[2] = 4;		//ip��ַ
	param[3] = remoteip[0];	//��������ip��ַ
	param[4] = remoteip[1];
	param[5] = remoteip[2];
	param[6] = remoteip[3];
	param[7] = (u8)(remoteport/256);	//�������Ķ˿ںţ���˸�ʽ�洢
	param[8] = (u8)(remoteport%256);
	param[9] = (u8)(localport/256);		
	param[10] = (u8)(localport%256);	
	datalen = MergeJJData(SPI_Data_Out , 0x28, 1, param, 11);	//����sockt client
	for (delay = 0; delay < 10; delay++)
	{
			delay_ms(1000);
	}
	SPIx_TransferCmd(SPI_Data_Out, datalen);
	
	
	
	
	while(1)
	{			
		delay_ms(100);
		cnt ++;
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);
			printf("\nReceiveData:");		
			for(i=0; i< 20; i++)
			{
				printf("%02x ", SPI_Data_In[i]);
			}			
			printf("\r\n");
			if(SPI_Data_In[8] == 0x02 && SPI_Data_In[9] == 0x28 && SPI_Data_In[10] == 0x00)	//����socket no
			{
				socketno = SPI_Data_In[12];
				if(UDP_CON == tcporudp)
					break;
			}
			
			if(SPI_Data_In[9] == 0xe8)	//
			{
					if(socketno > 0 && 1 == SPI_Data_In[13])	//[13]=1����ʾtcp�����ɹ���=0����ʾtcp����ʧ��
						break;
			}
			if(SPI_Data_In[9] == 0xe7)	//	�����Ѿ��Ͽ�
			{
				joinnet();		//����
				break;
			}
		}
		if(cnt>50)	//5����û���յ�Ӧ�𣬼�����������
		{
			memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
			datalen = MergeJJData(SPI_Data_Out , 0x28, 1, param, 11);	//����sockt client��Ϊ����֡���++������ģ���������
			SPIx_TransferCmd(SPI_Data_Out, datalen);
			cnt = 0;
		}
	}

}

#if POOL_RCV
void createclient_2(void)
{
	u16 datalen = 0;
	u8 param[64];
	u16 cnt = 0;
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	memset(param, 0, sizeof(param));
	param[0] = tcporudp;	//tcp or udp
	param[1] = 0;		//client
	param[2] = 4;		//ip��ַ
	param[3] = remoteip[0];	//��������ip��ַ
	param[4] = remoteip[1];
	param[5] = remoteip[2];
	param[6] = remoteip[3];
	param[7] = (u8)(remoteport_2/256);	//�������Ķ˿ںţ���˸�ʽ�洢
	param[8] = (u8)(remoteport_2%256);
	param[9] = (u8)(localport_2/256);		
	param[10] = (u8)(localport_2%256);	
	datalen = MergeJJData(SPI_Data_Out , 0x28, 1, param, 11);	//����sockt client
	SPIx_TransferCmd(SPI_Data_Out, datalen);
	
	while(1)
	{			
		delay_ms(100);
		cnt ++;
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);
			
			if(SPI_Data_In[8] == 0x02 && SPI_Data_In[9] == 0x28 && SPI_Data_In[10] == 0x00)	//����socket no
			{
				socketno_2 = SPI_Data_In[12];
				if(UDP_CON == tcporudp)
					break;
			}
			
			if(SPI_Data_In[9] == 0xe8)	//
			{
					if(socketno_2 > 0 && 1 == SPI_Data_In[13])	//[13]=1����ʾtcp�����ɹ���=0����ʾtcp����ʧ��
						break;
			}
			if(SPI_Data_In[9] == 0xe7)	//	�����Ѿ��Ͽ�
			{
				joinnet();		//����
				break;
			}
		}
		if(cnt>50)	//5����û���յ�Ӧ�𣬼�����������
		{
			memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
			datalen = MergeJJData(SPI_Data_Out , 0x28, 1, param, 11);	//����sockt client��Ϊ����֡���++������ģ���������
			SPIx_TransferCmd(SPI_Data_Out, datalen);
			cnt = 0;
		}
	}

}

void createclient_3(void)
{
	u16 datalen = 0;
	u8 param[64];
	u16 cnt = 0;
	tcporudp = UDP_CON;
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	memset(param, 0, sizeof(param));
	param[0] = tcporudp;	//udp
	param[1] = 0;		//client
	param[2] = 4;		//ip��ַ
	param[3] = remoteip[0];	//��������ip��ַ
	param[4] = remoteip[1];
	param[5] = remoteip[2];
	param[6] = remoteip[3];
	param[7] = (u8)(remoteport_3/256);	//�������Ķ˿ںţ���˸�ʽ�洢
	param[8] = (u8)(remoteport_3%256);
	param[9] = (u8)(localport_3/256);		
	param[10] = (u8)(localport_3%256);	
	datalen = MergeJJData(SPI_Data_Out , 0x28, 1, param, 11);	//����sockt client
	SPIx_TransferCmd(SPI_Data_Out, datalen);
	
	while(1)
	{			
		delay_ms(100);
		cnt ++;
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);
			
			if(SPI_Data_In[8] == 0x02 && SPI_Data_In[9] == 0x28 && SPI_Data_In[10] == 0x00)	//����socket no
			{
				socketno_3 = SPI_Data_In[12];
				if(UDP_CON == tcporudp)
					break;
			}
			
			if(SPI_Data_In[9] == 0xe8)	//
			{
					if(socketno_3 > 0 && 1 == SPI_Data_In[13])	//[13]=1����ʾtcp�����ɹ���=0����ʾtcp����ʧ��
						break;
			}
			if(SPI_Data_In[9] == 0xe7)	//	�����Ѿ��Ͽ�
			{
				joinnet();		//����
				break;
			}
		}
		if(cnt>50)	//5����û���յ�Ӧ�𣬼�����������
		{
			memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
			datalen = MergeJJData(SPI_Data_Out , 0x28, 1, param, 11);	//����sockt client��Ϊ����֡���++������ģ���������
			SPIx_TransferCmd(SPI_Data_Out, datalen);
			cnt = 0;
		}
	}
}

u16 sock_if_have_data(u8 sockno)
{
	u16 datalen = 0;
	u8 param[64];
	u16 cnt = 0;
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	memset(param, 0, sizeof(param));
	param[0] = sockno;	//sockno
	datalen = MergeJJData(SPI_Data_Out , 0x2e, 1, param, 1);	//����sockt client
	SPIx_TransferCmd(SPI_Data_Out, datalen);
	
	while(1)
	{			
		delay_ms(100);
		cnt ++;
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			memset(SPI_Data_In, 0, sizeof(SPI_Data_In));
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);
			
			if(SPI_Data_In[8] == 0x02 && SPI_Data_In[9] == 0x2e && SPI_Data_In[10] == 0x00)	//����socket no
			{				
					datalen = SPI_Data_In[12] + SPI_Data_In[13]<<8;				
					break;
			}			
		}
		if(cnt>50)	//5����û���յ�Ӧ�𣬼�����������
		{
			memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
			datalen = MergeJJData(SPI_Data_Out , 0x2e, 1, param, 3);	
			SPIx_TransferCmd(SPI_Data_Out, datalen);
			cnt = 0;
		}
	}
	return datalen;	
}	
#endif

void senddata(u8 *data, u16 len)
{
	u16 datalen;
	
	memset(SPI_Data_Out,0,sizeof(SPI_Data_Out));
	datalen = MergeJJData(SPI_Data_Out , 0, 1, data , len);//�����û�����
	SPIx_TransferData(SPI_Data_Out, datalen);
}

u16 receivedata(u8 *data)
{
	u16 datalen = 0;
	if(SPI_Data_In[0] == 0xaa && SPI_Data_In[1] == 0x00)
	{
		datalen = (SPI_Data_In[2] << 8) + SPI_Data_In[3];
		if(tcporudp == UDP_CON)
		{
			memcpy(data,SPI_Data_In+16,datalen);	//udpͷ������16�ֽ�
		}
		else
		{
			memcpy(data,SPI_Data_In+8,datalen);
		}
	}
	return datalen;
}

u8 txdata[1500];
u8 rxdata[1500];
u32 totalsndlen = 0;
u32 totalrcvlen = 0;
u32 prelen = 0;
int TW03_Test(void)
{	  
	u16 i;
	u16 datalen = 0;
	u8 param[64];
	
	u16 cnt = 0;
	u8 str[20];
	u16 size;
	u8 rcvflag = 0;
	
 	//Stm32_Clock_Init(9);//ϵͳʱ������
	delay_init(36);		//��ʱ��ʼ��
	//uart_init(36,115200);	//��ӡ����
	delay_init(36);		//��ʱ��ʼ��

	SPIx_Init();
	printf("\nspi init\n");
	GPIOB->ODR|=0<<12;
	delay_ms(1000);
	GPIOB->ODR|=1<<12;
	printf("\nspi init1\n");
	while(1)
	{
		memset(SPI_Data_In, 0, MAX_IN_OUT_DATA_SIZE);
		ClearSPIStatus(SPI_DATA_IN); //�Ȳ�Ҫ�ϴ�
		SPIx_ReceiveData(SPI_Data_In);		
		
		if(SPI_Data_In[9] == 0xe0)	//�豸�Ѿ�׼����
		{
			printf("\nspi ok\n");
			break;
		} 
		delay_ms(100);
	//	GPIOB->ODR|=0<<12;
		
		cnt ++;
		if(cnt>20)	//2����û���յ�Ӧ��
		{
			printf("\nspi off\n");
			break;
		}
		 
		 // delay_ms(100); 
	//	  GPIOB->ODR|=1<<12;
	}

#if 0
#if 1	
	TestSPISpeed();		//���Է����ٶ�
#else	
{
	SPIx_TransferCmd("rxtest", 6);
	cnt = 0;
	while(1)
	{
		//delay_ms(1);
		if(GetSPIStatus() & SPI_DATA_IN)
		{
			ClearSPIStatus(SPI_DATA_IN);
			totalrcvlen += SPIx_ReceiveData(SPI_Data_In);		
		}
		cnt ++;		
		if(cnt > 600)
		{
			//printf("\r\n%d\r\n",totalrcvlen);
			cnt = 0;
		}		
	}
}
#endif
	while(1);
#endif

	//setssid("TP-LINK_F0F58A",14);
	//setkey("20131118", 8);
	printf("ssid=%s,pwd=%s\r\n",ssid,pwd);
	setssid(ssid,strlen(ssid));
	setkey(pwd, strlen(pwd));	
	joinnet();		//����
	createclient();	//��������
#if POOL_RCV
  createclient_2();
	createclient_3();
#endif
	for(i = 0;i < 1500;i ++)
	{
		txdata[i] = 0x30+i%10;
	}
	cnt = 0;
	while(1)
	{	
	//	delay_ms(1);

		cnt ++;		
		if(cnt > 30)
		{
		//	senddata(txdata,1460);
			
	//		printf("\r\ntotal send len=%d,rcvlen=%d\r\n", totalsndlen , totalrcvlen);
			cnt = 0;
		}
#if POOL_RCV
		if(0 == rcvflag)
			continue;
		tcporudp = TCP_CON;
		size = sock_if_have_data(socketno);
		if(size > 0)
		{
#else		
		if(GetSPIStatus() & SPI_DATA_IN)
			{
#endif			
		
			memset(SPI_Data_In, 0, MAX_IN_OUT_DATA_SIZE);
			ClearSPIStatus(SPI_DATA_IN);
			SPIx_ReceiveData(SPI_Data_In);			
			switch(SPI_Data_In[9])
			{
				case 0xe7:
					joinnet();
					break;
				case 0xea:
					createclient(); 
					break;
				default:
					break;
			}	
#if POOL_RCV
			if(1)	
#else			
			if(/*SPI_Data_In[0] == 0xaa && SPI_Data_In[1] == 0x00*/1)
#endif				
			{
				memset(rxdata,0,sizeof(rxdata));
				datalen = receivedata(rxdata);
				totalrcvlen += datalen;	
				if(totalrcvlen - prelen > 100*1000)
				{
					printf("\r\n=%d\n",totalrcvlen);
					prelen = totalrcvlen;
				}
			}
		}
#if POOL_RCV
		else
		{
			size = sock_if_have_data(socketno_2);
			if(size > 0)
			{
				memset(SPI_Data_In, 0, MAX_IN_OUT_DATA_SIZE);
				ClearSPIStatus(SPI_DATA_IN);
				SPIx_ReceiveData(SPI_Data_In);		
			}
			else
			{
				tcporudp = UDP_CON;
				size = sock_if_have_data(socketno_3);
				if(size > 0)
				{
					memset(SPI_Data_In, 0, MAX_IN_OUT_DATA_SIZE);
					ClearSPIStatus(SPI_DATA_IN);
					SPIx_ReceiveData(SPI_Data_In);		
				}
			}
		}
#endif		
	}
  		  
}
