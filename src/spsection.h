#ifndef SPSECTION_H
#define SPSECTION_H

#include <limits.h>
#include <header.h>
#include <SPProcessor.hh>
#include <vector>
#include <map>
#include <Carnac/Seismic.h>

using namespace SP;

class SPTrace: public cgSeismicTraceInterface {
public:
	SPTrace(SPSegy* parent, cgTraceType type) :
		segy(parent), type(type) {
	}

	SPTrace(SPSegy* parent) :
		segy(parent), type(CG_TR_NORMAL) {
	}

	~SPTrace() {
	}

    cgTraceType trace_type() const {
		return type;
	}

    cgSampleType sample_type() const {
		return CG_ST_IEEE_FLOAT;
	}

	int size() const {
		return segy->get<short> ("ns");
	}

    const void* sample(int start, int n) const {
		return segy->getValue() + start;
	}

private:
	cgTraceType type;
	SPSegy* segy;
};

class SPSection: public cgSeismicSection {

public:

	SPSection() :
        ns(0), dt(0), t_start(0), _trace_origin(0.), _trace_step(1.),
        _eqSpacing(true) {
	}

	~SPSection() {
	}

	bool open(string fileName);
	bool useStdin();

	void clean() {
		for (int i = 0; i < data.size(); ++i)
			delete data[i];
		data.clear();
	}

	segy* operator[](int index) {
		return data[index]->getTrace();
	}

    const cgSeismicTraceInterface& allocate_trace(int index) {
        return *(new SPTrace(data[index]));
    }

	void free_trace(const cgSeismicTraceInterface& t) {
		delete &t;
	}

	int total_traces() const {
		return data.size();
	}

	int total_samples() const {
		return ns;
	}

	int total_headers() const {
        return HDRBYTES;
	}

    cgSampleUnits sample_units() const {
		return CG_SU_TIME;
	}

	double sample_rate() const {
		return 0.000001 * dt;
	}

    double  sample_origin () const {
        return 0.001 * t_start;
    }

	cgSampleType sample_type() const {
		return CG_ST_IEEE_FLOAT;
	}

    double trace_origin() const {
        return _trace_origin;
    }
    void trace_origin(double tr_origin){
        _trace_origin = tr_origin;
        return;
    }

    double trace_step() const {
        return _trace_step;
    }

    void trace_step(double tr_step){
        _trace_step = tr_step;
        return;
    }

    bool equal_spacing() const {
        return _eqSpacing;
    }

    void equal_spacing(bool equal){
        _eqSpacing = equal;
        return;
    }

    const cgSeismicHeaderValue* header(int index) const {  // dummy interface function
        return 0;
        //return (cgSeismicHeaderValue*) (data[index]->getValue());
    }

	vector<string>& singleValueNames() {
		loadNames();
		return singles;
	}

    double getTraceStartTime();

	vector<string>& multipleValueNames() {
		loadNames();
		return multis;
	}

private:
	void loadNames();

private:
	vector<SPSegy*> data;
	int ns;
	int dt;
    int t_start;

    double _trace_origin;
    double _trace_step;

    bool _eqSpacing;

	vector<string> singles;
	vector<string> multis;
};

class SPTraceMixer: public cgSeismicTraceMixer {
public:

	SPTraceMixer() :
        _axis(""), _minimum(0.), _maximum(0.), _ntrSource(0),
        _binSize(0.0), _binDet(1.0), _binSizeD(0.0),
        _xScale(1.), _xOffset(0.), _rescale(true) {
	}

	int total_traces() const {
        return (int) ( (_binSize > 0. ? (_maximum - _minimum )
                       : (_maximum - _minimum )) / _binSize + 1 );
	}

	int src_index(int index) const {
		map<int, int>::const_iterator i = reverse.find(index);
		return i == reverse.end() ? -1 : i->second;
	}

	int mix_index(int index) const {
        map<int, int>::const_iterator i = forward.find(index);
        return i == forward.end() ? -1 : i->second;
    }

	void axis(string anAxis) {
		_axis = anAxis;
	}

	const string& axis() const {
		return _axis;
	}

    void minimum(double aMinimum) {
		_minimum = aMinimum;
	}

    const double minimum() const {
        return _minimum;
	}

    void maximum(double aMaximum) {
		_maximum = aMaximum;
	}

    const double maximum() const {
        return _maximum;
	}

    void binSize(double aBinSize) {
		_binSize = aBinSize;
	}

    const double binSize() const {
        return _binSize;
	}

    void minDet(double aMinDet) {
		_minDet = aMinDet;
	}

    const double minDet() const {
		return _minDet;
	}

    void maxDet(double aMaxDet) {
		_maxDet = aMaxDet;
	}

    const double maxDet() const {
		return _maxDet;
	}

    void binDet(double aBinDet) {
		_binDet = aBinDet;
	}

    const double binDet() const {
        return _binDet;
	}

    void xScale(double xScaleFactor) {
        if (xScaleFactor != 0.0) _xScale = xScaleFactor;
    }

    const double xScale() const {
        return _xScale;
    }

    void xOffset(double xOffset) {
        _xOffset = xOffset;
    }

    const double xOffset() const {
        return _xOffset;
    }

    void rescale(bool aRescale) {
        _rescale = aRescale;
    }

    void statusMsg(const string& msg) {
        _statusMsg = msg;
    }

    const string statusMsg() {
        return _statusMsg;
    }

    const bool rescale() const {
        return _rescale;
    }

    const vector<string>& multiNames() {
		return _multiNames;
	}

	const vector<string>& singleNames() {
		return _singleNames;
	}

void section(const cgSeismicSection &s);
	int traceNumber(float x);

    void detRange(const int ntr, SPSection& section);
    bool mixTrace();

    void setRange();

    void setTraceRange(SPSection& section);

private:
    string _axis;  // header name whose values are used for H-axis
    double _minimum;  // display area nominal range
    double _maximum;
    double _binSize;
    double _xScale;
    double _xOffset;
    bool _rescale;

    int    _ntrSource;  //real data trace on input section
    double _minDet; // trace header's real value range
    double _maxDet;
    double _binDet;

    double _binSizeD;
    double _avgGap, _minGap, _maxGap;  // between  adjacent trace header values
    double _minDist, _maxDist;  // range of trace header values

    map<int, int> key2trcNo;  // map  header key value --> real trace index
    map<int, int> reverse;  // map  binned index --> real trace index
    map<int, int> forward; // array of binned trace index correspond to real trace index
	vector<string> _singleNames;
	vector<string> _multiNames;
    string _statusMsg;
};

class SPTickGen: public cgAxisTickGenerator {
public:
	SPTickGen();
	Iterator enumerate(cgDim, double, bool = false) const;
	Iterator enumerate(const cgRange &range,
			const cgRange &limits, double res, bool reverse = false) const;
};
#endif
