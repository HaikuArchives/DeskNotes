/*
 * Original copyright 2000 by Colin Stewart (http://www.owlfish.com/).
 * All rights reserved.
 * Distributed under the terms of the BSD (3-clause) License.
 *
 * Authors:
 *		Janus2, 2015
 *
 */


#ifndef _DESKNOTEWINDOW_H
#define _DESKNOTEWINDOW_H

#include <Window.h>
class DeskNoteView;

class DeskNoteWindow : public BWindow {
public:
					DeskNoteWindow(BRect rect);

	virtual void 	SaveNote(BMessage* msg);
	virtual void 	RestoreNote(BMessage* msg);
	bool 			QuitRequested();

private:
	DeskNoteView* 	myView;
};

#include "DeskNoteView.h"

#endif // _DESKNOTEWINDOW_H
