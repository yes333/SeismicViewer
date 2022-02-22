#include "spprojector.h"
#include <QFileInfo>
#include <QDir>

SPProjector::SPProjector(QWidget* p) :
	QObject(p) {

	speedDial = new SPSpeedDial(p);
	frameDial = new SPFrameDial(p);

	connect(frameDial, SIGNAL(settingChanged(int)), this, SLOT(select(int)));
	connect(speedDial, SIGNAL(settingChanged(int)), this, SLOT(setSpeed(int)));

	current = -1;
	timer = -1;
	speed = 1000;
	name = "";
	forward = true;

	dir = "";
	pattern = "";
	parent = p;
}

QString SPProjector::translate(QString dir, QString name, QString pattern) {
	QString n = name.section("/", -1).section(".", 0);
	return dir + "/" + pattern.replace("*", n);
}

QString SPProjector::getCurrentFile() {
	if(name == "") {
		return "";
	}
	QFileInfo fi(name);
	if(!fi.exists()) {
		return "";
	}
	if(!fi.isDir()) {
		return name;
	}
	QStringList s = getFiles();
	if(current < 0 || current >= s.size()) {
		return "";
	}
	return s[current];
}

void SPProjector::fileOpen() {
	QString title = "Open a data file";
	QString name = QFileDialog::getOpenFileName(parent, title, QString::null);
	if (!name.isEmpty()) {
		fileOpen(name);
	}
}

void SPProjector::fileOpen(QString n) {
	name = n;
	current = 0;
	setTimer(false);
	movieControl(false);
	resetScale();
	refresh(name);
}

void SPProjector::fileOpenDir() {
	QString title = "Open a directory with movie data files";
	QString name = QFileDialog::getExistingDirectory(parent, title, QString::null);
	if (!name.isEmpty()) {
		fileOpenDir(name);
	}
}

void SPProjector::fileOpenDir(QString n) {
	name = n;
	setTimer(false);
	forward = true;
	current = -1;
	movieControl(true);
	resetScale();
	oneForward();
}

void SPProjector::timerEvent(QTimerEvent* e) {
	if (e && e->timerId() == timer) {
		showFile(current + (forward ? 1 : -1));
	}
}

void SPProjector::dataPrint(bool all, QString directory, QString pattern) {
	if (!all) {
		dataPrint(translate(directory, name, pattern));
		return;
	}
	QStringList s = getFiles();
	for (int i = 0; i < s.size(); ++i) {
		refresh(s[i]);
		dataPrint(translate(directory, s[i], pattern));
	}
}

bool SPProjector::showFile(int i) {
	QStringList s = getFiles();
	if (i < 0 || i >= s.size()) {
		if (speed == 0) {
			speed = 1000;
		}
		return false;
	}
	current = i;
	frameDial->setSelect(i, s[i]);
	refresh(s[i]);
	return true;
}

QStringList SPProjector::getFiles() {
	QDir d(name);
	d.setFilter(QDir::Files);
	QStringList s = d.entryList();
	QStringList res;
	for (int i = 0; i < s.size(); ++i) {
		if(s[i].endsWith(".su")) {
		  res.append(name + "/" + s[i]);
		}
	}
	frameDial->setLimit(res.size() - 1);
	return res;
}

void SPProjector::setTimer(bool restart) {
	if (timer != -1) {
		killTimer(timer);
		timer = -1;
	}
	if (!restart) {
		return;
	}
	timer = startTimer(speed);
	QTimerEvent* e = new QTimerEvent(timer);
	QApplication::sendEvent(this, e);
}

void SPProjector::popFrame() {
	showFile(current);
	setTimer(false);
	frameDial->show();
}
