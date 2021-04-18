/*
 * Original copyright 2000 by Colin Stewart (http://www.owlfish.com/).
 * All rights reserved.
 * Distributed under the terms of the BSD (3-clause) License.
 *
 * Authors:
 *		Janus2, 2015
 *
 */


#include "DeskNoteWindow.h"


DeskNoteWindow::DeskNoteWindow(BRect rect)
	:
	BWindow(rect, "DeskNotes", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		B_ASYNCHRONOUS_CONTROLS)
{
	myView = new DeskNoteView(Bounds());

	AddChild(myView);
}


void
DeskNoteWindow::SaveNote(BMessage* msg)
{
	myView->SaveNote(msg);
}


void
DeskNoteWindow::RestoreNote(BMessage* msg)
{
	myView->RestoreNote(msg);
}


bool
DeskNoteWindow::QuitRequested()
{
	BMessage msg(B_QUIT_REQUESTED);

	be_app->PostMessage(&msg);
	return false;
}
