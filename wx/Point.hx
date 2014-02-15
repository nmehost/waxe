package wx;

class Point
{
   public var x : Int;
   public var y : Int;

   public function new(inX : Int=0, inY : Int=0)
   {
      x = inX;
      y = inY;
   }

   public function add(v : Point) : Point
   {
      return new Point(v.x+x,v.y+y);
   }

   public function clone() : Point
   {
      return new Point(x,y);
   }

   public function equals(toCompare : Point) : Bool
   {
      return toCompare.x==x && toCompare.y==y;
   }

   public var length(get_length,null) : Float;
   function get_length()
   {
      return Math.sqrt(x*x+y*y);
   }

   public function offset(dx : Int, dy : Int) : Void
   {
      x+=dx;
      y+=dy;
   }
   public function subtract(v : Point) : Point
   {
      return new Point(x-v.x,y-v.y);
   }

   public static function distance(pt1 : Point, pt2 : Point) : Float
   {
      var dx = pt1.x-pt2.x;
      var dy = pt1.y-pt2.y;
      return Math.sqrt(dx*dy + dy*dy);
   }

   public static function interpolate(pt1 : Point, pt2 : Point, f : Float) : Point
   {
      return new Point( Std.int(pt2.x + f*(pt1.x-pt2.x)),
                        Std.int(pt2.y + f*(pt1.y-pt2.y)) );
   }

}
