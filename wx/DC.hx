package wx;

class DC
{
   public var pen(null,set):Pen;
   public var brush(null,set):Brush;
   public var background(null,set):Brush;
   public var font(null,set):Font;
   var wxHandle:Dynamic;

   function new(handle:Dynamic)
   {
      wxHandle = handle;
   }
   public function destroy() { wx_object_destroy(wxHandle); }

   public function drawLine(x1:Int, y1:Int, x2:Int, y2:Int)
   {
      wx_dc_draw_line(wxHandle,x1,y1,x2,y2);
   }
   public function clear()
   {
      wx_dc_clear(wxHandle);
   }
   public function set_pen(inPen:Pen) : Pen
   {
      wx_dc_set_pen(wxHandle,inPen.wxGetHandle());
      return inPen;
   }
   public function set_brush(inBrush:Brush) : Brush
   {
      wx_dc_set_brush(wxHandle,inBrush.wxGetHandle());
      return inBrush;
   }
   public function set_background(inBrush:Brush) : Brush
   {
      wx_dc_set_background(wxHandle,inBrush.wxGetHandle());
      return inBrush;
   }
   public function set_font(inFont:Font) : Font
   {
      wx_dc_set_font(wxHandle,inFont.wxGetHandle());
      return inFont;
   }
   public function drawRectangle(x:Int, y:Int, width:Int, height:Int)
   {
      wx_dc_draw_rectangle(wxHandle,x,y,width,height);
   }
   public function drawCircle(x:Int, y:Int, rad:Int)
   {
      wx_dc_draw_circle(wxHandle,x,y,rad);
   }
   public function drawEllipse(x:Int, y:Int, width:Int, height:Int)
   {
      wx_dc_draw_ellipse(wxHandle,x,y,width,height);
   }
   public function drawText(text:String,x:Int, y:Int)
   {
      wx_dc_draw_text(wxHandle,text,x,y);
   }
   public function drawBitmap(bitmap:Bitmap,x:Int, y:Int, inTransparent:Bool)
   {
      wx_dc_draw_bitmap(wxHandle,bitmap.wxHandle,x,y,inTransparent);
   }

   static public function createPaintDC(inWindow:Window)
   {
      return new DC(wx_dc_create_paint(inWindow.wxHandle));
   }

   static var wx_dc_create_paint = Loader.load("wx_dc_create_paint",1);
   static var wx_dc_draw_line = Loader.load("wx_dc_draw_line",5);
   static var wx_object_destroy = Loader.load("wx_object_destroy",1);
   static var wx_dc_clear = Loader.load("wx_dc_clear",1);
   static var wx_dc_set_pen = Loader.load("wx_dc_set_pen",2);
   static var wx_dc_set_font = Loader.load("wx_dc_set_font",2);
   static var wx_dc_set_brush = Loader.load("wx_dc_set_brush",2);
   static var wx_dc_set_background = Loader.load("wx_dc_set_background",2);
   static var wx_dc_draw_rectangle = Loader.load("wx_dc_draw_rectangle",5);
   static var wx_dc_draw_circle = Loader.load("wx_dc_draw_circle",4);
   static var wx_dc_draw_ellipse = Loader.load("wx_dc_draw_ellipse",5);
   static var wx_dc_draw_text = Loader.load("wx_dc_draw_text",4);
   static var wx_dc_draw_bitmap = Loader.load("wx_dc_draw_bitmap",5);
}
