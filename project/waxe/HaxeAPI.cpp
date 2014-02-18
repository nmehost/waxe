#include "HaxeAPI.h"
#include <map>

// --- Helpers --------------------------------------------

DEFINE_KIND(gObjectKind);

extern "C" void InitCFFI()
{
  gObjectKind = alloc_kind();
}

DEFINE_ENTRY_POINT(InitCFFI)


wxString Val2Str(value inVal)
{
   if (val_is_string(inVal))
      return  wxString::FromAscii( val_string(inVal) );
   if (val_is_object(inVal))
   {
      value __s = val_field(inVal,val_id("__s"));
      if (val_is_string(__s))
         return  wxString::FromAscii( val_string(__s) );
   }
   else if (val_is_object(inVal))
      return val_bool(inVal) ? wxString(wxT("true")) : wxString(wxT("false"));
   //wxMessageBox(wxT("Bad string translation"));
   return wxT("");
}

value WXToValue(const wxString &inStr)
{
   return alloc_string(inStr.mb_str(wxConvUTF8));
}

int Val2Int(value inVal, int inDefault)
{
   if (val_is_null(inVal))
      return inDefault;
   if (val_is_int(inVal))
      return val_int(inVal);
   if (val_is_number(inVal))
      return val_number(inVal);
   if (val_is_bool(inVal))
      return val_bool(inVal);
   return inDefault;
}

bool Val2Bool(value inVal, bool inDefault)
{
   if (val_is_bool(inVal))
      return val_bool(inVal);
   return inDefault;
}


wxPoint Val2Point(value inVal)
{
   static int x_id = val_id("x");
   static int y_id = val_id("y");
   if (!val_is_object(inVal))
      return wxDefaultPosition;
   value x = val_field(inVal,x_id);
   value y = val_field(inVal,y_id);
   return wxPoint( val_is_number(x) ? val_number(x) : 0,
                   val_is_number(y) ? val_number(y) : 0 );
}

wxSize Val2Size(value inVal)
{
   static int w_id = val_id("width");
   static int h_id = val_id("height");
   if (!val_is_object(inVal))
      return wxDefaultSize;
   value w = val_field(inVal,w_id);
   value h = val_field(inVal,h_id);
   return wxSize( val_is_number(w) ? val_number(w) : -1,
                  val_is_number(h) ? val_number(h) : -1 );
}

wxColour Val2Colour(value inVal)
{
	if (val_is_string(inVal))
		return wxColour( Val2Str(inVal) );
	int col  = val_int(inVal);
	return wxColour( (col & 0xff0000) >> 16, (col & 0xff00) >> 8, col & 0xff );
}

wxArrayString &Val2ArrayString(value inVal,wxArrayString &outStrings)
{
	outStrings.Clear();
	if (val_is_array(inVal))
	{
		int n = val_array_size(inVal);
		outStrings.Alloc(n);
		for(int i=0;i<n;i++)
			outStrings.Add(Val2Str( val_array_i(inVal,i) ));
	}
	return outStrings;
}

ByteData Val2ByteData(value inData)
{
	ByteData result;
	result.data = 0;
	result.length = 0;
	// Neko byte array....
	if (val_is_string(inData))
	{
		result.length = val_strlen(inData);
		result.data = (uint8 *)val_string(inData);
	}
	else if (val_is_buffer(inData))
	{
		buffer buf = val_to_buffer(inData);
		result.length = buffer_size(buf);
		result.data = (uint8 *)buffer_data(buf);
	}
	return result;
}




value WXToValue(wxObject *inObj)
{
   value result = alloc_abstract(gObjectKind,inObj);
   return result;
}

static void delete_object(value inObj)
{
	val_gc(inObj,0);
	if (val_is_kind(inObj,gObjectKind))
	{
		wxObject *obj = (wxObject *)val_data(inObj);
		delete obj;
	}
}

value WXToDeletingValue(wxObject *inObj)
{
	value result = WXToValue(inObj);
	val_gc(result,delete_object);
	return result;
}

value wx_object_destroy(value inObj)
{
	if (!val_is_null(inObj))
		delete_object(inObj);
	return alloc_null();
}
DEFINE_PRIM(wx_object_destroy,1)

