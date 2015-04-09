#include  <Uefi.h>
#include  <Library/BaseLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleTextIn.h>
#include  <Protocol/SimpleTextOut.h>


#include "conio.h"
#include "cpu.h"

static struct text_info settings;

/**
 * Returns information of the screen.
 * @see text_info
 */
void gettextinfo (struct text_info * info)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;
	UINTN	Columns;
	UINTN	Rows;

	Proto = gST->ConOut;

	Proto->QueryMode(Proto, 0, &Columns, &Rows);

	memset(&settings, 0, sizeof(struct text_info));	// clear settings

	settings.screenheight = Columns;
	settings.screenwidth  = Rows;
	settings.attribute = LIGHTGRAY << 4 | LIGHTGRAY;
	settings.normattr =  LIGHTGRAY << 4 | LIGHTGRAY;

	return;
}

/**
 * Call this if you need real value of normattr attribute in the text_info
 * structure.
 * @see text_info
 */
void inittextinfo (void)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	//
	return;
}

/**
 * Clears rest of the line from cursor position to the end of line without
 * moving the cursor.
 */
void clreol (void)
{
	return;
}

/**
 * Clears whole screen.
 */
void clrscr()
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;
	
	Proto->ClearScreen(Proto);
}


/**
 * Delete the current line (line on which is cursor) and then moves all lines
 * below one line up. Lines below the line are moved one line up.
 */
void delline (void)
{
	return;
}

/**
 * Insert blank line at the cursor position.
 * Original content of the line and content of lines below moves one line down.
 * The last line is deleted.
 */
void insline (void)
{
	return;
}

/**
 * Gets text from the screen. If you haven't defined <TT>_CONIO_NO_GETTEXT_</TT>
 * prior to including <TT>conio2.h</TT> you can use this function also under
 * the <TT>gettext</TT> name.
 * @see char_info
 * @see puttext
 * @param left Left coordinate of the rectangle, inclusive, starting from 1.
 * @param top Top coordinate of the rectangle, inclusive, starting from 1.
 * @param right Right coordinate of the rectangle, inclusive, starting from 1.
 * @param bottom Bottom coordinate of the rectangle, inclusive, starting from 1.
 * @param buf You have to pass buffer of size
 * <TT>(right - left + 1) * (bottom - top + 1) * sizeof(char_info)</TT>.
 */
void conio_gettext (int left, int top, int right, int bottom,
                     struct char_info * buf)
{
 return;
}

/**
 * Puts text back to the screen.
 * @see char_info
 * @see _conio_gettext
 * @param left Left coordinate of the rectangle, inclusive, starting from 1.
 * @param top Top coordinate of the rectangle, inclusive, starting from 1.
 * @param right Right coordinate of the rectangle, inclusive, starting from 1.
 * @param bottom Bottom coordinate of the rectangle, inclusive, starting from 1.
 * @param buf You have to pass buffer of size
 * <TT>(right - left + 1) * (bottom - top + 1) * sizeof(char_info)</TT>.
 */
void puttext (int left, int top, int right, int bottom, struct char_info * buf)
{
	return;
}

/**
 * Copies text.
 * @param left Left coordinate of the rectangle, inclusive, starting from 1.
 * @param top Top coordinate of the rectangle, inclusive, starting from 1.
 * @param right Right coordinate of the rectangle, inclusive, starting from 1.
 * @param bottom Bottom coordinate of the rectangle, inclusive, starting from 1.
 * @param destleft Left coordinate of the destination rectangle.
 * @param desttop Top coordinate of the destination rectangle.
 */
void movetext (int left, int top, int right, int bottom, int destleft,
              int desttop)
{
	return;
}

/**
 * Moves cursor to the specified position.
 * @param x horizontal position
 * @param y vertical position
 */
void gotoxy(int x, int y)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	Proto->SetCursorPosition(Proto, x, y);

	return;
}

/**
 * Puts string at the specified position.
 * @param x horizontal position
 * @param y vertical position
 * @param str string
 */
void cputsxy (int x, int y, char * str)
{
	return;
}

/**
 * Puts char at the specified position.
 * @param x horizontal position
 * @param y vertical position
 * @param ch char
 */
void putchxy (int x, int y, char ch)
{
	return;
}

/**
 * Sets the cursor type.
 * @see @ref cursortypes
 * @param type cursor type, under Win32 it is height of the cursor in percents
 */
void _setcursortype (int type)
{
	return;
}

/**
 * Sets attribute of text.
 * @param _attr new text attribute
 */
void textattr (int _attr)
{
	return;
}

/**
 * Sets text attribute back to value it had after program start.
 * It uses text_info's normattr value.
 * @see text_info
 */
void normvideo (void)
{
	return;
}


/**
 * Sets text background color.
 * @see COLORS
 * @param color new background color
 */
void textbackground (int color)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	color <<= 4;

	Proto->SetAttribute(Proto, color);
}

/**
 * Sets text foreground color.
 * @see COLORS
 * @param color new foreground color
 */
void textcolor (int color)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;
	
	Proto->SetAttribute(Proto, color);
}

/**
 * Reads the cursor X position.
 * @returns cursor X position
 */
int wherex (void)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	return 0;
}

/**
 * Reads the cursor Y position.
 * @returns cursor Y position
 */
int wherey (void)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	return 0;
}


/**
 * Reads password. This function behaves like cgets.
 */
char * getpass (const char * prompt, char * str)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	return NULL;
}


/**
 * Makes foreground colors light.
 * If the current foreground color is less than <TT>DARKGRAY</TT> adds
 * 8 to the its value making dark colors light.
 * @see COLORS
 * @see lowvideo
 */
void highvideo (void)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	return;
}

/**
 * Makes foreground colors dark.
 * If the current foreground color is higher than <TT>LIGHTGRAY</TT> substracts
 * 8 from its value making light colors dark.
 * @see COLORS
 * @see highvideo
 */
void lowvideo (void)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	return;
}

/**
 * Pauses program execution for a given time.
 * @see switchbackground
 * @param ms miliseconds
 */
void delay (int ms)
{
	usleep(ms * 1000);
	return;
}

/**
 * Replaces background color in the whole window. The text however
 * is left intact. Does not modify textbackground().
 * @see flashbackground
 * @param color background color
 */
void switchbackground (int color)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	return;
}

/**
 * Changes background color for a given time and then it restores it back.
 * You can use it for visual bell. Does not modify textbackground().
 * @see switchbackground
 * @see delay
 * @param color background color
 * @param ms miliseconds
 */
void flashbackground (int color, int ms)
{
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *Proto;

	Proto = gST->ConOut;

	return;
}


/**
 * Clears the keyboard buffer.
 * To see it in effect run <TT>conio_test</TT> and try to press a key during
 * the 'Flashing...' phase.
 */
void clearkeybuf (void)
{
	EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *Proto;

	Proto = gST->ConIn;

	//Proto->Reset(Proto);

	return;
}
