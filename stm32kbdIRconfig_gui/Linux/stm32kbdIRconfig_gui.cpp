/*
 *  GUI Config Tool for IRMP STM32 KBD devices
 *
 *  Copyright (C) 2015-2025 Joerg Riechardt
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
#include "fxkeys_jr.h"
#include "upgrade.h"
#include "protocols.h"

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
	#include <FXCP1252Codec.h>
#else
	#include <unistd.h>
#endif

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
		ID_PIRDATA,
		ID_PKEY,
		ID_PREPEAT,
		ID_PRWAKEUP,
		ID_PRMACRO,
		ID_PRIRDATA,
		ID_PR_KBD_IRDATA,
		ID_GWAKEUP,
		ID_GMACRO,
		ID_GIRDATA,
		ID_GKEY,
		ID_GREPEAT,
		ID_GCAP,
		ID_GEEPROM,
		ID_PEEPROM,
		ID_REEPROM,
		ID_AGET,
		ID_ASET,
		ID_RWAKEUP,
		ID_RMACRO,
		ID_RIRDATA,
		ID_RKEY,
		ID_RREPEAT,
		ID_RALARM,
		ID_UPGRADE,
		ID_PRINT,
		ID_CLEAR,
		ID_MAC_TIMER,
		ID_LAST,
		ID_WSLISTBOX,
		ID_MNLISTBOX,
		ID_MSLISTBOX,
		ID_RSLISTBOX,
		ID_OPEN,
		ID_SAVE,
		ID_SAVE_LOG,
		ID_DEVLIST,
		ID_KBD_TIMER,
		ID_CEEPROM,
		ID_GREEPROM,
		ID_PRIRDATA_TIMER
	};

enum access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum command {
	CMD_CAPS,
	CMD_ALARM,
	CMD_IRDATA,
	CMD_KEY,
	CMD_WAKE,
	CMD_REBOOT,
	CMD_IRDATA_REMOTE,
	CMD_WAKE_REMOTE,
	CMD_REPEAT,
	CMD_EEPROM_RESET,
	CMD_EEPROM_COMMIT,
	CMD_EEPROM_GET_RAW,
	CMD_HID_TEST,
	CMD_STATUSLED,
	CMD_EMIT,
	CMD_NEOPIXEL,
	CMD_MACRO,
	CMD_MACRO_REMOTE,
};

enum status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

enum report_id {
	REPORT_ID_KBD = 1,
	REPORT_ID_CONFIG_IN = 2,
	REPORT_ID_CONFIG_OUT = 3
};

	FXGUISignal *guisignal = new FXGUISignal(getApp(), this, ID_PRINT);
	Upgrade doUpgrade;

private:
	FXList *device_list;
	FXButton *connect_button;
	FXButton *disconnect_button;
	FXButton *rescan_button;
	FXButton *output_button;
	FXButton *reboot_button;
	FXButton *pwakeup_button;
	FXButton *pmacro_button;
	FXButton *pirdata_button;
	FXButton *pkey_button;
	FXButton *prepeat_button;
	FXButton *prwakeup_button;
	FXButton *prmacro_button;
	FXButton *prirdata_button;
	FXButton *pr_keyboard_and_irdata_button;
	FXButton *gwakeup_button;
	FXButton *gmacro_button;
	FXButton *girdata_button;
	FXButton *gkey_button;
	FXButton *grepeat_button;
	FXButton *gcap_button;
	FXButton *aget_button;
	FXButton *aset_button;
	FXButton *rwakeup_button;
	FXButton *rmacro_button;
	FXButton *rirdata_button;
	FXButton *rkey_button;
	FXButton *rrepeat_button;
	FXButton *ralarm_button;
	FXButton *upgrade_button;
	FXButton *open_button;
	FXButton *save_button;
	FXButton *flash_button;
	FXButton *get_button;
	FXButton *reset_button;
	FXButton *commit_button;
	FXButton *get_raw_button;
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
	FXTextField *pr_kbd_irdata_text;
	FXTextField *pr_kbd_irdata_text_2;
	FXText *input_text;
	FXText *map_text21;
	FXListBox* wslistbox;
	FXListBox* mnlistbox;
	FXListBox* mslistbox;
	FXListBox* rslistbox;
	FXTextField *repeat_text;
	FXTextField *modifier_text;
	FXTextField *key_text;
	FXTextField *line_text;
	FXStatusBar *statusbar;
	struct hid_device_info *devices;
	hid_device *connected_device;
	size_t getDataFromTextField(FXTextField *tf, uint8_t *buf, size_t len);
	uint8_t buf[64];
	uint8_t bufw[64];
	int wakeupslots;
	int macrodepth;
	int macroslots;
	int irdatanr;
	int in_size;
	int out_size;
	FXString protocols;
	FXString firmware;
	FXString firmware1;
	FXString uC;
	FXColor storedShadowColor;
	FXColor storedBaseColor;
	FXColor storedBackColor;
	int RepeatCounter;
	FXString map[400];
	int mapbeg[200];
	int active_lines;
	int max;
	int count;
	char firmwarefile[512];
	char print[1024];
	char printcollect[4096];
	FXint cur_item;
	FXint num_devices_before_upgrade;
	FXint num_devices_after_rescan;
	FXlong starttime;
	int got_key;
	int got_modifier;
	int PR_kbd_irdata_Active;
	int PR_irdata_Active;
	int PR_wakeup_Active;
	FXString last_modifier;
	FXString last_key;
	int template_mode;

protected:
	MainWindow() {};
public:
	MainWindow(FXApp *app);
	~MainWindow();
	virtual void create();
	
	long onConnect(FXObject *sender, FXSelector sel, void *ptr);
	long onDisconnect(FXObject *sender, FXSelector sel, void *ptr);
	long onRescan(FXObject *sender, FXSelector sel, void *ptr);
	long onReboot(FXObject *sender, FXSelector sel, void *ptr);
	long onSendOutputReport(FXObject *sender, FXSelector sel, void *ptr);
	long onPwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onPmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onPirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onPkey(FXObject *sender, FXSelector sel, void *ptr);
	long onPrepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onPRwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onPRmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onPRirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onGwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onGmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onGirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onGkey(FXObject *sender, FXSelector sel, void *ptr);
	long onGrepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onGcaps(FXObject *sender, FXSelector sel, void *ptr);
	long onAget(FXObject *sender, FXSelector sel, void *ptr);
	long onAset(FXObject *sender, FXSelector sel, void *ptr);
	long onRwakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onRmacro(FXObject *sender, FXSelector sel, void *ptr);
	long onRirdata(FXObject *sender, FXSelector sel, void *ptr);
	long onRkey(FXObject *sender, FXSelector sel, void *ptr);
	long onRrepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onRalarm(FXObject *sender, FXSelector sel, void *ptr);
	long onUpgrade(FXObject *sender, FXSelector sel, void *ptr);
	long onPrint(FXObject *sender, FXSelector sel, void *ptr);
	long onClear(FXObject *sender, FXSelector sel, void *ptr);
	long onMacTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onCmdwsListBox(FXObject*,FXSelector,void*);
	long onCmdmnListBox(FXObject*,FXSelector,void*);
	long onCmdmsListBox(FXObject*,FXSelector,void*);
	long onCmdrsListBox(FXObject*,FXSelector,void*);
	long onNew(FXObject *sender, FXSelector sel, void *ptr);
	long onOpen(FXObject *sender, FXSelector sel, void *ptr);
	long onSave(FXObject *sender, FXSelector sel, void *ptr);
	long onSaveLog(FXObject *sender, FXSelector sel, void *ptr);
	long onReeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onCeeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onGeeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onGReeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onPeeprom(FXObject *sender, FXSelector sel, void *ptr);
	uint8_t get_hex_from_key(FXString s);
	FXString get_key_from_hex(uint8_t hex);
	long Write(int out_len);
	long Read(int show_len);
	long Write_and_Check(int out_len, int show_len);
	long saveFile(const FXString& file);
	long saveLogFile(const FXString& file);
	long onApply(FXObject *sender, FXSelector sel, void *ptr);
	long onDevDClicked(FXObject *sender, FXSelector sel, void *ptr);
	long onCmdQuit(FXObject *sender, FXSelector sel, void *ptr);
	long onKeyPress(FXObject *sender, FXSelector sel, void *ptr);
	FXString get_key_from_event_code(uint32_t code);
	uint32_t timestamp;
	long onPR_kbd_irdata(FXObject *sender, FXSelector sel, void *ptr);
	long onKbdTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onPRirdataTimeout(FXObject *sender, FXSelector sel, void *ptr);
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
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PMACRO, MainWindow::onPmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PIRDATA, MainWindow::onPirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PKEY, MainWindow::onPkey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PREPEAT, MainWindow::onPrepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PRWAKEUP, MainWindow::onPRwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PRMACRO, MainWindow::onPRmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PRIRDATA, MainWindow::onPRirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PR_KBD_IRDATA, MainWindow::onPR_kbd_irdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GWAKEUP, MainWindow::onGwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GMACRO, MainWindow::onGmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GIRDATA, MainWindow::onGirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GKEY, MainWindow::onGkey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GREPEAT, MainWindow::onGrepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GCAP, MainWindow::onGcaps ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CEEPROM, MainWindow::onCeeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GEEPROM, MainWindow::onGeeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GREEPROM, MainWindow::onGReeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_PEEPROM, MainWindow::onPeeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_REEPROM, MainWindow::onReeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_AGET, MainWindow::onAget ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_ASET, MainWindow::onAset ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RWAKEUP, MainWindow::onRwakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RMACRO, MainWindow::onRmacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RIRDATA, MainWindow::onRirdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RKEY, MainWindow::onRkey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RREPEAT, MainWindow::onRrepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RALARM, MainWindow::onRalarm ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_UPGRADE, MainWindow::onUpgrade ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CLEAR, MainWindow::onClear ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_MAC_TIMER, MainWindow::onMacTimeout ),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_WSLISTBOX, MainWindow::onCmdwsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_WSLISTBOX, MainWindow::onCmdwsListBox),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_MNLISTBOX, MainWindow::onCmdmnListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_MNLISTBOX, MainWindow::onCmdmnListBox),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_MSLISTBOX, MainWindow::onCmdmsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_MSLISTBOX, MainWindow::onCmdmsListBox),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_RSLISTBOX, MainWindow::onCmdrsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RSLISTBOX, MainWindow::onCmdrsListBox),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_OPEN, MainWindow::onOpen ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SAVE, MainWindow::onSave ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SAVE_LOG, MainWindow::onSaveLog ),
	FXMAPFUNC(SEL_CLOSE,   0, MainWindow::onCmdQuit ),
	FXMAPFUNC(SEL_IO_READ, MainWindow::ID_PRINT, MainWindow::onPrint),
	FXMAPFUNC(SEL_KEYPRESS, MainWindow::ID_PR_KBD_IRDATA, MainWindow::onKeyPress),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_KBD_TIMER, MainWindow::onKbdTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_PRIRDATA_TIMER, MainWindow::onPRirdataTimeout ),
};

FXIMPLEMENT(MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER(MainWindowMap));

MainWindow::MainWindow(FXApp *app)
	: FXMainWindow(app, "IRMP STM32 KBD Configuration", NULL, NULL, DECOR_ALL, 425, 39, 1200, 1030)  // for 1920x1080
{
	this->setIcon(new FXGIFIcon(app,Icon,0,IMAGE_OPAQUE)); // for taskbar
	this->setMiniIcon(new FXGIFIcon(app,Icon,0,IMAGE_OPAQUE)); // for titlebar
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
	pmacro_button = new FXButton(gb121, "macro", NULL, this, ID_PMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	pirdata_button = new FXButton(gb121, "irdata", NULL, this, ID_PIRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	pkey_button = new FXButton(gb121, "key", NULL, this, ID_PKEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	prepeat_button = new FXButton(gb121, "repeat", NULL, this, ID_PREPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	aset_button = new FXButton(gb121, "alarm", NULL, this, ID_ASET, BUTTON_NORMAL|LAYOUT_FILL_X);
	//set by remote Group Box
	FXGroupBox *gb122 = new FXGroupBox(hf12, "set by remote", FRAME_GROOVE|LAYOUT_FILL_X);
	prwakeup_button = new FXButton(gb122, "wakeup", NULL, this, ID_PRWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	prmacro_button = new FXButton(gb122, "macro", NULL, this, ID_PRMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	prirdata_button = new FXButton(gb122, "irdata", NULL, this, ID_PRIRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	pr_keyboard_and_irdata_button = new FXButton(gb122, "keyboard + irdata", NULL, this, ID_PR_KBD_IRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	pr_kbd_irdata_text = new FXTextField(new FXHorizontalFrame(gb122,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	pr_kbd_irdata_text_2 = new FXTextField(new FXHorizontalFrame(gb122,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	pr_kbd_irdata_text->setEditable(false);
	pr_kbd_irdata_text_2->setEditable(false);
	//get Group Box
	FXGroupBox *gb123 = new FXGroupBox(hf12, "get", FRAME_GROOVE|LAYOUT_FILL_X);
	gwakeup_button = new FXButton(gb123, "wakeup", NULL, this, ID_GWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	gmacro_button = new FXButton(gb123, "macro", NULL, this, ID_GMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	girdata_button = new FXButton(gb123, "irdata", NULL, this, ID_GIRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	gkey_button = new FXButton(gb123, "key", NULL, this, ID_GKEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	grepeat_button = new FXButton(gb123, "repeat", NULL, this, ID_GREPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	aget_button = new FXButton(gb123, "alarm", NULL, this, ID_AGET, BUTTON_NORMAL|LAYOUT_FILL_X);
	//gcap_button = new FXButton(gb123, "caps", NULL, this, ID_GCAP, BUTTON_NORMAL|LAYOUT_FILL_X);
	//reset Group Box
	FXGroupBox *gb124 = new FXGroupBox(hf12, "reset", FRAME_GROOVE|LAYOUT_FILL_X);
	rwakeup_button = new FXButton(gb124, "wakeup", NULL, this, ID_RWAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	rmacro_button = new FXButton(gb124, "macro", NULL, this, ID_RMACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	rirdata_button = new FXButton(gb124, "irdata", NULL, this, ID_RIRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	rkey_button = new FXButton(gb124, "key", NULL, this, ID_RKEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	rrepeat_button = new FXButton(gb124, "repeat", NULL, this, ID_RREPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	ralarm_button = new FXButton(gb124, "alarm", NULL, this, ID_RALARM, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for IR Group Box, alarm Group Box, select listboxes, PC->STM32 Group Box, eeprom group box, firmware group box and eeprom map group box
	FXHorizontalFrame *hf13 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	// 3 vertical frames 2:1:1
	FXSpring *s131 = new FXSpring(hf13, LAYOUT_FILL_X, 200, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf131 = new FXVerticalFrame(s131, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	FXSpring *s132 = new FXSpring(hf13, LAYOUT_FILL_X, 100, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf132 = new FXVerticalFrame(s132, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,3,4,0, 0,8);
	FXSpring *s133 = new FXSpring(hf13, LAYOUT_FILL_X, 100, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf133 = new FXVerticalFrame(s133, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 3,4,4,0, 0,0);

	// horizontal frame for IR Group Box, alarm Group Box and select Group Box
	FXHorizontalFrame *hf1311 = new FXHorizontalFrame(vf131, LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
	// 2 vertical frames 1:1
	FXSpring *s13111 = new FXSpring(hf1311, LAYOUT_FILL_X, 200, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf13111 = new FXVerticalFrame(s13111, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
	FXSpring *s13112 = new FXSpring(hf1311, LAYOUT_FILL_X, 200, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf13112 = new FXVerticalFrame(s13112, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);

	//IR Group Box
	FXGroupBox *gb131111 = new FXGroupBox(vf13111, "IR (hex)", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 4,4,4,10);
	FXMatrix *m131111 = new FXMatrix(gb131111, 4, MATRIX_BY_COLUMNS|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN, 0,0,0,0, 0,0,0,0, 0,0);
	new FXLabel(m131111, "protocol");
	new FXLabel(m131111, "address");
	new FXLabel(m131111, "command");
	new FXLabel(m131111, "flag");
	protocol_text = new FXTextField(m131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	address_text = new FXTextField(m131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	command_text = new FXTextField(m131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	flag_text = new FXTextField(m131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	//alarm Group Box
	FXGroupBox *gb131112 = new FXGroupBox(vf13111, "alarm (dec)", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 4,4,4,10);
	FXMatrix *m131112 = new FXMatrix(gb131112, 4, MATRIX_BY_COLUMNS|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN, 0,0,0,0, 0,0,0,0, 0,0);
	new FXLabel(m131112, "days");
	new FXLabel(m131112, "hours");
	new FXLabel(m131112, "minutes");
	new FXLabel(m131112, "seconds");
	days_text = new FXTextField(m131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	hours_text = new FXTextField(m131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	minutes_text = new FXTextField(m131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	seconds_text = new FXTextField(m131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);

	// select Group Box
	FXGroupBox *gb13112 = new FXGroupBox(vf13112, "select", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,0,6);
	wslistbox=new FXListBox(gb13112,this,ID_WSLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	mnlistbox=new FXListBox(gb13112,this,ID_MNLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	mslistbox=new FXListBox(gb13112,this,ID_MSLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	rslistbox=new FXListBox(gb13112,this,ID_RSLISTBOX,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	FXHorizontalFrame *hf13112 = new FXHorizontalFrame(gb13112, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0/*, 0,0*/);
	new FXLabel(hf13112, "repeat");
	repeat_text = new FXTextField(hf13112, 10, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);

	// eeprom group box
	FXGroupBox *gb1321 = new FXGroupBox(vf132, "eeprom", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,0,6);
	flash_button = new FXButton(gb1321, "flash", NULL, this, ID_PEEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_button = new FXButton(gb1321, "get", NULL, this, ID_GEEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	reset_button = new FXButton(gb1321, "reset", NULL, this, ID_REEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	commit_button = new FXButton(gb1321, "commit", NULL, this, ID_CEEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_raw_button = new FXButton(gb1321, "get raw", NULL, this, ID_GREEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);

	// firmware group box
	FXGroupBox *gb1322 = new FXGroupBox(vf132, "firmware", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 4,4,4,11);
	upgrade_button = new FXButton(gb1322, "upgrade", NULL, this, ID_UPGRADE, BUTTON_NORMAL|LAYOUT_FILL_X);

	// eeprom map group box
	FXGroupBox *gb133 = new FXGroupBox(vf133, "eeprom map", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,0,4);
	open_button = new FXButton(gb133, "open file", NULL, this, ID_OPEN, BUTTON_NORMAL|LAYOUT_FILL_X);
	save_button = new FXButton(gb133, "save file", NULL, this, ID_SAVE, BUTTON_NORMAL|LAYOUT_FILL_X);
	new FXLabel(gb133, "modifier");
	FXVerticalFrame *innerVF10 = new FXVerticalFrame(gb133, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	modifier_text = new FXTextField(new FXHorizontalFrame(innerVF10,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	new FXLabel(gb133, "key");
	FXVerticalFrame *innerVF9 = new FXVerticalFrame(gb133, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	key_text = new FXTextField(new FXHorizontalFrame(innerVF9,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	new FXLabel(gb133, "line");
	FXVerticalFrame *innerVF11 = new FXVerticalFrame(gb133, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	line_text = new FXTextField(new FXHorizontalFrame(innerVF11,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, map_text21, FXText::ID_CURSOR_ROW, LAYOUT_FILL_X);

	// horizontal frame for PC->STM32 Group Box
	FXHorizontalFrame *hf1312 = new FXHorizontalFrame(vf131, LAYOUT_FILL_X, 0,0,0,0/*, 0,0,0,0, 0,0*/);
	// PC->STM32 Group Box
	FXGroupBox *gb1312 = new FXGroupBox(hf1312, "PC->STM32", FRAME_GROOVE|LAYOUT_FILL_X);
	FXHorizontalFrame *hf13121 = new FXHorizontalFrame(gb1312, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,4,3/*, 0,0*/);
	new FXLabel(hf13121, "Data");
	output_text = new FXTextField(hf13121, 29, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	output_button = new FXButton(hf13121, "Send to IRMP Device", NULL, this, ID_SEND_OUTPUT_REPORT, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for Input Group Box
	FXHorizontalFrame *hf16 = new FXHorizontalFrame(vf1, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	// Input Group Box
	FXGroupBox *gb16 = new FXGroupBox(hf16, "debug messages", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXHorizontalFrame *innerHF16 = new FXHorizontalFrame(gb16, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	input_text = new FXText(new FXHorizontalFrame(innerHF16,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	input_text->setEditable(false);
	FXVerticalFrame *innerVF16 = new FXVerticalFrame(innerHF16,LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXButton *clear_button = new FXButton(innerVF16, "Clear", NULL, this, ID_CLEAR, BUTTON_NORMAL|LAYOUT_FILL_X);
	FXButton *save_log_button = new FXButton(innerVF16, "Save", NULL, this, ID_SAVE_LOG, BUTTON_NORMAL|LAYOUT_FILL_X);

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
	pmacro_button->setHelpText("set macro");
	pirdata_button->setHelpText("set irdata");
	pkey_button->setHelpText("set key");
	prwakeup_button->setHelpText("set wakeup by remote");
	prmacro_button->setHelpText("set macro by remote");
	prirdata_button->setHelpText("set irdata by remote");
	pr_keyboard_and_irdata_button->setHelpText("set key by keyboard and irdata by remote, press again in order to stop - press twice for testing keys");
	gwakeup_button->setHelpText("get wakeup");
	gmacro_button->setHelpText("get macro");
	girdata_button->setHelpText("get irdata");
	gkey_button->setHelpText("get key");
	//gcap_button->setHelpText("get capabilities");
	rwakeup_button->setHelpText("reset wakeup");
	rmacro_button->setHelpText("reset macro");
	rirdata_button->setHelpText("reset irdata");
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
	upgrade_button->setHelpText("upgrade firmware");
	commit_button->setHelpText("RP2xxx: commit eeprom");
	get_raw_button->setHelpText("RP2xxx: get eeprom raw");
	aset_button->setHelpText("set alarm");
	aget_button->setHelpText("get alarm");
	wslistbox->setHelpText("wakeup to be set");
	mnlistbox->setHelpText("macronumber to be set");
	mslistbox->setHelpText("macroslot to be set");
	rslistbox->setHelpText("repeat to be set");
	repeat_text->setHelpText("repeat");
	output_text->setHelpText("data to be sent to device (experts only)");
	output_button->setHelpText("send to device");
	input_text->setHelpText("debug messages");
	clear_button->setHelpText("clear debug messages");
	save_log_button->setHelpText("save debug messages");
	open_button->setHelpText("open translation map");
	modifier_text->setHelpText("modifier i.e. LeftShift");
	key_text->setHelpText("key");
	save_button->setHelpText("save translation map");
	map_text21->setHelpText("eeprom translation map");
	flash_button->setHelpText("flash into eeprom");
	get_button->setHelpText("get from eeprom");
	reset_button->setHelpText("reset eeprom");
	line_text->setHelpText("line in eeprom map");
	prepeat_button->setHelpText("set repeat");
	grepeat_button->setHelpText("get repeat");
	rrepeat_button->setHelpText("reset repeat");
	pr_kbd_irdata_text->setHelpText("hints for keyboard + irdata");
	pr_kbd_irdata_text_2->setHelpText("hints for keyboard + irdata");

	// disable buttons
	output_button->disable();
	pwakeup_button->disable();
	pmacro_button->disable();
	pirdata_button->disable();
	pkey_button->disable();
	prwakeup_button->disable();
	prmacro_button->disable();
	prirdata_button->disable();
	pr_keyboard_and_irdata_button->disable();
	gwakeup_button->disable();
	gkey_button->disable();
	gmacro_button->disable();
	girdata_button->disable();
	//gcap_button->disable();
	aget_button->disable();
	aset_button->disable();
	rwakeup_button->disable();
	rmacro_button->disable();
	rirdata_button->disable();
	rkey_button->disable();
	ralarm_button->disable();
	reboot_button->disable();
	prepeat_button->disable();
	grepeat_button->disable();
	rrepeat_button->disable();
	flash_button->disable();
	get_button->disable();
	reset_button->disable();
	commit_button->disable();
	get_raw_button->disable();

	// save Colors
        storedShadowColor = pr_keyboard_and_irdata_button->getShadowColor();
        storedBaseColor = pr_keyboard_and_irdata_button->getBaseColor();
        storedBackColor = pr_keyboard_and_irdata_button->getBackColor();

	// initialize
	RepeatCounter = 0;
	active_lines = 0;
	wakeupslots = 0;
	macrodepth = 0;
	macroslots = 0;
	irdatanr = 0;
	protocols = "";
	firmware = "";
	firmware1 = "";
	max = 0;
	count = 0;
	got_key = 0;
	got_modifier = 0;
	PR_kbd_irdata_Active = 0;
	PR_wakeup_Active = 0;
	PR_irdata_Active = 0;
	pr_kbd_irdata_text->setTextColor(FXRGB(255,0,0));
	pr_kbd_irdata_text_2->setTextColor(FXRGB(255,0,0));
	last_modifier = "";
	last_key = "";
	template_mode = 0;
}

MainWindow::~MainWindow()
{
	if (connected_device)
		hid_close(connected_device);
	hid_exit();
	delete guisignal;
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
	if (connected_device)
		return 1;

	starttime = FXThread::time() / 1000000; // ms
	
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

	hid_set_nonblocking(connected_device, 1);

	unsigned char descriptor[HID_API_MAX_REPORT_DESCRIPTOR_SIZE];
	int res = 0;

	res = hid_get_report_descriptor(connected_device, descriptor, sizeof(descriptor));
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, "Report Descriptor Error", "Unable to get Report Descriptor");
		return -1;
	} else {
		/*printf("Report Descriptor Size: %d\n", res);
		printf("Report Descriptor:");
		for (int i = 0; i < res; i++) {
			printf(" %02x", descriptor[i]);
		}
		printf("\n");*/
	}

	/* Get Report count */
	for(int n = 0; n < res; n++) {
		if(descriptor[n] == 0x95 && descriptor[n+2] == 0x81){ // REPORT_COUNT, INPUT
			in_size = descriptor[n+1] + 1;
		}
		if(descriptor[n] == 0x95 && descriptor[n+2] == 0x91){ // REPORT_COUNT, OUTPUT
			out_size = descriptor[n+1] + 1;
			break;
		}
	}

	FXString s, t, u, v, w, x;
	s.format("%x %x %x %x 0 ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_CAPS); // hex!
	output_text->setText(s);
	Write_and_Check(5, 9);

	if(in_size != (buf[7] ? buf[7] : 17))
		u.format("warning: hid in report count mismatch: %u %u\n", in_size, buf[7] ? buf[7] : 17);
	if(out_size != (buf[8] ? buf[8] : 17))
		v.format("warning: hid out report count mismatch: %u %u\n", out_size,  buf[8] ? buf[8] : 17);
	if(!buf[7] || !buf[8])
		w.format("old firmware!\n");
	x += u+v+w;

	if(onGcaps(NULL, 0, NULL) == -1)
		return -1;
	s.format("Connected to: %04hx:%04hx -", device_info->vendor_id, device_info->product_id);
	//s += FXString(" ") + device_info->manufacturer_string;
	s += FXString(" ") + device_info->product_string;
	connected_label->setText(s);
	s = "Firmware: ";
	FXint pos = firmware.find("   ", 3);
	firmware1 = firmware.left(pos);
	firmware.substitute("___","   ");
	firmware.substitute(":_",": ");
	s += firmware;
	connected_label2->setText(s);
	s = "Protocols: ";
	s += protocols;
	connected_label3->setText(s);
	for(int i = 0; i < wakeupslots; i++) {
		s = (i < wakeupslots-1) ? "wakeup" : "reboot";
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
		s += (i > 0 && i < wakeupslots-1) ? t : "";
#else
		s += (i > 0 && i < wakeupslots-1) ? FXStringVal(i,10) : "";
#endif
		wslistbox->appendItem(s);	
	}
	wslistbox->setNumVisible(wakeupslots);
	for(int i = 0; i < macrodepth; i++) {
		s = "macro";
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
		s += t;
#else
		s += FXStringVal(i,10);
#endif
		mnlistbox->appendItem(s);
	}
	mnlistbox->setNumVisible(macrodepth);
	for(int i = 0; i < macroslots; i++) {
		s = "macroslot";
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
		s += t;
#else
		s += FXStringVal(i,10);
#endif
		mslistbox->appendItem(s);
	}
	mslistbox->setNumVisible(macroslots);
	rslistbox->appendItem("repeat delay");
	rslistbox->appendItem("repeat period");
	rslistbox->appendItem("repeat timeout");
	rslistbox->setNumVisible(3);
	onGeeprom(NULL, 0, NULL);
	output_button->enable();
	pwakeup_button->enable();
	pmacro_button->enable();
	pirdata_button->enable();
	pkey_button->enable();
	prepeat_button->enable();
	prwakeup_button->enable();
	prmacro_button->enable();
	prirdata_button->enable();
	pr_keyboard_and_irdata_button->enable();
	gwakeup_button->enable();
	gmacro_button->enable();
	girdata_button->enable();
	gkey_button->enable();
	grepeat_button->enable();
	//gcap_button->enable();
	aget_button->enable();
	aset_button->enable();
	rwakeup_button->enable();
	rmacro_button->enable();
	rirdata_button->enable();
	rkey_button->enable();
	ralarm_button->enable();
	rrepeat_button->enable();
	connect_button->disable();
	disconnect_button->enable();
	flash_button->enable();
	get_button->enable();
	reset_button->enable();
	reboot_button->enable();
	if(uC == "RP2xxx"){
		commit_button->enable();
		get_raw_button->enable();
	}

	//list wakeups, macros and alarm and warn if no STM32
	for(int i = 0; i < wakeupslots; i++) {
#if (FOX_MINOR >= 7)
		t.fromInt(i,16);
#else
		t = FXStringVal(i,16);
#endif
		s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_WAKE);
		s += t;
		output_text->setText(s);
		Write_and_Check(5, 10);
		s = (i < wakeupslots-1) ? "wakeup: " : "reboot: ";
		t.format("%02x", buf[4]);
		v = t;
		t.format("%02x", buf[6]);
		v += t;
		t.format("%02x", buf[5]);
		v += t;
		t.format("%02x", buf[8]);
		v += t;
		t.format("%02x", buf[7]);
		v += t;
		t.format("%02x", buf[9]);
		v += t;
		s += v;
		s += "\n";
		if(v != "ffffffffffff") {
			u += s;
		}
	}
int show_macro = 0;
	for(int i = 0; i < macroslots; i++) {
		for(int k = 0; k < macrodepth; k++) {
#if (FOX_MINOR >= 7)
			t.fromUInt(i,16);
			v.fromUInt(k,16);
#else
			t = FXStringVal(i,16);
			v = FXStringVal(k,16);
#endif
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_MACRO);
			s += t;
			s += " ";
			s += v;
			output_text->setText(s);
			Write_and_Check(6, 10);
			t.format("%02x", buf[4]);
			v = t;
			t.format("%02x", buf[6]);
			v += t;
			t.format("%02x", buf[5]);
			v += t;
			t.format("%02x", buf[8]);
			v += t;
			t.format("%02x", buf[7]);
			v += t;
			t.format("%02x", buf[9]);
			v += t;
			if(v != "ffffffffffff") {
				if(!k) {
					w += "macro: ";
					w += v;
					w += " ->";
				} else {
					w += " ";
					w += v;
					if(k == macrodepth - 1)
						w += "\n";
					show_macro = 1;
				}
			} else {
				if(!k) {
					i = macroslots; // exit both loops
					break;
				} else {
					w += "\n";
					break;
				}
			}
		}
	}
	s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_ALARM);
	output_text->setText(s);
	Write_and_Check(4, 8);
	unsigned int alarm = *((uint32_t *)&buf[4]);
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
	if(uC != "STM32" && uC != "RP2xxx"){
		s += "WARNING: This device's microcontroller is a ";
		s += uC;
		s += ", NOT a STM32 or RP2xxx!\n";
	}
	input_text->setText("");
	output_text->setText("");
	input_text->appendText(x);
	input_text->appendText(u);
	if(show_macro)
		input_text->appendText(w);
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onDisconnect(FXObject *sender, FXSelector sel, void *ptr)
{
	if (!connected_device)
		return 1;
	hid_close(connected_device);
	connected_device = NULL;
	connected_label->setText("Disconnected");
	connected_label2->setText("Firmware:");
	connected_label3->setText("Protocols:");
	protocols = "";
	firmware = "";
	firmware1 = "";
	max = 0;
	count = 0;
	wslistbox->clearItems();
	mnlistbox->clearItems();
	mslistbox->clearItems();
	rslistbox->clearItems();
	output_button->disable();
	pwakeup_button->disable();
	pmacro_button->disable();
	pirdata_button->disable();
	pkey_button->disable();
	prwakeup_button->disable();
	prmacro_button->disable();
	prirdata_button->disable();
	pr_keyboard_and_irdata_button->disable();
	gwakeup_button->disable();
	gkey_button->disable();
	gmacro_button->disable();
	girdata_button->disable();
	//gcap_button->disable();
	aget_button->disable();
	aset_button->disable();
	rwakeup_button->disable();
	rmacro_button->disable();
	rirdata_button->disable();
	rkey_button->disable();
	ralarm_button->disable();
	connect_button->enable();
	disconnect_button->disable();
	reboot_button->disable();
	prepeat_button->disable();
	grepeat_button->disable();
	rrepeat_button->disable();
	flash_button->disable();
	get_button->disable();
	reset_button->disable();
	commit_button->disable();
	get_raw_button->disable();
	getApp()->removeTimeout(this, ID_KBD_TIMER);

	return 1;
}

long
MainWindow::onRescan(FXObject *sender, FXSelector sel, void *ptr)
{
	// the selected device's position in the list may change, so make a new onConnect() mandatory
	onDisconnect(NULL, 0, NULL);

	struct hid_device_info *cur_dev;

	device_list->clearItems();
	
	// List the Devices
	hid_free_enumeration(devices);
	devices = hid_enumerate(0x1209, 0x4445);
	cur_dev = devices;	
	while (cur_dev) {
		// select the hidraw device, not the keyboard device
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

	if (device_list->getNumItems() == 0) {
		device_list->appendItem("*** No Devices Connected ***");
		num_devices_after_rescan = 0;
	}
	else {
		device_list->selectItem(0);
		num_devices_after_rescan = device_list->getNumItems();
	}

	return 1;
}

long
MainWindow::onReboot(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXint success = 1;
	s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REBOOT);
	output_text->setText(s);

	FXint cur_item = device_list->getCurrentItem();
	FXint num_devices_before_reboot = device_list->getNumItems();
	Write_and_Check(4, 4);
	FXThread::sleep(1500000000); // 1,5 s
	do { // wait for device to reappear
		FXThread::sleep(100000000); // 100 ms
		onRescan(NULL, 0, NULL);
		count++;
		if(count > 30) {
			printf("stopped waiting\n");
			success = 0;
			break;
		}
	} while(num_devices_after_rescan != num_devices_before_reboot);
	if(success) {
		device_list->setCurrentItem(cur_item);
		device_list->deselectItem(0);
		device_list->selectItem(cur_item);
		onConnect(NULL, 0, NULL);
	}

	return 1;
}

size_t
MainWindow::getDataFromTextField(FXTextField *tf, uint8_t *buf, size_t len)
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
		char *endptr = NULL;
		long int val = strtol(token, &endptr, 16); // hex!
		buf[i++] = val;
		if (i > len) {
			FXMessageBox::error(this, MBOX_OK, "Invalid length", "Data field is too long.");
			break;
		}
		token = strtok(NULL, delim);
	}
	
	free(str);
	return i;
}

long
MainWindow::Read(int show_len)
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

	int res = hid_read(connected_device, buf, in_size); // nonblocking, must read full length (because of Windows, µC sends full length)!
	
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, "Error Reading", "Could not read from device. Error reported was: %ls", hid_error(connected_device));
		onRescan(NULL, 0, NULL);
		input_text->appendText("read error\n");
		input_text->setBottomLine(INT_MAX);
		return -1;
	} else {
		if (res == 0)
			return 0;

		s.format("Received %d bytes:\n", res);
		for (int i = 0; i < show_len; i++) {
			FXString t;
			t.format("%02x ", buf[i]);
			s += t;
		}
#ifndef _WIN32
		if (buf[0] == REPORT_ID_KBD){
			s += "from remote control, modifier: ";
			s += get_key_from_hex(buf[1]);
			s += ", key: ";
			s += get_key_from_hex(buf[3]);
		}
#endif
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::Write(int out_len)
{
	FXString s;
	memset(bufw, 0, sizeof(bufw));
	FXint output_text_len = getDataFromTextField(output_text, bufw, sizeof(bufw));
	if(out_len > output_text_len)
		out_len = output_text_len;

	FXlong time = FXThread::time() / 1000000; // ms
	uint32_t timestamp = (uint32_t)(time - starttime);

	bufw[43] = timestamp >> 24;
	bufw[44] = (timestamp >> 16) & 0xFF;
	bufw[45] = (timestamp >> 8) & 0xFF;
	bufw[46] = timestamp & 0xFF;

	if (!connected_device) {
		FXMessageBox::error(this, MBOX_OK, "Device Error W", "Unable To Connect to Device");
		s = "Unable To Connect to Device W\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	int res = hid_write(connected_device, bufw, out_len); // may write arbitrary length
	if (res < 0) {
		FXMessageBox::error(this, MBOX_OK, "Error Writing", "Could not write to device. Error reported was: %ls", hid_error(connected_device));
		input_text->appendText("write error\n");
		input_text->setBottomLine(INT_MAX);
		onRescan(NULL, 0, NULL);
		return -1;
	} else {
		s.format("Sent %d bytes:\n", res);
		for (int i = 0; i < out_len; i++) {
			FXString t;
			t.format("%02x ", bufw[i]);
			s += t;
		}
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::Write_and_Check(int out_len, int show_len)
{
	FXString s;
	int read, count = 0;
	long retVal = 1;
	s = "";
#if (0)
	// before writing first empty buffers and read away old stuff
	read = Read(show_len);
	if(read  == -1) {
		s += "W&C first Read(): -1\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	while ((buf[0] == REPORT_ID_CONFIG_IN) && count < 200) {
		read = Read(show_len);
		if(read == -1) {
			s = "W&C loop Read(): -1\n";
			input_text->appendText(s);
			input_text->setBottomLine(INT_MAX);
			return -1;
		}
		s = "cleared read buffer\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		count++;
		FXThread::sleep(3000000); // 3ms
	}
#endif
	if(Write(out_len) == -1) {
		s = "W&C Write(): -1\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	FXThread::sleep(3000000); // 3ms

	read = Read(show_len);
	if(read  == -1) {
		s = "W&C first Read(): -1\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	//TODO consider making Write_and_Check a background thread in order to stay responsive to user interactions, otherwise the user just has to wait during "set by remote"
	while ((buf[0] == REPORT_ID_KBD || read == 0) && count < 5500 ) { // over 5 sec for "set by remote", Read() is nonblocking
		//printf("buf[0] %d, read %d, loop %d\n", buf[0], read, count);
		FXThread::sleep(1000000); // 1ms
		read = Read(show_len);
		if(read == -1) {
			s = "W&C loop Read(): -1\n";
			input_text->appendText(s);
			input_text->setBottomLine(INT_MAX);
			return -1;
		}
		count++;
	}
#if (0)
	while(buf[3] != CMD_CAPS  && (buf[43] != bufw[43] || (buf[44] != bufw[44]) || (buf[45] != bufw[45]) || (buf[46] != bufw[46])) && count < 200) {
		s += "*****************WRONG TIMESTAMP*********************\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);

		FXThread::sleep(3000000); // 3ms
		read = Read(show_len);
		if(read == -1) {
			s += "W&C loop Read(): -1\n";
			input_text->appendText(s);
			input_text->setBottomLine(INT_MAX);
			return -1;
		}
		count++;
	}
#endif
	if((buf[0] == REPORT_ID_CONFIG_IN) && (buf[1] == STAT_SUCCESS) && (buf[2] == bufw[2]) && (buf[3] == bufw[3])) {
		s += "************************OK***************************\n";
		retVal = 1;
	} else {
		s += "**********************ERROR**************************\n";
		retVal = -1;
	}
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);

	return retVal;
}

long
MainWindow::onSendOutputReport(FXObject *sender, FXSelector sel, void *ptr)
{
	Write_and_Check(64, 64);

	return 1;
}

long
MainWindow::onPwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
	t.format("%d ", wslistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_WAKE);
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

	Write_and_Check(11, 4);

	return 1;
}

long
MainWindow::onPmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_MACRO);
	s += t;
	t.format("%d ", mslistbox->getCurrentItem());
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

	Write_and_Check(12, 4);

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
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_IRDATA);
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

	Write_and_Check(11, 4);

	onGirdata(NULL, 0, NULL);

	if(map_text21->isModified())
		onApply(NULL, 0, NULL);
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
	map_text21->setModified(1);

	return 1;
}

long
MainWindow::onPkey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_KEY);
	s += t;
#if (FOX_MINOR >= 7)
	t.fromUInt(get_hex_from_key((key_text->getText())),16);
#else
	t = FXStringVal(get_hex_from_key((key_text->getText())),16);
#endif
#if (FOX_MINOR >= 7)
	u.fromUInt(get_hex_from_key((modifier_text->getText())),16);
#else
	u = FXStringVal(get_hex_from_key((modifier_text->getText())),16);
#endif
	s += t;
	s += " ";
	s += u;
	s += " ";
	output_text->setText(s);

	Write_and_Check(7, 4);

	onGkey(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(map_text21->isModified())
		onApply(NULL, 0, NULL);
	map_text21->removeText(mapbeg[i-1]+map[(i-1)*2].length()+1, map[(i-1)*2+1].length());
	s = modifier_text->getText();
	s += "|";
	s += key_text->getText();
	map_text21->insertText(mapbeg[i-1]+map[(i-1)*2].length()+1, s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i-1]);
	map_text21->setModified(1);

	return 1;
}

long
MainWindow::onPrepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REPEAT);
	u.format("%x ", rslistbox->getCurrentItem());
#if (FOX_MINOR >= 7)
	t.format("%x %x", repeat_text->getText().toUInt() & 0xFF, (repeat_text->getText().toUInt() >> 8) & 0xFF );
#else
	t.format("%x %x", FXUIntVal(repeat_text->getText(), 10) & 0xFF, (FXUIntVal(repeat_text->getText(), 10) >> 8) & 0xFF);
#endif
	s += u;
	s += " ";
	s += t;
	output_text->setText(s);

	Write_and_Check(7, 4);

	return 1;
}

long
MainWindow::onPRwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);
	getApp()->repaint();
	t.format("%x ", wslistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_WAKE_REMOTE);
	s += t;
	output_text->setText(s);

	if(Write_and_Check(5, 4) == -1){
		s = "wakeup timeout\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	onGwakeup(NULL, 0, NULL);

	return 1;
}

long
MainWindow::onPRmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);
	getApp()->repaint();
	t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_MACRO_REMOTE);
	s += t;
	t.format("%d ", mslistbox->getCurrentItem());
	s += t;
	output_text->setText(s);

	getApp()->repaint();

	if(Write_and_Check(6, 4) == -1){
		s = "macro timeout\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		return -1;
	}

	onGmacro(NULL, 0, NULL);

	return 1;
}

long
MainWindow::onPRirdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t, p, a, c;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	if(template_mode){
		s = "we are in loop mode, in order to exit wait for timeout or press same button again\n";
		s += "enter IR data by pressing a button on the remote control within 5 sec\n";
		pr_kbd_irdata_text->setText("press button on remote");
		pr_kbd_irdata_text_2->setText("or wait until timeout");
	} else {
		s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	}
	input_text->appendText(s);
	input_text->setBottomLine(INT_MAX);
	getApp()->repaint();
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_IRDATA_REMOTE);
	s += t;
	output_text->setText(s);

	getApp()->repaint();

	if(Write_and_Check(5, 4) == -1){
		s = "irdata timeout\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		if(template_mode){
			pr_kbd_irdata_text->setText("");
			pr_kbd_irdata_text_2->setText("");
		}
		return -1;
	}

	onGirdata(NULL, 0, NULL);

	// is this already in eeprom map?
	for( int i = 0; i < active_lines; i++) {
		map_text21->extractText(p, mapbeg[i], 2);
		map_text21->extractText(a, mapbeg[i]+2, 4);
		map_text21->extractText(c, mapbeg[i]+6, 4);
#if (FOX_MINOR >= 7)
		if((i != line_text->getText().toInt() - 1) &&
#else
		if((i != FXIntVal(line_text->getText(), 10) - 1) &&
#endif
		   (protocol_text->getText() == p) &&
		   (address_text->getText() == a) &&
		   (command_text->getText() == c)) {
			s = "warning: irdata ";
			s += p;
			s += a;
			s += c;
			s += "00";
			s += " already in eeprom map line ";
#if (FOX_MINOR >= 7)
			t.fromUInt(i + 1, 10);
#else
			t = FXStringVal(i + 1, 10);
#endif
			s += t;
			s += "\n";
			input_text->appendText(s);
			input_text->setBottomLine(INT_MAX);
			if(template_mode){
				pr_kbd_irdata_text->setText("");
				pr_kbd_irdata_text_2->setText("");
			}
			return -1;
		}
	}

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(map_text21->isModified())
		onApply(NULL, 0, NULL);
	map_text21->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += "00";
	map_text21->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	map_text21->setModified(1);

	if(template_mode)
		getApp()->addTimeout(this, ID_PRIRDATA_TIMER, 300 * timeout_scalar /* 300 ms*/); // avoid bounces

	return 1;
}

long
MainWindow::onGwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%x", wslistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_WAKE);
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 10);

	s = "";
	t.format("%02x", buf[4]);
	s += t;
	protocol_text->setText(s);
		
	s = "";
	t.format("%02x", buf[6]);
	s += t;
	t.format("%02x", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02x", buf[8]);
	s += t;
	t.format("%02x", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02x", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_MACRO);
	s += t;
	t.format("%d", mslistbox->getCurrentItem());
	s += t;
	output_text->setText(s);

	Write_and_Check(6, 10);

	s = "";
	t.format("%02x", buf[4]);
	s += t;
	protocol_text->setText(s);

	s = "";
	t.format("%02x", buf[6]);
	s += t;
	t.format("%02x", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02x", buf[8]);
	s += t;
	t.format("%02x", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02x", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGirdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_IRDATA);
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 10);
	
	s = "";
	t.format("%02x", buf[4]);
	s += t;
	protocol_text->setText(s);
		
	s = "";
	t.format("%02x", buf[6]);
	s += t;
	t.format("%02x", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02x", buf[8]);
	s += t;
	t.format("%02x", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02x", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGkey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t, v;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_KEY);
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 6);

	s = get_key_from_hex(buf[5]);
	if (s == "error"){
		t = "invalid hid_modifier ";
#if (FOX_MINOR >= 7)
		v.fromUInt(buf[5],16);
#else
		v = FXStringVal(buf[5],16);
#endif
		t += v;
		FXMessageBox::error(this, MBOX_OK, t.text(), "got invalid hid_modifier");
	}
	modifier_text->setText(s);

	s = get_key_from_hex(buf[4]);
	if (s == "error"){
		t = "invalid hid_key ";
#if (FOX_MINOR >= 7)
		v.fromUInt(buf[5],16);
#else
		v = FXStringVal(buf[5],16);
#endif
		t += v;
		FXMessageBox::error(this, MBOX_OK, t.text(), "got invalid hid_key");
	}
	key_text->setText(s);

	return 1;
}

long
MainWindow::onGrepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_REPEAT);
	u.format("%x ", rslistbox->getCurrentItem());
	s += u;
	s += " ";
	output_text->setText(s);

	Write_and_Check(5, 6);

#if (FOX_MINOR >= 7)
	t.fromUInt(*((uint16_t*)&buf[4]),10);
#else
	t = FXStringVal(*((uint16_t*)&buf[4]),10);
#endif
	repeat_text->setText(t);

	return 1;
}

long
MainWindow::onGcaps(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t, u;
	int jump_to_firmware, romtable;
	jump_to_firmware = 0;
	romtable = 0;
	uC = "";
	for(int i = 0; i < 20; i++) { // for safety stop after 20 loops
		s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_CAPS);
#if (FOX_MINOR >= 7)
		t.fromInt(i,16);
		s += t;
#else
		s += FXStringVal(i,16);
#endif
		s += " ";
		output_text->setText(s);

		Write_and_Check(5, i == 0 ? 9 : in_size);

		if (!i) { // first query for slots and depth
			irdatanr = buf[4];
			s.format("number of irdata: %u\n", buf[4]);
			macroslots = buf[9];
			t.format("number of macros: %u\n", buf[9]);
			s += t;
			macrodepth = buf[10];
			t.format("macro depth: %u\n", buf[10]);
			s += t;
			wakeupslots = buf[6];
			t.format("number of wakeups: %u\n", buf[6]);
			s += t;
			t.format("hid in report count: %u\n", in_size);
			s += t;
			t.format("hid out report count: %u\n", out_size);
			s += t;
		} else {
			if (!jump_to_firmware) { // queries for supported_protocols
				s = "protocols: \n";
				for (int k = 4; k < in_size; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						input_text->appendText(s);
						input_text->setBottomLine(INT_MAX);
						jump_to_firmware = 1;
						goto again;
					}
					t.format("%u ", buf[k]);
					protocols += t;
					u = protocol[buf[k]];
					s += t;
					s += u;
					s += "\n";
				}
			} else { // queries for firmware
				s = "firmware: ";
				for (int k = 4; k < in_size; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						input_text->appendText(s);
						input_text->setBottomLine(INT_MAX);
						return 1;
					}
					if (buf[k] == 42) { // * separator
						romtable = 1;
						firmware += "   uC: ";
						s += "*";
					} else {
						t.format("%c", buf[k]);
						firmware += t;
						if(romtable)
							uC += t;
						s += t;
					}
				}
			}
		}
		s += "\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
again:	;
	}
	return 1;
}

