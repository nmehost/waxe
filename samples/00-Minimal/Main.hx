import wx.EventID;
import wx.Sizer;
import wx.App;
import wx.StaticText;
import wx.TextCtrl;
import wx.Frame;
import wx.Button;
//import wx.MessageDialog;

class Main
{
	
	function new()
	{
		var frame : Frame = Frame.create(null, null, "Frame", null, { width: 300, height: 200 } );	
		StaticText.create(frame, null, 'StaticText: Hello World!');		
		
		App.setTopWindow(frame);
		frame.shown = true;
	}

	public static function main()
	{
		App.boot(function() { new Main(); });
	}
}
