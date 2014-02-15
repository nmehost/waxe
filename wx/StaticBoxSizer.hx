package wx;

class StaticBoxSizer extends BoxSizer
{
	function new(inHandle:Dynamic) { super(inHandle); }

	public static function create(inVertical:Bool, parent:Window, inLabel:String="")
	{
		return new StaticBoxSizer( wx_sizer_create_static_box(inVertical,parent.wxHandle,inLabel) );
	}

	public static function createFromStaticBox(inBox:StaticBox, inVertical:Bool)
	{
		return new StaticBoxSizer( wx_sizer_create_from_static_box(inBox,inVertical) );
	}


	static var wx_sizer_create_from_static_box = Loader.load("wx_sizer_create_from_static_box",2);
	static var wx_sizer_create_static_box = Loader.load("wx_sizer_create_static_box",3);

}
