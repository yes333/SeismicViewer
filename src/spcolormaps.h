#ifndef SPCOLORMAPS_H
#define SPCOLORMAPS_H

#include <limits.h>
#include <QtTool/QTools.h>
#include <Carnac/Seismic.h>

class SPViewer;

class SPAMap {
public:
	SPAMap(const cgRgbColor& astart, const cgRgbColor& amiddle,
			const cgRgbColor& anend, const QColor& apositive,
			const QColor& anegative);

    const cgColorMap getMap();

	const QColor& getPositive() const {
		return positive;
	}

	const QColor& getNegative() const {
		return negative;
	}

private:
	cgRgbColor start;
	cgRgbColor middle;
	cgRgbColor end;
	QColor positive;
	QColor negative;
};

class SPColorMaps {

public:
    static const int MaxNumberOfColorMaps = 4;

	SPColorMaps();

	void next(SPViewer* v) {
        if (++current >= MaxNumberOfColorMaps)
			current = 0;
		update(v);
	}

	void update(SPViewer* v);

private:
	SPAMap** cmap;
	int current;
};

#endif
