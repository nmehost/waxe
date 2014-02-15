package wx;

class Sizer
{
	public static var BORDER_LEFT   = 0x0010;
   public static var BORDER_RIGHT  = 0x0020;
   public static var BORDER_UP     = 0x0040;
   public static var BORDER_DOWN   = 0x0080;

   public static var BORDER_TOP    = 0x0040;
   public static var BORDER_BOTTOM = 0x0080;

   public static var BORDER_ALL    = 0x00f0;

	public static var EXPAND = 0x2000;

   public static var ALIGN_NOT               = 0x0000;
   public static var ALIGN_CENTER_HORIZONTAL = 0x0100;
   public static var ALIGN_CENTRE_HORIZONTAL = 0x0100;
   public static var ALIGN_LEFT              = 0;
   public static var ALIGN_TOP               = 0;
   public static var ALIGN_RIGHT             = 0x0200;
   public static var ALIGN_BOTTOM            = 0x0400;
   public static var ALIGN_CENTER_VERTICAL   = 0x0800;
   public static var ALIGN_CENTRE_VERTICAL   = 0x0800;

   public static var ALIGN_CENTER            = 0x0900;
   public static var ALIGN_CENTRE            = 0x0900;

   public static var ALIGN_MASK              = 0x0f00;


	var wxHandle:Dynamic;

	function new(inHandle:Dynamic)
	{
		wxHandle = inHandle;
		if (wxHandle!=null)
			wx_set_data(wxHandle,this);
	}
	function _wx_deleted() { wxHandle = null; }

	public function add(sizerOrWindow:Dynamic, proportion:Int = 0, flag:Int = 0, border:Int = 0 )
	{
		var handle = Reflect.field(sizerOrWindow,"wxHandle");
		wx_sizer_add(wxHandle,handle,proportion,flag,border);
	}
	public function addSpacer(inSize:Int)
   {
		wx_sizer_add_spacer(wxHandle,inSize,0);
   }
	public function addStretchSpacer(inProportion:Int)
   {
		wx_sizer_add_spacer(wxHandle,0,inProportion);
   }



	public function setSizeHints(inWindow:Window)
   {
		wx_sizer_set_size_hints(wxHandle,inWindow.wxHandle);
   }

	public function fit(inWindow:Window)
   {
		wx_sizer_fit(wxHandle,inWindow.wxHandle);
   }
	public function wxGetHandle() { return wxHandle; }

	static var wx_set_data = Loader.load("wx_set_data",2);
   static var wx_get_data = Loader.load("wx_get_data",1);
   static var wx_sizer_add = Loader.load("wx_sizer_add",5);
   static var wx_sizer_add_spacer = Loader.load("wx_sizer_add_spacer",3);
   static var wx_sizer_set_size_hints = Loader.load("wx_sizer_set_size_hints",2);
   static var wx_sizer_fit = Loader.load("wx_sizer_fit",2);

}




