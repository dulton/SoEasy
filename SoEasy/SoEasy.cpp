#include "stdafx.h"
#include "SoEasy.h"

SoEasy::SoEasy(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setContextMenuPolicy(Qt::NoContextMenu);
	ui.mainToolBar->hide();
	ui.centralWidget->setLayout(&m_centerWidgetLayout);
	m_centerWidgetLayout.setContentsMargins(0,0,0,0);
	m_centerWidgetLayout.setHorizontalSpacing(0);
	m_centerWidgetLayout.setVerticalSpacing(0);
	m_centerWidgetLayout.setSpacing(0);

	//m_pPlayerWidget = new QPlayerWidget();
	//m_centerWidgetLayout.addWidget(m_pPlayerWidget);
	//m_pPlayerWidget->show();
	//ui.statusBar->hide();

//	connect(m_pPlayerWidget,SIGNAL(closeCurrentWidget()),this,SLOT(doCloseDstWidget()));
//	setWindowFlags( this->windowFlags() | Qt::Popup);	

	setWindowFlags( Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::Popup);	
	QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
	animation->setDuration(1000);
	animation->setStartValue(0);
	animation->setEndValue(1);
	animation->start();
	createSysTrayAction();
	initStatusBar();
	ui.statusBar->setFixedHeight(35);
	ui.statusBar->setStyleSheet(QString("QStatusBar{background-color:rgb(240,255,253);}QStatusBar::item {border:0px;}"));
	////该页上的同类型widget统一风格: tableView为绿底,白字,白网格线;按钮颜色为蓝绿色
	//setStyleSheet(
	//	"QTableView { color: rgb(255, 255, 255); gridline-color: rgb(255, 255, 255);}"
	//	"QPushButton { color: rgb(255,255,255); background-color: rgb(12,134,174); }"
	//	"QLabel { color: rgb(255,255,255); }"
	//	"QTableWidget{background-color:rgb(0,71,67);}");
}

SoEasy::~SoEasy()
{

}
//************************************
//函数名:  closeEvent
//描述：qt关闭事件API，只会隐藏界面，不会真正退出
//参数：关闭事件
//返回值：无
//时间：2016/3/23 WZQ
//************************************
void SoEasy::closeEvent(QCloseEvent *event)
{	
	hide();
//	QApplication::quit();
}
//************************************
//函数名:  quitAndDeleteAllInfo
//描述：真正退出程序函数
//参数：无
//返回值：无
//时间：2016/3/23 WZQ
//************************************
void SoEasy::quitAndDeleteAllInfo()
{	
	qApp->quit();	
}


void SoEasy::initStatusBar()
{
	m_updateNoteLabel = new QLabel(this);
	m_updateNoteLabel->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	m_updateNoteLabel->setText(QString("   测试："));
	ui.statusBar->addPermanentWidget(m_updateNoteLabel);

	QLabel* pLabel3 = new QLabel(this);
	pLabel3->setMinimumWidth(20);
	ui.statusBar->addPermanentWidget(pLabel3);


	QLabel* pCurUserLabel = new QLabel(this);
	pCurUserLabel->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	pCurUserLabel->setText(QString("   当前用户："));
	ui.statusBar->addWidget(pCurUserLabel);

	QLabel* pTempLabel = new QLabel(this);
	pTempLabel->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	pTempLabel->setMinimumWidth(20);
	ui.statusBar->addWidget(pTempLabel);

	m_pLabelUpdateState = new QLabel(this);
	m_pLabelUpdateState->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	ui.statusBar->addWidget(m_pLabelUpdateState);
}
//************************************
//函数名:  enterEvent
//描述：鼠标进入界面事件
//参数：事件
//返回值：无
//时间：2016/3/23 WZQ
//************************************
void SoEasy::enterEvent(QEvent *)
{
	QRect rc;
	QRect rect;
	rect = this->geometry();
	rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
	if(rect.top()<0)
	{
		rect.setX(rc.x());
		rect.setY(0);
		move(rc.x(),-2);
	}
}