long
MainWindow::onAget(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_ALARM);
	output_text->setText(s);

	Write_and_Check(4, 8);

	unsigned int alarm = *((uint32_t *)&buf[4]);

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
	setalarm += 60 * 60 * 24 * days_text->getText().toUInt();
	setalarm += 60 * 60 * hours_text->getText().toUInt();
	setalarm += 60 * minutes_text->getText().toUInt();
	setalarm += seconds_text->getText().toUInt();
#else
	setalarm += 60 * 60 * 24 * FXUIntVal(days_text->getText(), 10);
	setalarm += 60 * 60 * FXUIntVal(hours_text->getText(), 10);
	setalarm += 60 * FXUIntVal(minutes_text->getText(), 10);
	setalarm += FXUIntVal(seconds_text->getText(), 10);
#endif
	if(setalarm < 2) {
		setalarm = 2;
		u = "set alarm to 2 in order to prevent device or program hangup\n";
	 }

	FXString s;
	FXString t;
	const char *z = " ";
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_ALARM);
#if (FOX_MINOR >= 7)
	t.fromUInt(setalarm,16);
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

	Write_and_Check(8, 4);

	input_text->appendText(u);
	input_text->setBottomLine(INT_MAX);
	
	return 1;
}

long
MainWindow::onRwakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d", wslistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_WAKE);
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 4);

	return 1;
}

