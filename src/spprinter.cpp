#include "spprinter.h"
#include <cstdlib>

using namespace std;

SPPrinter::SPPrinter(QWidget* parent) :
	QDialog(parent) {
	setup = false;

	setupUi(this);

	connect(Print1, SIGNAL( clicked() ), this, SLOT( printOne() ));
	connect(PrintAll, SIGNAL( clicked() ), this, SLOT( printAll() ));
	connect(SaveDefault, SIGNAL( clicked() ), this, SLOT( save() ));
	connect(DirSelect, SIGNAL( clicked() ), this, SLOT( selectPath() ));

	QString d(getenv("HOME"));
	d += "/tmp";
	DirPath->setText(d);
}

void SPPrinter::quickPrint() {
	if (!setup) {
		print();
	} else {
		printOne();
	}
}

void SPPrinter::print() {
	this->show();
}

void SPPrinter::printAll() {
	this->hide();
	setup = true;
	emit dataPrint(true, DirPath->text(), FileNamePattern->text());
}

void SPPrinter::printOne() {
	this->hide();
	setup = true;
	emit dataPrint(false, DirPath->text(), FileNamePattern->text());
}

void SPPrinter::save() {
	this->hide();
	setup = true;
}

void SPPrinter::selectPath() {
	QString name = QFileDialog::getExistingDirectory(this, "Open a data file",
			DirPath->text());
	DirPath->setText(name);
}
