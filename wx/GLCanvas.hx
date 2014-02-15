package wx;

import wx.Window;

class GLCanvas extends Window
{
   function new(inHandle:Dynamic) { super(inHandle); }
   public static function create(inParent:Window, ?inID:Null<Int>,
                   ?inPosition:Position,
                   ?inSize:Size, ?inStyle:Int )
   {
      if (inParent==null)
         throw Error.INVALID_PARENT;
      var handle = wx_glcanvas_create( [inParent.wxHandle,inID,"",inPosition,inSize, inStyle] );

      return new GLCanvas(handle);
   }

	public function makeCurrent()
	{
	   wx_glcanvas_make_current(wxHandle);
	}

	public function flip()
	{
	   wx_glcanvas_flip(wxHandle);
	}


   static var wx_glcanvas_create:Array<Dynamic>->Dynamic = Loader.load("wx_glcanvas_create",1);
   static var wx_glcanvas_make_current = Loader.load("wx_glcanvas_make_current",1);
   static var wx_glcanvas_flip = Loader.load("wx_glcanvas_flip",1);
}