long
MainWindow::onRmacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d ", mnlistbox->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_MACRO);
	s += t;
	t.format("%d ", mslistbox->getCurrentItem());
	s += t;
	output_text->setText(s);

	Write_and_Check(6, 4);

	return 1;
}

long
MainWindow::onRirdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_IRDATA);
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 4);

	onGirdata(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(map_text21->isModified())
		onApply(NULL, 0, NULL);
	map_text21->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += flag_text->getText();
	map_text21->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	map_text21->setModified(1);

	return 1;
}

long
MainWindow::onRkey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_KEY);
	s += t;
	output_text->setText(s);

	Write_and_Check(5, 4);

	onGkey(NULL, 0, NULL);

	int i = 0;
	FXint pos = map_text21->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(map_text21->isModified())
		onApply(NULL, 0, NULL);
	map_text21->removeText(mapbeg[i-1]+map[(i-1)*2].length()+1, map[(i-1)*2+1].length());
	s = modifier_text->getText();
	s += "|";
	s += key_text->getText();
	map_text21->insertText(mapbeg[i-1]+map[(i-1)*2].length()+1, s);
	onApply(NULL, 0, NULL);
	map_text21->setCursorPos(mapbeg[i]);
	map_text21->setModified(1);

	return 1;
}

long
MainWindow::onRrepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_REPEAT);
	u.format("%x ", rslistbox->getCurrentItem());
	s += u;
	s += " ";
	output_text->setText(s);

	Write_and_Check(5, 4);

	return 1;
}

