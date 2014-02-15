package wx;

import wx.Window;

class Button extends Window
{
	public var label(get,set):String;
	public var onClick(null,set) : Dynamic->Void;

   public static function create(inParent:Window, ?inID:Null<Int>, inLabel:String="",
	                ?inPosition:Position,
                   ?inSize:Size, ?inStyle:Int )
   {
		if (inParent==null)
			throw Error.INVALID_PARENT;
      var handle = wx_button_create(
			[inParent.wxHandle,inID,inLabel,inPosition,inSize, inStyle] );
      return new Button(handle);
   }


   public function new(inHandle:Dynamic)
   {
	   super(inHandle);
   }

	function set_onClick(f:Dynamic->Void) {setHandler(wx.EventID.COMMAND_BUTTON_CLICKED,f); return f;}

	public function set_label(inString:String) : String
	{
		wx_button_set_label(wxHandle,inString);
		return inString;
	}
	public function get_label() : String
	{
		return wx_button_get_label(wxHandle);
	}


   static var wx_button_create:Array<Dynamic>->Dynamic = Loader.load("wx_button_create",1);
   static var wx_button_get_label = Loader.load("wx_button_get_label",1);
   static var wx_button_set_label = Loader.load("wx_button_set_label",2);
}
