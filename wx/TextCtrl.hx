package wx;

import wx.Window;

class TextCtrl extends Window
{
	public var value(get,set):String;

   public static function create(inParent:Window, ?inID:Null<Int>, inText:String="",
	                ?inPosition:Position,
                   ?inSize:Size, ?inStyle:Int )
   {
		if (inParent==null)
			throw Error.INVALID_PARENT;
      var handle = wx_text_ctrl_create(
			[inParent.wxHandle,inID,inText,inPosition,inSize, inStyle] );
      return new TextCtrl(handle);
   }


   public function new(inHandle:Dynamic)
   {
	   super(inHandle);
   }

	public function set_value(inString:String) : String
	{
		wx_text_ctrl_set_value(wxHandle,inString);
		return inString;
	}
	public function get_value() : String
	{
		return wx_text_ctrl_get_value(wxHandle);
	}


   static var wx_text_ctrl_create:Array<Dynamic>->Dynamic = Loader.load("wx_text_ctrl_create",1);
   static var wx_text_ctrl_get_value = Loader.load("wx_text_ctrl_get_value",1);
   static var wx_text_ctrl_set_value = Loader.load("wx_text_ctrl_set_value",2);
}