long
MainWindow::onRalarm(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_ALARM);
	output_text->setText(s);

	Write_and_Check(4, 4);

	return 1;
}

long
MainWindow::onUpgrade(FXObject *sender, FXSelector sel, void *ptr)
{
	//if(uC != "RP2xxx"){
		const FXchar patterns[]="All Files (*)\nFirmware Files (*.bin)";
		FXString s, v, Filename, FilenameText;
		FXFileDialog open(this,"Open a firmware file");
		open.setPatternList(patterns);
		open.setCurrentPattern(1);
		if(open.execute()){
			Filename = open.getFilename();
			FXint pos = Filename.rfind(PATHSEP);
			FXint endpos = Filename.length();
			FXint suffix_length = open.getCurrentPattern() ? 4 : 0;
			FXString Firmwarename = Filename.mid(pos + 1, endpos - pos - 1 - suffix_length);
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Really upgrade?","Old Firmware: %s\nNew Firmware: %s", firmware1.text(),  Firmwarename.text())) return 1;
			sprintf(printcollect, "%s", "");
#ifndef WIN32
			sprintf(firmwarefile, "%s", Filename.text());
#else
			FXCP1252Codec codec;
			FXString mbstring=codec.utf2mb(Filename); // on Windows file encoding is cp1252, needed for umlaut
			sprintf(firmwarefile, "%s", mbstring.text());
#endif

			if(uC == "STM32"){
			doUpgrade.set_firmwarefile(firmwarefile);
			doUpgrade.set_print(print);
			doUpgrade.set_printcollect(printcollect);
			doUpgrade.set_signal(guisignal);
			doUpgrade.set_RP2xxx_device(false);
			doUpgrade.start();
			}

			cur_item = device_list->getCurrentItem();
			num_devices_before_upgrade = device_list->getNumItems();
			s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REBOOT);
			output_text->setText(s);
			if(connected_device)
				Write_and_Check(4, 4);
			onDisconnect(NULL, 0, NULL);

			if(uC == "RP2xxx"){
			doUpgrade.set_firmwarefile(firmwarefile);
			doUpgrade.set_print(print);
			doUpgrade.set_printcollect(printcollect);
			doUpgrade.set_signal(guisignal);
			doUpgrade.set_RP2xxx_device(true);
			doUpgrade.start();
			}
		}
	/*} else {
		if(MBOX_CLICKED_OK==FXMessageBox::information(this, MBOX_OK_CANCEL, "Firmware upgrade", "Switch the Pico into mass storage device mode.\nIn your file manager than drag and drop the firmware file *.uf2 onto the newly appeared mass storage device.\nThan press buttons 'Re-Scan devices' and 'Connect'.")){
			FXString s;
			s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REBOOT);
			output_text->setText(s);
			Write_and_Check(4, 4);
			FXThread::sleep(1000000000); // 1 s
			onRescan(NULL, 0, NULL);
		}
	}*/

	return 1;
}

