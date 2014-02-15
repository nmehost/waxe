package wx;

typedef Position = { x:Int, y:Int };
typedef Size = { width:Int, height:Int };

class Window extends EventHandler
{
   var wxEventHandlers:Map<Int,Dynamic->Void>;


   // For use with top-level windows
   public static inline var CENTRE                = 1<<0;
   public static inline var FRAME_NO_TASKBAR      = 1<<1;
   public static inline var FRAME_TOOL_WINDOW     = 1<<2;
   public static inline var FRAME_FLOAT_ON_PARENT = 1<<3;
   public static inline var FRAME_SHAPED = 1<<4;

   public static inline var RESIZE_BORDER         = 1<<5;
   public static inline var TINY_CAPTION_VERT     = 1<<7;
   public static inline var DIALOG_NO_PARENT      = 1<<8;
   public static inline var MAXIMIZE_BOX          = 1<<9;
   public static inline var MINIMIZE_BOX          = 1<<10;
   public static inline var SYSTEM_MENU           = 1<<11;
   public static inline var CLOSE_BOX             = 1<<12;
   public static inline var MAXIMIZE              = 1<<13;
   public static inline var MINIMIZE              = 1<<14;
   public static inline var STAY_ON_TOP          = 1<<15;

   /*
    Summary of the bits used by various styles.

    High word, containing styles which can be used with many windows:

   */

   public static inline var FULL_REPAINT_ON_RESIZE = 1<<16;
   public static inline var POPUP_WINDOW           = 1<<17;
   public static inline var WANTS_CHARS            = 1<<18;
   public static inline var TAB_TRAVERSAL          = 1<<19;
   public static inline var TRANSPARENT_WINDOW     = 1<<20;
   public static inline var BORDER_NONE            = 1<<21;
   public static inline var CLIP_CHILDREN          = 1<<22;
   public static inline var ALWAYS_SHOW_SB         = 1<<23;
   public static inline var BORDER_STATIC          = 1<<24;
   public static inline var BORDER_SIMPLE          = 1<<25;
   public static inline var BORDER_RAISED          = 1<<26;
   public static inline var BORDER_SUNKEN          = 1<<27;
   public static inline var BORDER_DOUBLE          = 1<<28;
   public static inline var CAPTION                = 1<<29;
   public static inline var CLIP_SIBLINGS          = 1<<29;
   public static inline var HSCROLL                = 1<<30;
   #if !neko
   public static inline var VSCROLL = 1<<31;
   #end


   public static var INVALID_PARENT = "Invalid Parent";


   public var size(get,set):Size;
   public var sizer(get,set):Sizer;
   public var clientSize(get,set):Size;
   public var position(get,set):Position;
   public var shown(get,set):Bool;
   public var name(get,set):String;
   public var backgroundColour(get,set):Int;


   public static function create(inParent:Window,?inID:Int,?inPosition:Position,
                   ?inSize:Size, ?inStyle:Int )
   {
      if (inParent==null)
         throw Error.INVALID_PARENT;
      var handle = wx_window_create([inParent.wxHandle,inID,"",inPosition,inSize, inStyle] );
      return new Window(handle);
   }

   function new(inHandle:Dynamic)
   {
      super(inHandle);
      wxEventHandlers = new Map<Int,Dynamic->Void>();
   }


   override function HandleEvent(event:Dynamic)
   {
      var type:Int = event.type;

      // Debug event type...
      if (false)
      {
         var e = Type.createEnumIndex( wx.EventID, type );
         trace(type + ":" + e);
      }

      if (wxEventHandlers.exists(type))
      {
         var func = wxEventHandlers.get(type);
         if (func!=null)
         {
            event.skip = false;
            func(event);
         }
      }
   }

   public function setHandler(inID:EventID,inFunc:Dynamic->Void)
   {
      wxEventHandlers.set(Type.enumIndex(inID),inFunc);
   }

   public function fit() { wx_window_fit(wxHandle); }
   public function refresh() { wx_window_refresh(wxHandle); }
   public function destroy() { wx_window_destroy(wxHandle); }

