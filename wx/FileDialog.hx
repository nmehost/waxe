package wx;

import wx.Window;

class FileDialog
{
   public var parent:Dynamic;
   public var message:String;
   public var directory:String;
   public var file:String;
   public var filter:String;
   public var style:Int;
   public var position:Position;
   public var size:Size;
   public var name:String;
   public var files:Array<String>;

   // Style flags
   public static inline var DEFAULT_STYLE    = 0x01;
   public static inline var OPEN             = 0x01;
   public static inline var SAVE             = 0x02;
   public static inline var OVERWRITE_PROMPT = 0x04;
   public static inline var FILE_MUST_EXIST =  0x10;
   public static inline var MULTIPLE =         0x20;
   public static inline var CHANGE_DIR =       0x80;
   public static inline var PREVIEW =          0x100;



   public function new(inParent:Window=null, inMessage:String="Choose a file",
           inDefaultDir:String="", inDefaultFile:String="", inFilter:String="*",
           inStyle:Int=1, inPosition:Position=null, inSize:Size=null, inName:String="filedlg")
   {
      parent = inParent==null ? null : inParent.wxHandle;
      message=inMessage;
      directory=inDefaultDir;
      file=inDefaultFile;
      filter = inFilter;
      style = inStyle;
      position = inPosition;
      size = inSize;
      name = inName;
      files = [];
   }

   public function showModal() : Bool
   {
      return wx_file_dialog_show(this);
   }

   static var wx_file_dialog_show = Loader.load("wx_file_dialog_show",1);
}


