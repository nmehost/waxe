package wx;

class Colour
{
	public var red(get,set):Int;
	public var green(get,set):Int;
	public var blue(get,set):Int;
	public var combined(get,set):Int;

	var mRGB:Int;

   public function new(red:Int, green:Int, blue:Int)
	{
		mRGB = (red<<16) | (green<<8) | blue;
	}

	public function get_red():Int { return (mRGB>>16) & 0xff; }
	public function set_red(inRed:Int):Int
		{ mRGB = (mRGB & 0x00ffff) | (inRed & 0xff); return inRed; }

	public function get_green():Int { return (mRGB>>8) & 0xff; }
	public function set_green(inGreen:Int):Int
		{ mRGB = (mRGB & 0xff00ff) | (inGreen & 0xff); return inGreen; }

	public function get_blue():Int { return (mRGB>>16) & 0xff; }
	public function set_blue(inBlue:Int):Int
		{ mRGB = (mRGB & 0xffff00) | (inBlue & 0xff); return inBlue; }

	public function get_combined() { return mRGB; }
	public function set_combined(inVal:Int) { mRGB = inVal & 0xffffff; return mRGB; }


   public static function LightPink() { return new Colour(255,182,193); }
   public static function Pink() { return new Colour(255,192,203); }
   public static function Crimson() { return new Colour(220,20,60); }
   public static function LavenderBlush() { return new Colour(255,240,245); }
   public static function PaleVioletRed() { return new Colour(219,112,147); }
   public static function HotPink() { return new Colour(255,105,180); }
   public static function DeepPink() { return new Colour(255,20,147); }
   public static function MediumVioletRed() { return new Colour(199,21,133); }
   public static function Orchid() { return new Colour(218,112,214); }
   public static function Thistle() { return new Colour(216,191,216); }
   public static function Plum() { return new Colour(221,160,221); }
   public static function Violet() { return new Colour(238,130,238); }
   public static function Fuchsia() { return new Colour(255,0,255); }
   public static function DarkMagenta() { return new Colour(139,0,139); }
   public static function Purple() { return new Colour(128,0,128); }
   public static function MediumOrchid() { return new Colour(186,85,211); }
   public static function DarkViolet() { return new Colour(148,0,211); }
   public static function DarkOrchid() { return new Colour(153,50,204); }
   public static function Indigo() { return new Colour(75,0,130); }
   public static function BlueViolet() { return new Colour(138,43,226); }
   public static function MediumPurple() { return new Colour(147,112,219); }
   public static function MediumSlateBlue() { return new Colour(123,104,238); }
   public static function SlateBlue() { return new Colour(106,90,205); }
   public static function DarkSlateBlue() { return new Colour(72,61,139); }
   public static function GhostWhite() { return new Colour(248,248,255); }
   public static function Lavender() { return new Colour(230,230,250); }
   public static function Blue() { return new Colour(0,0,255); }
   public static function MediumBlue() { return new Colour(0,0,205); }
   public static function DarkBlue() { return new Colour(0,0,139); }
   public static function Navy() { return new Colour(0,0,128); }
   public static function MidnightBlue() { return new Colour(25,25,112); }
   public static function RoyalBlue() { return new Colour(65,105,225); }
   public static function CornflowerBlue() { return new Colour(100,149,237); }
   public static function LightSteelBlue() { return new Colour(176,196,222); }
   public static function LightSlateGray() { return new Colour(119,136,153); }
   public static function SlateGray() { return new Colour(112,128,144); }
   public static function DodgerBlue() { return new Colour(30,144,255); }
   public static function AliceBlue() { return new Colour(240,248,255); }
   public static function SteelBlue() { return new Colour(70,130,180); }
   public static function LightSkyBlue() { return new Colour(135,206,250); }
   public static function SkyBlue() { return new Colour(135,206,235); }
   public static function DeepSkyBlue() { return new Colour(0,191,255); }
   public static function LightBlue() { return new Colour(173,216,230); }
   public static function PowderBlue() { return new Colour(176,224,230); }
   public static function CadetBlue() { return new Colour(95,158,160); }
   public static function DarkTurquoise() { return new Colour(0,206,209); }
   public static function Azure() { return new Colour(240,255,255); }
   public static function LightCyan() { return new Colour(224,255,255); }
   public static function PaleTurquoise() { return new Colour(175,238,238); }
   public static function Aqua() { return new Colour(0,255,255); }
   public static function DarkCyan() { return new Colour(0,139,139); }
   public static function teal() { return new Colour(0,128,128); }
   public static function DarkSlateGrey() { return new Colour(47,79,79); }
   public static function Mediumturquoise() { return new Colour(72,209,204); }
   public static function Lightseagreen() { return new Colour(32,178,170); }
   public static function Turquoise() { return new Colour(64,224,208); }
   public static function Aquamarine() { return new Colour(127,255,212); }
   public static function MediumAquamarine() { return new Colour(102,205,170); }
   public static function MediumSpringGreen() { return new Colour(0,250,154); }
   public static function MintCream() { return new Colour(245,255,250); }
   public static function SpringGreen() { return new Colour(0,255,127); }
   public static function MediumseaGreen() { return new Colour(60,179,113); }
   public static function Seagreen() { return new Colour(46,139,87); }
   public static function HoneyDew() { return new Colour(240,255,240); }
   public static function DarkseaGreen() { return new Colour(143,188,143); }
   public static function PaleGreen() { return new Colour(152,251,152); }
   public static function LightGreen() { return new Colour(144,238,144); }
   public static function LimeGreen() { return new Colour(50,205,50); }
   public static function Lime() { return new Colour(0,255,0); }
   public static function ForestGreen() { return new Colour(34,139,34); }
   public static function Green() { return new Colour(0,128,0); }
   public static function DarkGreen() { return new Colour(0,100,0); }
   public static function LawnGreen() { return new Colour(124,252,0); }
   public static function Chartreuse() { return new Colour(127,255,0); }
   public static function Greenyellow() { return new Colour(173,255,47); }
   public static function DarkOliveGreen() { return new Colour(85,107,47); }
   public static function YellowGreen() { return new Colour(154,205,50); }
   public static function OliveDrab() { return new Colour(107,142,35); }
   public static function Ivory() { return new Colour(255,255,240); }
   public static function Beige() { return new Colour(245,245,220); }
   public static function LightYellow() { return new Colour(255,255,224); }
   public static function LightGoldenRodYellow() { return new Colour(250,250,210); }
   public static function Yellow() { return new Colour(255,255,0); }
   public static function Olive() { return new Colour(128,128,0); }
   public static function DarkKhaki() { return new Colour(189,183,107); }
   public static function PalegOldenRod() { return new Colour(238,232,170); }
   public static function LemonChiffon() { return new Colour(255,250,205); }
   public static function Khaki() { return new Colour(240,230,140); }
   public static function Gold() { return new Colour(255,215,0); }
   public static function CornSilk() { return new Colour(255,248,220); }
   public static function GoldenRod() { return new Colour(218,165,32); }
   public static function DarkGoldenRod() { return new Colour(184,134,11); }
   public static function FloralWhite() { return new Colour(255,250,240); }
   public static function OldLace() { return new Colour(253,245,230); }
   public static function Wheat() { return new Colour(245,222,179); }
   public static function Orange() { return new Colour(255,165,0); }
   public static function Moccasin() { return new Colour(255,228,181); }
   public static function PapayaWhip() { return new Colour(255,239,213); }
   public static function BlanchedAlmond() { return new Colour(255,235,205); }
   public static function NavajoWhite() { return new Colour(255,222,173); }
   public static function AntiqueWhite() { return new Colour(250,235,215); }
   public static function Tan() { return new Colour(210,180,140); }
   public static function BurlyWood() { return new Colour(222,184,135); }
   public static function DarkOrange() { return new Colour(255,140,0); }
   public static function Bisque() { return new Colour(255,228,196); }
   public static function Linen() { return new Colour(250,240,230); }
   public static function Peru() { return new Colour(205,133,63); }
   public static function PeachPuff() { return new Colour(255,218,185); }
   public static function SandyBrown() { return new Colour(244,164,96); }
   public static function Chocolate() { return new Colour(210,105,30); }
   public static function SaddleBrown() { return new Colour(139,69,19); }
   public static function SeaShell() { return new Colour(255,245,238); }
   public static function Sienna() { return new Colour(160,82,45); }
   public static function LightSalmon() { return new Colour(255,160,122); }
   public static function Coral() { return new Colour(255,127,80); }
   public static function OrangeRed() { return new Colour(255,69,0); }
   public static function DarkSalmon() { return new Colour(233,150,122); }
   public static function Tomato() { return new Colour(255,99,71); }
   public static function Salmon() { return new Colour(250,128,114); }
   public static function MistyRose() { return new Colour(255,228,225); }
   public static function LightCoral() { return new Colour(240,128,128); }
   public static function Snow() { return new Colour(255,250,250); }
   public static function RosyBrown() { return new Colour(188,143,143); }
   public static function IndianRed() { return new Colour(205,92,92); }
   public static function Red() { return new Colour(255,0,0); }
   public static function Brown() { return new Colour(165,42,42); }
   public static function FireBrick() { return new Colour(178,34,34); }
   public static function DarkRed() { return new Colour(139,0,0); }
   public static function Maroon() { return new Colour(128,0,0); }
   public static function White() { return new Colour(255,255,255); }
   public static function WhiteSmoke() { return new Colour(245,245,245); }
   public static function Gainsboro() { return new Colour(220,220,220); }
   public static function LightGrey() { return new Colour(211,211,211); }
   public static function Silver() { return new Colour(192,192,192); }
   public static function DarkGrey() { return new Colour(169,169,169); }
   public static function Grey() { return new Colour(128,128,128); }
   public static function DimGrey() { return new Colour(105,105,105); }
   public static function Black() { return new Colour(0,0,0); }

}
