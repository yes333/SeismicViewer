#include "spdetail.h"
#include "spviewer.h"
#include <Geometry/Coords.h>
#include <hdr.h>
#include <header.h>

using namespace SP;

void SPDetail::mouseReleased(QMouseEvent* e) {
	if (!isVisible() || !findPosition(e)) {
		return;
	}

	int d = getViewer()->getTrace(x);
	if (d < 0) {
		return;
	}
	SPSection& sec = (SPSection&) getViewer()->getData();

	if (fixedData == "") {
		vector<SPAbstractPicker*> ps = getPickers(sec.singleValueNames());
		fixedData = buildTable(sec[d], sec.singleValueNames(), ps);
		names = sec.multipleValueNames();
		pickers = getPickers(names);
	}
	browser->clear();
	browser->insertHtml("<br><b>Frame fixed header values</b><hr>");
	browser->insertHtml(fixedData);
	browser->insertHtml("<br><br><br><b>Trace dependent header values</b><hr>");
	browser->insertHtml(buildTable(sec[d], names, pickers));
	browser->insertHtml("<br><br><br><b>Other header values are always 0</b>");
}

void SPDetail::mouseMoved(QMouseEvent* e) {
	if (!findPosition(e)) {
		return;
	}

	SPSection& sec = (SPSection&) getViewer()->getData();
	int d = getViewer()->getTrace(x);
	ostringstream c;

	if (d > -1) {
		const vector<string>& fields = getViewer()->getStatusBarFields();
		for (int i = 0; i < fields.size(); ++i) {
			string n = fields[i];
			string v = SPSegy::getPicker()[n]->getString(sec[d]);
			c << n << "=" << v << "; ";
		}
		int sample = (int) (y / sec.sample_rate());
		double v = *((float*) sec.trace(d).sample(sample, 0));
        c << "trace = " << d << ": (" << x << ", "
				<< y << ", " << v << ")";
	} else {
		c.str("");
	}
	getViewer()->statusBar()->showMessage(c.str().c_str());
}

bool SPDetail::findPosition(QMouseEvent* e) {
	cgQSeismicPlotWidget * d = getViewer()->getDataDisplay();
	cgCrd pt = d->viewToContents(cgQPoint(e->pos()));

	if (!(pt < d->visibleLimits())) {
		return false;
	}
	x = pt.x;
	y = pt.y;
	return true;
}

QString SPDetail::buildTable(segy* data, vector<string>& names, vector<
		SPAbstractPicker*>& pickers) {
	ostringstream c;

	c << "<table border=0 cellpadding='2px'><tr>" << endl;
	for (int i = 0; i < names.size(); ++i) {
		c << "<td align='right' width='10%'>" << names[i] << ":&nbsp;</td>";
		c << "<td width='20%'><b><tt>" << pickers[i]->getString(data);
		c << "</tt></b></td>" << endl;
		if (!((i + 1) % 3)) {
			c << "</tr><tr>" << endl;
		}
	}
	c << "</tr></table>" << endl;
	return QString(c.str().c_str());
}

vector<SPAbstractPicker*> SPDetail::getPickers(vector<string>& names) {
	SPPickerBox& box = SPSegy::getPicker();
	vector<SPAbstractPicker*> res;

	for (int i = 0; i < names.size(); ++i) {
		res.push_back(box[names[i]]);
	}
	return res;
}
