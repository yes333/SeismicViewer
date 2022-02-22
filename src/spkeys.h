#ifndef SPKEYS_H
#define SPKEYS_H

#include <limits.h>
#include <QtTool/QTools.h>
#include <Carnac/Seismic.h>
#include <QtSeismic/QSeismicPlotWidget.h>

class SPViewer;

class SPKeys {

public:
	SPKeys(): shifted(false), ctrled(false) {}

	bool pressed(QKeyEvent* event, SPViewer* data);
	void released(QKeyEvent* event, SPViewer* data);
	static QString help();

private:
	bool shifted;
	bool ctrled;
};

#endif
