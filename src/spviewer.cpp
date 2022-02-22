#include "spviewer.h"
#include <string>
#include <QByteArray>
#include <QProcess>
#include <QFileInfo>
#include <QtDraw/QCgmExport.h>
#include <QtDraw/QExport.h>
#include <QtDraw/QPostscriptExport.h>
#include <Plot/Plot.h>

using namespace std;

SPViewer::SPViewer(QWidget* parent) :
	QMainWindow(parent) {
	setupUi(this);

	printer = new SPPrinter(this);
	projector = new SPProjector(this);
	details = new SPDetail(this);
	help = new SPHelp(this);
	keys = new SPKeys();
	mixer = new SPTraceMixer();
	section_mixer = new cgSeismicSectionMixer(segy_section, *mixer);
	axis = new SPAxis(this, mixer);
	cmaps = new SPColorMaps();

	connect(actionExit, SIGNAL( activated() ), this, SLOT( fileExit() ));

	connect(actionOpen, SIGNAL( activated() ), projector, SLOT( fileOpen() ));
	connect(actionOpen_Movie, SIGNAL( activated() ), projector, SLOT( fileOpenDir() ));
	connect(actionPrint, SIGNAL( activated() ), printer, SLOT( print() ));
	connect(actionQuickPrint, SIGNAL( activated() ), printer, SLOT( quickPrint() ));
	connect(action_Duplicate, SIGNAL( activated() ), this, SLOT(popNew()));

    connect(action_Seismic_data_setting, SIGNAL( activated() ), this, SLOT(optionsSeismicProperties()));
	connect(action_Plot_setting, SIGNAL( activated() ), this, SLOT(optionsPlotProperties()));
	connect(action_Color_map_setting, SIGNAL( activated() ), this, SLOT(optionsColormap()));
	connect(action_Trace_details, SIGNAL( activated() ), this, SLOT(showDetails()));
	connect(actionConfig_axis, SIGNAL( activated() ), axis, SLOT( pop() ));

	connect(action_Zoom_out, SIGNAL( activated() ), this, SLOT( zoomOut() ));
	connect(action_Mode_switch, SIGNAL( activated() ), this, SLOT( changeDisplayMaterial() ));
    connect(action_Vertical_flip, SIGNAL( activated() ), this, SLOT( onVerticalFlipToggled() ));
    connect(action_Horizontal_flip, SIGNAL( activated() ), this, SLOT( onHorizontalFlipToggled() ));
    connect(action_Parity_flip, SIGNAL( activated() ), this, SLOT( parityFlip() ));
    connect(action_Horizontal_grid, SIGNAL( activated() ), this, SLOT( horizontalGrid() ));
	connect(action_Color_map, SIGNAL( activated() ), this, SLOT( colorMap() ));

	connect(action_Forward, SIGNAL( activated() ), projector, SLOT( runForward() ));
	connect(action_Backward, SIGNAL( activated() ), projector, SLOT( runBackward() ));
	connect(action_Stop, SIGNAL( activated() ), projector, SLOT( stop() ));
	connect(actionForward_1, SIGNAL( activated() ), projector, SLOT( oneForwardM() ));
	connect(action_1, SIGNAL( activated() ), projector, SLOT( oneBackwardM() ));
	connect(actionTo_Beginning, SIGNAL( activated() ), projector, SLOT( first() ));
	connect(actionTo_End, SIGNAL( activated() ), projector, SLOT( last() ));
	connect(action_Select_frame, SIGNAL( activated() ), projector, SLOT( popFrame() ));
	connect(actionSet_s_peed, SIGNAL( activated() ), projector, SLOT( popSpeed() ));

    connect(action_Shortcuts, SIGNAL( activated() ), help, SLOT( show() ));
    connect(action_About, SIGNAL( activated() ), this, SLOT( onAbout() ));
    connect(action_Known_bugs, SIGNAL( activated() ), this, SLOT( onKnownBugs() ));

	connect(printer, SIGNAL(dataPrint(bool, QString, QString)), projector, SLOT(dataPrint(bool, QString, QString)));

	connect(projector, SIGNAL(movieControl(bool)), this, SLOT(movieControl(bool)));
	connect(projector, SIGNAL(dataPrint(QString)), this, SLOT(dataPrint(QString)));
	connect(projector, SIGNAL(refresh(const QString&)), this, SLOT(refresh(const QString&)));
	connect(projector, SIGNAL(resetScale()), this, SLOT(resetScale()));

	connect(seisdata, SIGNAL( mouseReleased( QMouseEvent* ) ), details, SLOT( mouseReleased( QMouseEvent* ) ) );
    connect(seisdata, SIGNAL( mouseMoved( QMouseEvent* ) ), details, SLOT( mouseMoved( QMouseEvent* ) ) );
    //connect(seisdata, SIGNAL( resizeEvent(QResizeEvent *event) ), this, SLOT( onResize(QResizeEvent *event) ) );

    //connect(mixer, SIGNAL(showStatus(const string& msg), this, SLOT(onShowStatus(const string& msg)));

    connect(axis, SIGNAL(getAxisRange()), this, SLOT(onAxisRange()));
    connect(axis, SIGNAL(remix()), this, SLOT(onRemix()));

	init();
}

