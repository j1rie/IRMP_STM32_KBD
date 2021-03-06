/*
 *  GUI Config Tool for IRMP STM32 KBD devices
 *
 *  Copyright (C) 2015-2018 Joerg Riechardt
 *
 *  based on work by Alan Ott
 *  Copyright 2010  Alan Ott
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <fx.h>

#include "hidapi.h"
//#include "mac_support.h"
#include <limits.h>
#include <inttypes.h>
#include <FXArray.h>
#include "icons.h"
#include "usb_hid_keys.h"

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#ifdef _WIN32
	#pragma warning(disable:4996)
#endif

enum __attribute__ ((__packed__)) access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum __attribute__ ((__packed__)) command {
	CMD_CAPS,
	CMD_ALARM,
	CMD_IRDATA,
	CMD_KEY,
	CMD_WAKE,
	CMD_REBOOT,
	CMD_IRDATA_REMOTE,
	CMD_WAKE_REMOTE,
	CMD_REPEAT
};

enum __attribute__ ((__packed__)) status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

class MainWindow : public FXMainWindow {
	FXDECLARE(MainWindow)
	
public:
	enum {
		ID_FIRST = FXMainWindow::ID_LAST,
		ID_CONNECT,
		ID_DISCONNECT,
		ID_RESCAN,
		ID_REBOOT,
		ID_SEND_OUTPUT_REPORT,
		ID_PWAKEUP,
		ID_PMACRO,
		ID_PKEY,
		ID_PREPEAT,
		ID_PRWAKEUP,
		ID_PRMACRO,
		ID_GWAKEUP,
		ID_GMACRO,
		ID_GKEY,
		ID_GREPEAT,
		ID_GCAP,
		ID_GEEPROM,
		ID_PEEPROM,
		ID_AGET,
		ID_ASET,
		ID_RWAKEUP,
		ID_RMACRO,
		ID_RKEY,
		ID_RREPEAT,
		ID_RALARM,
		ID_CLEAR,
		ID_MAC_TIMER,
		ID_LAST,
		ID_WSLISTBOX,
		ID_RSLISTBOX,
		ID_OPEN,
		ID_SAVE,
		ID_DEVLIST
	};
	
private:
	FXList *device_list;
	FXButton *connect_button;
	FXButton *disconnect_button;
	FXButton *rescan_button;
	FXButton *output_button;
	FXButton *reboot_button;
	FXButton *pwakeup_button;
	FXButton *pmacro_button;
	FXButton *pkey_button;
	FXButton *prepeat_button;
	FXButton *prwakeup_button;
	FXButton *prmacro_button;
	FXButton *gwakeup_button;
	FXButton *gmacro_button;
	FXButton *gkey_button;
	FXButton *grepeat_button;
	FXButton *gcap_button;
	FXButton *aget_button;
	FXButton *aset_button;
	FXButton *rwakeup_button;
	FXButton *rmacro_button;
	FXButton *rkey_button;
	FXButton *rrepeat_button;
	FXButton *ralarm_button;
	FXButton *open_button;
	FXButton *save_button;
	FXButton *flash_button;
	FXButton *get_button;
	FXLabel *connected_label;
	FXLabel *connected_label2;
	FXLabel *connected_label3;
	FXTextField *output_text;
	FXTextField *protocol_text;
	FXTextField *address_text;
	FXTextField *command_text;
	FXTextField *flag_text;
	FXTextField *days_text;
	FXTextField *hours_text;
	FXTextField *minutes_text;
	FXTextField *seconds_text;
	FXText *input_text;
	FXText *map_text21;
	FXListBox* wslistbox;
	FXListBox* rslistbox;
	FXTextField *repeat_text;
	FXTextField *modifier_text;
	FXTextField *key_text;
	FXTextField *line_text;
	FXStatusBar *statusbar;
	struct hid_device_info *devices;
	hid_device *connected_device;
	size_t getDataFromTextField(FXTextField *tf, char *buf, size_t len);
	unsigned char buf[17];
	int wakeupslots;
	int irdatanr;
	FXString protocols;
	FXString firmware;
	FXColor storedShadowColor;
	FXColor storedBaseColor;
	FXColor storedBackColor;
	int RepeatCounter;
	FXString map[400];
	int mapbeg[200];
	int active_lines;
	int max;
	int count;

protected:
	MainWindow() {};
public:
	MainWindow(FXApp *a);
	~MainWindow();
	virtual void create();
	
	long onConnect(FXObject *sender, FXSelector sel, void *ptr);
	long onDisconnect(FXObject *sender, FXSelector sel, void *ptr);
	long onRescan(FXObject *sender, FXSelector sel, void *ptr);
	long onReboot(FXObject *sender, FXSelector sel, void *ptr);
	long onSendOutputReport(FXObject *sender, FXSelector sel, void *ptr);
	long onPwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onPirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onPkey(FXObject *sender, FXSelector sel, void *ptr);
	long onPrepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onPRwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onPRirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onGwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onGirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onGkey(FXObject *sender, FXSelector sel, void *ptr);
	long onGrepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onGcaps(FXObject *sender, FXSelector sel, void *ptr);
	long onAget(FXObject *sender, FXSelector sel, void *ptr);
	long onAset(FXObject *sender, FXSelector sel, void *ptr);
	long onRwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onRirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onRkey(FXObject *sender, FXSelector sel, void *ptr);
	long onRrepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onRalarm(FXObject *sender, FXSelector sel, void *ptr);
	long onReadIR(FXObject *sender, FXSelector sel, void *ptr);
	long onClear(FXObject *sender, FXSelector sel, void *ptr);
	long onTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onMacTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onCmdwsListBox(FXObject*,FXSelector,void*);
	long onCmdrsListBox(FXObject*,FXSelector,void*);
	long onNew(FXObject *sender, FXSelector sel, void *ptr);
	long onOpen(FXObject *sender, FXSelector sel, void *ptr);
	long onSave(FXObject *sender, FXSelector sel, void *ptr);
	long onGeeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onPeeprom(FXObject *sender, FXSelector sel, void *ptr);
	uint8_t get_key_nr(FXString s);
	FXString get_key_from_nr(uint8_t nr);
	long Write();
	long Read();
	long Write_and_Check();
	long saveFile(const FXString& file);
	long onApply(FXObject *sender, FXSelector sel, void *ptr);
	long onDevDClicked(FXObject *sender, FXSelector sel, void *ptr);
	long onCmdQuit(FXObject *sender, FXSelector sel, void *ptr);
};

// FOX 1.7 changes the timeouts to all be nanoseconds.
// Fox 1.6 had all timeouts as milliseconds. // this is true only for addTimeout(), but NOT for FXThread::sleep() !
#if (FOX_MINOR >= 7)
	const int timeout_scalar = 1000*1000;
#else
	const int timeout_scalar = 1;
#endif

FXMainWindow *g_main_window;


FXDEFMAP(MainWindow) MainWindowMap [] = {
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CONNECT, MainWindow::onConnect ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_DISCONNECT, MainWindow::onDisconnect ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESCAN, MainWindow::onRescan ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_REBOOT, MainWindow::onReboot ),
	FXMAPFUNC(SEL_DOUBLECLICKED, MainWindow::ID_DEVLIST, MainWindow::onDevDClicked ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SEND_OUTPUT_REPORT, MainWindow::onSendOutputReport ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PWAKEUP, MainWindow::onPwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PMACRO, MainWindow::onPirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PKEY, MainWindow::onPkey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PREPEAT, MainWindow::onPrepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PRWAKEUP, MainWindow::onPRwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PRMACRO, MainWindow::onPRirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GWAKEUP, MainWindow::onGwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GMACRO, MainWindow::onGirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GKEY, MainWindow::onGkey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GREPEAT, MainWindow::onGrepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GCAP, MainWindow::onGcaps ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GEEPROM, MainWindow::onGeeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PEEPROM, MainWindow::onPeeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_AGET, MainWindow::onAget ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_ASET, MainWindow::onAset ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RWAKEUP, MainWindow::onRwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RMACRO, MainWindow::onRirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RKEY, MainWindow::onRkey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RREPEAT, MainWindow::onRrepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RALARM, MainWindow::onRalarm ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CLEAR, MainWindow::onClear ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_MAC_TIMER, MainWindow::onMacTimeout ),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_WSLISTBOX, MainWindow::onCmdwsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_WSLISTBOX, MainWindow::onCmdwsListBox),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_RSLISTBOX, MainWindow::onCmdrsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RSLISTBOX, MainWindow::onCmdrsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_OPEN, MainWindow::onOpen ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SAVE, MainWindow::onSave ),
	FXMAPFUNC(SEL_CLOSE,   0, MainWindow::onCmdQuit ),
};

FXIMPLEMENT(MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER(MainWindowMap));

MainWindow::MainWindow(FXApp *app)
	: FXMainWindow(app, "IRMP STM32 KBD Configuration", NULL, NULL, DECOR_ALL, 275, 38, 880, 978)  // for 1280x1024
{
	this->setIcon(new FXGIFIcon(app,Icon)); // for taskbar
	this->setMiniIcon(new FXICOIcon(app,MiniIcon)); // for titlebar
	devices = NULL;
	connected_device = NULL;

	// create horizontal frame with two vertical frames
	FXHorizontalFrame *hf1 = new FXHorizontalFrame(this, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 2,0);
	FXVerticalFrame *vf1 = new FXVerticalFrame(hf1, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
	FXVerticalFrame *vf2 = new FXVerticalFrame(hf1, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0/*,0,0,0*/);

	// second vertical frame for map_text21
	new FXLabel(vf2, "eeprom map                                       ");
	FXVerticalFrame *innerVF21 = new FXVerticalFrame(vf2, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
	map_text21 = new FXText(new FXHorizontalFrame(innerVF21,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0,0,0), NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y/*,0,0,0,0, 0,0,0,0*/);

	// first vertical frame: everything else
	// Device List and Connect/Disconnect buttons
	FXHorizontalFrame *hf11 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X);
	device_list = new FXList(new FXHorizontalFrame(hf11,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0), this, ID_DEVLIST, LISTBOX_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,300,200);
	FXVerticalFrame *buttonVF11 = new FXVerticalFrame(hf11);
	connect_button = new FXButton(buttonVF11, "Connect", NULL, this, ID_CONNECT, BUTTON_NORMAL|LAYOUT_FILL_X);
	disconnect_button = new FXButton(buttonVF11, "Disconnect", NULL, this, ID_DISCONNECT, BUTTON_NORMAL|LAYOUT_FILL_X);
	disconnect_button->disable();
	rescan_button = new FXButton(buttonVF11, "Re-Scan devices", NULL, this, ID_RESCAN, BUTTON_NORMAL|LAYOUT_FILL_X);
	reboot_button = new FXButton(buttonVF11, "Reboot device", NULL, this, ID_REBOOT, BUTTON_NORMAL|LAYOUT_FILL_X);
	connected_label = new FXLabel(vf1, "Disconnected");
	connected_label2 = new FXLabel(vf1, "Firmware:");
	connected_label3 = new FXLabel(vf1, "Protocols:");
	
	// horizontal frame of group boxes
	FXHorizontalFrame *hf12 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
	//set Group Box
	FXGroupBox *gb121 = new FXGroupBox(hf12, "set", FRAME_GROOVE|LAYOUT_FILL_X);
	pwakeup_button = new FXButton(gb121, "wakeup", NULL, this, ID_PWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	pmacro_button = new FXButton(gb121, "irdata", NULL, this, ID_PMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	pkey_button = new FXButton(gb121, "key", NULL, this, ID_PKEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	prepeat_button = new FXButton(gb121, "repeat", NULL, this, ID_PREPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	aset_button = new FXButton(gb121, "alarm", NULL, this, ID_ASET, BUTTON_NORMAL|LAYOUT_FILL_X);
	//set by remote Group Box
	FXGroupBox *gb122 = new FXGroupBox(hf12, "set by remote", FRAME_GROOVE|LAYOUT_FILL_X);
	prwakeup_button = new FXButton(gb122, "wakeup", NULL, this, ID_PRWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	prmacro_button = new FXButton(gb122, "irdata", NULL, this, ID_PRMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	//get Group Box
	FXGroupBox *gb123 = new FXGroupBox(hf12, "get", FRAME_GROOVE|LAYOUT_FILL_X);
	gwakeup_button = new FXButton(gb123, "wakeup", NULL, this, ID_GWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	gmacro_button = new FXButton(gb123, "irdata", NULL, this, ID_GMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	gkey_button = new FXButton(gb123, "key", NULL, this, ID_GKEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	grepeat_button = new FXButton(gb123, "repeat", NULL, this, ID_GREPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	aget_button = new FXButton(gb123, "alarm", NULL, this, ID_AGET, BUTTON_NORMAL|LAYOUT_FILL_X);
	//gcap_button = new FXButton(gb123, "caps", NULL, this, ID_GCAP, BUTTON_NORMAL|LAYOUT_FILL_X);
	//reset Group Box
	FXGroupBox *gb124 = new FXGroupBox(hf12, "reset", FRAME_GROOVE|LAYOUT_FILL_X);
	rwakeup_button = new FXButton(gb124, "wakeup", NULL, this, ID_RWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	rmacro_button = new FXButton(gb124, "irdata", NULL, this, ID_RMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	rkey_button = new FXButton(gb124, "key", NULL, this, ID_RKEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	rrepeat_button = new FXButton(gb124, "repeat", NULL, this, ID_RREPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	ralarm_button = new FXButton(gb124, "alarm", NULL, this, ID_RALARM, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for IR Group Box, repeat Group Box, alarm Group Box, select listboxes and map group box
	FXHorizontalFrame *hf13 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	// two vertical frames, left for map, right for everything else
	FXSpring *s131 = new FXSpring(hf13, LAYOUT_FILL_X, 300, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf131 = new FXVerticalFrame(s131, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	FXSpring *s132 = new FXSpring(hf13, LAYOUT_FILL_X, 100, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf132 = new FXVerticalFrame(s132, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0/*,0,0,0*/);

	// horizontal frame for IR Group Box and repeat Group Box
	FXHorizontalFrame *hf131 = new FXHorizontalFrame(vf131, LAYOUT_FILL_X);
	//IR Group Box
	FXSpring *s1311 = new FXSpring(hf131,LAYOUT_FILL_X, 200, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXGroupBox *gb1311 = new FXGroupBox(s1311, "IR (hex)", FRAME_GROOVE|LAYOUT_FILL_X);
	FXMatrix *m1311 = new FXMatrix(gb1311, 4, MATRIX_BY_COLUMNS|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN, 0,0,0,0, 0,0,0,0, 0,0);
	new FXLabel(m1311, "protocol");
	new FXLabel(m1311, "address");
	new FXLabel(m1311, "command");
	new FXLabel(m1311, "flag");
	protocol_text = new FXTextField(m1311, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	address_text = new FXTextField(m1311, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	command_text = new FXTextField(m1311, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	flag_text = new FXTextField(m1311, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);

	// repeat Group Box
	FXSpring *s1312 = new FXSpring(hf131,LAYOUT_FILL_X, 100, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXGroupBox *gb1312 = new FXGroupBox(s1312, "repeat", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 0,0,0,32, 0,0);
	repeat_text = new FXTextField(gb1312, 10, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);

	// horizontal frame for alarm Group Box and select listboxes
	FXHorizontalFrame *hf132 = new FXHorizontalFrame(vf131, LAYOUT_FILL_X/*,0,0,0,0, 0,0,0,0*/);
	//alarm Group Box
	FXSpring *s1321 = new FXSpring(hf132,LAYOUT_FILL_X, 200, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXGroupBox *gb14 = new FXGroupBox(s1321, "alarm (dec)", FRAME_GROOVE|LAYOUT_FILL_X);
	FXMatrix *m14 = new FXMatrix(gb14, 4, MATRIX_BY_COLUMNS|LAYOUT_FILL_X, 0,0,0,0, 2,2,2,4, 2,2);
	new FXLabel(m14, "days");
	new FXLabel(m14, "hours");
	new FXLabel(m14, "minutes");
	new FXLabel(m14, "seconds");
	days_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	hours_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	minutes_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	seconds_text = new FXTextField(m14, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	// select listboxes
	FXSpring *s1322 = new FXSpring(hf132,LAYOUT_FILL_X, 100, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXGroupBox *gb143 = new FXGroupBox(s1322, "select", FRAME_GROOVE|LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/);
	wslistbox=new FXListBox(gb143,this,ID_WSLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	rslistbox=new FXListBox(gb143,this,ID_RSLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);

	// map group box
	FXGroupBox *gb132 = new FXGroupBox(vf132, "eeprom map", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	open_button = new FXButton(gb132, "open file", NULL, this, ID_OPEN, BUTTON_NORMAL|LAYOUT_FILL_X);
	save_button = new FXButton(gb132, "save file", NULL, this, ID_SAVE, BUTTON_NORMAL|LAYOUT_FILL_X);
	flash_button = new FXButton(gb132, "flash eeprom", NULL, this, ID_PEEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_button = new FXButton(gb132, "get eeprom", NULL, this, ID_GEEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	new FXLabel(gb132, "modifier");
	FXVerticalFrame *innerVF10 = new FXVerticalFrame(gb132, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	modifier_text = new FXTextField(new FXHorizontalFrame(innerVF10,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	new FXLabel(gb132, "key");
	FXVerticalFrame *innerVF9 = new FXVerticalFrame(gb132, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	key_text = new FXTextField(new FXHorizontalFrame(innerVF9,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	new FXLabel(gb132, "line");
	FXVerticalFrame *innerVF11 = new FXVerticalFrame(gb132, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	line_text = new FXTextField(new FXHorizontalFrame(innerVF11,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, map_text21, FXText::ID_CURSOR_ROW, LAYOUT_FILL_X);

	// horizontal frame for Output Group Box
	FXHorizontalFrame *hf15 = new FXHorizontalFrame(vf131, LAYOUT_FILL_X);
	// Output Group Box
	FXGroupBox *gb15 = new FXGroupBox(hf15, "PC->STM32", FRAME_GROOVE|LAYOUT_FILL_X);
	FXMatrix *m3 = new FXMatrix(gb15, 2, MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
	new FXLabel(m3, "Data");
	new FXLabel(m3, "");
	output_text = new FXTextField(m3, 29, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	output_button = new FXButton(m3, "Send to IRMP Device", NULL, this, ID_SEND_OUTPUT_REPORT, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for Input Group Box
	FXHorizontalFrame *hf16 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	// Input Group Box
	FXGroupBox *gb16 = new FXGroupBox(hf16, "debug messages", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	FXVerticalFrame *innerVF16 = new FXVerticalFrame(gb16, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	input_text = new FXText(new FXHorizontalFrame(innerVF16,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	input_text->setEditable(false);
	FXButton *clear_button = new FXButton(innerVF16, "Clear", NULL, this, ID_CLEAR, BUTTON_NORMAL|LAYOUT_RIGHT);

	// horizontal frame for Status Bar
	FXHorizontalFrame *hf17 = new FXHorizontalFrame(vf1, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X,0,0,0,0, 1,2,0,3);
	//Status Bar
	statusbar = new FXStatusBar(hf17, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

	// HelpText
	device_list->setHelpText("select one of found IRMP_STM32 devices (doubleclick connects)");
	connect_button->setHelpText("connect to selected device");
	disconnect_button->setHelpText("disconnect device");
	rescan_button->setHelpText("rescan devices");
	reboot_button->setHelpText("reboot device");
	pwakeup_button->setHelpText("set wakeup");
	pmacro_button->setHelpText("set irdata");
	pkey_button->setHelpText("set key");
	prwakeup_button->setHelpText("set wakeup by remote");
	prmacro_button->setHelpText("set irdata by remote");
	gwakeup_button->setHelpText("get wakeup");
	gmacro_button->setHelpText("get irdata");
	gkey_button->setHelpText("get key");
	//gcap_button->setHelpText("get capabilities");
	rwakeup_button->setHelpText("reset wakeup");
	rmacro_button->setHelpText("reset irdata");
	rkey_button->setHelpText("reset key");
	ralarm_button->setHelpText("reset alarm");
	protocol_text->setHelpText("IR protocol");
	address_text->setHelpText("IR address");
	command_text->setHelpText("IR command");
	flag_text->setHelpText("IR flags");
	days_text->setHelpText("days");
	hours_text->setHelpText("hours");
	minutes_text->setHelpText("minutes");
	seconds_text->setHelpText("seconds");
	aset_button->setHelpText("set alarm");
	aget_button->setHelpText("get alarm");
	wslistbox->setHelpText("wakeup to be set");
	rslistbox->setHelpText("repeat to be set");
	repeat_text->setHelpText("repeat");
	output_text->setHelpText("data to be sent to device (experts only)");
	output_button->setHelpText("send to device");
	input_text->setHelpText("debug messages");
	clear_button->setHelpText("clear debug messages");
	open_button->setHelpText("open translation map");
	modifier_text->setHelpText("modifier i.e. LeftShift)");
	key_text->setHelpText("key");
	save_button->setHelpText("save translation map");
	map_text21->setHelpText("eeprom translation map");
	flash_button->setHelpText("flash into eeprom");
	get_button->setHelpText("get from eeprom");
	line_text->setHelpText("line in eeprom map");
	prepeat_button->setHelpText("set repeat");
	grepeat_button->setHelpText("get repeat");
	rrepeat_button->setHelpText("reset repeat");

	// disable buttons
	output_button->disable();
	pwakeup_button->disable();
	pmacro_button->disable();
	pkey_button->disable();
	prwakeup_button->disable();
	prmacro_button->disable();
	gwakeup_button->disable();
	gkey_button->disable();
	gmacro_button->disable();
	//gcap_button->disable();
	aget_button->disable();
	aset_button->disable();
	rwakeup_button->disable();
	rmacro_button->disable();
	rkey_button->disable();
	ralarm_button->disable();
	reboot_button->disable();
	prepeat_button->disable();
	grepeat_button->disable();
	rrepeat_button->disable();
	open_button->disable();
	save_button->disable();
	flash_button->disable();
	get_button->disable();

	// initialize
	RepeatCounter = 0;
	active_lines = 0;
	wakeupslots = 0;
	irdatanr = 0;
	protocols = "";
	firmware = "";
	max = 0;
	count = 0;

}

MainWindow::~MainWindow()
{
	if (connected_device)
		hid_close(connected_device);
	hid_exit();
}

long
MainWindow::onCmdQuit(FXObject *sender, FXSelector sel, void *ptr)
{
	if(map_text21->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	getApp()->exit(0);
	return 0;
}

void
MainWindow::create()
{
	FXMainWindow::create();
	show();

	onRescan(NULL, 0, NULL);
	onConnect(NULL, 0, NULL);

#ifdef __APPLE__
	init_apple_message_system();
	
	getApp()->addTimeout(this, ID_MAC_TIMER,
		50 * timeout_scalar /*50ms*/);
#endif
}

long
MainWindow::onConnect(FXObject *sender, FXSelector sel, void *ptr)
{
	if (connected_device != NULL)
		return 1;
	
	FXint cur_item = device_list->getCurrentItem();
	if (cur_item < 0)
		return -1;
	FXListItem *item = device_list->getItem(cur_item);
	if (!item)
		return -1;
	struct hid_device_info *device_info = (struct hid_device_info*) item->getData();
	if (!device_info)
		return -1;
	
	connected_device =  hid_open_path(device_info->path);
	
	if (!connected_device) {
		FXMessageBox::error(this, MBOX_OK, "Device Error oC", "Unable To Connect to Device");
		return -1;
	}

	if(onGcaps(NULL, 0, NULL) == -1)
		return -1;
	FXString s;
	s.format("Connected to: %04hx:%04hx -", device_info->vendor_id, device_info->product_id);
	//s += FXString(" ") + device_info->manufacturer_string;
	s += FXString(" ") + device_info->product_string;
	connected_label->setText(s);
	s = "Firmware: ";
	firmware.substitute("___","   ");
	firmware.substitute(":_",": ");
	s += firmware;
	connected_label2->setText(s);
	s = "Protocols: ";
	s += protocols;
	connected_label3->setText(s);
	for(int i = 0; i < wakeupslots; i++) {
		FXString s;
		s = (i < wakeupslots-1) ? "wakeup" : "reboot";
#if (FOX_MINOR >= 7)
		FXString t;
		t.fromInt(i,10);
		s += (i > 0 && i < wakeupslots-1) ? t : "";
#else
		s += (i > 0 && i < wakeupslots-1) ? FXStringVal(i,10) : "";
#endif
		wslistbox->appendItem(s);	
	}
	wslistbox->setNumVisible(wakeupslots);
	rslistbox->appendItem("repeat delay");
	rslistbox->appendItem("repeat period");
	rslistbox->appendItem("repeat timeout");
	rslistbox->setNumVisible(3);
	onGeeprom(NULL, 0, NULL);
	output_button->enable();
	pwakeup_button->enable();
	pmacro_button->enable();
	pkey_button->enable();
	prepeat_button->enable();
	prwakeup_button->enable();
	prmacro_button->enable();
	gwakeup_button->enable();
	gmacro_button->enable();
	gkey_button->enable();
	grepeat_button->enable();
	//gcap_button->enable();
	aget_button->enable();
	aset_button->enable();
	rwakeup_button->enable();
	rmacro_button->enable();
	rkey_button->enable();
	ralarm_button->enable();
	rrepeat_button->enable();
	connect_button->disable();
	disconnect_button->enable();
	reboot_button->enable();
	open_button->enable();
	save_button->enable();
	flash_button->enable();
	get_button->enable();
	input_text->setText("");
	output_text->setText("");

	//list wakeups and alarm
	FXString u;
	for(int i = 0; i < wakeupslots; i++) {
		FXString s;
		FXString t;
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
#else
		t = FXStringVal(i,10);
#endif
		s = "3 0 0 4 "; // Report_ID STAT_CMD ACC_GET CMD_WAKE
		s += t;
		output_text->setText(s);
		Write_and_Check();
		s = (i < wakeupslots-1) ? "wakeup: " : "reboot: ";
		t.format("%02hhx", buf[4]);
		s += t;
		t.format("%02hhx", buf[6]);
		s += t;
		t.format("%02hhx", buf[5]);
		s += t;
		t.format("%02hhx", buf[8]);
		s += t;
		t.format("%02hhx", buf[7]);
		s += t;
		t.format("%02hhx", buf[9]);
		s += t;
		s += "\n";
		if(!(s == "wakeup: ffffffffffff\n")) {
			u += s;
		}
	}
	output_text->setText("3 0 0 1"); // Report_ID STAT_CMD ACC_GET CMD_ALARM
	Write_and_Check();
	unsigned int alarm = *((uint32_t *)&buf[4]);
	FXString t;	
	s = "alarm: ";
	t.format("%u", alarm/60/60/24);
	s += t;
	s += " days, ";
	t.format("%d", (alarm/60/60) % 24);
	s += t;
	s += " hours, ";
	t.format("%d", (alarm/60) % 60);
	s += t;
	s += " minutes, ";
	t.format("%d", alarm % 60);
	s += t;
	s += " seconds\n";
	onClear(NULL, 0, NULL);
	output_text->setText("");
	input_text->appendText(u);
	input_text->setBottomLine(INT_MAX);
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);

	hid_set_nonblocking(connected_device, 1);

	return 1;
}

long
MainWindow::onDisconnect(FXObject *sender, FXSelector sel, void *ptr)
{
	hid_close(connected_device);
	connected_device = NULL;
	connected_label->setText("Disconnected");
	connected_label2->setText("Firmware:");
	connected_label3->setText("Protocols:");
	protocols = "";
	firmware = "";
	max = 0;
	count = 0;
	wslistbox->clearItems();
	rslistbox->clearItems();
	output_button->disable();
	pwakeup_button->disable();
	pmacro_button->disable();
	pkey_button->disable();
	prwakeup_button->disable();
	prmacro_button->disable();
	gwakeup_button->disable();
	gkey_button->disable();
	gmacro_button->disable();
	//gcap_button->disable();
	aget_button->disable();
	aset_button->disable();
	rwakeup_button->disable();
	rmacro_button->disable();
	rkey_button->disable();
	ralarm_button->disable();
	connect_button->enable();
	disconnect_button->disable();
	reboot_button->disable();
	prepeat_button->disable();
	grepeat_button->disable();
	rrepeat_button->disable();
	open_button->disable();
	save_button->disable();
	flash_button->disable();
	get_button->disable();

	return 1;
}

long
MainWindow::onRescan(FXObject *sender, FXSelector sel, void *ptr)
{
	struct hid_device_info *cur_dev;

	device_list->clearItems();
	
	// List the Devices
	hid_free_enumeration(devices);
	devices = hid_enumerate(0x1209, 0x4445);
	cur_dev = devices;	
	while (cur_dev) {
		// only hidraw, not keyboard
		if(cur_dev->usage == 0x01) {
			// Add it to the List Box.
			FXString s;
			s.format("%04hx:%04hx -", cur_dev->vendor_id, cur_dev->product_id);
			s += FXString(" ") + cur_dev->manufacturer_string;
			s += FXString(" ") + cur_dev->product_string;
			FXListItem *li = new FXListItem(s, NULL, cur_dev);
			device_list->appendItem(li);
		}
		cur_dev = cur_dev->next;
	}

	if (device_list->getNumItems() == 0)
		device_list->appendItem("*** No Devices Connected ***");
	else {
		device_list->selectItem(0);
	}

	return 1;
}

long
MainWindow::onReboot(FXObject *sender, FXSelector sel, void *ptr)
{
	output_text->setText("3 0 1 5"); // Report_ID STAT_CMD ACC_SET CMD_REBOOT

	FXint cur_item = device_list->getCurrentItem();
	Write_and_Check();
	FXThread::sleep(2500000000);
	onDisconnect(NULL, 0, NULL);
	onRescan(NULL, 0, NULL);
	device_list->setCurrentItem(cur_item);
	device_list->deselectItem(0);
	device_list->selectItem(cur_item);
	onConnect(NULL, 0, NULL);

	return 1;
}

size_t
MainWindow::getDataFromTextField(FXTextField *tf, char *buf, size_t len)
{
	const char *delim = " ,{}\t\r\n";
	FXString data = tf->getText();
	const FXchar *d = data.text();
	size_t i = 0;
	
	// Copy the string from the GUI.
	size_t sz = strlen(d);
	char *str = (char*) malloc(sz+1);
	strcpy(str, d);
	
	// For each token in the string, parse and store in buf[].
	char *token = strtok(str, delim);
	while (token) {
		char *endptr;  // TODO why not NULL?!
		long int val = strtol(token, &endptr, 16); // hex!
		buf[i++] = val;
		token = strtok(NULL, delim);
	}
	
	free(str);
	return i;
}

long
MainWindow::Read()
{
	memset(buf, 0, sizeof(buf));
	FXString s;
	if (!connected_device) {
		FXMessageBox::error(this, MBOX_OK, "Device Error R", "Unable To Connect to Device");
		s = "Unable To Connect to Device R\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	int res = hid_read(connected_device, buf, sizeof(buf));
	
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, "Error Reading", "Could not read from device. Error reported was: %ls", hid_error(connected_device));
		onDisconnect(NULL, 0, NULL);
		onRescan(NULL, 0, NULL);
		input_text->appendText("read error\n");
		input_text->setBottomLine(INT_MAX);
		return -1;
	} else {
		if (res == 0)
			return 0;

		s.format("Received %d bytes:\n", res);
		for (int i = 0; i < res; i++) {
			FXString t;
			t.format("%02hhx ", buf[i]);
			s += t;
		}
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::onReadIR(FXObject *sender, FXSelector sel, void *ptr)
{
	if(Read() <= 0)
		return 0;

	FXString s;
	FXString t;

	if (buf[0] == 1) { // REPORT_ID_IR
		// Repeat Counter
			if (!buf[6]) {
				RepeatCounter = 0;
			} else {
				RepeatCounter++;
			}
			FXString u;
			u.format("RepeatCounter: %d \n", RepeatCounter);
			input_text->appendText(u);
			input_text->setBottomLine(INT_MAX);
			g_main_window->repaint();

		// show received IR
		s = "";
		t.format("%02hhx", buf[1]);
		s += t;
		protocol_text->setText(s);
		
		s = ""; t = "";
		t.format("%02hhx", buf[3]);
		s += t; t = "";
		t.format("%02hhx", buf[2]);
		s += t;
		address_text->setText(s);

		s = ""; t = "";
		t.format("%02hhx", buf[5]);
		s += t; t = "";
		t.format("%02hhx", buf[4]);
		s += t;
		command_text->setText(s);

		s = ""; t = "";
		t.format("%02hhx", buf[6]);
		s += t;
		flag_text->setText(s);

		//translate by map and show
		int k = 0;
		t = protocol_text->getText();
		t += address_text->getText();
		t += command_text->getText();
		t += "00";
		s = "translated:";
		map_text21->killHighlight();
		for(int i = 0; i < active_lines; i++) {
			if(map[i*2] == t) {
				s += " ";
				s += map[i*2+1];
				key_text->setText(map[i*2+1]);
				k++;
				map_text21->setHighlight(mapbeg[i], mapbeg[i+1] - mapbeg[i] - 1);
			}
		}
		if(k > 1)
			s += ", WARNING: multiple entries!";
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);		
	}

	return 1;
}

long
MainWindow::Write()
{
	FXString s;
	char bufw[17];
	memset(bufw, 0, sizeof(bufw));
	getDataFromTextField(output_text, bufw, sizeof(bufw));

	if (!connected_device) { //TODO this helps, but where is the error message?!
		FXMessageBox::error(this, MBOX_OK, "Device Error W", "Unable To Connect to Device");
		s = "Unable To Connect to Device W\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	int res = hid_write(connected_device, (const unsigned char*)bufw, 17);
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, "Error Writing", "Could not write to device. Error reported was: %ls", hid_error(connected_device));
		input_text->appendText("write error\n");
		input_text->setBottomLine(INT_MAX);
		onDisconnect(NULL, 0, NULL);
		onRescan(NULL, 0, NULL);
		return -1;
	} else {
		s.format("Sent %d bytes:\n", res);
		for (int i = 0; i < res; i++) {
			FXString t;
			t.format("%02hhx ", static_cast<unsigned char>(bufw[i]));
			s += t;
		}
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
	}
	
	return 1;
}

long
MainWindow::Write_and_Check()
{
	FXString s;
	int read;
	s = "";
    if(Write() == -1) {
		s += "W&C Write(): -1\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	FXThread::sleep(2000);

	read = Read();
    if(read  == -1) {
		s += "W&C first Read(): -1\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

    while (buf[0] == 0x01 || read == 0) {
		read = Read();
		if(read == -1) {
			s += "W&C loop Read(): -1\n";
			input_text->appendText(s);
			input_text->setBottomLine(INT_MAX);
			return -1;
		}
	}

	if(buf[1] == 0x01) { // STAT_SUCCESS // TODO compare other indices too?
		s += "************************OK***************************\n";	
	} else {
		s += "**********************ERROR**************************\n";
	}
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onSendOutputReport(FXObject *sender, FXSelector sel, void *ptr)
{
	Write();

	return 1;
}

long
MainWindow::onPwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
	t.format("%x ", wslistbox->getCurrentItem());
	s = "3 0 1 4 "; // Report_ID STAT_CMD ACC_SET CMD_WAKE
	s += t;
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	output_text->setText(s);

	Write_and_Check();

	return 1;
}

long
MainWindow::onPirdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toInt() - 1);
#else
	t.format("%x ", FXIntVal(line_text->getText(), 10) - 1);
#endif
	s = "3 0 1 2 "; // Report_ID STAT_CMD ACC_SET CMD_IRDATA
	s += t;
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	output_text->setText(s);

	Write_and_Check();

	onGirdata(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	map_text21->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += "00";
	map_text21->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	//map_text21->setModified(1);

	return 1;
}

long
MainWindow::onPkey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toInt() - 1);
#else
	t.format("%x ", FXIntVal(line_text->getText(), 10) - 1);
#endif
	s = "3 0 1 3 "; // Report_ID STAT_CMD ACC_SET CMD_KEY
	s += t;
#if (FOX_MINOR >= 7)
	t.fromInt(get_key_nr((key_text->getText())),16);
#else
	t = FXStringVal(get_key_nr((key_text->getText())),16);
#endif
#if (FOX_MINOR >= 7)
	u.fromInt(get_key_nr((modifier_text->getText())),16);
#else
	u = FXStringVal(get_key_nr((modifier_text->getText())),16);
#endif
	s += t;
	s += " ";
	s += u;
	s += " ";
	output_text->setText(s);

	Write_and_Check();

	onGkey(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	map_text21->removeText(mapbeg[i-1]+map[(i-1)*2].length()+1, map[(i-1)*2+1].length());
	s = modifier_text->getText();
	s += "|";
	s += key_text->getText();
	map_text21->insertText(mapbeg[i-1]+map[(i-1)*2].length()+1, s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	//map_text21->setModified(1);

	return 1;
}

long
MainWindow::onPrepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s = "3 0 1 8 "; // Report_ID STAT_CMD ACC_SET CMD_REPEAT
	u.format("%x ", rslistbox->getCurrentItem());
#if (FOX_MINOR >= 7)
	t.format("%x ", repeat_text->getText().toInt());
#else
	t.format("%x ", FXIntVal(repeat_text->getText(), 10));
#endif
	s += u;
	s += " ";
	s += t;
	s += " ";
	output_text->setText(s);

	Write_and_Check();

	return 1;
}

long
MainWindow::onPRwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);
	t.format("%x ", wslistbox->getCurrentItem());
	s = "3 0 1 7 "; // Report_ID STAT_CMD ACC_SET CMD_WAKE_REMOTE
	s += t;
	output_text->setText(s);

	Write_and_Check();

	return 1;
}

long
MainWindow::onPRirdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toInt() - 1);
#else
	t.format("%x ", FXIntVal(line_text->getText(), 10) - 1);
#endif
	s = "3 0 1 6 "; // Report_ID STAT_CMD ACC_SET CMD_IRDATA_REMOTE
	s += t;
	output_text->setText(s);

	Write_and_Check();

	onGirdata(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	map_text21->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += "00";
	map_text21->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	//map_text21->setModified(1);

	return 1;
}


long
MainWindow::onGwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%x", wslistbox->getCurrentItem());
	s = "3 0 0 4 "; // Report_ID STAT_CMD ACC_GET CMD_WAKE
	s += t;
	output_text->setText(s);

	Write_and_Check();

	s = "";
	t.format("%02hhx", buf[4]);
	s += t;
	protocol_text->setText(s);
		
	s = "";
	t.format("%02hhx", buf[6]);
	s += t;
	t.format("%02hhx", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02hhx", buf[8]);
	s += t;
	t.format("%02hhx", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02hhx", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGirdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	s = "3 0 0 2 "; // Report_ID STAT_CMD ACC_GET CMD_IRDATA
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toInt() - 1);
#else
	t.format("%x ", FXIntVal(line_text->getText(), 10) - 1);
#endif
	s += t;
	output_text->setText(s);

	Write_and_Check();
	
	s = "";
	t.format("%02hhx", buf[4]);
	s += t;
	protocol_text->setText(s);
		
	s = "";
	t.format("%02hhx", buf[6]);
	s += t;
	t.format("%02hhx", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02hhx", buf[8]);
	s += t;
	t.format("%02hhx", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02hhx", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGkey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	s = "3 0 0 3 "; // Report_ID STAT_CMD ACC_GET CMD_KEY
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toInt() - 1);
#else
	t.format("%x ", FXIntVal(line_text->getText(), 10) - 1);
#endif
	s += t;
	output_text->setText(s);

	Write_and_Check();

	modifier_text->setText(get_key_from_nr(buf[5]));
	key_text->setText(get_key_from_nr(buf[4]));

	return 1;
}

long
MainWindow::onGrepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s = "3 0 0 8 "; // Report_ID STAT_CMD ACC_GET CMD_REPEAT
	u.format("%x ", rslistbox->getCurrentItem());
	s += u;
	s += " ";
	output_text->setText(s);

	Write_and_Check();

#if (FOX_MINOR >= 7)
	t.fromInt(*((uint16_t*)&buf[4]),10);
#else
	t = FXStringVal(*((uint16_t*)&buf[4]), 10);
#endif
	repeat_text->setText(t);

	return 1;
}

long
MainWindow::onGcaps(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	int read;
	int jump_to_firmware;
	jump_to_firmware = 0;
	for(int i = 0; i < 20; i++) { // for safety stop after 20 loops
		s = "3 0 0 0 "; // Report_ID STAT_CMD ACC_GET CMD_CAPS
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
		s += t;
#else
		s += FXStringVal(i,10);
#endif
		s += " ";
		output_text->setText(s);

  	 	if(Write() == -1) {
			t = "onGcaps Write(): -1\n";
			input_text->appendText(t);
			input_text->setBottomLine(INT_MAX);
			return -1;
		}

		FXThread::sleep(2000);
		
		read = Read();
		if(read == -1) {
			t = "onGcaps first Read(): -1\n";
			input_text->appendText(t);
			input_text->setBottomLine(INT_MAX);
			return -1;
		}

  	 	while (buf[0] == 0x01 || read == 0) {
			read = Read();
			if(read == -1) {
				t = "onGcaps loop Read(): -1\n";
				input_text->appendText(t);
				input_text->setBottomLine(INT_MAX);
				return -1;
			}
		}

		if (!i) { // first query for slots and depth
			s.format("number of irdata: %u\n", buf[4]);
			irdatanr = buf[4];
			//t.format("macro_depth: %u\n", buf[5]);
			//s += t;
			wakeupslots = buf[6];
			t.format("number of wakeups: %u", buf[6]);
			s += t;
		} else {
			if (!jump_to_firmware) { // queries for supported_protocols
				s = "protocols: ";
				for (int k = 4; k < 17; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						input_text->appendText(s);
						input_text->setBottomLine(INT_MAX);
						jump_to_firmware = 1;
						goto again;
					}
					t.format("%u ", buf[k]);
					protocols += t;  // TODO line break ?
					s += t;
				}
			} else { // queries for firmware
				s = "firmware: ";
				for (int k = 4; k < 17; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						//s.substitute("_"," ");
						input_text->appendText(s);
						input_text->setBottomLine(INT_MAX);
						return 1;
					}
					t.format("%c", buf[k]);
					firmware += t;  // TODO line break ?
					s += t;
				}
			}
		}
		s += "\n";
		//s.substitute("_"," ");
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
again:	;
	}
	return 1;
}

long
MainWindow::onAget(FXObject *sender, FXSelector sel, void *ptr)
{
	output_text->setText("3 0 0 1"); // Report_ID STAT_CMD ACC_GET CMD_ALARM

	Write_and_Check();

	unsigned int alarm = *((uint32_t *)&buf[4]);

	FXString s;
	FXString t;	
	s = "";
	t.format("%u", alarm/60/60/24);
	s += t;
	days_text->setText(s);
		
	s = "";
	t.format("%d", (alarm/60/60) % 24);
	s += t;
	hours_text->setText(s);

	s = "";
	t.format("%d", (alarm/60) % 60);
	s += t;
	minutes_text->setText(s);

	s = "";
	t.format("%d", alarm % 60);
	s += t;
	seconds_text->setText(s);

	return 1;
}

long
MainWindow::onAset(FXObject *sender, FXSelector sel, void *ptr)
{
	unsigned int setalarm = 0;
	FXString u = "";
#if (FOX_MINOR >= 7)
	setalarm += 60 * 60 * 24 * days_text->getText().toInt();
	setalarm += 60 * 60 * hours_text->getText().toInt();
	setalarm += 60 * minutes_text->getText().toInt();
	setalarm += seconds_text->getText().toInt();
#else
	setalarm += 60 * 60 * 24 * FXIntVal(days_text->getText(), 10);
	setalarm += 60 * 60 * FXIntVal(hours_text->getText(), 10);
	setalarm += 60 * FXIntVal(minutes_text->getText(), 10);
	setalarm += FXIntVal(seconds_text->getText(), 10);
#endif
	if(setalarm < 2) {
		setalarm = 2;
		u = "set alarm to 2 in order to prevent device or program hangup\n";
	 }

	FXString s;
	FXString t;
	const char *z = " ";
	s = "3 0 1 1 "; // Report_ID STAT_CMD ACC_SET CMD_ALARM
#if (FOX_MINOR >= 7)
	t.fromInt(setalarm,16);
#else
	t = FXStringVal(setalarm, 16);
#endif
	int len = t.length();
	for (int i = 0; i < 8 - len; i++) {
		t.prepend("0");
	}
	t.insert(2, " ");
	t.insert(5, " ");
	t.insert(8, " ");
	s += t.section(z, 3, 1);
	s += " ";
	s += t.section(z, 2, 1);
	s += " ";
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	output_text->setText(s);

	Write_and_Check();

	input_text->appendText(u);
	input_text->setBottomLine(INT_MAX);
	
	return 1;
}

long
MainWindow::onRwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%x", wslistbox->getCurrentItem());
	s = "3 0 2 4 "; // Report_ID STAT_CMD ACC_RESET CMD_WAKE
	s += t;
	output_text->setText(s);

	Write_and_Check();

	return 1;
}

long
MainWindow::onRirdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toInt() - 1);
#else
	t.format("%x ", FXIntVal(line_text->getText(), 10) - 1);
#endif
	s = "3 0 2 2 "; // Report_ID STAT_CMD ACC_RESET CMD_IRDATA
	s += t;
	output_text->setText(s);

	Write_and_Check();

	onGirdata(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	map_text21->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += flag_text->getText();
	map_text21->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	//map_text21->setModified(1);

	return 1;
}

long
MainWindow::onRkey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toInt() - 1);
#else
	t.format("%x ", FXIntVal(line_text->getText(), 10) - 1);
#endif
	s = "3 0 2 3 "; // Report_ID STAT_CMD ACC_RESET CMD_KEY
	s += t;
	output_text->setText(s);

	Write_and_Check();

	onGkey(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	map_text21->removeText(mapbeg[i-1]+map[(i-1)*2].length()+1, map[(i-1)*2+1].length());
	s = modifier_text->getText();
	s += "|";
	s += key_text->getText();
	map_text21->insertText(mapbeg[i-1]+map[(i-1)*2].length()+1, s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	//map_text21->setModified(1);

	return 1;
}

long
MainWindow::onRrepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s = "3 0 2 8 "; // Report_ID STAT_CMD ACC_RESET CMD_REPEAT
	u.format("%x ", rslistbox->getCurrentItem());
	s += u;
	s += " ";
	output_text->setText(s);

	Write_and_Check();

	return 1;
}

long
MainWindow::onRalarm(FXObject *sender, FXSelector sel, void *ptr)
{
	output_text->setText("3 0 2 1"); // Report_ID STAT_CMD ACC_RESET CMD_ALARM

	Write_and_Check();

	return 1;
}

long
MainWindow::onOpen(FXObject *sender, FXSelector sel, void *ptr)
{
	if(map_text21->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	const FXchar patterns[]="All Files (*)\nmap Files (*.map)";
	long loaded = 0;
	FXint size = 0;
	FXint n = 0;
	FXFileDialog open(this,"Open a map file");
	open.setPatternList(patterns);
	open.setCurrentPattern(1);
	if(open.execute()){
		map_text21->setText(NULL,0);
		FXString file=open.getFilename();
		FXFile textfile(file,FXFile::Reading);
		// Opened file?
 		if(textfile.isOpen()){
  			FXchar *text; 

			// Get file size
			size=textfile.size();

			// Make buffer to load file
			if(allocElms(text,size)){

				// Set wait cursor
				getApp()->beginWaitCursor();

				// Read the file
				n=textfile.readBlock(text,size);
				if(0<=n){

					// Set text
					map_text21->setText(text,n);

					// Success
					loaded=1;
				}

				// Kill wait cursor
				getApp()->endWaitCursor();

				// Free buffer
				freeElms(text);
			}
		}

		map_text21->setModified(0);
		FXString u;
		FXString v;
		u = "opened: ";
		v = open.getFilename().text();
		u += v;
		u += "\n";
		u += "opened size: ";
#if (FOX_MINOR >= 7)
		v.fromInt(size, 10);
#else
		v = FXStringVal(size, 10);
#endif
		u += v;
		u += "\n";
		input_text->appendText(u);
		input_text->setBottomLine(INT_MAX);
		onApply(NULL, 0, NULL);
    }

	return loaded;
}

long
MainWindow::onSave(FXObject *sender, FXSelector sel, void *ptr){
	const FXchar patterns[]="All Files (*)\nmap Files (*.map)";
	FXFileDialog save(this,"save the map file");
	FXString file;
	save.setPatternList(patterns);
	save.setCurrentPattern(1);
	if(save.execute()){
		file=save.getFilename();
		if(compare(file.right(4), ".map") && (save.getCurrentPattern() == 1))
			file += ".map";
		if(FXStat::exists(file)){
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) return 1;
		}
		if(!saveFile(file)){
			getApp()->beep();
			FXMessageBox::error(this,MBOX_OK,tr("Error Saving File"),tr("Unable to save file: %s"),file.text());
			return 1;
		}
		onApply(NULL, 0, NULL);
		map_text21->setModified(0);
		FXString u;
		u = "saved: ";
		u += file;
		u += "\n";
		input_text->appendText(u);
		input_text->setBottomLine(INT_MAX);
    }

	return 1;
}

// Save file
long 
MainWindow::saveFile(const FXString& file){
	FXFile textfile(file,FXFile::Writing);
	long saved=0;

	// Opened file?
	if(textfile.isOpen()){
		FXchar *text; FXint size,n;

		// Get size
		size=map_text21->getLength();

		// Alloc buffer
		if(allocElms(text,size+1)){

			// Set wait cursor
			getApp()->beginWaitCursor();

			// Get text from editor
			map_text21->getText(text,size);

			// Write the file
			n=textfile.writeBlock(text,size);
			if(n==size){

				// Success
				saved=1;
			}

			// Kill wait cursor
			getApp()->endWaitCursor();

			// Free buffer
			freeElms(text);

			FXString u;
			FXString v;
			u = "saved size: ";
#if (FOX_MINOR >= 7)
			v.fromInt(size, 10);
#else
			v = FXStringVal(size, 10);
#endif
			u += v;
			u += "\n";
			input_text->appendText(u);
			input_text->setBottomLine(INT_MAX);
		}
	}
	return saved;
}

uint8_t
MainWindow::get_key_nr(FXString s){
	for(int i=0; i < lines; i++) {
		if(!compare(mapusb[i].key, s)) {
			return mapusb[i].usb_hid_key;
		}
	}
	return 0xFF;
}

FXString
MainWindow::get_key_from_nr(uint8_t nr){
	for(int i=0; i < lines; i++) {
		if(nr == mapusb[i].usb_hid_key) {
			return mapusb[i].key;
		}
	}
	return "ff";
}

long
MainWindow::onGeeprom(FXObject *sender, FXSelector sel, void *ptr){
	if(map_text21->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	map_text21->setText(NULL,0);
	for(int i = 0; i < irdatanr; i++) {
	    FXString u;
#if (FOX_MINOR >= 7)
	    FXString v;
	    v.fromInt(i + 1,10);
	    u += v;
#else
	    u += FXStringVal(i + 1,10);
#endif
	    line_text->setText(u);
	    onGirdata(NULL, 0, NULL);
	    FXString s;
	    FXString t;
	    s = protocol_text->getText();
	    t = address_text->getText();
	    s += t;
	    t = command_text->getText();
	    s += t;
	    s += flag_text->getText();
	    s += " ";
	    onGkey(NULL, 0, NULL);
	    s += modifier_text->getText();
	    s += "|";
	    s += key_text->getText();
	    if(i < irdatanr - 1)
		s += "\n";
	    map_text21->appendText(s);
	}
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	key_text->setText("KEY_");
	modifier_text->setText("ff");
	onApply(NULL, 0, NULL);
	//map_text21->setModified(1);

	return 1;
}

long
MainWindow::onPeeprom(FXObject *sender, FXSelector sel, void *ptr){
	if(map_text21->isModified()){
		onApply(NULL, 0, NULL);
	}
	FXString nr, nrp;
	for(int i = 0; i < active_lines; i++) {
	if(i >= irdatanr) {
		nr += "too many lines\n";
		input_text->appendText(nr);
		input_text->setBottomLine(INT_MAX);
	return 1;
	}

#if (FOX_MINOR >= 7)
	nrp.fromInt(i + 1,10);
#else
	nrp = FXStringVal(i + 1,10);
#endif
	line_text->setText(nrp);

#if (FOX_MINOR >= 7)
	nr.fromInt(i,16);
#else
	nr = FXStringVal(i,16);
#endif

	FXString s, u, t;
	map_text21->extractText(u, mapbeg[i], 12);
	if(compare(u, "ffffffffffff")) { // flash only if not ffffffffffff
		s = "3 0 1 2 "; // Report_ID STAT_CMD ACC_SET CMD_IRDATA
		s += nr;
		s += " ";
		map_text21->extractText(u, mapbeg[i], 2);
		s += u;
		s += " ";
		map_text21->extractText(u, mapbeg[i]+4, 2);
		s += u;
		s += " ";
		map_text21->extractText(u, mapbeg[i]+2, 2);
		s += u;
		s += " ";
		map_text21->extractText(u, mapbeg[i]+8, 2);
		s += u;
		s += " ";
		map_text21->extractText(u, mapbeg[i]+6, 2);
		s += u;
		s += " ";
		map_text21->extractText(u, mapbeg[i]+10, 2);
		s += u;
		output_text->setText(s);

		Write_and_Check();
	}

	s = "3 0 1 3 "; // Report_ID STAT_CMD ACC_SET CMD_KEY
	s += nr;
	s += " ";
	map_text21->extractText(u, mapbeg[i] + map[i*2].length() + 1, map[i*2+1].length());
	// remove #comment from KEY_X|KEY_Y#comment
	const char *z = "#";
	u = u.section(z, 0, 1);
	// split KEY_X|KEY_Y
	const char *y = "|";
#if (FOX_MINOR >= 7)
	t.fromInt(get_key_nr(u.section(y, 1, 1)),16);
#else
	t = FXStringVal(get_key_nr(u.section(y, 1, 1)),16);
#endif
	if(!compare(t, "0")) // flash only if not ff or empty
		return 1;
	s += t;
	s += " ";
#if (FOX_MINOR >= 7)
	t.fromInt(get_key_nr(u.section(y, 0, 1)),16);
#else
	t = FXStringVal(get_key_nr(u.section(y, 0, 1)),16);
#endif
	s += t;
	s += " ";
	output_text->setText(s);

	Write_and_Check();

	//onGeeprom();
  }
	return 1;
}

long
MainWindow::onApply(FXObject *sender, FXSelector sel, void *ptr){
	// fill map[] and mapbeg[]
	const char *delim = " \t\r\n"; // Space, Tab, CR and LF
	FXString data = map_text21->getText();
	const FXchar *d = data.text();
	size_t i = 0;
	size_t sz = strlen(d);
	char *str = (char*) malloc(sz+1);
	strcpy(str, d);
	char *token = strtok(str, delim);
	memset(mapbeg, 0, sizeof(mapbeg));
	int count = 0;
	while (token) {
		map[i++] = token;
		count += map[i-1].length() + 1;
		if(!(i%2))
			mapbeg[(i+1)/2] = count;
		token = strtok(NULL, delim);
	}
	free(str);
	active_lines = i / 2;

	FXString u;
	FXString v;
	u = "map mapbeg: \n";
	for(int i = 0; i < active_lines; i++) {
		v = map[i*2];
		u += v;
		u += " ";
		v = map[i*2+1];
		u += v;
		u += " ";
#if (FOX_MINOR >= 7)
		v.fromInt(mapbeg[i], 10);
#else
		v = FXStringVal(mapbeg[i],10);
#endif
		u += v;
		u += "\n";
	}
	input_text->appendText(u);
	input_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onClear(FXObject *sender, FXSelector sel, void *ptr)
{
	input_text->setText("");

	return 1;
}

long MainWindow::onCmdwsListBox(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,wslistbox->getCurrentItem()));
	return 1;
  }

long MainWindow::onCmdrsListBox(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,rslistbox->getCurrentItem()));
	return 1;
  }


long MainWindow::onDevDClicked(FXObject *sender, FXSelector sel, void *ptr){
	onDisconnect(NULL, 0, NULL);
	onConnect(NULL, 0, NULL);
	return 1;
}

long
MainWindow::onMacTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
#ifdef __APPLE__
	check_apple_events();
	
	getApp()->addTimeout(this, ID_MAC_TIMER,
		50 * timeout_scalar /*50ms*/);
#endif

	return 1;
}

int main(int argc, char **argv)
{
	FXApp app("IRMP STM32 Configuration", "");
	app.init(argc, argv);
	g_main_window = new MainWindow(&app);
	app.create();
	app.run();
	return 0;
}
