#ifndef SPFRAMEDIAL_H
#define SPFRAMEDIAL_H

#include "ui_spdials.h"
#include <iostream>
#include <sstream>
#include <QtGui/QDialog>

using namespace std;

class SPFrameDial: public QDialog, public Ui::spdials {

Q_OBJECT

public:
	SPFrameDial(QWidget* parent): QDialog(parent){
		setupUi(this);

		connect(dial, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
		setWindowTitle("Select movie frame");
	}

	void setSelect(int sel, const QString& title) {
		if(sel != dial->value()) {
			dial->setValue(sel);
		}
		ostringstream ss;
		ss << sel + 1 << "/" << dial->maximum() + 1 << ": " << title.toStdString();
		value->setText(ss.str().c_str());
	}

	void setLimit(int limit) {
		dial->setRange(0, limit);
	}

public slots:
    void valueChanged(int value) {
    	settingChanged(value);
    }

	signals:
	void settingChanged(int setting)								;
};

#endif
