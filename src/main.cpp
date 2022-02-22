#include "spviewer.h"
#include <qapplication.h>
#include "SPProcessor.hh"

using namespace SP;

// make SU doc happy
const char
		* sdoc[] = {
                "SUVIEWER – show trace data graphically from files",
				"",
                " suviewer [optional parameters]",
				"",
				" Required parameter:",
				"",
				"      none",
				"",
				" Optional parameters:",
				"",
				"      source= Path to the data file(s). The path can lead to either",
				"              a file containing Segy traces, or a directory with",
				"              files containing Segy data inside. These files must",
				"              have names ending with \".su\". If the path lead to a",
				"              directory, the module enters movie mode. All data",
				"              files in the directory are listed as frames in the ",
				"              movie. The order is strictly by file names.",
				"      autorun= false. If autorun is set to true and the path leads",
				"              to a directory, the module automatically starts the",
				"              movie show.",
				"      [anything] to start the viewer following SU standard, the",
				"              command without any parameters will be interpreted as",
				"              a call for help, and this help text will be shown.",
				"              To start the viewer without any real parameters,",
				"              therefore, you need to add something in addition to",
                "              \"suviewer\" in the command line.",
				"",
				" Note: if the module is started with a pipe to its standard input",
				"       it will read the pipe as a data file and show that file",
				"",
				" Interactive controls:",
				"",
				"      The viewer has direct control for all its activities in",
				"      its graphical user interface. These controls reside in",
				"      pop up dialogs that can be activated from the main window.",
				"      The following are brief descriptions of these dialogs:",
				"",
				"           1. The main window. The main window has a menu bar",
				"              and a tool bar for activating the dialogs. (The",
				"              dialogs have their own close buttons.) It also",
				"              contains the area for showing the trace data",
				"              graphically.This area has its own scaling and",
				"              scrolling controls. In addition, it has a status",
				"              bar for simple data about the trace currently",
				"              under the mouse pointer.",
				"",
				"           2. Open file dialog. This is the standard dialog for",
				"              opening a data file. Once a file selection completes,",
				"              the main window will display the traces in that file.",
				"              If the selection is a directory, all files in the",
				"              directory are regarded as SU data files and a movie",
				"              is ready to play.",
				"",
				"           3. Plot properties editor. This dialog has the fields",
				"              for the control of the trace display in the main",
				"              window.",
				"",
				"           4. Seismic properties editor. This dialog lets user set",
				"              and change the properties of the way data is shown.",
				"",
				"           5. Color map definition dialog. Changes the color",
				"              assignment of the signal amplitudes.",
				"",
				"           6. Trace details display. This is a dialog box that shows",
				"              details about a trace user selected. It is updated each",
				"              time the user clicks on a trace.",
				"",
				"           7. Movie control toolbar. This toolbar contains the usual",
				"              control elements to steer the movie show such as",
				"              forward/backward, speed control, single stepping, etc.",
				"",
				"           8. Print control dialog. Lets user to control the printable",
				"              file generation for the plot in display.",
				"",
                " Feature highlight: change amplitudes interactively by ctrl+>|<",
                "",
                "",
                " Note: header keyword 'delrt' is reserved to set trace starting time in ms",
                "",
                "",
				" Examples:",
				"",
				"      start the viewer for pure interactive use:",
				"",
                "        suviewer a",
				"",
				"      create printable files for all data files in a directory",
				"",
                "        suviewer source=qadata/frames print=ps(qadata/prints) \\",
                "                 control=autoclose",
                "",
                "      start a movie with all files in a directory",
                "",
                "        suviewer source=qadata/frames control=autostart",
                "",
                "",
                " Version 1.1, last modified by Sanyu Ye, July 2020, Oslo Norway",
                "", 0 };

int main(int argc, char** argv) {
	SPProcessor args;
	args.initParam(argc, argv);
	SPSegy::initAccessor();
	cout << filestat(STDIN) << " / " << PIPE << endl;
	if (args.getParameterNames().size() == 0 && filestat(STDIN) == 0) {
		int i = 0;
		while (sdoc[i] != 0) {
			cout << sdoc[i++] << endl;
		}
		exit(0);
	}

    Application app(argc, argv);

	SPViewer mainwindow;
	mainwindow.show();
	mainwindow.setArgs(&args);
	app.connect(&app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

	return app.exec();
}
