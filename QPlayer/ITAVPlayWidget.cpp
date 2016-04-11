#include "stdafx.h"
#include "ITAVPlayWidget.h"
#include <QtConcurrent>
#include "Global_Fun.h"

inline unsigned char CONVERT_ADJUST(double tmp)
{
	return (unsigned char)((tmp >= 0 && tmp <= 255)?tmp:(tmp < 0 ? 0 : 255));
}
//YUV420P to RGB24
void CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight)
{
	unsigned char *tmpbuf=(unsigned char *)malloc(nWidth*nHeight*3);
	unsigned char Y,U,V,R,G,B;
	unsigned char* y_planar,*u_planar,*v_planar;
	int rgb_width , u_width;
	rgb_width = nWidth * 3;
	u_width = (nWidth >> 1);
	int ypSize = nWidth * nHeight;
	int upSize = (ypSize>>2);
	int offSet = 0;

	y_planar = yuv_src;
	u_planar = yuv_src + ypSize;
	v_planar = u_planar + upSize;

	for(int i = 0; i < nHeight; i++)
	{
		for(int j = 0; j < nWidth; j ++)
		{
			Y = *(y_planar + nWidth * i + j);
			offSet = (i>>1) * (u_width) + (j>>1);
			V = *(u_planar + offSet);
			U = *(v_planar + offSet);

			R = CONVERT_ADJUST((Y + (1.4075 * (V - 128))));
			G = CONVERT_ADJUST((Y - (0.3455 * (U - 128) - 0.7169 * (V - 128))));
			B = CONVERT_ADJUST((Y + (1.7790 * (U - 128))));			
			offSet = rgb_width * i + j * 3;

			rgb_dst[offSet] = B;
			rgb_dst[offSet + 1] = G;
			rgb_dst[offSet + 2] = R;
		}
	}
	free(tmpbuf);
}
ITAVPlayWidget::ITAVPlayWidget(QWidget *parent)
	: QGLWidget(parent)
{
	ui.setupUi(this);

	m_pDevice=NULL;
	m_pContext=NULL;
	m_pAVStreamPares = new ITAVStreamParse;
	m_bStopPlay=false;
	m_pAudioBuffer=NULL;
	m_pAudioData=NULL;
	m_nAudioDataSize=0;
	m_outSourceID=0;
	m_outBuffID=0;
	m_nVideoWidth=0;
	m_nVideoHeight=0;
	m_defaultImage.load(QString(":/MOSSOCX/Resources/novideo.png"));
	m_bDefaultImageShow = false;
	m_bPreview=false;
	connect(this,SIGNAL(audioPlayFini()),this,SLOT(doAudioPlayFini()));
}

