package wx;

import wx.Window;

class Scintilla extends Window
{
   public var value(get,set):String;

   public static function create(inParent:Window, ?inID:Null<Int>,
                   ?inPosition:Position,
                   ?inSize:Size, ?inStyle:Int )
   {
      if (inParent==null)
         throw Error.INVALID_PARENT;
      var handle = wx_scintilla_create( [inParent.wxHandle,inID,"",inPosition,inSize, inStyle] );
      return new Scintilla(handle);
   }


   public function new(inHandle:Dynamic)
   {
      super(inHandle);
   }

   public function set_value(inString:String) : String
   {
      wx_scintilla_set_value(wxHandle,inString);
      return inString;
   }
   public function get_value() : String
   {
      return wx_scintilla_get_value(wxHandle);
   }

   static var wx_scintilla_create:Array<Dynamic>->Dynamic = Loader.load("wx_scintilla_create",1);
   static var wx_scintilla_get_value = Loader.load("wx_scintilla_get_value",1);
   static var wx_scintilla_set_value = Loader.load("wx_scintilla_set_value",2);
}
