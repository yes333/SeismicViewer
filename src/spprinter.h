#ifndef SPPRINTER_H
#define SPPRINTER_H

#include "ui_spprinter.h"
#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QFileDialog>
#include <QtCore/QStringList>
#include <QtTool/QTools.h>

class SPPrinter: public QDialog, public Ui::spPrinterDialog {

Q_OBJECT

public:
	SPPrinter(QWidget* viewer);

	bool wholeArea() {
		return AreaSelect->currentIndex() == 0;
	}

	bool postscript() {
		return FormatSelect->currentIndex() == 1;
	}

public slots:
	void print();
	void quickPrint();

	void printAll();
	void printOne();
	void save();
	void selectPath();

	signals:
	void dataPrint(bool all, QString directory, QString pattern);

private:
	bool setup;
};

#endif