long
MainWindow::onPrint(FXObject *sender, FXSelector sel, void *ptr)
{
		FXint success = 1;
		FXString t = print;
		input_text->appendText(t);
		input_text->setBottomLine(INT_MAX);
		if(t == "=== Firmware Upgrade successful! ===\n"){
			int count = 0;
			do { // wait for device to reappear
				FXThread::sleep(100000000); // 100 ms
				onRescan(NULL, 0, NULL);
				count++;
				if(count > 20) {
					printf("stopped waiting\n");
					success = 0;
					break;
				}
			} while(num_devices_after_rescan != num_devices_before_upgrade);
			if(success){
				device_list->setCurrentItem(cur_item);
				device_list->deselectItem(0);
				device_list->selectItem(cur_item);
				onConnect(NULL, 0, NULL);
			} else
				input_text->setText("");
			FXString u = printcollect;
			input_text->appendText(u);
			input_text->setBottomLine(INT_MAX);
		}

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
	FXint n;
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

					template_mode = 1;
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
		u += "entered loop mode for 'set by remote - irdata'\n";
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
		map_text21->setModified(0);
		FXString u;
		u = "save eeprom map to ";
		u += file;
		u += "\n";
		input_text->appendText(u);
		input_text->setBottomLine(INT_MAX);
    }

	return 1;
}

