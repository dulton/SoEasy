#ifndef SOEASY_H
#define SOEASY_H

#include <QtWidgets/QMainWindow>
#include "ui_SoEasy.h"
#include <QGridLayout>
//#include "QPlayerWidget.h"

class SoEasy : public QMainWindow
{
	Q_OBJECT

public:
	SoEasy(QWidget *parent = 0);
	~SoEasy();

	void createSysTrayAction();
	void initStatusBar();
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void closeEvent(QCloseEvent *event);
	
public slots:
	void doBtnClicked();
	void doIconActivated(QSystemTrayIcon::ActivationReason reason);
	void quitAndDeleteAllInfo();
	void doCloseDstWidget();
	void changeWidgetFlags();
private:
	Ui::SoEasyClass ui;
	QGridLayout			m_centerWidgetLayout;
//	QPlayerWidget*		m_pPlayerWidget;
	bool				m_bmove;
	QPoint				m_pressedpoint;
	QPoint				m_windowspoint;
	QPoint				m_oldmousepoint;
	QAction*			m_pMinimizeAction;		//最小号按钮
	QAction*			m_pMaximizeAction;		//最大化按钮
	QAction*			m_pRestoreAction;		//恢复按钮
	QAction*			m_pChangeFlagsAction;	//变身按钮
	QAction*			m_pQuitAction;			//退出按钮
	QSystemTrayIcon*	m_pTrayIcon;			//系统托盘图标
	QMenu *				m_pTrayIconMenu;		//系统托盘菜单

	QLabel* m_updateNoteLabel;	
	QLabel* m_pLabelUpdateState;
};

#endif // SOEASY_H
