#ifndef SPVIEWER_H
#define SPVIEWER_H

#include "ui_spviewer.h"
#include "spprinter.h"
#include "spprojector.h"
#include "spdetail.h"
#include "sphelp.h"
#include "spkeys.h"
#include "spsection.h"
#include "spcolormaps.h"
#include "spaxis.h"
#include "SPProcessor.hh"

#include <QMessageBox>
#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>
#include <QtCore/QStringList>
#include <QtTool/QTools.h>
#include <Carnac/Seismic.h>

using namespace SP;

class SPDetail;


/*****************************************************************************/
/*             This is to catch exceptions during event handling             */
/*****************************************************************************/

class Application: public QApplication
{
   Q_OBJECT


public:
   Application(int & c, char **v): QApplication( c, v ) {

   }

   bool notify( QObject * rec, QEvent * ev ) {
       try {
           return QApplication::notify( rec, ev );
       }

       catch( cgAssertionError & err ) {
           err.put( cerr );
           abort();
       }

       catch( SPException & err ) {
           cerr << "SPException: " << err.what() << "--> " << endl;
           abort();
       }

       catch (...) {
           cerr << "Unknown exception!" << endl;
           abort();
       }
       return true;
   }
};


class SPViewer: public QMainWindow, public Ui::ViewerMain {

Q_OBJECT

public:
	SPViewer(QWidget * parent = 0)																							;

void	setArgs(SPProcessor* a);

	cgQSeismicPlotWidget* getDataDisplay() {
		return seisdata;
	}

	cgSeismicSection& getData() {
		return segy_section;
	}

	SPProjector* getProjector() {
		return projector;
	}

	int getTrace(float x) {
		return mixer->traceNumber(x);
	}

	float getPosition(float x) {
        return x * mixer->binSize() + mixer->minimum();  // wrong
	}

	void keyPressEvent ( QKeyEvent * event ) {
		if(!keys->pressed(event, this)) {
			QMainWindow::keyPressEvent(event);
		}
	}

	void keyReleaseEvent ( QKeyEvent * event ) {
		keys->released(event, this);
	}

	cgQRect getSize() {
		return seisdata->scrollableLimits();
	}

	cgQRect getScale() {
		return seisdata->visibleLimits();
	}

	void setScale(cgQRect s) {
		seisdata->visibleLimits(s);
	}

	const vector<string>& getStatusBarFields() {
        return axis->getFields();
	}

public slots:

	void fileExit() {
		QCoreApplication::exit();
	}

    void optionsSeismicProperties() {
		seisdata->editSeismicProperties();
	}

	void optionsColormap() {
		seisdata->editColorMap();
	}

	void optionsPlotProperties() {
		seisdata->editPlotProperties();
	}

	void zoomOut() {
		seisdata->zoomToFit(cgQtPlot::FM_Both);
	}

	void showDetails() {
		details->show();
	}

    void movieControl(bool on) {
        movieBar->setEnabled(on);
    }

	void resetScale() {
		keepScale = false;
	}

    void onAxisRange() {
        segy_section.equal_spacing(true); //reset upon key change
        mixer->section(segy_section);
        mixer->setRange();
        mixer->mixTrace();
        statusBar()->showMessage(mixer->statusMsg().c_str());
    }

    void Mix() {
        mixer->section(segy_section);
        mixer->setRange();
        mixer->mixTrace();
        seisdata->seismicSection(section_mixer);
        seisdata->zoomToFit(cgQtPlot::FM_Both);
    }

    void onRemix() {
        bool trcSpacingEqual = mixer->mixTrace();
        bool densityDisplay = seisdata->densityType() != cgQSeismicModelBase::NoDensity ;
        statusBar()->showMessage(mixer->statusMsg().c_str());
        QMessageBox::StandardButton reply = QMessageBox::No;
        if (!trcSpacingEqual && densityDisplay) {
            reply = QMessageBox::question(this, "Uneven trace spacing",
                "Unable to display in dennsity mode\n\nSwitch to wiggle/area display?",
                QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    segy_section.equal_spacing(false);
                    seisdata->showWiggle( true );
                    seisdata->showNegativeFill( true );
                    seisdata->showPositiveFill( true );
                    seisdata->densityType( cgQSeismicModelBase::NoDensity );
                }
        } else {
            reply = QMessageBox::Yes;
        }
        if (reply == QMessageBox::Yes) {
            mixer->setTraceRange(segy_section);
            seisdata->seismicSection(section_mixer);
            seisdata->zoomToFit(cgQtPlot::FM_Both);
        }
    }


    void onAbout()
    {
        QString msg;
        msg.append("Version 1.1, last modified July 2020\n").append("Authors: Sanyu Ye & Lin Li\n");
        msg.append("Use allowed only for non-commercial purposes\n").append("sanyu.ye@outlook.com");
        QMessageBox::about( this, "About suViewer", msg);
    }

    void onKnownBugs()
    {
        QString msg;
        msg.append("1. no plotting unevenly spaced seismic when density (variable colors) display is enabled.\n");
        msg.append("2. resizing (enlarging) display window may not result in redrawing seismic.\n");
        msg.append("3. amplitude changes by shortcut keys ctrl->/<- only work for wiggles in trace-normalized display.\n");
        QMessageBox::information( this, "Konwn bugs", msg);
    }

    void onShowStatus(const string &msg){

        statusBar()->showMessage(msg.c_str());
}

    void onHorizontalFlipToggled()
    {
        bool flip = action_Horizontal_flip->isChecked();
        seisdata->horizontalFlip( flip );
    }

    void onVerticalFlipToggled()
    {
        bool flip = action_Vertical_flip->isChecked();
        seisdata->verticalFlip( flip );
    }

    void parityFlip() {
        bool flip = action_Parity_flip->isChecked();
        seisdata->reverseAmplitude( flip );
        //seisdata->activeVisual()->reverseAmplitude(flip);
        seisdata->redrawSeismic();
	}

	void horizontalGrid() {
		seisdata->showHorizontalGrid( seisdata->showHorizontalGrid() == cgQXYPlotBase::NoItems ?
				cgQXYPlotBase::MajorItems: cgQXYPlotBase::NoItems);
	}

	void colorMap() {
		cmaps->next(this);
	}

    void resizeEvent(QResizeEvent *event) {
        if (QApplication::mouseButtons() == Qt::LeftButton) {
        QSize oldSize = event->oldSize();
        QSize newSize = event->size();
        cgQRect scale = getScale();
        seisdata->zoomToFit(cgQtPlot::FM_Both);
        seisdata->redrawSeismic();
        cgQRect new_scale = getScale();
        return;
        }
    }

	void changeDisplayMaterial();
	void refresh(const QString& fileName);
	void dataPrint(QString name);
	void popNew();

private:
	void init();

private:
	SPSection segy_section;

	SPPrinter* printer;
	SPProjector* projector;
	SPDetail* details;
	SPHelp* help;
	SPKeys* keys;
	SPColorMaps* cmaps;
	bool keepScale;
	int fillMode;

	SPAxis* axis;
	SPTraceMixer* mixer;
	cgSeismicSectionMixer* section_mixer;

	QString command;
};

#endif
