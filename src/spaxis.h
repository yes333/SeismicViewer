#ifndef SPAXIS_H
#define SPAXIS_H

#include "ui_spaxis.h"
#include "spsection.h"
#include "spsection.h"
#include <QtGui/QDialog>
#include <QtTool/QTools.h>

using namespace SP;

class SPAxis: public QDialog, public Ui::AxisDefinition {

Q_OBJECT

public:
	SPAxis(QWidget* parent, SPTraceMixer* m);


public slots:
    void loaded(const bool isLoaded) {
        _isLoaded = isLoaded;
    }

    const bool loaded() {
        return _isLoaded;
    }

    void onSelectionChanged(const QString& xAxisField);

	void apply();

	void pop() {
        loaded(false);
		show();
        LoadAxisInfo();
        loaded(true);
	}

	void close() {
		hide();
	}

	void ok() {
		apply();
		hide();
	}

	const vector<string>& getFields() {
		return statuses;
	}

	void removeItem(QListWidgetItem * item);
	void addItem(QListWidgetItem * item);

	signals:
    void getAxisRange();
    void remix();

private:
    void LoadAxisInfo();
    void loadAxisInfoFromMixer();
    void LoadStatusBarInfo();
    void UpdateAxisInfoToMixer();

private:
    bool _isLoaded;

	SPTraceMixer* mixer;
	vector<string> statuses;
};

#endif