ITAVPlayWidget::~ITAVPlayWidget()
{

}
int ITAVPlayWidget::initAudio()
{
	m_pDevice=alcOpenDevice(NULL);	//打开默认音频器	
	if(m_pDevice == NULL)
	{
		printf("open Default Device Ero");
		return -1;
	}
	m_pContext=alcCreateContext(m_pDevice,NULL);
	if(m_pContext==NULL)
	{
		alcCloseDevice(m_pDevice);
		m_pDevice=NULL;
	}
	alcMakeContextCurrent(m_pContext);		//如果有多个播放环境时，用这个来管理当前的播放环境？
	alGenSources(1,&m_outSourceID);		//捆绑音频源 相当于在一个空间中放一个音频源，用于建立播放环境
	alSpeedOfSound(1.0);
	alDopplerVelocity(1.0);
	alDopplerFactor(1.0);
	alSourcef(m_outSourceID, AL_PITCH, 1.0f);
	alSourcef(m_outSourceID, AL_GAIN, 1.0f);
	alSourcei(m_outSourceID, AL_LOOPING, AL_FALSE);
	alSourcef(m_outSourceID, AL_SOURCE_TYPE, AL_STREAMING);
	alGenBuffers(1, &m_outBuffID);	
	alSourceQueueBuffers(m_outSourceID, 1, &m_outBuffID);
	Sleep(20);
	m_pAudioBuffer=new MemBuffer<uint8_t>(g_audioBuffSize);	
	m_pAudioData=new unsigned char[m_nAudioDataSize+1];
	memset(m_pAudioData,0,m_nAudioDataSize+1);
}
int ITAVPlayWidget::finiAudio()
{
	stopSound();
	if(m_pAudioBuffer)
	{
		delete m_pAudioBuffer;
		m_pAudioBuffer=NULL;
	}
	if(m_pAudioData)
	{
		delete[] m_pAudioData;
		m_pAudioData=NULL;
	}
	m_nAudioDataSize=0;
	if(m_outSourceID)
	{
		alDeleteSources( 1, &m_outSourceID );
		m_outSourceID=0;
	}
	if(m_outBuffID)
	{
		alDeleteBuffers( 1, &m_outBuffID );
		m_outBuffID=0;
	}
	if (m_pContext != NULL) 
	{ 
		alcDestroyContext(m_pContext); 
		m_pContext=NULL; 
	} 
	if (m_pDevice != NULL) 
	{ 
		alcCloseDevice(m_pDevice); 
		m_pDevice=NULL; 
	} 
	return true;
}
bool ITAVPlayWidget::updateAudioBuff(int nSampleFmt,int nBitRate,unsigned char* pData,int nLen)
{
	if(!pData || nLen <= 0)
	{
		return false;
	}
	/*if(m_audioFormat != nSampleFmt || m_audioBitRate != nBitRate )
	{
		m_audioFormat = nSampleFmt;
		m_audioBitRate = nBitRate;
		///清空缓冲区，重新填充
	}*/
	ALenum format=AL_FORMAT_STEREO16;	///理论上按传入的格式进行播放，再次默认认为是16采样

	ALuint  buffer=0; 
	alSourceUnqueueBuffers(m_outSourceID, 1, &buffer);	//清除播放的数据		
	//获取已经播放完的缓冲区，再填充新的进去，而不是新建，否则会一直循环一个  
	alBufferData(buffer, format, pData, (ALsizei)nLen,44100);
	alSourceQueueBuffers(m_outSourceID, 1, &buffer);	
	/*ALint iState,iQueuedBuffers;
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &iState);
	if (iState != AL_PLAYING)
	{		
		alGetSourcei(m_outSourceID, AL_BUFFERS_QUEUED, &iQueuedBuffers);
		if (iQueuedBuffers)
		{
			alSourcePlay(m_outSourceID);
		}		
	}*/
	return true;
}
bool ITAVPlayWidget::playSound()
{
	ALint  state,iQueuedBuffers; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	if (state != AL_PLAYING) 
	{ 
		alGetSourcei(m_outSourceID, AL_BUFFERS_QUEUED, &iQueuedBuffers);
		if (iQueuedBuffers)
		{
			alSourcePlay(m_outSourceID);
		}		
	}    
	return true;
}
bool ITAVPlayWidget::stopSound()
{
	ALint  state; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	if (state != AL_STOPPED) 
	{ 
		alSourceStop(m_outSourceID); 
	} 
	return true;
}
bool ITAVPlayWidget::pauseSound()
{
	ALint  state; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	if (state != AL_PAUSED) 
	{ 
		alSourcePause(m_outSourceID); 
	} 
	return true;
}
bool ITAVPlayWidget::isPlaySounding()
{
	ALint  state; 
	alGetSourcei(m_outSourceID, AL_SOURCE_STATE, &state); 
	return state == AL_PLAYING ? true:false;	
}
bool ITAVPlayWidget::isAudioQueueEmpty()
{
	ALint iQueuedBuffers=0;
	alGetSourcei(m_outSourceID, AL_BUFFERS_QUEUED, &iQueuedBuffers);
	return iQueuedBuffers?false:true;
}
void ITAVPlayWidget::contextMenuEvent(QContextMenuEvent *event)    
{
	QMenu menu;
	menu.addAction("打开", this, SLOT(doOpenAVFile()));
	menu.addAction("停止播放", this, SLOT(doStopPlay()));
	menu.exec(QCursor::pos());
}
bool ITAVPlayWidget::closeLastPlay()
{
	m_bStopPlay=true;
	m_pAVStreamPares->closeDstAVFile();
	finiAudio();
	return true;
}
void ITAVPlayWidget::doOpenAVFile()
{
	closeLastPlay();
	QString filePath = QFileDialog::getOpenFileName(NULL,"打开文件","./","*");//第一个是基于那个窗口，第二个参数是描述信息，第三个打开路径，第四个过滤掉其他格式
	if(filePath.isEmpty())
	{
		return;
	}
	m_bStopPlay=false;
	m_pAVStreamPares->openDstAVFile(filePath.toStdString().c_str());
	int64_t pps;
	int nLen=0;
		
	m_nAudioDataSize=m_pAVStreamPares->getAudioChannel()*m_pAVStreamPares->getAudioFrequency();
	if(m_nAudioDataSize<=4096)
	{
		m_nAudioDataSize=44100;
	}
	initAudio();	
	playSound();	
	m_nVideoWidth=m_pAVStreamPares->getVideoWidth();
	m_nVideoHeight=m_pAVStreamPares->getVideoHeight();
	int ysize=m_nVideoWidth*m_nVideoHeight;
	m_pVideoData=new unsigned char[ysize*3+1];
	memset(m_pVideoData,0,ysize*3+1);
	m_bPreview=true;
	initializeGL();
	QtConcurrent::run(playAudioThread,this);
	QtConcurrent::run(playVideoThread,this);
}
void ITAVPlayWidget::playAudioThread(ITAVPlayWidget* pThis)
{
	while(!pThis->m_bStopPlay)
	{		
		int processed=0;
		alGetSourcei(pThis->m_outSourceID, AL_BUFFERS_PROCESSED, &processed); 
		while (processed)		
		{	
			while(!pThis->m_pAVStreamPares->isAudioBuffEmpty())
			{
				if(pThis->m_pAudioBuffer->size() >= pThis->m_nAudioDataSize)
				{					
					int nRealLen = pThis->m_pAudioBuffer->pop_front(pThis->m_pAudioData,pThis->m_nAudioDataSize);
					pThis->updateAudioBuff(16,44100,pThis->m_pAudioData,nRealLen);							
					break;
				}else
				{
					unsigned char* pBuf=NULL;
					int nLen=0;
					int64_t pps=0;
					pThis->m_pAVStreamPares->getNextPcmData(pps,&pBuf,nLen);
					if(pBuf && nLen>0)
					{
						pThis->m_pAudioBuffer->push_back(pBuf,nLen);
						delete[] pBuf;
						pBuf=NULL;
						nLen=0;
					}
				}
			}
			if(pThis->m_pAVStreamPares->isAudioBuffEmpty())
			{
				if(pThis->m_pAudioBuffer->size() < 1)
				{
					pThis->refreshAudioQueue();
					break;
				}
				int nRealLen = pThis->m_pAudioBuffer->pop_front(pThis->m_pAudioData,pThis->m_nAudioDataSize);
				pThis->updateAudioBuff(16,44100,pThis->m_pAudioData,nRealLen);					
				break;
			}			
			--processed;
		}		
		if(!pThis->isPlaySounding())
		{
			if(!pThis->isAudioQueueEmpty())
				pThis->playSound();
			else
			{
				emit pThis->audioPlayFini();
				break;	///音频文件已播放完成，退出
			}
		}		
		Sleep(10);				
	}
}
void ITAVPlayWidget::playVideoThread(ITAVPlayWidget* pThis)
{
	while(!pThis->m_bStopPlay)
	{
		int64_t pts;
		unsigned char* pBuf=NULL;
		int nLen=0;
		int nWidth=0;
		int nHeight=0;
		pThis->m_pAVStreamPares->getNextYuvData(pts,&pBuf,nLen,nWidth,nHeight);		
		if(pBuf && nLen>0)
		{
			
			unsigned char* buffer_convert=new unsigned char[nWidth*nHeight*3+1];
			memset(buffer_convert,0,nWidth*nHeight*3+1);
			CONVERT_YUV420PtoRGB24(pBuf,buffer_convert,nWidth,nHeight);
			memset(pThis->m_pVideoData,0,nWidth*nHeight*3/2+1);
			memmove(pThis->m_pVideoData,buffer_convert,nWidth*nHeight*3);
			delete[] buffer_convert;
			pThis->displayVideo();//updateGL();
		}
	}
}
void ITAVPlayWidget::doAudioPlayFini()
{
	finiAudio();
}
void ITAVPlayWidget::doStopPlay()
{
	closeLastPlay();
}
bool ITAVPlayWidget::refreshAudioQueue()
{
	ALuint  uiBuffer= 0;
	alSourceUnqueueBuffers(m_outSourceID, 1, &uiBuffer);
	return true;
}

