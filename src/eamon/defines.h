#ifndef DEFINES_H
#define DEFINES_H

#include <QDir>

/*
 * Settings IDs for path
 */
#define SETTING_PATH_GAMES "path/games"
#define SETTING_PATH_DISK "path/disk"
#define SETTING_PATH_DISKIMG "path/diskimg"
#define SETTING_PATH_FILE "path/file"
#define SETTING_PATH_EXPORT "path/export"
#define SETTING_PATH_SCREENSHOT "path/screenshot"
/*
 * Settings values for path
 */
#define SETTING_VALUE_PATH_GAMES (QDir::homePath()+"/.eamon")
#define SETTING_VALUE_PATH_DISK "."
#define SETTING_VALUE_PATH_DISKIMG "."
#define SETTING_VALUE_PATH_FILE "."
#define SETTING_VALUE_PATH_EXPORT "."
#define SETTING_VALUE_PATH_SCREENSHOT "."


/*
 * Settings IDs for game screen configuration
 */
#define SETTING_SCREEN_FONT "screen/font"
#define SETTING_SCREEN_LOWERCASE "screen/lowercase"
#define SETTING_SCREEN_CAPITALIZE "screen/capitalize"
#define SETTING_SCREEN_TEXT_COLOR "screen/textcolor"
#define SETTING_SCREEN_BKG_COLOR "screen/backgroundcolur"
#define SETTING_SCREEN_LINESPACING "screen/linespacing"
/*
 * Settings values for game screen configuration
 */
#define SETTING_VALUE_SCREEN_FONT "PrintChar21,12,-1,5,50,0,0,0,0,0"
#define SETTING_VALUE_SCREEN_LOWERCASE false
#define SETTING_VALUE_SCREEN_CAPITALIZE true
#define SETTING_VALUE_SCREEN_TEXT_COLOR QColor(0,0,0)
#define SETTING_VALUE_SCREEN_BKG_COLOR QColor(255,255,255)
#define SETTING_VALUE_SCREEN_LINESPACING 0

/*
 * Settings IDs for global styles
 */
#define SETTING_STYLE_STYLE "style/style"
#define SETTING_STYLE_PALETTE "style/palette"
/*
 * Settings values for global styles
 */
#define SETTING_VALUE_STYLE_STYLE "fusion"
#define SETTING_VALUE_STYLE_PALETTE ""

/*
 * Settings IDs for the virtual machine
 */
#define SETTING_VM_SLOWDOWN "vm/slowdown"
/*
 * Settings value for the virtual machine
 */
#define SETTING_VALUE_VM_SLOWDOWN 0

/*
 * Settings IDs for general behaviour
 */
#define SETTING_AUTOSTART "app/autostart"
/*
 * Settings values for general behaviour
 */
#define SETTING_VALUE_AUTOSTART true


/*
 * Settings IDs for editor
 */
#define SETTING_SCRIPT_FONT "editor/font"
/*
 * Settings values for editor
 */
#define SETTING_VALUE_SCRIPT_FONT "Courier,12,-1,5,50,0,0,0,0,0"

#endif // DEFINES_H
