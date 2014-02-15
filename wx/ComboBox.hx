package wx;

import wx.Window;

class ComboBox extends ControlWithItems
{
	//public var label(getLabel,setLabel):String;
	public var onSelected(null,set) : Dynamic->Void;
	public var onTextEnter(null,set) : Dynamic->Void;
	public var onTextUpdated(null,set) : Dynamic->Void;

   public static function create(inParent:Window, ?inID:Null<Int>, inValue:String="",
	                ?inPosition:Position,
                   ?inSize:Size, ?inChoices:Array<String>, ?inStyle:Int )
   {
		if (inParent==null)
			throw Error.INVALID_PARENT;
      var handle = wx_combo_box_create(
			[inParent.wxHandle,inID,inValue,inPosition,inSize, inStyle], inChoices );
      return new ComboBox(handle);
   }


   public function new(inHandle:Dynamic)
   {
	   super(inHandle);
   }

	function set_onSelected(f:Dynamic->Void)
	   {setHandler(wx.EventID.COMMAND_COMBOBOX_SELECTED,f); return f;}
	function set_onTextUpdated(f:Dynamic->Void)
	   {setHandler(wx.EventID.COMMAND_TEXT_UPDATED,f); return f;}
	function set_onTextEnter(f:Dynamic->Void)
	   {setHandler(wx.EventID.COMMAND_TEXT_ENTER,f); return f;}


   static var wx_combo_box_create:Array<Dynamic>->Array<String>->Dynamic = Loader.load("wx_combo_box_create",2);
}