void SPViewer::init() {
	keepScale = false;

	//Use CarnacQtDriver
	seisdata->driverType(cgQWidgetBase::CarnacQtDriver);

    seisdata->setScale( 5, cgQSeismicPlotWidget::TracesPerCm, 2, cgQSeismicPlotWidget::CmPerSecond);

    cmaps->update(this);
    //set SeismicPlotWidget Enum properties since these don't get set from QtDesigner.
    fillMode = 2;  //no vriable density, only wiggle fill
    seisdata->showWiggle( true );
    seisdata->showNegativeFill( true );
    seisdata->showPositiveFill( true );
    //seisdata->densityType( cgQSeismicModelBase::NoDensity );
    seisdata->densityType( cgQSeismicVisualInterface::InterpolatedDensity );
    seisdata->bitmapType( cgQSeismicModelBase::TrueColor );
    seisdata->colorInterpolation( cgQSeismicModelBase::Quadratic );
    seisdata->wiggleInterpolation( cgQSeismicModelBase::Linear );
    seisdata->normalizationMode( cgQSeismicModelBase::TraceMaximum );
    seisdata->showVerticalGrid( cgQXYPlotBase::NoItems );
    seisdata->showHorizontalGrid( cgQXYPlotBase::NoItems );

    cgColorMap cmap = cgColorRamp( 128, cgRgbColor( 1., 1., 1. ), cgRgbColor( 0., 0., 0. ) );
    cgColorRamp second( 127, cgRgbColor( 1., 0.5, 0. ), cgRgbColor( 1., 1., 1. ) );
    cmap.merge( second, 254 );
    seisdata->colorMap( cmap );

	seisdata->setTool(new cgQZoomPanTool(seisdata, false));

	seisdata->setProperty("leftTitle", "Time (second)");
	//seisdata->setProperty("rightTitle", "");
	seisdata->showRightTitle(false);
	seisdata->setProperty("topTitle", "Frame: ");
	//seisdata->setProperty("bottomTitle", "");
	seisdata->showBottomTitle(false);

	seisdata->getAxis(cgQXYPlotBase::Left)->desiredWidth(50);
	seisdata->getAxis(cgQXYPlotBase::Top)->desiredHeight(50);
	seisdata->getAxis(cgQXYPlotBase::Right)->desiredWidth(5);
	seisdata->getAxis(cgQXYPlotBase::Right)->showMajorTicks(false);
	seisdata->getAxis(cgQXYPlotBase::Bottom)->desiredHeight(28);
    seisdata->getAxis(cgQXYPlotBase::Bottom)->showMajorLabels(false);
	seisdata->getAxis(cgQXYPlotBase::Bottom)->showMajorTicks(false);       
    seisdata->getAxis(cgQXYPlotBase::Left)->tickGenType(cgQSimpleAxisBase::AdaptiveLinear);
	seisdata->getAxis(cgQXYPlotBase::Left)->majorTickStep( 40 );

}

