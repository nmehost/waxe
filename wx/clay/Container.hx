package wx.clay;

class Container
{
   public var parent(wxGetParent,null) : Container;
   public var manager(wxGetManager,null) : Manager;
   public var shown(wxGetShown,wxSetShown) : Bool;
   public var rect(wxGetRect,wxSetRect) : wx.Rect;
   public var window(wxGetWindow,null) : wx.Window;
   public var minWidth(wxGetMinWidth,wxSetMinWidth) : Int;
   public var minHeight(wxGetMinHeight,wxSetMinHeight) : Int;

   public var wxHandle:Dynamic;
   function _wx_deleted() { wxHandle=null; }

   static var stBest = 0;
   static var stMin = 1;
   static var stMax = 2;
   static var stFloating = 3;
   static var stWideDock = 4;
   static var stTallDock = 5;
   
   
   function new() {}

   public function raise()
   {
   }

   public function addContainer(inContainer:Container, inWhere:AddPosition)
   {
   }

   public function addWindow(inWindow:wx.Window, inWhere:AddPosition, /* Icons */
                     inFlags:Int = 0 ) : Container
   {
       var container = new Container();
       container.wxHandle = wx_container_add_window(wxHandle, inWindow.wxHandle, container,
           Type.enumIndex(inWhere), inFlags );
       return container;
   }
   public function addToolbox(inToolbox:Toolbox, inWhere:AddPosition )
   {
       var container = new Container();
       container.wxHandle = wx_container_add_window(wxHandle, inToolbox.wxHandle, container,
           Type.enumIndex(inWhere), 0 );
       return container;
   }

   public function wxGetParent() : Container { return null; }
   public function wxGetManager() : Manager { return null; }

   public function wxGetShown() : Bool { return false; }
   public function wxSetShown(inShow:Bool) : Bool { return false; }

   public function wxGetRect() : wx.Rect { return null; }
   public function wxSetRect(inRect:wx.Rect) : wx.Rect { return null; }

   public function wxGetWindow() : wx.Window { return null; }

   function wxGetMinWidth() : Int { return wx_container_get_width(wxHandle,stMin); }
   function wxSetMinWidth(inW:Int) : Int { return wx_container_set_width(wxHandle,stMin,inW); return inW; }
   function wxGetMinHeight() : Int { return wx_container_get_height(wxHandle,stMin); }
   function wxSetMinHeight(inH:Int) : Int { return wx_container_set_height(wxHandle,stMin,inH); return inH; }


   public static function wxCreate() { return new Container(); }


    static var wx_container_add_window = Loader.load("wx_container_add_window",-1);
    static var wx_container_get_width = Loader.load("wx_container_get_width",2);
    static var wx_container_set_width = Loader.load("wx_container_set_width",3);
    static var wx_container_get_height = Loader.load("wx_container_get_height",2);
    static var wx_container_set_height = Loader.load("wx_container_set_height",3);
}



