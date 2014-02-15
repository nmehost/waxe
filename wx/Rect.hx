package wx;

class Rect
{
   public var x : Int;
   public var y : Int;
   public var width : Int;
   public var height : Int;

   public function new(inX : Int=0, inY : Int=0, inWidth : Int=0, inHeight : Int=0) : Void
   {
      x = inX;
      y = inY;
      width = inWidth;
      height = inHeight;
   }

   public var left(get_left,set_left) : Int;
   function get_left() { return x; }
   function set_left(l:Int) { x=l; return l;}

   public var right(get_right,set_right) : Int;
   function get_right() { return x+width; }
   function set_right(r:Int) { width = r-x; return r;}

   public var top(get_top,set_top) : Int;
   function get_top() { return y; }
   function set_top(t:Int) { y=t; return t;}

   public var bottom(get_bottom,set_bottom) : Int;
   function get_bottom() { return y+height; }
   function set_bottom(b:Int) { height = b-y; return b;}

   public var topLeft(get_topLeft,set_topLeft) : Point;
   function get_topLeft() { return new Point(x,y); }
   function set_topLeft(p:Point) { x=p.x;y=p.y; return p.clone(); }

   public var size(get_topLeft,set_topLeft) : Point;
   function get_size() { return new Point(width,height); }
   function set_size(p:Point) { width=p.x;height=p.y; return p.clone(); }

   public var bottomRight(get_bottomRight,set_bottomRight) : Point;
   function get_bottomRight() { return new Point(x+width,y+height); }
   function set_bottomRight(p:Point)
   {
      width = p.x-x;
      height = p.y-y;
      return p.clone();
   }

   public function clone() : Rect
   {
      return new Rect(x,y,width,height);
   }
   public function contains(inX : Int, inY : Int) : Bool
   {
      return inX>=x && inY>=y && inX<right && inY<bottom;
   }
   public function containsPoint(point : Point) : Bool
   {
      return contains(point.x,point.y);
   }
   public function containsRect(rect : Rect) : Bool
   {
     return contains(rect.x,rect.y) && containsPoint(rect.bottomRight);
   }
   public function equals(toCompare : Rect) : Bool
   {
      return x==toCompare.x && y==toCompare.y &&
             width==toCompare.width && height==toCompare.height;
   }
   public function inflate(dx : Int, dy : Int) : Void
   {
      x-=dx; width+=dx*2;
      y-=dy; height+=dy*2;
   }
   public function inflatePoint(point : Point) : Void
   {
      inflate(point.x,point.y);
   }
   public function intersection(toIntersect : Rect) : Rect
   {
      var x0 = x<toIntersect.x ? toIntersect.x : x;
      var x1 = right>toIntersect.right ? toIntersect.right : right;
      if (x1<=x0)
         return new Rect();

      var y0 = y<toIntersect.y ? toIntersect.x : y;
      var y1 = bottom>toIntersect.bottom ? toIntersect.bottom : bottom;
      if (y1<=y0)
         return new Rect();

      return new Rect(x0,y0,x1-x0,y1-y0);
   }

   public function intersects(toIntersect : Rect) : Bool
   {
      var x0 = x<toIntersect.x ? toIntersect.x : x;
      var x1 = right>toIntersect.right ? toIntersect.right : right;
      if (x1<=x0)
         return false;

      var y0 = y<toIntersect.y ? toIntersect.y : y;
      var y1 = bottom>toIntersect.bottom ? toIntersect.bottom : bottom;
      return y1>y0;
   }

   public function union(toUnion : Rect) : Rect
   {
      var x0 = x>toUnion.x ? toUnion.x : x;
      var x1 = right<toUnion.right ? toUnion.right : right;
      var y0 = y>toUnion.y ? toUnion.x : y;
      var y1 = bottom<toUnion.bottom ? toUnion.bottom : bottom;
      return new Rect(x0,y0,x1-x0,y1-y0);
   }

   public function isEmpty() : Bool { return width==0 && height==0; }
   public function offset(dx : Int, dy : Int) : Void
   {
      x+=dx;
      y+=dy;
   }

   public function offsetPoint(point : Point) : Void
   {
      x+=point.x;
      y+=point.y;
   }

   public function setEmpty() : Void { x = y = width = height = 0; }
   public function extendBounds(r:Rect)
   {
      var dx = x-r.x;
      if (dx>0)
      {
         x-=dx;
         width+=dx;
      }
      var dy = y-r.y;
      if (dy>0)
      {
         y-=dy;
         height+=dy;
      }
      if (r.right>right)
         right = r.right;
      if (r.bottom>bottom)
         bottom = r.bottom;
   }
}
