package wx;

// The order of these matches the order in HaxeAPI.cpp
enum EventID
{
    COMMAND_BUTTON_CLICKED;
    COMMAND_CHECKBOX_CLICKED;
    COMMAND_CHOICE_SELECTED;
    COMMAND_LISTBOX_SELECTED;
    COMMAND_LISTBOX_DOUBLECLICKED;
    COMMAND_CHECKLISTBOX_TOGGLED;

	 COMMAND_TEXT_UPDATED;
    COMMAND_TEXT_ENTER;

    COMMAND_MENU_SELECTED;
    COMMAND_SLIDER_UPDATED;
    COMMAND_RADIOBOX_SELECTED;
    COMMAND_RADIOBUTTON_SELECTED;

    COMMAND_SCROLLBAR_UPDATED;
    COMMAND_VLBOX_SELECTED;
    COMMAND_COMBOBOX_SELECTED;
    COMMAND_TOOL_RCLICKED;
    COMMAND_TOOL_ENTER;
    COMMAND_SPINCTRL_UPDATED;

    // Sockets and timers send events, too
    SOCKET;
    TIMER;

    // Mouse event types
    LEFT_DOWN;
    LEFT_UP;
    MIDDLE_DOWN;
    MIDDLE_UP;
    RIGHT_DOWN;
    RIGHT_UP;
    MOTION;
    ENTER_WINDOW;
    LEAVE_WINDOW;
    LEFT_DCLICK;
    MIDDLE_DCLICK;
    RIGHT_DCLICK;
    SET_FOCUS;
    KILL_FOCUS;
    CHILD_FOCUS;
    MOUSEWHEEL;

    // Non-client mouse events
    NC_LEFT_DOWN;
    NC_LEFT_UP;
    NC_MIDDLE_DOWN;
    NC_MIDDLE_UP;
    NC_RIGHT_DOWN;
    NC_RIGHT_UP;
    NC_MOTION;
    NC_ENTER_WINDOW;
    NC_LEAVE_WINDOW;
    NC_LEFT_DCLICK;
    NC_MIDDLE_DCLICK;
    NC_RIGHT_DCLICK;

    // Character input event type
    CHAR;
    CHAR_HOOK;
    NAVIGATION_KEY;
    KEY_DOWN;
    KEY_UP;
    HOTKEY;

    // Set cursor event
    SET_CURSOR;

    // wxScrollBar and wxSlider event identifiers
    SCROLL_TOP;
    SCROLL_BOTTOM;
    SCROLL_LINEUP;
    SCROLL_LINEDOWN;
    SCROLL_PAGEUP;
    SCROLL_PAGEDOWN;
    SCROLL_THUMBTRACK;
    SCROLL_THUMBRELEASE;
    SCROLL_CHANGED;

    // Scroll events from wxWindow
    SCROLLWIN_TOP;
    SCROLLWIN_BOTTOM;
    SCROLLWIN_LINEUP;
    SCROLLWIN_LINEDOWN;
    SCROLLWIN_PAGEUP;
    SCROLLWIN_PAGEDOWN;
    SCROLLWIN_THUMBTRACK;
    SCROLLWIN_THUMBRELEASE;

    // System events
    SIZE;
    MOVE;
    CLOSE_WINDOW;
    END_SESSION;
    QUERY_END_SESSION;
    ACTIVATE_APP;
    // 406..408 are power events
    ACTIVATE;
    CREATE;
    DESTROY;
    SHOW;
    ICONIZE;
    MAXIMIZE;
    MOUSE_CAPTURE_CHANGED;
    MOUSE_CAPTURE_LOST;
    PAINT;
    ERASE_BACKGROUND;
    NC_PAINT;
    PAINT_ICON;
    MENU_OPEN;
    MENU_CLOSE;
    MENU_HIGHLIGHT;
    CONTEXT_MENU;
    SYS_COLOUR_CHANGED;
    DISPLAY_CHANGED;
    SETTING_CHANGED;
    QUERY_NEW_PALETTE;
    PALETTE_CHANGED;
    JOY_BUTTON_DOWN;
    JOY_BUTTON_UP;
    JOY_MOVE;
    JOY_ZMOVE;
    DROP_FILES;
    DRAW_ITEM;
    MEASURE_ITEM;
    COMPARE_ITEM;
    INIT_DIALOG;
    IDLE;
    UPDATE_UI;
    SIZING;
    MOVING;
    HIBERNATE;
    // more power events follow -- see wx/power.h

    // Clipboard events
    COMMAND_TEXT_COPY;
    COMMAND_TEXT_CUT;
    COMMAND_TEXT_PASTE;

    // Generic command events
    // Note: a click is a higher-level event than button down/up
    COMMAND_LEFT_CLICK;
    COMMAND_LEFT_DCLICK;
    COMMAND_RIGHT_CLICK;
    COMMAND_RIGHT_DCLICK;
    COMMAND_SET_FOCUS;
    COMMAND_KILL_FOCUS;
    COMMAND_ENTER;

    // Help events
    HELP;
    DETAILED_HELP;
}
