/* 
 *��򵥵Ļ���FFmpeg����Ƶ������
 *Simplest FFmpeg Video Encoder
*/

#include "stdafx.h"

extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
};



#include <WinSock2.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

#include <process.h>  

#include <stdio.h>  
#include <process.h>  
#include <windows.h>  

#include <cstdio>
#include <ctime>
#include <algorithm>
#include <functional>

int i=0;

//int thread_exit=0;

//int flag=0;
//SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);

//long g_nNum;  
unsigned int __stdcall Fun(void *pPM);  
//const int THREAD_NUM = 2;  
//�ź�����ؼ���  
//HANDLE            g_hThreadParameter;  
//HANDLE            g_hThreadParameter2;  
//CRITICAL_SECTION  g_csThreadCode; 

//typedef unsigned long DWORD;
//DWORD a=100;
//DWORD b=1;


BOOL SetConsoleColor(WORD wAttributes)  
{  
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  
	if (hConsole == INVALID_HANDLE_VALUE)  
		return FALSE;  

	return SetConsoleTextAttribute(hConsole, wAttributes);  
}  
const int END_PRODUCE_NUMBER = 25;  //������Ʒ����  
const int BUFFER_SIZE = 4;          //����������  
int g_Buffer[BUFFER_SIZE];          //�����  
int g_i, g_j;  
//�ź�����ؼ���  
CRITICAL_SECTION g_cs;  
HANDLE g_hSemaphoreBufferEmpty, g_hSemaphoreBufferFull;  
//�������̺߳���  


char savebuffer1[sizeof(AVPacket)];
char savebuffer2[sizeof(AVPacket)];
char savebuffer3[sizeof(AVPacket)];
char savebuffer4[sizeof(AVPacket)];

int count=1;
int count2=1;
AVPacket aaaa;
int flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index)
{

	int ret;
	int got_frame;
	AVPacket enc_pkt;
	if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
		CODEC_CAP_DELAY))
		return 0;
	//while (1) {
		printf("Flushing stream #%u encoder\n", stream_index);
		//ret = encode_write_frame(NULL, stream_index, &got_frame);

		enc_pkt.data = NULL;
		enc_pkt.size = 0;
		av_init_packet(&enc_pkt);
		ret = avcodec_encode_video2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt,
			NULL, &got_frame);



		memcpy(savebuffer1,&enc_pkt,sizeof(AVPacket));
		//send(sockClient,savebuffer1,strlen(AVPacket)+1,0);



		av_frame_free(NULL);
		/*if (ret < 0)
			break;*/
		/*if (!got_frame)
		{ret=0;break;}*/
		printf("����ɹ�1֡��\n");
		/* mux encoded frame */
		ret = av_write_frame(fmt_ctx, &enc_pkt);
		//if (ret < 0)
			//break;
	//}
	return ret;
}


