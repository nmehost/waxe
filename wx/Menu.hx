package wx;

class Menu extends EventHandler
{
   public static var NORMAL = 0;
   public static var SEPARATOR = 1;
   public static var CHECK = 2;
   public static var RADIO = 3;


   public function new(inTitle:String="",inFlags:Int = 0)
   {
      super( wx_menu_create(inTitle,inFlags) );
   }

   override function HandleEvent(event:Dynamic)
   {
      trace("Menu Event!");
   }
   public function append(inID:Int, inItem:String="", inHelp:String="", inKind:Int=0) : Int
   {
      wx_menu_append(wxHandle,inID,inItem,inHelp,inKind);
      return inID;
   }
   public function appendSeparator()
   {
      wx_menu_append_separator(wxHandle);
   }


   static var wx_menu_create = Loader.load("wx_menu_create",2);
   static var wx_menu_append = Loader.load("wx_menu_append",5);
   static var wx_menu_append_separator = Loader.load("wx_menu_append_separator",1);
}



