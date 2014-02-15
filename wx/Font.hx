package wx;

class Font
{
   public static var FAMILY_DEFAULT = 70;
   public static var FAMILY_DECORATIVE = 71;
   public static var FAMILY_ROMAN = 72;
   public static var FAMILY_SCRIPT = 73;
   public static var FAMILY_SWISS = 74;
   public static var FAMILY_MODERN = 75;
   public static var FAMILY_TELETYPE = 76;

   public static var STYLE_NORMAL = 90;
   public static var STYLE_SLANT = 92;
   public static var STYLE_ITALIC = 93;

   public static var WEIGHT_NORMAL = 90;
   public static var WEIGHT_LIGHT = 91;
   public static var WEIGHT_BOLD = 92;


   var wxHandle:Dynamic;



   public function new(size:Int, family:Int = 70, style:Int=90,
              weight=90, underline:Bool = false,
              faceName:String = "" )
   {
      wxHandle = wx_font_create(size,family,style,weight,underline,faceName);
   }
   public function wxGetHandle() { return wxHandle; }

   static var wx_font_create = Loader.load("wx_font_create",-1);
}