long
MainWindow::onSaveLog(FXObject *sender, FXSelector sel, void *ptr){
	const FXchar patterns[]="All Files (*)\nlog Files (*.log)";
	FXFileDialog save(this,"save the log file");
	FXString file;
	save.setPatternList(patterns);
	save.setCurrentPattern(1);
	if(save.execute()){
		file=save.getFilename();
		if(compare(file.right(4), ".log") && (save.getCurrentPattern() == 1))
			file += ".log";
		if(FXStat::exists(file)){
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) return 1;
		}
		if(!saveLogFile(file)){
			getApp()->beep();
			FXMessageBox::error(this,MBOX_OK,tr("Error Saving File"),tr("Unable to save file: %s"),file.text());
			return 1;
		}
		FXString u;
		u = "save debug messages to ";
		u += file;
		u += "\n";
		input_text->appendText(u);
		input_text->setBottomLine(INT_MAX);
    }

	return 1;
}

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

long
MainWindow::saveLogFile(const FXString& file){
	FXFile textfile(file,FXFile::Writing);
	long saved=0;

	// Opened file?
	if(textfile.isOpen()){
		FXchar *text; FXint size,n;

		// Get size
		size=input_text->getLength();

		// Alloc buffer
		if(allocElms(text,size+1)){

			// Set wait cursor
			getApp()->beginWaitCursor();

			// Get text from debug messages field
			input_text->getText(text,size);

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
MainWindow::get_hex_from_key(FXString s){
	for(int i=0; i < lines; i++) {
		if(!compare(mapusb[i].key, s)) {
			return mapusb[i].usb_hid_key;
		}
	}
	FXString t;
	t = "invalid key ";
	t += s;
	FXMessageBox::error(this, MBOX_OK, t.text(), "only keyboard keys are possible");
	return 0xFF;
}

FXString
MainWindow::get_key_from_hex(uint8_t hex){
	for(int i=0; i < lines; i++) {
		if(hex == mapusb[i].usb_hid_key) {
			return mapusb[i].key;
		}
	}
	return "error";
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
	    v.fromUInt(i + 1,10);
	    u += v;
#else
	    u += FXStringVal(i + 1,10);
#endif
	    line_text->setText(u);
	    FXThread::sleep(3000000); // 3ms
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
	    FXThread::sleep(3000000); // 3ms
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
	map_text21->setCursorPos(0);
	map_text21->setModified(0);
	template_mode = 0;
	input_text->appendText("stopped loop mode for 'set by remote - irdata'\n");
	input_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onPeeprom(FXObject *sender, FXSelector sel, void *ptr){

	int res = onApply(NULL, 0, NULL); // make sure to apply all changes to eeprom map before flashing
	if(!res)
		return 0;

	FXString nr, nrp;
	for(int i = 0; i < active_lines; i++) {
		if(i >= irdatanr) {
			nr = "too many lines\n";
			input_text->appendText(nr);
			input_text->setBottomLine(INT_MAX);
		return 1;
		}

#if (FOX_MINOR >= 7)
		nrp.fromUInt(i + 1,10);
#else
		nrp = FXStringVal(i + 1,10);
#endif
		line_text->setText(nrp);

#if (FOX_MINOR >= 7)
		nr.fromUInt(i,16);
#else
		nr = FXStringVal(i,16);
#endif

		FXString s, u, t, v;
		map_text21->extractText(u, mapbeg[i], map[i*2].length());
		//if(compare(u, "ffffffffffff")) { // flash only if not ffffffffffff
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_IRDATA);
			s += nr;
			s += " ";
			map_text21->extractText(u, mapbeg[i], 2); // TODO check if number
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

			FXThread::sleep(3000000); // 3ms
			Write_and_Check(11, 4);
		//}

		map_text21->extractText(u, mapbeg[i] + map[i*2].length() + 1, map[i*2+1].length());
		// remove #comment from KEY_X|KEY_Y#comment
		const char *z = "#";
		u = u.section(z, 0, 1);
		// split KEY_X|KEY_Y
		const char *y = "|";
#if (FOX_MINOR >= 7)
		t.fromUInt(get_hex_from_key(u.section(y, 1, 1)),16); // TODO include line in error message
		v.fromUInt(get_hex_from_key(u.section(y, 0, 1)),16);
#else
		t = FXStringVal(get_hex_from_key(u.section(y, 1, 1)),16);
		v = FXStringVal(get_hex_from_key(u.section(y, 0, 1)),16);
#endif
		//if(compare(t, "0") || compare(v, "0") ) { // flash only if not ff|ff or empty
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_KEY);
			s += nr;
			s += " ";
			s += t;
			s += " ";
			s += v;
			s += " ";
			output_text->setText(s);

			FXThread::sleep(3000000); // 3ms
			Write_and_Check(7, 4);
		//}
	}

	//onGeeprom();

	return 1;
}

long
MainWindow::onReeprom(FXObject *sender, FXSelector sel, void *ptr){
	if(FXMessageBox::question(this,MBOX_YES_NO,tr("reset eeprom"),tr("really reset eeprom?"))==MBOX_CLICKED_NO) return 1;

	FXString s;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_EEPROM_RESET);

	output_text->setText(s);

	Write_and_Check(4, 4);

	onGeeprom(NULL, 0, NULL);

	return 1;
}

long
MainWindow::onCeeprom(FXObject *sender, FXSelector sel, void *ptr){
	FXString s;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_EEPROM_COMMIT);

	output_text->setText(s);

	Write_and_Check(4, 4);

	return 1;
}