void SPViewer::refresh(const QString& file) {
	if(file.length() > 0) {
	QByteArray name;
	name.append(file.toAscii());
	name.append('\0');

	setWindowTitle(QString("SPViewer -- ") + file);
	if (!segy_section.open(name.data())) {
		cout << "failed to open " << file.toStdString() << endl;
		return;
	}
	} else {
		if (!segy_section.useStdin()) {
			cout << "failed to read data from standard in" << endl;
			return;
		}
	}
	details->clearFixed();

	cgQRect scale = getScale();
    cmaps->update(this);
    Mix();  // display

	if (keepScale) {
		setScale(scale);
	} else {
		seisdata->zoomToFit(cgQtPlot::FM_Both);
        //keepScale = true;
	}

	cgQSimpleAxisBase* a = seisdata->getAxis(cgQXYPlotBase::Top);
        a->tickGenType(cgQSimpleAxisBase::AdaptiveLinear);
	//a->majorTickGen(new cgAxisAdaptiveLinearTickGenerator(100.0, 10000.0));
	a->majorTickStep( 40 );
	cmaps->update(this);
}

void SPViewer::popNew() {
	QString s = projector->getCurrentFile();
	QStringList p;
	if (s != "") {
		p << QString("source=") + s;
	} else {
		p << "a";
	}
	QProcess::startDetached(command, p);
}

void SPViewer::setArgs(SPProcessor* a) {
	command = a->getCommand().c_str();

	string source = a->getStringParameter("source");
        
	string control = a->getStringParameter("autorun");

	if ( source != "") {
            cerr << " source=" << "\"" << source << "\"" << endl;
//#ifdef _DEBUG
	} else	if (filestat(STDIN) != 0) {
            cerr << " Assuming input from pipe " << endl;      
		refresh("");
		return;
//#endif
	} else if (source == "") {
            cerr << " no source, run interactive" << endl;
		return;
	}

	QString name(source.c_str());
	QFileInfo fi(name);
	if (!fi.exists()) {
		return;
	}
        
	if (fi.isDir()) {
        movieBar->setEnabled(true);
		projector->fileOpenDir(name);
		if (control == "true") {
			projector->runForward();
		}
	} else {
		projector->fileOpen(name);
        movieBar->setEnabled(false);
	}
}

void SPViewer::changeDisplayMaterial() {
    if (!segy_section.equal_spacing()) {
        fillMode = 2;
        QMessageBox::information(this, "Uneven trace spacing", "Only wiggle display availlable");
        return;
    }

	++fillMode;
    if(fillMode > 2) {
		fillMode = 0;
	}
    if(fillMode == 0) {  // wiggle + density
		seisdata->showWiggle(true);
		seisdata->showNegativeFill(true);
		seisdata->showPositiveFill(true);
		seisdata->densityType(cgQSeismicModelBase::InterpolatedDensity);
    } else if(fillMode == 1) {  // density only
		seisdata->showWiggle(false);
		seisdata->showNegativeFill(false);
		seisdata->showPositiveFill(false);
		seisdata->densityType(cgQSeismicModelBase::InterpolatedDensity);
    } else { // wiggle only
		seisdata->showWiggle(true);
		seisdata->showNegativeFill(true);
		seisdata->showPositiveFill(true);
		seisdata->densityType(cgQSeismicModelBase::NoDensity);
	}
}


void SPViewer::dataPrint(QString filename) {

    if (printer->wholeArea()) {
        zoomOut();
    }
    cgQPoint pt0 = seisdata->contentsToView(cgQPoint(0, 0));
    cgQPoint pt1 = seisdata->contentsToView(cgQPoint(segy_section.trace_step(),
            segy_section.sample_rate()));

    cgQPoint scale = pt1 - pt0;

    seisdata->exportLimits(getScale());
    seisdata->exportScale(scale.x, cgQSeismicPlotWidget::PixelsPerTrace,
            scale.y, cgQSeismicPlotWidget::PixelsPerSample);

    QString ending = printer->postscript() ? ".ps" : ".cgm";
    if (!filename.endsWith(ending)) {
        filename.append(ending);
    }

    QByteArray name;
    name.append(filename.toAscii());
    name.append('\0');

    if (printer->postscript()) {
        cgQPostscriptExport(seisdata, name, 1.0);
    } else {
        cgQCgmExport(seisdata, name, 72, false);
    }
}
