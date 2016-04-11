#ifndef QPLAYER_H
#define QPLAYER_H

#include <QtWidgets/QMainWindow>
#include "ui_QPlayer.h"
#include "ITAVPlayWidget.h"
#include <QToolBar>
#include <QAction>

class QPlayer : public QMainWindow
{
	Q_OBJECT

public:
	QPlayer(QWidget *parent = 0);
	~QPlayer();

	void initAction();
	void setupUi();
signals:
	void closeCurrentWidget();
protected:
	QString GetFileVirsion(QString aFileName);
public slots:
	void doActionClicked();
private:
	Ui::QPlayerClass ui;
	QGridLayout		m_centerWidgetLayout;
	ITAVPlayWidget* m_pPlayWidget;
	QAction*		m_pOpenAction;
	QAction*		m_pAboutAction;
	QAction*		m_pCloseAction;


};

#endif // QPLAYER_H
