#ifndef SPDETAIL_H
#define SPDETAIL_H

#include "ui_spdetail.h"
#include "SPAccessors.hh"
#include "SPProcessor.hh"
#include "spsection.h"
#include <QtGui/QDialog>
#include <QtTool/QTools.h>
#include <Carnac/Seismic.h>

using namespace SP;

class SPViewer;

class SPDetail: public QDialog, public Ui::TraceDetail {

Q_OBJECT

public:
	SPDetail(SPViewer* parent): QDialog((QWidget*)parent) {
		setupUi(this);
		setWindowTitle("Trace details");
	}

	void clearFixed() {
		fixedData = "";
	}

public slots:
	void mouseReleased( QMouseEvent* me);
	void mouseMoved( QMouseEvent* me);

private :
	SPViewer* getViewer() {
		return (SPViewer*)parent();
	}
	bool findPosition(QMouseEvent* me);
	QString buildTable(segy* data, vector<string>& names,
			          vector<SPAbstractPicker*>& pickers);
	static vector<SPAbstractPicker*> getPickers(vector<string>& names);

private:
	float x;
	float y;
	QString fixedData;
	vector<string> names;
	vector<SPAbstractPicker*> pickers;
};

#endif
