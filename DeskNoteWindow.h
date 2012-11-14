
// The DeskNoteWindow header file.

#ifndef _DESKNOTEWINDOW_H
#define _DESKNOTEWINDOW_H

#include <Window.h>
class DeskNoteView;

class DeskNoteWindow : public BWindow
{
	public:
		DeskNoteWindow (BRect rect);

		virtual void SaveNote (BMessage *msg);
		virtual void RestoreNote (BMessage *msg);
		bool QuitRequested ();

	private:
		DeskNoteView *myView;

};

#include "DeskNoteView.h"

#endif

