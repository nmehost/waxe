package wx;

import wx.Window;

class DirDialog
{
   public var parent:Dynamic;
   public var message:String;
   public var directory:String;
   public var style:Int;
   public var position:Position;
   public var size:Size;
   public var name:String;

   // Style flags
   public static inline var CHANGE_DIR       = 0x0100;
   public static inline var DIR_MUST_EXIST   = 0x0200;

   public function new(inParent:Window=null, inMessage:String="Choose a directory",
           inDefaultDir:String="",
           inStyle:Int=Dialog.DEFAULT_STYLE,
           inPosition:Position=null, inSize:Size=null, inName:String="dirdlg")
   {
      parent = inParent==null ? null : inParent.wxHandle;
      message=inMessage;
      directory=inDefaultDir;
      style = inStyle;
      position = inPosition;
      size = inSize;
      name = inName;
   }

   public function showModal() : Bool
   {
      return wx_dir_dialog_show(this);
   }

   static var wx_dir_dialog_show = Loader.load("wx_dir_dialog_show",1);
}