value WXToValue(const wxPoint &inPoint)
{
	value obj = alloc_empty_object();
	alloc_field(obj,val_id("x"),alloc_int(inPoint.x));
	alloc_field(obj,val_id("y"),alloc_int(inPoint.y));
	return obj;
}

value WXToValue(const wxSize &inSize)
{
	value obj = alloc_empty_object();
	alloc_field(obj,val_id("width"),alloc_int(inSize.GetWidth()));
	alloc_field(obj,val_id("height"),alloc_int(inSize.GetHeight()));
	return obj;
}

value WXToValue(const int &inVal)
{
	return alloc_int(inVal);
}

value WXToValue(const bool &inVal)
{
	return alloc_bool(inVal);
}

value WXToValue(const wxColour &inCol)
{
	return alloc_int( ( inCol.Red()<<16 ) | (inCol.Green()<<8) | inCol.Blue() );
}



CreationParams::CreationParams(value inParams,int inDefaultStyle)
{
   ValueToWX(val_array_i(inParams,0),parent);
	id = (wxWindowID)Val2Int(val_array_i(inParams,1),wxID_ANY);
   text = Val2Str(val_array_i(inParams,2));
   position = Val2Point(val_array_i(inParams,3));
   size = Val2Size(val_array_i(inParams,4));
   flags = Val2Int(val_array_i(inParams,5),inDefaultStyle) /*| wxCLIP_SIBLINGS*/;
}



// --- HaxeEventHandler --------------------------------------


static int handler_count = 0;
void SetupEventMap();
static std::map<int,int> gWX2HaxeIDMap;
static int sgHandlers = 0;

HaxeEventHandler::HaxeEventHandler(wxWindow *inWindow,value inHandler)
{
	SetupEventMap();
   mWindow = inWindow;
   mObject = alloc_root();
   *mObject = inHandler;
   sgHandlers++;
   if (inWindow)
      inWindow->PushEventHandler(this);

   ++handler_count;
}

HaxeEventHandler::~HaxeEventHandler()
{
   sgHandlers--;
   if (!val_is_null(*mObject))
      val_ocall0(*mObject,val_id("_wx_deleted"));
   free_root(mObject);
   --handler_count;
}


#define WXK_TRANS(x) case WXK_##x: return key##x;


// Removed in 2.9.2?
#define WXK_NEXT 0
#define WXK_PRIOR 0
#define wxEVT_COMMAND_SPINCTRL_UPDATED 0
#define wxEVT_SOCKET 0
#define wxEVT_NC_LEFT_DOWN 0
#define wxEVT_NC_LEFT_UP 0
#define wxEVT_NC_MIDDLE_DOWN 0
#define wxEVT_NC_MIDDLE_UP 0
#define wxEVT_NC_RIGHT_DOWN 0
#define wxEVT_NC_RIGHT_UP 0
#define wxEVT_NC_MOTION 0
#define wxEVT_NC_ENTER_WINDOW 0
#define wxEVT_NC_LEAVE_WINDOW 0
#define wxEVT_NC_LEFT_DCLICK 0
#define wxEVT_NC_MIDDLE_DCLICK 0
#define wxEVT_NC_RIGHT_DCLICK 0
#define wxEVT_PAINT_ICON 0
#define wxEVT_SETTING_CHANGED 0
#define wxEVT_DRAW_ITEM 0
#define wxEVT_MEASURE_ITEM 0
#define wxEVT_COMPARE_ITEM 0

enum KeyCode
{
   keyA = 65,
   keyZ = 90,

