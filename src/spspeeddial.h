#ifndef SPSPEEDDIAL_H
#define SPSPEEDDIAL_H

#include "ui_spdials.h"
#include <sstream>
#include <QtGui/QDialog>

class SPSpeedDial: public QDialog, public Ui::spdials {

Q_OBJECT

public:
	SPSpeedDial(QWidget* parent): QDialog(parent){
		setupUi(this);

		connect(dial, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
		setWindowTitle("Change movie speed");
		dial->setRange(0,10);
	}

	virtual ~SPSpeedDial() {};

public slots:
    void valueChanged(int s) {
    	int v = 0;
    	QString t = "vary fast";
    	if(s < 6) {
    		v = 15 * 1000 / (s + 1);
        	std::ostringstream ss;
    		ss << (4 * s + 4) << " frames/min";
    		t = ss.str().c_str();
    	} else if ( s < 10){
    		v = 500 * (10 - s);
        	std::ostringstream ss;
    		ss << 60000/v <<  " frames/min";
    		t = ss.str().c_str();
    	}
		value->setText(t);
    	settingChanged(v);
    }

	signals:
	void settingChanged(int setting);
};

#endif