long
MainWindow::onGReeprom(FXObject *sender, FXSelector sel, void *ptr){
	FXString s, t, u;
	for(int k = 31; k >= 0; k--) { // FLASH_SECTOR_SIZE * nr_sectors / size
		for(int l = 0; l < 16; l++) { // size / 32
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_EEPROM_GET_RAW);
#if (FOX_MINOR >= 7)
			t.fromInt(k,16);
			s += t;
#else
			s += FXStringVal(k,16);
#endif
			s += " ";
#if (FOX_MINOR >= 7)
			t.fromInt(l,16);
			s += t;
#else
			s += FXStringVal(l,16);
#endif
			s += " ";
			output_text->setText(s);

			Write_and_Check(6, 36);

			for (int i = 4; i < 36; i++) { // 32
				t.format("%02x", buf[i]);
				u += t;
			}
		}
		u += "\n";
	}
	input_text->appendText(u);
	input_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onApply(FXObject *sender, FXSelector sel, void *ptr){
	// fill map[] and mapbeg[]
	const char *delim = " \t\r\n"; // Space, Tab, CR and LF
	FXString data = map_text21->getText();
	const FXchar *d = data.text();
	size_t k = 0;
	size_t sz = strlen(d);
	char *str = (char*) malloc(sz+1);
	strcpy(str, d);
	char *token = strtok(str, delim);
	memset(mapbeg, 0, sizeof(mapbeg));
	int count = 0;
	while (token) {
		map[k] = token;
		count += map[k].length() + 1;
		if(k%2){ //uneven
			mapbeg[(k+1)/2] = count;
		} else { // even
			if(map[k].length() != 12){ // IRMP_DATA
				FXString s, t;
#if (FOX_MINOR >= 7)
				t.fromUInt(k/2+1,10);
#else
				t = FXStringVal(k/2+1,10);

#endif
				s = "map line ";
				s += t;
				s += " invalid";
				FXMessageBox::error(this, MBOX_OK, s.text(), "irdata length needs to be 12");
				return 0;
			}
		}
		token = strtok(NULL, delim);
		k++;
	}
	free(str);
	active_lines = k / 2;

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
	//input_text->appendText(u);
	//input_text->setBottomLine(INT_MAX);

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

long MainWindow::onCmdmnListBox(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,mnlistbox->getCurrentItem()));
	return 1;
  }

