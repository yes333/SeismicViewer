#include "spkeys.h"
#include "spviewer.h"
#include "spprojector.h"
#include "string.h"

double move(double t, double r, double d) {
	double x = d / 20;
	if (r + x >= t) {
		x = t - r;
	}
	return x;
}

const std::string bindings[] = { "shift-Up", "zoom-in in Y direction",//
		"shift-Down", "zoom-out in Y direction",//
		"shift-Right", "zoom-in in X direction",//
		"shift-Left", "zoom-out in X direction",//
		"shift-Space", "zoom to fit entire plot",//
		"", "separator",//
		"ctrl-Up", "increase clip factor",//
		"ctrl-Down", "decrease clip factor",//
		"ctrl-Left", "increase amplitude scale",//
		"ctrl-Right", "decrease  amplitude scale",//
		"ctrl-Space", "change display mode",//
		"ctrl-N", "open a duplicate window",//
		"", "separator",//
		"Up", "move view window up",//
		"Down", "move view window down",//
		"Right", "move view window right",//
		"Left", "move view window left",//
		"", "separator",//
		"PageDown", "next frame in the movie",//
		"PageUp", "previous frame in the movie",//
		"Home", "first frame in the movie",//
		"End", "last frame in the movie",//
		"", "" };

bool SPKeys::pressed(QKeyEvent* event, SPViewer* data) {
	int k = event->key();
	if (k == Qt::Key_Control) {
		ctrled = true;
		shifted = false;
		return true;
	} else if (k == Qt::Key_Shift) {
		ctrled = false;
		shifted = true;
		return true;
	}

	if (shifted) {
		cgQSeismicPlotWidget* w = data->getDataDisplay();
		if (k == Qt::Key_Up) {
			w->zoom(1.0, 1.2);
		} else if (k == Qt::Key_Down) {
			w->zoom(1.0, 0.8);
		} else if (k == Qt::Key_Left) {
			w->zoom(0.8, 1.0);
		} else if (k == Qt::Key_Right) {
			w->zoom(1.2, 1.0);
		} else if (k == Qt::Key_Space) {
			w->zoomToFit(cgQtPlot::FM_Both);
		} else {
			return false;
		}
	} else if (ctrled) {
		cgQSeismicPlotWidget* w = data->getDataDisplay();
		if (k == Qt::Key_Right) {
            double scaleA = w->scale();
            scaleA *= 1.25;
            w->scale(scaleA);

            //int NormMode = w->normalizationMode();
            //double normFactor = w->normFactor();

            cgRange normRange = w->normLimits();
            normRange.low  *= 0.8;
            normRange.high *= 0.8;
            w->normLimits(normRange);
		} else if (k == Qt::Key_Left) {
			w->scale(0.8 * w->scale());
            cgRange normRange = w->normLimits();
            normRange.low  *= 1.25;
            normRange.high *= 1.25;
            w->normLimits(normRange);
        } else if (k == Qt::Key_Up) {
			w->clipType(cgQSeismicModelBase::ClipWiggleAndFill);
			w->clipFactor(1.2 * w->clipFactor());
		} else if (k == Qt::Key_Down) {
			w->clipType(cgQSeismicModelBase::ClipWiggleAndFill);
			w->clipFactor(0.8 * w->clipFactor());
		} else if (k == Qt::Key_Space) {
			data->changeDisplayMaterial();
		} else if (k == Qt::Key_N) {
			data->popNew();
		} else {
			return false;
		}
	} else {
		cgQRect r = data->getScale();
		cgQRect t = data->getSize();
		double d;
		if (k == Qt::Key_Down) {
			d = move(t.top(), r.top(), r.top() - r.bottom());
			r.top(r.top() + d);
			r.bottom(r.bottom() + d);
			data->setScale(r);
		} else if (k == Qt::Key_Up) {
			d = move(r.bottom(), t.bottom(), -r.top() + r.bottom());
			r.top(r.top() + d);
			r.bottom(r.bottom() + d);
			data->setScale(r);
		} else if (k == Qt::Key_Right) {
			d = move(r.left(), t.left(), -r.left() + r.right());
			r.left(r.left() + d);
			r.right(r.right() + d);
			data->setScale(r);
		} else if (k == Qt::Key_Left) {
			d = move(t.right(), r.right(), +r.left() - r.right());
			r.left(r.left() + d);
			r.right(r.right() + d);
			data->setScale(r);
		} else if (k == Qt::Key_PageDown) {
			data->getProjector()->oneForwardM();
		} else if (k == Qt::Key_PageUp) {
			data->getProjector()->oneBackwardM();
		} else if (k == Qt::Key_Home) {
			data->getProjector()->first();
		} else if (k == Qt::Key_End) {
			data->getProjector()->last();
		} else {
			return false;
		}
	}
	return true;
}

void SPKeys::released(QKeyEvent* event, SPViewer* data) {
	int k = event->key();
	if (k == Qt::Key_Control || k == Qt::Key_Shift) {
		ctrled = false;
		shifted = false;
	}
}

QString SPKeys::help() {
	ostringstream c;

	c << "<br><b>Key bindings defined for the main window</b><hr>" << endl;
	c << "<table border=0 cellpadding='2px' width='90%'><tr>" << endl;
	int i = 0;
	for (;;) {
		const string* k = bindings + i++;
		const string* v = bindings + i++;
		if(*k == "") {
			if(*v == "") {
				c << "</table>" << endl;
				return QString(c.str().c_str());
			} else {
				c << "<tr><td>&nbsp;</td><td>&nbsp;</dt></tr>" << endl;
			}
		} else {
			c << "<tr><td><tt>" << *k << "</tt></td><td>" << *v << "</dt></tr>" << endl;
		}
	}
}
