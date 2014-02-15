package wx;

class Pen
{
   public static var SOLID = 100;
   public static var DOT = 101;
   public static var LONG_DASH = 102;
   public static var SHORT_DASH = 103;
   public static var DOT_DASH = 104;
   public static var USER_DASH = 105;
   public static var TRANSPARENT = 106;

	var wxHandle:Dynamic;

	public function new(inColour:Colour,inWidth=1,inStyle:Int=100)
	{
		wxHandle = wx_pen_create(inColour.combined,inWidth,inStyle);
	}
	public function wxGetHandle() { return wxHandle; }

   static var wx_pen_create = Loader.load("wx_pen_create",3);
}
