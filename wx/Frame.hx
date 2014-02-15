package wx;

class Frame extends TopLevelWindow
{
   public var menuBar(null,set) : wx.MenuBar;
   var menuMap : Map<Int,Dynamic->Void>;

   public static function create(inParent:Window, ?inID:Int, inTitle:String="",
                  ?inPosition:{x:Int,y:Int},
                   ?inSize:{width:Int,height:Int}, ?inStyle:Int )
   {
      var handle = wx_frame_create(
         [inParent==null ? null : inParent.wxHandle,inID,inTitle,inPosition,inSize, inStyle] );
      return new Frame(handle);
   }


   public function new(inHandle:Dynamic)
   {
      super(inHandle);
      setHandler(EventID.COMMAND_MENU_SELECTED, onMenu);
      menuMap = new Map<Int,Dynamic->Void>();
   }

   public function wxSetMenuBar(inBar:wx.MenuBar)
   {
      wx_frame_set_menu_bar(wxHandle,inBar.wxHandle);
   }
   public function set_menuBar(inBar:wx.MenuBar) 
   {
      wx_frame_set_menu_bar(wxHandle,inBar.wxHandle);
      return inBar;
   }

   public function handle(id:Int,handler:Dynamic->Void)
   {
      menuMap.set(id,handler);
   }

   public function onMenu(event:Dynamic)
   {
      var id:Int = event.id;
      if (menuMap.exists(id))
         menuMap.get(id)(event);
   }

   static var wx_frame_create:Array<Dynamic>->Dynamic = Loader.load("wx_frame_create",1);
   static var wx_frame_set_menu_bar = Loader.load("wx_frame_set_menu_bar",2);

}
