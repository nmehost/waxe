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
	var counter:Int = 0;
	function new()
	{
		var frame : Frame = Frame.create(null, null, "This is a Frame", null, { width: 800, height: 600 });
		var textCtrl:TextCtrl = TextCtrl.create(frame, null, 'Type something here', {x:20, y:40}, {width:200, height:22});
		var staticText:StaticText = StaticText.create(frame, null, 'StaticText', { x:20, y: 80 } );
		var btnClose:Button = Button.create(frame,null,"Close",  { x:20, y: 120 } );
		
		textCtrl.onTextUpdated = function(e) {
			staticText.label = textCtrl.value;
		}
		
		btnClose.onClick = function(e) {
			App.quit();
		}		
		
		frame.onSize = function(e) { 
			staticText.label = 'Frame size is changed : $counter';
			counter++;
			e.skip = true; 
		}		

		App.setTopWindow(frame);
		frame.shown = true;
	}

	public static function main()
	{
		App.boot(function() { new Main(); });
	}
}
