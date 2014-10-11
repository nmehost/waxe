import wx.EventID;
import wx.Sizer;
import wx.App;
import wx.StaticText;
import wx.TextCtrl;
import wx.Frame;
//import wx.MessageDialog;

class Main
{
	var frame : Frame;
	var textCtrl:TextCtrl;
	var staticText:StaticText;
	var counter:Int = 1;

	function new()
	{
		this.frame = wx.Frame.create(null, null, "This is a Frame", null, { width: 800, height: 600 });
		this.textCtrl = TextCtrl.create(frame, null, 'Type something here', {x:20, y:40}, {width:200, height:22});
		this.staticText = StaticText.create(frame, null, 'StaticText', { x:20, y: 80 } );
		
		this.textCtrl.onTextUpdated = function(e) {
			staticText.label = textCtrl.value;
		}
		
		this.frame.onSize = function(e) { 
			this.staticText.label = 'Frame size is changed : $counter';
			counter++;
			e.skip = true; 
		}		

		App.setTopWindow(this.frame);
		this.frame.shown = true;
	}

	public static function main()
	{
		App.boot(function() { new Main(); });
	}
}