void ITAVPlayWidget::initializeGL()
{
	initializeOpenGLFunctions(); 
	glClearDepth(1.0);
	glClearColor(0.0,0.0,0.0,0.0);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
//	setGeometry(300, 150, 640, 480);//设置窗口初始位置和大小
	glShadeModel(GL_FLAT);//设置阴影平滑模式
	glClearColor(0.5, 1.0, 0.2, 0);//改变窗口的背景颜色，不过我这里貌似设置后并没有什么效果
//	glClearDepth(1.0);//设置深度缓存
//	glEnable(GL_DEPTH_TEST);//允许深度测试
//	glDepthFunc(GL_LEQUAL);//设置深度测试类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正
}

void ITAVPlayWidget::resizeGL( int width, int height )
{
	if(0 == height)
		height = 1;//防止一条边为0
	glViewport(0, 0, (GLint)width, (GLint)height);//重置当前视口，本身不是重置窗口的，只不过是这里被Qt给封装好了
	glMatrixMode(GL_PROJECTION);//选择投影矩阵
	glLoadIdentity();//重置选择好的投影矩阵
	// gluPerspective(45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0);//建立透视投影矩阵
	//glMatirxMode(GL_MODELVIEW);//以下2句和上面出现的解释一样
	glLoadIdentity();  
}