   keyALTERNATE = 18,
   keyBACKQUOTE = 192,
   keyBACKSLASH = 220,
   keyBACKSPACE = 8,
   keyCAPS_LOCK = 20,
   keyCOMMA = 188,
   keyCOMMAND = 15,
   keyCONTROL = 17,
   keyDELETE = 46,
   keyDOWN = 40,
   keyEND = 35,
   keyENTER = 13,
   keyEQUAL = 187,
   keyESCAPE = 27,
   keyF1 = 112,
   keyF2 = 113,
   keyF3 = 114,
   keyF4 = 115,
   keyF5 = 116,
   keyF6 = 117,
   keyF7 = 118,
   keyF8 = 119,
   keyF9 = 120,
   keyF10 = 121,
   keyF11 = 122,
   keyF12 = 123,
   keyF13 = 124,
   keyF14 = 125,
   keyF15 = 126,
   keyHOME = 36,
   keyINSERT = 45,
   keyLEFT = 37,
   keyLEFTBRACKET = 219,
   keyMINUS = 189,
   keyNUMBER_0 = 48,
   keyNUMBER_1 = 49,
   keyNUMBER_2 = 50,
   keyNUMBER_3 = 51,
   keyNUMBER_4 = 52,
   keyNUMBER_5 = 53,
   keyNUMBER_6 = 54,
   keyNUMBER_7 = 55,
   keyNUMBER_8 = 56,
   keyNUMBER_9 = 57,
   keyNUMPAD = 21,
   keyNUMPAD_0 = 96,
   keyNUMPAD_1 = 97,
   keyNUMPAD_2 = 98,
   keyNUMPAD_3 = 99,
   keyNUMPAD_4 = 100,
   keyNUMPAD_5 = 101,
   keyNUMPAD_6 = 102,
   keyNUMPAD_7 = 103,
   keyNUMPAD_8 = 104,
   keyNUMPAD_9 = 105,
   keyNUMPAD_ADD = 107,
   keyNUMPAD_DECIMAL = 110,
   keyNUMPAD_DIVIDE = 111,
   keyNUMPAD_ENTER = 108,
   keyNUMPAD_MULTIPLY = 106,
   keyNUMPAD_SUBTRACT = 109,
   keyPAGE_DOWN = 34,
   keyPAGE_UP = 33,
   keyPERIOD = 190,
   keyQUOTE = 222,
   keyRIGHT = 39,
   keyRIGHTBRACKET = 221,
   keySEMICOLON = 186,
   keySHIFT = 16,
   keySLASH = 191,
   keySPACE = 32,
   keyTAB = 9,
   keyUP = 38,
};




int FlashCode(int inKey)
{
	if (inKey>=keyA && inKey<=keyZ)
      return inKey;
   if (inKey>='0' && inKey<='9')
      return inKey;

   if (inKey>=WXK_NUMPAD0 && inKey<=WXK_NUMPAD9)
      return inKey - WXK_NUMPAD0 + keyNUMPAD_0;

   if (inKey>=WXK_F1 && inKey<=WXK_F15)
      return inKey - WXK_F1 + keyF1;


   switch(inKey)
   {
      case WXK_MENU:
         return keyALTERNATE;

      case WXK_SHIFT:
         return keySHIFT;

      case WXK_CONTROL:
         return keyCONTROL;
      case WXK_WINDOWS_LEFT:
      case WXK_WINDOWS_RIGHT:
         return keyCOMMAND;

      case WXK_CAPITAL: return keyCAPS_LOCK;
      //case WXK_NEXT: return keyPAGE_DOWN;
      //case WXK_PRIOR: return keyPAGE_UP;
      case '=': return keyEQUAL;
      case WXK_RETURN:
         return keyENTER;

      case '`' : return keyBACKQUOTE;
      //case '\\' : return keyBACKSLASH;
      case ',' : return keyCOMMA;
      case WXK_BACK : return keyBACKSPACE;
      case '-' : return keyMINUS;
      case '.' : return keyPERIOD;
      WXK_TRANS(DELETE)
      WXK_TRANS(DOWN)
      WXK_TRANS(END)
      WXK_TRANS(ESCAPE)
      WXK_TRANS(HOME)
      WXK_TRANS(INSERT)
      WXK_TRANS(LEFT)
      // case '(' : return keyLEFTBRACKET;
      //WXK_TRANS(QUOTE)
      WXK_TRANS(RIGHT)
      //WXK_TRANS(RIGHTBRACKET)
      //WXK_TRANS(SEMICOLON)
      //WXK_TRANS(SLASH)
      WXK_TRANS(SPACE)
      WXK_TRANS(TAB)
      WXK_TRANS(UP)
   }

   return inKey;
}