long MainWindow::onCmdmsListBox(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,mslistbox->getCurrentItem()));
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

FXString
MainWindow::get_key_from_event_code(uint32_t code){
	for(int i=0; i < fxkey_lines; i++) {
		if(code == fxkey_map[i].fx_key) {
			return fxkey_map[i].key;
		}
	}
	return "error";
}

long
MainWindow::onKeyPress(FXObject *sender, FXSelector sel, void *ptr)
{
	FXEvent *event = (FXEvent*)ptr;
	FXString s;

	// first reset
	key_text->setText("KEY_");
	modifier_text->setText("ff");

#if (FOX_MINOR >= 7)
		//s.fromUInt(event->code, 16);
#else
		//s = FXStringVal(event->code,16);
#endif

	if(0xFFE0 < event->code && event->code < 0xFFEF){
		modifier_text->setText(get_key_from_event_code(event->code));
		got_modifier = 1;
		//input_text->appendText(s);
		//input_text->appendText(" ");
		input_text->appendText("got modifier ");
		input_text->appendText(modifier_text->getText());
		input_text->appendText("\n");
		input_text->setBottomLine(INT_MAX);
		getApp()->repaint();
	} else {
		key_text->setText(get_key_from_event_code(event->code));
		got_key = 1;
		//input_text->appendText(s);
		//input_text->appendText(" ");
		input_text->appendText("got key ");
		input_text->appendText(key_text->getText());
		input_text->appendText("\n");
		input_text->setBottomLine(INT_MAX);
		getApp()->repaint();
	}

	return 1;
}

long
MainWindow::onPR_kbd_irdata(FXObject *sender, FXSelector sel, void *ptr)
{
	if (!PR_kbd_irdata_Active) {
		PR_kbd_irdata_Active = 1;
                pr_keyboard_and_irdata_button->setBackColor(FXRGB(255,207,207));
                pr_keyboard_and_irdata_button->setBaseColor(FXRGB(0,0,255));
                pr_keyboard_and_irdata_button->setShadowColor(makeShadowColor(FXRGB(0,0,255)));

		modifier_text->setText("ff");
		key_text->setText("KEY_EDIT");
		got_key = 0;
		got_modifier = 0;

		output_button->disable();
		pwakeup_button->disable();
		pmacro_button->disable();
		pirdata_button->disable();
		pkey_button->disable();
		prwakeup_button->disable();
		prmacro_button->disable();
		prirdata_button->disable();
		gwakeup_button->disable();
		gkey_button->disable();
		gmacro_button->disable();
		girdata_button->disable();
		aget_button->disable();
		aset_button->disable();
		rwakeup_button->disable();
		rmacro_button->disable();
		rirdata_button->disable();
		rkey_button->disable();
		ralarm_button->disable();
		reboot_button->disable();
		prepeat_button->disable();
		grepeat_button->disable();
		rrepeat_button->disable();
		flash_button->disable();
		get_button->disable();
		reset_button->disable();
		open_button->disable();
		save_button->disable();
		upgrade_button->disable();
		map_text21->disable();
		device_list->disable();
		disconnect_button->disable();
		rescan_button->disable();
		output_text->disable();
		protocol_text->disable();
		address_text->disable();
		command_text->disable();
		flag_text->disable();
		days_text->disable();
		hours_text->disable();
		minutes_text->disable();
		seconds_text->disable();
		input_text->disable();
		wslistbox->disable();
		mnlistbox->disable();
		mslistbox->disable();
		rslistbox->disable();
		repeat_text->disable();
		modifier_text->disable();
		key_text->disable();
		line_text->disable();
		pr_kbd_irdata_text->disable();
		pr_kbd_irdata_text_2->disable();
		commit_button->disable();
		get_raw_button->disable();

		FXString s;
		s = "entered keyboard + irdata mode\n";
		s += "press the button again in order to stop\n";
		s += "while waiting for irdata you can't leave until irdata reception or timeout (firmware is waiting 5 sec for IR reception)\n";
		input_text->appendText(s);
		input_text->setBottomLine(INT_MAX);
		pr_kbd_irdata_text->setText("press modifier or key on keyboard");
		pr_kbd_irdata_text_2->setText("or stop");
		getApp()->repaint();

		getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);
	} else {
		getApp()->removeTimeout(this, ID_KBD_TIMER);
		PR_kbd_irdata_Active = 0;
                pr_keyboard_and_irdata_button->setBaseColor(storedBaseColor);
                pr_keyboard_and_irdata_button->setShadowColor(storedShadowColor);
                pr_keyboard_and_irdata_button->setBackColor(storedBackColor);
		pr_kbd_irdata_text->setText("");
		pr_kbd_irdata_text_2->setText("");

		output_button->enable();
		pwakeup_button->enable();
		pmacro_button->enable();
		pirdata_button->enable();
		pkey_button->enable();
		prepeat_button->enable();
		prwakeup_button->enable();
		prmacro_button->enable();
		prirdata_button->enable();
		gwakeup_button->enable();
		gmacro_button->enable();
		girdata_button->enable();
		gkey_button->enable();
		grepeat_button->enable();
		aget_button->enable();
		aset_button->enable();
		rwakeup_button->enable();
		rmacro_button->enable();
		rirdata_button->enable();
		rkey_button->enable();
		ralarm_button->enable();
		rrepeat_button->enable();
		connect_button->disable();
		disconnect_button->enable();
		flash_button->enable();
		get_button->enable();
		reset_button->enable();
		open_button->enable();
		save_button->enable();
		upgrade_button->enable();
		reboot_button->enable();
		if(uC == "RP2xxx"){
			commit_button->enable();
			get_raw_button->enable();
		}

		map_text21->enable();
		key_text->setText("KEY_");
		modifier_text->setText("ff");
		last_modifier = "";
		last_key = "";
		device_list->enable();
		disconnect_button->enable();
		rescan_button->enable();
		output_text->enable();
		protocol_text->enable();
		address_text->enable();
		command_text->enable();
		flag_text->enable();
		days_text->enable();
		hours_text->enable();
		minutes_text->enable();
		seconds_text->enable();
		input_text->enable();
		wslistbox->enable();
		mnlistbox->enable();
		mslistbox->enable();
		rslistbox->enable();
		repeat_text->enable();
		modifier_text->enable();
		key_text->enable();
		line_text->enable();
		pr_kbd_irdata_text->enable();
		pr_kbd_irdata_text_2->enable();

		input_text->appendText("stopped keyboard + irdata mode\n");
		input_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::onKbdTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	if(got_modifier){
		got_modifier = 0;

		pr_kbd_irdata_text->setText("press key on keyboard");
		pr_kbd_irdata_text_2->setText("or stop");
		getApp()->repaint();
	}

	if(got_key){
		got_key = 0;

		/* if same modifier and key continue
		** (one button press on the remote may send several IR codes, which triggers the corresponding key
		** and under Windows our config app receives keys send by our device, so ignore those)
		*/
		if((modifier_text->getText() == last_modifier) && (key_text->getText() == last_key)){
			input_text->appendText("same key, ignored\n");
			input_text->setBottomLine(INT_MAX);
			getApp()->repaint();
			if(PR_kbd_irdata_Active)
				getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);
			return 1;
		} else {
			if(modifier_text->getText() != "error" && key_text->getText() != "error"){
				onPkey(NULL, 0, NULL);
				last_modifier = modifier_text->getText();
				last_key = key_text->getText();
			} else {
				input_text->appendText("invalid modifier/key, try again\n");
				input_text->setBottomLine(INT_MAX);
				getApp()->repaint();
				if(PR_kbd_irdata_Active)
					getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);
				return 1;
			}
		}

		pr_kbd_irdata_text->setText("press button on remote");
		pr_kbd_irdata_text_2->setText("or wait until timeout");
		getApp()->repaint();

		/* exit, if timeout or multiple entries */
		if(onPRirdata(NULL, 0, NULL) == -1) {
			onPR_kbd_irdata(NULL, 0, NULL);
			return 1;
		}

		modifier_text->setText("ff");
		key_text->setText("KEY_EDIT");

		pr_kbd_irdata_text->setText("press modifier or key on keyboard");
		pr_kbd_irdata_text_2->setText("or stop");
		getApp()->repaint();
	}

	if(PR_kbd_irdata_Active)
		getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);

	return 1;
}

long
MainWindow::onPRirdataTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	onPRirdata(NULL, 0, NULL);

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