void ITAVPlayWidget::paintGL()
{
	//glClear()函数在这里就是对initializeGL()函数中设置的颜色和缓存深度等起作用
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();//重置当前的模型观察矩阵？不是很理解！

}

void ITAVPlayWidget::displayVideo()
{
	

	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glLoadIdentity();
	///*unsigned char* buffer_convert=new unsigned char[nWidth*nHeight*3+1];
	//memset(buffer_convert,0,nWidth*nHeight*3+1);
	//CONVERT_YUV420PtoRGB24(m_pVideoData,buffer_convert,nWidth,nHeight);
	//memset(m_pVideoData,0,nWidth*nHeight*3/2+1);
	//memmove(m_pVideoData,buffer_convert,nWidth*nHeight*3);*/
	//
	//glTexImage2D(GL_TEXTURE_2D,0,4,m_nVideoWidth,m_nVideoHeight,0,GL_RGB,GL_UNSIGNED_BYTE,m_pVideoData); //生成纹理
	//glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_nVideoWidth,m_nVideoHeight,GL_RGB,GL_UNSIGNED_BYTE,m_pVideoData); //生成纹理
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//设置纹理参数
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	////!绘制纹理
	//glBegin(GL_QUADS);
	//glTexCoord2f(0.0, 1.0);glVertex2f(-1.0f, 1.0f); //!左上角坐标（0.0,1.0）
	//glTexCoord2f(0.0, 0.0);glVertex2f(-1.0f, -1.0f);//!左下角坐标 (0.0,0.0)
	//glTexCoord2f(1.0, 0.0);glVertex2f(1.0f, -1.0f);	//!右下角坐标（1.0,0,0）
	//glTexCoord2f(1.0, 1.0);glVertex2f(1.0f, 1.0f);	
	//glEnd();
	///*glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glRasterPos3f(-1.0f,1.0f,0);*/
	////glPixelZoom((float)1366/(float)nWidth, -(float)768/(float)nHeight);
	///*glDrawPixels(m_nVideoWidth, m_nVideoHeight,GL_RGB, GL_UNSIGNED_BYTE, m_pVideoData);
	//glFlush();*/
}


void ITAVPlayWidget::paintEvent( QPaintEvent *ev )
{
	if(m_bDefaultImageShow)
	{
		int nWidth = m_defaultImage.width();
		int nHeight = m_defaultImage.height();
		QImage fixedImage(nWidth,nHeight,QImage::Format_RGB32);
		QPainter painter(&fixedImage);	
		painter.fillRect(fixedImage.rect(),Qt::transparent);
		painter.drawImage(0,0,m_defaultImage);
		painter.end();
		QPainter pat(this);
		pat.drawImage(rect(),fixedImage);
	}
	QGLWidget::paintEvent(ev);
}