void SoEasy::leaveEvent(QEvent *)
{
	QRect rc;
	QRect rect;
	rect = this->geometry();
	rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
	if(rect.top()<0)
	{
		move(rc.x(),-rc.height()+2);
	}
}

//无边框移动  通过鼠标点击拖动
void SoEasy::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton)
	{
		m_bmove=true;
		m_pressedpoint = event->globalPos();	//鼠标点击桌面位置
		m_windowspoint = this->pos();			//界面所在桌面位置
		m_oldmousepoint = m_pressedpoint-m_windowspoint;	//鼠标点击位置 在界面起始点的比较
	}
}

void SoEasy::mouseMoveEvent(QMouseEvent *event)
{
	if(m_bmove)
	{	
		move(event->globalPos()-m_oldmousepoint);		//move移动的是界面坐标 新的鼠标位置减去以前鼠标在界面的位置差 就是要移动的目标点 移动的是界面的起始坐标
	}
}
void SoEasy::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton)
	{
		m_bmove=false;
	}
}
//************************************
//函数名:  createSysTrayAction
//描述：创建系统托盘文件
//参数：无
//返回值：无
//时间：2016/3/23 WZQ
//************************************
void SoEasy::createSysTrayAction()
{
	m_pChangeFlagsAction = new QAction("变身",this);
	connect(m_pChangeFlagsAction, SIGNAL(triggered()), this, SLOT(changeWidgetFlags()));

	m_pMinimizeAction = new QAction("最小化", this);		///托盘菜单项
	connect(m_pMinimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

	m_pMaximizeAction = new QAction("最大化", this);
	connect(m_pMaximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

	m_pRestoreAction = new QAction("恢复", this);
	connect(m_pRestoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

	m_pQuitAction = new QAction("退出", this);
	connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(quitAndDeleteAllInfo()));
	m_pTrayIconMenu = new QMenu(this);				///托盘菜单
	m_pTrayIconMenu->addAction(m_pChangeFlagsAction);
	m_pTrayIconMenu->addAction(m_pMinimizeAction);
	m_pTrayIconMenu->addAction(m_pMaximizeAction);
	m_pTrayIconMenu->addAction(m_pRestoreAction);
	m_pTrayIconMenu->addSeparator();
	m_pTrayIconMenu->addAction(m_pQuitAction);

	m_pTrayIcon = new QSystemTrayIcon(this);			///托盘图标 
	connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(doIconActivated(QSystemTrayIcon::ActivationReason)));
	m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
	m_pTrayIcon->setToolTip("SoEasy");
	m_pTrayIcon->setIcon(QIcon(":/SoEasy/Resources/soeasy.png"));
	m_pTrayIcon->show();
	//	m_pTrayIcon->showMessage(QString("后台更新："), QString("正在检查最新版本信息，请稍后！"));	//显示的数据，自动几秒后消失

}
//************************************
//函数名:  doIconActivated
//描述：处理托盘图标点击后的信号 槽函数
//参数：无
//返回值：无
//时间：2016/3/23 WZQ
//************************************
void SoEasy::doIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) 
	{
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:	
		showNormal();				///恢复
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		;
	}
}


void SoEasy::doBtnClicked()
{
	QObject* obj = sender();
	/*if(obj == ui.pushButton_close)
	{
		QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
		animation->setDuration(1000);
		animation->setStartValue(1);
		animation->setEndValue(0);
		animation->start();
		connect(animation, SIGNAL(finished()), this, SLOT(close()));
	}else if(obj == ui.pushButton_min)
	{
		hide();
	}else if(obj == ui.pushButton_max)
	{
		if(isMaximized())
			showNormal();
		else
			showMaximized();
	}*/
}
void SoEasy::doCloseDstWidget()
{
	QObject* obj=sender();
	
}
void SoEasy::changeWidgetFlags()
{
	if((this->windowFlags() & Qt::WindowType_Mask) == Qt::Tool)
	{
		setWindowFlags( Qt::Window);	
	}else
		setWindowFlags( Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::Popup);	

}