#ifndef SPHELP_H
#define SPHELP_H

#include "ui_spdetail.h"
#include "spkeys.h"
#include <QtGui/QDialog>

class SPHelp: public QDialog, public Ui::TraceDetail {

Q_OBJECT

public:
	SPHelp(QWidget* parent): QDialog(parent) {
		setupUi(this);
		setWindowTitle("Key bindings");
		QString t = SPKeys::help();
		browser->clear();
		browser->insertHtml(t);
	}
};

#endif
