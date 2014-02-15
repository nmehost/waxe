package wx;

class Brush
{
   public static var SOLID = 100;
   public static var TRANSPARENT = 106;
   public static var STIPPLE = 110;
   public static var BDIAGONAL_HATCH = 111;
   public static var CROSSDIAG_HATCH = 112;
   public static var FDIAGONAL_HATCH = 113;
   public static var CROSS_HATCH = 114;
   public static var HORIZONTAL_HATCH = 115;
   public static var VERTICAL_HATCH = 116;


	var wxHandle:Dynamic;

	public function new(inColour:Colour,inStyle:Int=100)
	{
		wxHandle = wx_brush_create(inColour.combined,inStyle);
	}
	public function wxGetHandle() { return wxHandle; }

   static var wx_brush_create = Loader.load("wx_brush_create",2);
}