bool HaxeEventHandler::ProcessEvent(wxEvent& event)
{
   value data = val_null;

   wxString name;
   wxWindow *window = wxDynamicCast(event.GetEventObject(), wxWindow);
   if (window)
      name = window->GetName();

   int type = event.GetEventType();

   if (type==wxEVT_DESTROY && window==mWindow)
   {
      // Deleted because it is client data
      mWindow->PopEventHandler();
      return false;
   }

	// ?
	if (gWX2HaxeIDMap.find(type)==gWX2HaxeIDMap.end())
	{
      return wxEvtHandler::ProcessEvent(event);
	}

	int haxe_id = gWX2HaxeIDMap[type];

	value obj = alloc_empty_object();

	alloc_field(obj,val_id("type"),alloc_int(haxe_id));
	alloc_field(obj,val_id("name"),WXToValue(name));
	alloc_field(obj,val_id("skip"),alloc_bool(true));
	alloc_field(obj,val_id("veto"),alloc_bool(false));
	alloc_field(obj,val_id("id"),alloc_int(event.GetId()));

	wxMouseEvent *me = wxDynamicCast(&event,wxMouseEvent);
	if (me)
	{
		alloc_field(obj,val_id("x"),alloc_int(me->GetX()));
		alloc_field(obj,val_id("y"),alloc_int(me->GetY()));
		alloc_field(obj,val_id("leftIsDown"),alloc_bool(me->LeftIsDown()));
		alloc_field(obj,val_id("middleIsDown"),alloc_bool(me->MiddleIsDown()));
		alloc_field(obj,val_id("rightIsDown"),alloc_bool(me->RightIsDown()));
		alloc_field(obj,val_id("wheelDelta"),alloc_bool(me->GetWheelDelta()));
		alloc_field(obj,val_id("controlDown"),alloc_bool(me->ControlDown()));
		alloc_field(obj,val_id("metaDown"),alloc_bool(me->MetaDown()));
		alloc_field(obj,val_id("shiftDown"),alloc_bool(me->ShiftDown()));
		alloc_field(obj,val_id("cmdDown"),alloc_bool(me->CmdDown()));
	}

	wxKeyEvent *ke = wxDynamicCast(&event,wxKeyEvent);
	if (ke)
	{
		alloc_field(obj,val_id("x"),alloc_int(ke->GetX()));
		alloc_field(obj,val_id("y"),alloc_int(ke->GetY()));
		alloc_field(obj,val_id("controlDown"),alloc_bool(ke->ControlDown()));
		alloc_field(obj,val_id("metaDown"),alloc_bool(ke->MetaDown()));
		alloc_field(obj,val_id("shiftDown"),alloc_bool(ke->ShiftDown()));
		alloc_field(obj,val_id("cmdDown"),alloc_bool(ke->CmdDown()));
		int code = ke->GetKeyCode();
		if (code==WXK_DELETE) code = 8;
		alloc_field(obj,val_id("code"),alloc_int(code<=193 ?code:0));
		alloc_field(obj,val_id("flashCode"),alloc_int(FlashCode(code)));
	}


	val_ocall1(*mObject,val_id("_handle_event"),obj);

	bool skip =  val_bool(val_field(obj,val_id("skip")));

	if ( type==wxEVT_CLOSE_WINDOW )
	{
		wxCloseEvent *evt = wxDynamicCast(&event,wxCloseEvent);
		if (evt)
		{
			value veto_field = val_field(obj,val_id("veto"));
			if (val_is_bool(veto_field) && val_bool(veto_field) )
				evt->Veto(true);
		}
	}

   event.Skip(skip);
	if (!skip)
		return true;

   return wxEvtHandler::ProcessEvent(event);
}

// --- Event Handling ---------------------------------------------------------

value wx_set_window_handler(value inWindow,value inObj)
{
   wxEvtHandler *handler;
   if (ValueToWX(inWindow,handler))
   {
      wxClientData *data = handler->GetClientObject();
      if (data)
         val_throw(alloc_string("SetObject - already has a handler"));
      else
      {
         HaxeEventHandler *haxe = new HaxeEventHandler(wxDynamicCast(handler,wxWindow),inObj);
         handler->SetClientObject(haxe);
      }
   }
   return alloc_null();
}
DEFINE_PRIM(wx_set_window_handler,2)


value wx_get_window_handler(value inWindow)
{
   wxEvtHandler *handler;
   if (ValueToWX(inWindow,handler))
   {
      HaxeEventHandler *haxe = wxDynamicCast(handler->GetClientObject(),HaxeEventHandler);
      if (!haxe)
         return alloc_null();

      return haxe->GetHaxeObject();
   }
   return alloc_null();
}
DEFINE_PRIM(wx_get_window_handler,1)