unsigned int __stdcall ProducerThreadFun(PVOID pM)  
{  

	
    //int nThreadNum = *(int *)pPM; 

	AVFormatContext* pFormatCtx;
	
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;

	uint8_t* picture_buf;
	AVFrame* picture;
	int size;

	FILE *in_file = fopen("src01_480x272.yuv", "rb");	//��ƵYUVԴ�ļ� 
	int in_w=480,in_h=272;//���	
	int framenum=50;
	const char* out_file = "src01.h264";					//����ļ�·��

	/*
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested=MAKEWORD(1,1);

		err=WSAStartup(wVersionRequested,&wsaData);
		if (err)
			return;

		if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
		{
			WSACleanup();
			return;
		}*/
		//�����׽���

	printf("start registering all\n");
	av_register_all();
	//����1.���ʹ�ü�������
	pFormatCtx = avformat_alloc_context();
	//�¸�ʽ
	fmt = av_guess_format(NULL, out_file, NULL);
	pFormatCtx->oformat = fmt;

	//����2.�����Զ���һЩ
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
	//fmt = pFormatCtx->oformat;


	//ע�����·��
	if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0)
	{
		printf("����ļ���ʧ��");
		return -1;
	}

	video_st = av_new_stream(pFormatCtx, 0);
	if (video_st==NULL)
	{
		return -1;
	}
	pCodecCtx = video_st->codec;
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	pCodecCtx->width = in_w;  
	pCodecCtx->height = in_h;
	pCodecCtx->time_base.num = 1;  
	pCodecCtx->time_base.den = 25;  
	pCodecCtx->bit_rate = 400000;  
	pCodecCtx->gop_size=250;
	//H264
	//pCodecCtx->me_range = 16;
	//pCodecCtx->max_qdiff = 4;
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;
	//pCodecCtx->qcompress = 0.6;
	//�����ʽ��Ϣ
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec)
	{
		printf("û���ҵ����ʵı�������\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0)
	{
		printf("��������ʧ�ܣ�\n");
		return -1;
	}
	picture = avcodec_alloc_frame();
	size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	printf("the size:%d",size);
	picture_buf = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)picture, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//д�ļ�ͷ
	avformat_write_header(pFormatCtx,NULL);

	AVPacket pkt;
	printf("the AVpacket:%d\n",sizeof(AVPacket));
	printf("the AVframe:%d\n",sizeof(AVFrame));

	int y_size = pCodecCtx->width * pCodecCtx->height;
	av_new_packet(&pkt,y_size*3);
	printf("׼����ʼ����\n");





	for (int i = 1; i <= END_PRODUCE_NUMBER; i++)  
	{  
		
		//�ȴ��пյĻ���������  
		WaitForSingleObject(g_hSemaphoreBufferEmpty, INFINITE);  

		//����ķ��ʻ�����  
		EnterCriticalSection(&g_cs);  
		g_Buffer[g_i] = i;  
		printf("�������ڻ���ص�%d����������Ͷ������%d\n", g_i, g_Buffer[g_i]);  
	    //����YUV
		if (fread(picture_buf, 1, y_size*3/2, in_file) < 0)
		{
			printf("�ļ���ȡ����\n");
			return -1;
		}else if(feof(in_file)){
			break;
		}
		picture->data[0] = picture_buf;  // ����Y
		picture->data[1] = picture_buf+ y_size;  // U 
		picture->data[2] = picture_buf+ y_size*5/4; // V
		//PTS
		picture->pts=i;
		int got_picture=0;
		//����
		int ret = avcodec_encode_video2(pCodecCtx, &pkt,picture, &got_picture);
		if(ret < 0)
		{
			printf("�������\n");
			return -1;
		}
		if (got_picture==1)
		{

			printf("����ɹ�1֡��\n");
			pkt.stream_index = video_st->index;
			ret = av_write_frame(pFormatCtx, &pkt);

			/*switch(count){
			case 1:memcpy(savebuffer1,&pkt,sizeof(AVPacket));
				printf("print out savebuffer1: %s\n",savebuffer1);
				count=count+1;
				  break;
			case 2:memcpy(savebuffer2,&pkt,sizeof(AVPacket));
				printf("print out savebuffer2: %s\n",savebuffer2);
				count=count+1;
				  break;
			case 3:memcpy(savebuffer3,&pkt,sizeof(AVPacket));
				printf("print out savebuffer3: %s\n",savebuffer3);
				count=count+1;
				  break;
			case 4:memcpy(savebuffer4,&pkt,sizeof(AVPacket));
				printf("print out savebuffer4: %s\n",savebuffer4);
				count=1;
				  break;
			} */ 
			


			//��������
			//char recvBuf[100];
			//recv(sockClient,recvBuf,100,0);
			//printf("%s\n",recvBuf);
			//��������
			//�ر��׽���
			//closesocket(sockClient);
			//WSACleanup();
			//getchar();
			FILE *fp=fopen("info.txt","wb+");
			//fprintf(fp,"the output is: %s\n",test->streams[i]);
			fclose(fp);
			av_free_packet(&pkt);
			
			printf("һ�ֽ���\n");
		}
		 ret = flush_encoder(pFormatCtx,0);
		if (ret < 0) {
			printf("Flushing encoder failed\n");
			return -1;
		}
        g_i = (g_i + 1) % BUFFER_SIZE;  



		LeaveCriticalSection(&g_cs);  

		//֪ͨ����������������  
		ReleaseSemaphore(g_hSemaphoreBufferFull, 1, NULL);  
		  
	}
	printf("��������������߳̽�������\n");  



	//Flush Encoder
	

	//д�ļ�β
	av_write_trailer(pFormatCtx);

	//����
	if (video_st)
	{
		avcodec_close(video_st->codec);
		av_free(picture);
		av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	fclose(in_file);
	
	return 0;  
	

}


unsigned int __stdcall ConsumerThreadFun(PVOID pM)  
{  
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested=MAKEWORD(1,1);

	err=WSAStartup(wVersionRequested,&wsaData);
	if (err)
		return 0;

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return 0;
	}
	//�����׽���
	SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");//�����������ַ
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);

	
	//���������ѭ����Ҫ�ر��׽��֣��ͷ���Դ
	//closesocket(sockSrv);
	//WSACleanup();
	char sendBuf[50];
	int k=0;
	for(k=0;k<50;k++){  
    //int nThreadNum = *(int *)pPM; 
	
		//�ȴ��ǿյĻ���������  
		WaitForSingleObject(g_hSemaphoreBufferFull, INFINITE);  

		//����ķ��ʻ�����  
		EnterCriticalSection(&g_cs);  
		SetConsoleColor(FOREGROUND_GREEN);  
		printf("  ���Ϊ%d�������ߴӻ�����е�%d��������ȡ������%d\n", GetCurrentThreadId(), g_j, g_Buffer[g_j]);  
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  

		printf("��ʼ����\n");
		
		  
		connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
		//��������
		//send(sockClient,"This is lisi",strlen("This is lisi")+1,0);



		sprintf(  sendBuf, " ���������������������  Integer:   %d\n", i ); // C4996
		i=i+1;



		//���������������
		//send(sockClient,sendBuf,strlen(sendBuf)+1,0);
		send(sockClient,savebuffer1,80,0);
			

		/*switch (count2){
			
		case 1:send(sockClient,savebuffer1,strlen(savebuffer1)+1,0);
			count2=count2+1;
			  break;
		case 2:send(sockClient,savebuffer2,strlen(savebuffer1)+1,0);
			count2=count2+1;
			  break;
		case 3:send(sockClient,savebuffer3,strlen(savebuffer1)+1,0);
			count2=count2+1;
			  break;
		case 4:send(sockClient,savebuffer4,strlen(savebuffer1)+1,0);
			count2=1;
			  break;
		}
		*/

		char recvBuf[50];

		printf("�������˿�ʼ��������\n");
		recv(sockClient,recvBuf,50,0);
		printf("%s\n",recvBuf);
        //��������
		
        //closesocket(sockClient);
		
		//�ر��׽���
		
		

		if (g_Buffer[g_j] == END_PRODUCE_NUMBER)//������־  
		{  
			LeaveCriticalSection(&g_cs);  
			//֪ͨ��������������������(������־)  
			ReleaseSemaphore(g_hSemaphoreBufferFull, 1, NULL);  
			break;  
		}  
		g_j = (g_j + 1) % BUFFER_SIZE;  
		LeaveCriticalSection(&g_cs);  

		Sleep(50); //some other work to do  

		ReleaseSemaphore(g_hSemaphoreBufferEmpty, 1, NULL);  
	}  

      WSACleanup();
      getchar();



	SetConsoleColor(FOREGROUND_GREEN);  
	printf("  ���Ϊ%d���������յ�֪ͨ���߳̽�������\n", GetCurrentThreadId());  
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);  
	return 0;  
	
  
     
    
	

	//send(sockClient,"This is lisi",strlen("This is lisi")+1,0);
	
	//send(sockClient,"send the information successfully",strlen("send the information successfully")+1,0);

	
	
	
}


