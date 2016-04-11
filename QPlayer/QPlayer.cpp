#include "stdafx.h"
#include "QPlayer.h"
#include "ITPublic.h"

QPlayer::QPlayer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.centralWidget->setLayout(&m_centerWidgetLayout);
	m_centerWidgetLayout.setContentsMargins(0,0,0,0);
	m_centerWidgetLayout.setHorizontalSpacing(0);
	m_centerWidgetLayout.setVerticalSpacing(0);
	m_centerWidgetLayout.setSpacing(0);
	ui.mainToolBar->hide();
	ui.statusBar->hide();
	
	m_pPlayWidget = new ITAVPlayWidget;
	m_centerWidgetLayout.addWidget(m_pPlayWidget);
}

QPlayer::~QPlayer()
{

}


void QPlayer::initAction()
{	
	setContextMenuPolicy(Qt::NoContextMenu);
	ui.mainToolBar->setIconSize(QSize(20,20));	
	m_pOpenAction = ui.mainToolBar->addAction(QIcon(":/SoEasy/Resources/open_av.png"),"打开");	
	ui.mainToolBar->addSeparator();
//	m_pHelpAction = ui.mainToolBar->addAction(QIcon(":/MITSCameraTool/Resources/help.png"),"帮助");
	m_pAboutAction = ui.mainToolBar->addAction(QIcon(":/SoEasy/Resources/about.png"),"关于");		


	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui.mainToolBar->addWidget(spacer);
	m_pCloseAction = ui.mainToolBar->addAction(QIcon(":/SoEasy/Resources/close_av.png"),"退出");		

	/*m_pDBWidget = new MITSDBWidget;
	ui.mainToolBar->addWidget(m_pDBWidget);
	m_pDBWidget->show();	*/
	connect(m_pOpenAction,SIGNAL(triggered()),this,SLOT(doActionClicked()));
	connect(m_pAboutAction,SIGNAL(triggered()),this,SLOT(doActionClicked()));	
	connect(m_pCloseAction,SIGNAL(triggered()),this,SLOT(doActionClicked()));	

}

void QPlayer::doActionClicked()
{
	QObject* obj = sender();
	bool dbAction=false;
	if(obj == m_pOpenAction)
	{	

	}else if(obj == m_pAboutAction)
	{		
		QMessageBox msgBox(this);
		msgBox.setWindowTitle(QString("关于"));
	//	msgBox.setStyleSheet(QString("background-color:rgb(0,71,67);color:#ffffff;font-size:14px;"));		
		QString versionstr = GetFileVirsion(QCoreApplication::applicationFilePath());
		msgBox.setText(QString("版本信息：%1 \n").arg(versionstr));
		QPushButton* btn = msgBox.addButton(QString("确定"),QMessageBox::AcceptRole);
		//btn->setStyleSheet(QString("QPushButton{;border:1px solid rgb(0,55,50);border-radius:4px;color:#ffffff;}QPushButton::hover{background-color:rgb(0,204,195);}QPushButton::pressed{background-color:rgb(0,97,93);}"));
		btn->setMaximumSize(65,25);
		btn->setMinimumSize(65,25);		
		msgBox.exec();	
	}else if(obj == m_pCloseAction)
	{		
		emit closeCurrentWidget();
	}
}

QString QPlayer::GetFileVirsion(QString aFileName)
{
	QString vRetVersion;
	std::string vFileName = aFileName.toStdString();
	char vVersion[256] = {0};
	DWORD vHandle = 0;
	DWORD vInfoSize = 0;
	vInfoSize = ::GetFileVersionInfoSize(ANSITOUNICODE1(vFileName.c_str()),&vHandle);
	if(vInfoSize <= 0)
	{
		return "获取版本信息失败";
	}
	char* pData = new char[vInfoSize + 1];
	memset(pData,0,vInfoSize+1);
	if(!::GetFileVersionInfo(ANSITOUNICODE1(vFileName.c_str()),NULL,vInfoSize,pData))
	{
		if(pData)
			delete pData;
		return "获取版本信息失败";
	}
	std::string vTempStr = "\\";
	VS_FIXEDFILEINFO* pFileInfo;
	unsigned int vUInfoSize = 0;
	if(!::VerQueryValue(pData,ANSITOUNICODE1(vTempStr.c_str()),(void**)&pFileInfo,&vUInfoSize))
	{
		if(pData)
			delete pData;
		return "获取版本信息失败";
	}
	WORD vVer[4];
	vVer[0] = HIWORD(pFileInfo->dwProductVersionMS);
	vVer[1] = LOWORD(pFileInfo->dwProductVersionMS);
	vVer[2] = HIWORD(pFileInfo->dwProductVersionLS);
	vVer[3] = LOWORD(pFileInfo->dwProductVersionLS);
	sprintf(vVersion,"%d.%d.%d.%d",vVer[0],vVer[1],vVer[2],vVer[3]);
	if(pData)
		delete pData;
	std::string vVerStr = vVersion;
	vRetVersion = QString::fromStdString(vVerStr);
	return vRetVersion;
}

