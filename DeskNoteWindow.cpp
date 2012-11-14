
// File: $RCSFile$
// Revision: $Revision: 1.1 $
// Date: $Date: 2011/03/20 21:57:39 $
// DeskNoteWindow the code.

#include "DeskNoteWindow.h"

DeskNoteWindow::DeskNoteWindow (BRect rect):BWindow (rect,
	"DeskNotes", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE)
{
	// B_TITLED_WINDOW_LOOK

	myView = new DeskNoteView (Bounds());

	AddChild (myView);

}


void DeskNoteWindow::SaveNote (BMessage *msg)
{
	myView -> SaveNote (msg);
}


void DeskNoteWindow::RestoreNote (BMessage *msg)
{
	myView -> RestoreNote (msg);
}


bool DeskNoteWindow::QuitRequested ()
{
	BMessage msg (B_QUIT_REQUESTED);

	be_app -> PostMessage (&msg);
	return false;
}

