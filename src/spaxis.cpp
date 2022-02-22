#include "spaxis.h"
#include "spviewer.h"
#include <algorithm>
#include <SPBaseUtil.hh>
#include <Geometry/Coords.h>
#include <hdr.h>
#include <header.h>

using namespace SP;

SPAxis::SPAxis(QWidget* parent, SPTraceMixer* m) :
    QDialog(parent), mixer(m), _isLoaded(false) {
	setupUi(this);

	connect(OKButton, SIGNAL( clicked() ), this, SLOT(ok()));
	connect(ApplyButton, SIGNAL( clicked() ), this, SLOT(apply()));
    connect(CancelButton, SIGNAL( clicked() ), this, SLOT(close()));
    connect(XAxisSelection, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onSelectionChanged(const QString &)));

	connect(StatusBarFields, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(removeItem(QListWidgetItem *)));
	connect(StatusBarFieldSelect, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(addItem(QListWidgetItem *)));
	statuses.push_back("offset");
	statuses.push_back("sx");
}

void SPAxis::LoadStatusBarInfo()
{
    StatusBarFields->clear();
    StatusBarFieldSelect->clear();

    vector<string> w = mixer->multiNames();
    w.insert(w.end(), mixer->singleNames().begin(), mixer->singleNames().end());
    for (int i = 0; i < w.size(); ++i) {
        if (std::find(statuses.begin(), statuses.end(), w[i]) != statuses.end()) {
            StatusBarFields->addItem(w[i].c_str());
        } else {
            StatusBarFieldSelect->addItem(w[i].c_str());
        }
    }
}

void SPAxis::loadAxisInfoFromMixer()
{
    MinValueField->setText(SP::cat(mixer->minimum()).c_str());
    MaxValueField->setText(SP::cat(mixer->maximum()).c_str());
    BinSizeField->setText(SP::cat(mixer->binSize()).c_str());
    MinValueDetected->setText(SP::cat(mixer->minDet()).c_str());
    MaxValueDetected->setText(SP::cat(mixer->maxDet()).c_str());
    BinSizeAssigned->setText(SP::cat(mixer->binDet()).c_str());
    XOffsetField->setText(SP::cat(mixer->xOffset()).c_str());
    XScaleField->setText(SP::cat(mixer->xScale()).c_str());
}

void SPAxis::LoadAxisInfo() {  // initial loading from trace/section mixer
    // load fields from mixer
    loadAxisInfoFromMixer();

	string a = mixer->axis();
	if (a == "") {
		a = "traceNumber";
	}

	XAxisSelection->clear();
	const vector<string>& v = mixer->multiNames();
	XAxisSelection->addItem("traceNumber");
	for (int i = 0; i < v.size(); ++i) {
		XAxisSelection->addItem(v[i].c_str());
		if (v[i].c_str() == a) {
			XAxisSelection->setCurrentIndex(i + 1);
		}
	}

    LoadStatusBarInfo();
}

void SPAxis::UpdateAxisInfoToMixer() {
    bool ok;
    double x = BinSizeField->text().toDouble(&ok);
    if (ok && (x != 0.0)) {
        mixer->binSize(x);
    }
    x = MinValueField->text().toDouble(&ok);
    if (ok) {
        mixer->minimum(x);
    }
    x = MaxValueField->text().toDouble(&ok);
    if (ok) {
        mixer->maximum(x);
    }
    QString a = XAxisSelection->currentText();
    if (a == "traceNumber") {
        a = "";
    }
    x = XOffsetField->text().toDouble(&ok);
    if (ok) {
        mixer->xOffset(x);
    }
    x = XScaleField->text().toDouble(&ok);
    if (ok) {
        mixer->xScale(x);
    }
}

void SPAxis::onSelectionChanged(const QString& xAxisField) {

    cerr << "Axis field name changed to " << string(xAxisField.toAscii()) << endl;

    if(!loaded()) return;

    if (xAxisField == "traceNumber") {
        mixer->axis("");
    }
    else {
        mixer->axis(xAxisField.toStdString());
    }

    emit getAxisRange();

    loadAxisInfoFromMixer();

    if(xAxisField == "offset") {
        XScaleField->setText("0.001");
    }
    else {
        XScaleField->setText("1.0");
    }

    return;
}


void SPAxis::apply() {
    loaded(false);
    UpdateAxisInfoToMixer();
    emit remix();
    LoadAxisInfo();
    loaded(true);

}

void SPAxis::removeItem(QListWidgetItem * item) {
	for (int i = 0; i < statuses.size(); ++i) {
		if (statuses[i] == item->text().toStdString()) {
			statuses.erase(statuses.begin() + i);
            LoadAxisInfo();
			return;
		}
	}
}

void SPAxis::addItem(QListWidgetItem * item) {
	statuses.push_back(item->text().toStdString());
    LoadAxisInfo();
}
