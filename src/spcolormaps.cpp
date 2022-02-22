#include "spcolormaps.h"
#include "spviewer.h"

SPAMap::SPAMap(const cgRgbColor& astart, const cgRgbColor& amiddle,
		const cgRgbColor& anend, const QColor& apositive,
		const QColor& anegative) :
	start(astart), middle(amiddle), end(anend), positive(apositive),
			negative(anegative) {
}

// only 4 color bars/maps available
// ToDo let user choose interactively from a list of colar bars which are displayed
SPColorMaps::SPColorMaps() {
    current = 0;
    cmap = new SPAMap*[MaxNumberOfColorMaps];
    // greyscale with blue positive fill and red negative fill
    cmap[0] = new SPAMap(cgRgbColor(1., 1., 1.), cgRgbColor(0.5, 0.5, 0.5), cgRgbColor(
            0., 0., 0.), QColor(0, 0, 255), QColor(255, 0, 255));
    // orange - white - black
    cmap[1] = new SPAMap(cgRgbColor(1.0, 0.5, 0.0), cgRgbColor(1., 1., 1.), cgRgbColor(
            0., 0., 0.), QColor(0, 0, 255), QColor(255, 0, 255));
    cmap[2] = new SPAMap(cgRgbColor(0., 1., 0.), cgRgbColor(1., 1., 1.), cgRgbColor(
            0., 1., 1.), QColor(0, 0, 255), QColor(0, 255, 0));
    cmap[3] = new SPAMap(cgRgbColor(0., 0., 1.), cgRgbColor(1., 1., 1.), cgRgbColor(
            0., 0., 0.), QColor(255, 0, 255), QColor(0, 0, 0));
}

const cgColorMap SPAMap::getMap() {
    int size = 128;
    cgColorMap cmap = cgColorRamp(size, middle, start);
    cgColorRamp second(size, end, middle);
    cmap.merge(second, size*2);
    return cmap;
}

void SPColorMaps::update(SPViewer* v) {
	SPAMap& m = *cmap[current];
	cgQSeismicPlotWidget& d = *v->getDataDisplay();

    cgColorMap ref = m.getMap();
    d.colorMap(ref);
	d.positiveFillColor(m.getPositive());
	d.negativeFillColor(m.getNegative());
}
