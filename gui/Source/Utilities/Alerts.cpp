/*
  ==============================================================================

    Alerts.cpp
    Created: 12 Apr 2012 2:16:00am
    Author:  Peter Vasil

  ==============================================================================

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#include "../../JuceLibraryCode/JuceHeader.h"

#include "Alerts.h"

using namespace synthamodeler;

void Alerts::missingPerl()
{
	String title = "Missing perl executable";
	String msg = "Please install perl >= 5.10";
	missingAlert(title, msg);
}
void Alerts::missingSAMpreprocessor()
{
	String title = "Missing SAM-preprocessor compiler";
	String msg = "Please set the path for the DATA_DIR which consists ";
	msg << "the SAM-preprocessor script.";
	missingAlert(title, msg);
}
void Alerts::missingSAM()
{
	String title = "Missing Synth-A-Modeler compiler";
	String msg = "Please set the path for the DATA_DIR which consists ";
	msg << "the Synth-A-Modeler script.";
	missingAlert(title, msg);
}
void Alerts::missingFaust()
{
	String title = "Missing faust executable";
	String msg = "Please install faust and set path in preferences.";
	missingAlert(title, msg);
}

void Alerts::wrongFileType()
{
	String title = "You tried to open a wrong file type";
	String msg = "Open a file with the extension .mdl";
	missingAlert(title, msg);
}
void Alerts::missingAlert(const String& title, const String& msg)
{
	AlertWindow::showMessageBox(AlertWindow::WarningIcon, title, msg, "OK");
}
bool Alerts::confirmExport(const String& text)
{
	String title = "Export external";
	return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, title, text);
}

void Alerts::nameAlreadyExists()
{
    String title = "Name error";
    String msg = "Identifier already exists!";
    AlertWindow::showMessageBox(AlertWindow::WarningIcon, title, msg, "OK");
}