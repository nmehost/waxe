// haxe/lib/waxe/1.0.1/wx/MessageDialog.hx
 
package wx;
 
import wx.Window;
 
class MessageDialog
{
    public var parent:Dynamic;
    public var message:String;
    public var caption:String;
    public var style:Int;
 
    public static inline var OK             = 0x00000004;
    public static inline var YES         = 0x00000002;
    public static inline var NO             = 0x00000008;
    public static inline var CANCEL    = 0x00000010;
    public static inline var YES_NO    = YES | NO;
 
    public static inline var ICON_WARNING          = 0x00000100;
    public static inline var ICON_ERROR               = 0x00000200;
    public static inline var ICON_QUESTION        = 0x00000400;
    public static inline var ICON_INFORMATION    = 0x00000800;
 
    public function new (
        inParent:Window = null,
        inMessage:String = "Message",
        inCaption:String = "Caption",
        inStyle:Int = OK
    )
 
    {
        this.parent = (inParent==null) ? null : inParent.wxHandle;
        this.message = inMessage;
        this.caption = inCaption;
        this.style = inStyle;
    }
 
    public function showModal() : Int
    {
        return wx_message_dialog_show(this);
    }
 
    static var wx_message_dialog_show = Loader.load("wx_message_dialog_show", 1);
 
}