// --- Haxe Data ---------------------------------------------------------

value wx_set_data(value inWXObj,value inHXObj)
{
   wxClientDataContainer *container;
   if (ValueToWX(inWXObj,container))
   {
      wxClientData *data = container->GetClientObject();
      if (data)
         val_throw(alloc_string("SetData - already has data"));
      else
      {
         HaxeData *haxe = new HaxeData(inHXObj);
         container->SetClientData(haxe);
      }
   }
   return alloc_null();
}
DEFINE_PRIM(wx_set_data,2)


value wx_get_data(value inWXObj)
{
   wxClientDataContainer *container;
   if (ValueToWX(inWXObj,container))
   {
      HaxeData *haxe = (HaxeData *)(container->GetClientData());
      if (!haxe)
         return alloc_null();

      return haxe->GetHaxeObject();
   }
   return alloc_null();
}
DEFINE_PRIM(wx_get_data,1)




// -- Contants -------------------------------------------------------------
void SetupEventMap()
{
   if (!gWX2HaxeIDMap.empty())
		return;

   static int sEventIDMap[] =
   {
      wxEVT_COMMAND_BUTTON_CLICKED,
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxEVT_COMMAND_CHOICE_SELECTED,
      wxEVT_COMMAND_LISTBOX_SELECTED,
      wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,
      wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,

      wxEVT_COMMAND_TEXT_UPDATED,
      wxEVT_COMMAND_TEXT_ENTER,
 
      wxEVT_COMMAND_MENU_SELECTED,
      wxEVT_COMMAND_SLIDER_UPDATED,
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      wxEVT_COMMAND_RADIOBUTTON_SELECTED,
   
      wxEVT_COMMAND_SCROLLBAR_UPDATED,
      wxEVT_COMMAND_VLBOX_SELECTED,
      wxEVT_COMMAND_COMBOBOX_SELECTED,
      wxEVT_COMMAND_TOOL_RCLICKED,
      wxEVT_COMMAND_TOOL_ENTER,
      wxEVT_COMMAND_SPINCTRL_UPDATED,
   
       // Sockets and timers send events, too
      wxEVT_SOCKET,
      wxEVT_TIMER ,
   
       // Mouse event types
      wxEVT_LEFT_DOWN,
      wxEVT_LEFT_UP,
      wxEVT_MIDDLE_DOWN,
      wxEVT_MIDDLE_UP,
      wxEVT_RIGHT_DOWN,
      wxEVT_RIGHT_UP,
      wxEVT_MOTION,
      wxEVT_ENTER_WINDOW,
      wxEVT_LEAVE_WINDOW,
      wxEVT_LEFT_DCLICK,
      wxEVT_MIDDLE_DCLICK,
      wxEVT_RIGHT_DCLICK,
      wxEVT_SET_FOCUS,
      wxEVT_KILL_FOCUS,
      wxEVT_CHILD_FOCUS,
      wxEVT_MOUSEWHEEL,
   
       // Non-client mouse events
      wxEVT_NC_LEFT_DOWN,
      wxEVT_NC_LEFT_UP,
      wxEVT_NC_MIDDLE_DOWN,
      wxEVT_NC_MIDDLE_UP,
      wxEVT_NC_RIGHT_DOWN,
      wxEVT_NC_RIGHT_UP,
      wxEVT_NC_MOTION,
      wxEVT_NC_ENTER_WINDOW,
      wxEVT_NC_LEAVE_WINDOW,
      wxEVT_NC_LEFT_DCLICK,
      wxEVT_NC_MIDDLE_DCLICK,
      wxEVT_NC_RIGHT_DCLICK,
   
       // Character input event type
      wxEVT_CHAR,
      wxEVT_CHAR_HOOK,
      wxEVT_NAVIGATION_KEY,
      wxEVT_KEY_DOWN,
      wxEVT_KEY_UP,
      #ifndef __WXMSW__
		0,
      #else
      wxEVT_HOTKEY,
      #endif
   
       // Set cursor event
      wxEVT_SET_CURSOR,
   
       // wxScrollBar and wxSlider event identifiers
      wxEVT_SCROLL_TOP,
      wxEVT_SCROLL_BOTTOM,
      wxEVT_SCROLL_LINEUP,
      wxEVT_SCROLL_LINEDOWN,
      wxEVT_SCROLL_PAGEUP,
      wxEVT_SCROLL_PAGEDOWN,
      wxEVT_SCROLL_THUMBTRACK,
      wxEVT_SCROLL_THUMBRELEASE,
      wxEVT_SCROLL_CHANGED,
   
       // Scroll events from wxWindow
      wxEVT_SCROLLWIN_TOP,
      wxEVT_SCROLLWIN_BOTTOM,
      wxEVT_SCROLLWIN_LINEUP,
      wxEVT_SCROLLWIN_LINEDOWN,
      wxEVT_SCROLLWIN_PAGEUP,
      wxEVT_SCROLLWIN_PAGEDOWN,
      wxEVT_SCROLLWIN_THUMBTRACK,
      wxEVT_SCROLLWIN_THUMBRELEASE,
   
       // System events
      wxEVT_SIZE,
      wxEVT_MOVE,
      wxEVT_CLOSE_WINDOW,
      wxEVT_END_SESSION,
      wxEVT_QUERY_END_SESSION,
      wxEVT_ACTIVATE_APP,
       // 406..408 are power events
      wxEVT_ACTIVATE,
      wxEVT_CREATE,
      wxEVT_DESTROY,
      wxEVT_SHOW,
      wxEVT_ICONIZE,
      wxEVT_MAXIMIZE,
      wxEVT_MOUSE_CAPTURE_CHANGED,
      wxEVT_MOUSE_CAPTURE_LOST,
      wxEVT_PAINT,
      wxEVT_ERASE_BACKGROUND,
      wxEVT_NC_PAINT,
      wxEVT_PAINT_ICON,
      wxEVT_MENU_OPEN,
      wxEVT_MENU_CLOSE,
      wxEVT_MENU_HIGHLIGHT,
      wxEVT_CONTEXT_MENU,
      wxEVT_SYS_COLOUR_CHANGED,
      wxEVT_DISPLAY_CHANGED,
      wxEVT_SETTING_CHANGED,
      wxEVT_QUERY_NEW_PALETTE,
      wxEVT_PALETTE_CHANGED,
      wxEVT_JOY_BUTTON_DOWN,
      wxEVT_JOY_BUTTON_UP,
      wxEVT_JOY_MOVE,
      wxEVT_JOY_ZMOVE,
      wxEVT_DROP_FILES,
      wxEVT_DRAW_ITEM,
      wxEVT_MEASURE_ITEM,
      wxEVT_COMPARE_ITEM,
      wxEVT_INIT_DIALOG,
      wxEVT_IDLE,
      wxEVT_UPDATE_UI,
      wxEVT_SIZING,
      wxEVT_MOVING,
      wxEVT_HIBERNATE,
       // more power events follow -- see wx/power.h
   
       // Clipboard events
      wxEVT_COMMAND_TEXT_COPY,
      wxEVT_COMMAND_TEXT_CUT,
      wxEVT_COMMAND_TEXT_PASTE,
   
       // Generic command events
       // Note: a click is a higher-level event than button down/up
      wxEVT_COMMAND_LEFT_CLICK,
      wxEVT_COMMAND_LEFT_DCLICK,
      wxEVT_COMMAND_RIGHT_CLICK,
      wxEVT_COMMAND_RIGHT_DCLICK,
      wxEVT_COMMAND_SET_FOCUS,
      wxEVT_COMMAND_KILL_FOCUS,
      wxEVT_COMMAND_ENTER,
   
       // Help events
      wxEVT_HELP,
      wxEVT_DETAILED_HELP,
   	-999
   };

	for(int idx = 0; sEventIDMap[idx]>=0; idx++)
		gWX2HaxeIDMap[ sEventIDMap[idx] ] = idx;
}


// --- HaxeData --------------------------------------

HaxeData::HaxeData(value inData)
{
	mObject = alloc_root();
	*mObject = inData;
}

HaxeData::~HaxeData()
{
   if (!val_is_null(*mObject))
      val_ocall0(*mObject,val_id("_wx_deleted"));
   free_root(mObject);
	mObject = 0;
   --handler_count;
}

