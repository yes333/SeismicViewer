#include "spsection.h"
#include <algorithm>
#include <math.h>

bool SPSection::open(string fileName) {  // open .su file to read traces
	clean();
	FILE* f = fopen(fileName.c_str(), "r");
	if (!f) {
		return false;
	}
	segy tr;
	while (fgettr(f, &tr)) {
		SPSegy d(&tr);
        if (data.size() == 0) { // retrieve sample info at beginning
			dt = d.get<short> ("dt");
			ns = d.get<short> ("ns");
		}
		data.push_back(new SPSegy(d));
	}
	fclose(f);

	return true;
}

bool SPSection::useStdin() {
	clean();
	segy tr;
	while (gettr(&tr)) {
		SPSegy d(&tr);
		if (data.size() == 0) {
			dt = d.get<short> ("dt");
			ns = d.get<short> ("ns");
		}
		data.push_back(new SPSegy(d));
	}
	return true;
}

void SPSection::loadNames() { // determine trace headers with non-zero value
    if (singles.size() + multis.size() > 0) // allready loaded
		return;
    if (data.size() == 0) // no data trace
		return;

	vector<string>& names = SPSegy::getNames();
	SPPickerBox& box = SPSegy::getPicker();

	vector<SPAbstractPicker*> pickers;
	for (int i = 0; i < names.size(); ++i) {
		pickers.push_back(box[names[i]]);
	}

	int* types = new int[names.size()];
	string* values = new string[names.size()];

    for (int t = 0; t < data.size(); ++t) { //loop over traces
		void* dd = data[t]->getTrace();

        for (int i = 0; i < names.size(); ++i) { // loop over headers
            if (t == 0) {  // at first trace
				values[i] = pickers[i]->getString(dd);
				types[i] = 0;
			}
			if (types[i] == 0) {
                if (pickers[i]->getString(dd) != "0") {  // non-zero
                    types[i] = 1;  // single value
					values[i] = pickers[i]->getString(dd);
				}
			} else if (types[i] == 1) {
				if (pickers[i]->getString(dd) != values[i]) {
                    types[i] = 2;  // multiple values
				}
			}
		}
	}

	for (int i = 0; i < names.size(); ++i) {
		if (types[i] == 2) {
			multis.push_back(names[i]);
		} else if (types[i] == 1) {
			singles.push_back(names[i]);
		}
	}

    getTraceStartTime();
}

double SPSection::getTraceStartTime()   // by checking out key laga (lag time A)
{
    SPAbstractPicker* picker = SPSegy::getPicker()["delrt"];
    if ( !picker ) return 0.0;

    t_start = picker->getInt(data[0]);

    return (double) 0.001 * t_start;
}

void SPTraceMixer::setRange()
{
    _minimum = _minDet;
    _maximum = _maxDet;
    _binSize = _binDet;
    rescale(true);
}

void SPTraceMixer::detRange(const int ntr, SPSection& section)
{
    key2trcNo.clear();

    SPAbstractPicker* picker = SPSegy::getPicker()[_axis];

    int v;
    for (int i = 0; i < ntr; ++i) {  // loop over traces
        v = i;  // default value = trace sequence no
        if (picker != 0) {
            //v[i] = picker->getDouble(section[i]);
            v = picker->getInt(section[i]);
        }

        key2trcNo[v] = i;  // sorted automatically by v
    }

    int ntr_uniq = key2trcNo.size();
    map<int, int>::iterator itr = key2trcNo.begin();
    map<int, int>::reverse_iterator rit = key2trcNo.rbegin();
    _minDist = (double) itr->first;
    _maxDist = (double) rit->first;
    double range = _maxDist - _minDist;
    _avgGap = (ntr_uniq > 1)? range / ( ntr_uniq - 1 ) : 1;

    _minGap = range;
    _maxGap = 0.0;
    double d1 = _minDist;
    for (itr++; itr != key2trcNo.end(); itr++) {
        double gap = (double) itr->first - d1;
        if (gap < _minGap) _minGap = gap;
        if (gap > _maxGap) _maxGap = gap;
        d1 = (double) itr->first;
    }

    _binSizeD = (_minGap < 0.2*_avgGap )? 0.2 * _avgGap : _minGap; // bin size > 20%

    _binDet = _binSizeD;
    _minDet = _minDist;
    _maxDet = _maxDist;
}

bool SPTraceMixer::mixTrace() {
    reverse.clear();
    forward.clear();

    map<int, int>::iterator itr;

    for (itr = key2trcNo.begin(); itr != key2trcNo.end(); itr++) {
        int m = ((double) itr->first - _minimum + _binSize * 0.5) / _binSize; // mix trace index in new/mix section
        if (src_index(m) == -1) { // if not found in the map
            reverse[m] = itr->second;         // put into the map
            forward[itr->second] = m;   // add mix index to array
        }
    }
    ostringstream os;

    os << "X-axis field: " << _axis << " --> ";
    os << _ntrSource << " source, " << reverse.size() << " binned traces with bin size " << _binSize;
    //os.precision(3);
    //os.setf(std::ios::fixed, std::ios::floatfield);
    os << ", min/max spacing "<< _minGap << "/" << _maxGap;
    statusMsg(os.str());

    int m = (_maximum - _minimum) / _binSize + 1; //number trace on mix section
    return ( m == reverse.size() );
}

void SPTraceMixer::setTraceRange(SPSection& section)
{
    section.trace_origin( _xScale * (_minimum - _xOffset) );
    section.trace_step( _xScale * _binSize );
}

void SPTraceMixer::section(const cgSeismicSection &s) {
	SPSection& section = (SPSection&) s;
    _ntrSource = section.total_traces();
    if (_ntrSource == 0) {
        statusMsg("no source trace found!");
        return;
    }

    detRange(_ntrSource, section);
    setRange();
    mixTrace();

    setTraceRange(section);

    _singleNames = section.singleValueNames();
	_multiNames = section.multipleValueNames();

    return;
}

int SPTraceMixer::traceNumber(float x) {
    int nMixTraceIndex = (int) (x / _xScale - _minimum + _xOffset + 0.5*_binSize) / _binSize;

    return src_index(nMixTraceIndex);

}
