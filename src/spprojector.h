#ifndef SPPROJECTOR_H
#define SPPROJECTOR_H

#include "ui_spprinter.h"
#include "spspeeddial.h"
#include "spframedial.h"
#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtCore/QTimerEvent>
#include <QtGui/QFileDialog>
#include <QtCore/QString>
#include <QtTool/QTools.h>

class SPViewer;

class SPProjector: public QObject {

Q_OBJECT

public:
	SPProjector(QWidget* parent);
	static QString translate(QString directory, QString name, QString pattern);

	void timerEvent(QTimerEvent* e);

	void fileOpen(QString name);
	void fileOpenDir(QString name);

	QString getCurrentFile();

public slots:
	void dataPrint(bool all, QString directory, QString pattern);

	void fileOpen();
	void fileOpenDir();


	void runForward() {
		forward = true;
		setTimer(true);
	}

	void runBackward() {
		forward = false;
		setTimer(true);
	}

	void oneForwardM() {
		setTimer(false);
		if(!showFile(current + 1)){
			showFile(0);
		}
	}

	void oneForward() {
		setTimer(false);
		showFile(current + 1);
	}

	void oneBackwardM() {
		setTimer(false);
		if(!showFile(current - 1)){
			last();
		}
	}

	void oneBackward() {
		setTimer(false);
		showFile(current - 1);
	}

	void stop() {
		setTimer(false);
	}

	void first() {
		setTimer(false);
		showFile(0);
	}

	void last() {
		setTimer(false);
		QStringList s = getFiles();
		showFile(s.size() - 1);
	}

	void select(int newFrame) {
		showFile(newFrame);
	}

	void setSpeed(int s) {
		speed = s;
		setTimer(timer != -1);
	}


	void popSpeed() {
		speedDial->show();
	}

	void popFrame();

	signals:
	void dataPrint(QString fileName);
	void movieControl(bool on);
	void refresh(const QString& name);
	void resetScale();

private:
	bool showFile(int i);
	void setTimer(bool restart);
	QStringList getFiles();

private:
	int current;
	int timer;
	int speed;
	bool forward;
	QString name;

	QString dir;
	QString pattern;

	QWidget* parent;
	SPSpeedDial* speedDial;
	SPFrameDial* frameDial;
};

#endif