int _tmain(int argc, _TCHAR* argv[])
{
   
    
	
	//����ʱ��
	double start,stop;
	start = clock();
 
	printf("  ����������������   1������ 1������ 4������\n");  


	InitializeCriticalSection(&g_cs);  
	//��ʼ���ź���,һ����¼�в�Ʒ�Ļ���������,��һ����¼�ջ���������.  
	g_hSemaphoreBufferEmpty = CreateSemaphore(NULL, 4, 4, NULL);  
	g_hSemaphoreBufferFull  = CreateSemaphore(NULL, 0, 4, NULL);  
	g_i = 0;  
	g_j = 0;  
	memset(g_Buffer, 0, sizeof(g_Buffer));  

	const int THREADNUM =2 ;  
	HANDLE hThread[THREADNUM];  
	//�������߳�  
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, ProducerThreadFun, NULL, 0, NULL);  
	//�������߳�  
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThreadFun, NULL, 0, NULL);  
	//hThread[2] = (HANDLE)_beginthreadex(NULL, 0, ConsumerThreadFun, NULL, 0, NULL);  
	WaitForMultipleObjects(THREADNUM, hThread, TRUE, INFINITE);  
	for (int i = 0; i < THREADNUM; i++)  
		CloseHandle(hThread[i]);  



	//�����ź����͹ؼ���  
	CloseHandle(g_hSemaphoreBufferEmpty);  
	CloseHandle(g_hSemaphoreBufferFull);  
	DeleteCriticalSection(&g_cs);  
	return 0;  

		
	
		

    
}