   public function get_size() : Size { return wx_window_get_size(wxHandle); }
   public function set_size(inSize:Size) : Size
   {
      wx_window_set_size(wxHandle, inSize);
      return inSize;
   }

   public function get_sizer() : Sizer { return wx_window_get_sizer(wxHandle); }
   public function set_sizer(inSizer:Sizer) : Sizer
   {
      wx_window_set_sizer(wxHandle, inSizer.wxGetHandle());
      return inSizer;
   }

   public function get_clientSize() : Size { return wx_window_get_client_size(wxHandle); }
   public function set_clientSize(inSize:Size) : Size
   {
      wx_window_set_client_size(wxHandle, inSize.width, inSize.height);
      return inSize;
   }

   public function get_position() : Position { return wx_window_get_position(wxHandle); }
   public function set_position(inPos:Position) : Position
   {
      wx_window_set_position(wxHandle, inPos);
      return inPos;
   }

   public function get_shown() : Bool { return wx_window_get_shown(wxHandle); }
   public function isShown() : Bool { return wx_window_get_shown(wxHandle); }

   public function show(inShow:Bool = true) : Void
   {
      wx_window_set_shown(wxHandle, inShow);
   }
   public function set_shown(inShow:Bool = true) : Bool
   {
      wx_window_set_shown(wxHandle, inShow);
      return inShow;
   }

   public function get_backgroundColour() : Int { return wx_window_get_bg_colour(wxHandle); }
   public function set_backgroundColour(inColour:Int) : Int
   {
      wx_window_set_bg_colour(wxHandle, inColour);
      return inColour;
   }

   public function get_name() : String { return wx_window_get_name(wxHandle); }
   public function set_name(inName:String) : String
   {
      wx_window_set_name(wxHandle, inName);
      return inName;
   }





   // Helpers ...
   public var onClose(null,set) : Dynamic->Void;
   function set_onClose(f:Dynamic->Void) {setHandler(wx.EventID.CLOSE_WINDOW,f); return f;}
   public var onSize(null,set) : Dynamic->Void;
   function set_onSize(f:Dynamic->Void) {setHandler(wx.EventID.SIZE,f); return f;}
   public var onPaint(null,set) : DC->Void;
   function set_onPaint(f:DC->Void)
   {
      var me = this;
      setHandler(wx.EventID.PAINT, function(_)
         { var dc = wx.DC.createPaintDC(me); f(dc); dc.destroy(); } );
      return f;
   }



   static var wx_window_get_position = Loader.load("wx_window_get_position",1);
   static var wx_window_set_position = Loader.load("wx_window_set_position",2);
   static var wx_window_get_size = Loader.load("wx_window_get_size",1);
   static var wx_window_set_size = Loader.load("wx_window_set_size",2);
   static var wx_window_get_client_size = Loader.load("wx_window_get_client_size",1);
   static var wx_window_set_client_size = Loader.load("wx_window_set_client_size",2);
   static var wx_window_create:Array<Dynamic>->Dynamic = Loader.load("wx_window_create",1);
   static var wx_window_set_sizer = Loader.load("wx_window_set_sizer",2);
   static var wx_window_get_sizer = Loader.load("wx_window_get_sizer",1);
   static var wx_window_fit = Loader.load("wx_window_fit",1);
   static var wx_window_get_shown = Loader.load("wx_window_get_shown",1);
   static var wx_window_set_shown = Loader.load("wx_window_set_shown",2);
   static var wx_window_get_bg_colour = Loader.load("wx_window_get_bg_colour",1);
   static var wx_window_set_bg_colour = Loader.load("wx_window_set_bg_colour",2);
   static var wx_window_get_name = Loader.load("wx_window_get_name",1);
   static var wx_window_set_name = Loader.load("wx_window_set_name",2);
   static var wx_window_refresh = Loader.load("wx_window_refresh",1);
   static var wx_window_destroy = Loader.load("wx_window_destroy",1);
}


