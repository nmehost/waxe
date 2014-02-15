package wx;

import wx.Window;

class Panel extends Window
{
   public static function create(inParent:Window, ?inID:Null<Int>,
                   ?inPosition:Position,
                   ?inSize:Size, ?inStyle:Int )
   {
      if (inParent==null)
         throw Error.INVALID_PARENT;
      var handle = wx_panel_create(
         [inParent.wxHandle,inID,"",inPosition,inSize, inStyle] );
      return new Panel(handle);
   }

   static var wx_panel_create:Array<Dynamic>->Dynamic = Loader.load("wx_panel_create",1);
}

