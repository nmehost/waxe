import wx.EventID;
import wx.Sizer;
import wx.App;
//import wx.MessageDialog;

class Main
{
   var mFrame : wx.Frame;
   var mWindow : wx.Window;
   var mDrawArea : wx.Window;

   function new()
   {
      mFrame = wx.Frame.create(null, null, "Simple01", null, { width: 800, height: 600 });

      mFrame.onSize = function(evt) { layout(); evt.skip=true; }

     // new wx.MessageDialog(null, 'Hello world!', 'Message caption').showModal();
      
      
      mWindow = wx.Panel.create(mFrame);
      mDrawArea = wx.Panel.create(mWindow);


      var sizerVertical = wx.FlexGridSizer.create(null,1);
      sizerVertical.addGrowableCol(0);
      var sizerItems = wx.FlexGridSizer.create(null,2);
      var sizerButton = wx.BoxSizer.create(false);
      sizerVertical.add(sizerItems,0,Sizer.EXPAND);
      sizerVertical.add(mDrawArea,1,Sizer.EXPAND);
      //var scintilla = wx.Scintilla.create(mWindow,null);
      //vertical_sizer.add(scintilla,1,Sizer.EXPAND);
      sizerVertical.add(sizerButton,0,
         Sizer.ALIGN_CENTRE | Sizer.BORDER_TOP | Sizer.BORDER_BOTTOM, 10);
      sizerVertical.addGrowableRow(1);
      var btnClose = wx.Button.create(mWindow,null,"Close");
      sizerButton.add(btnClose);
	btnClose.onClick = function(_) App.quit();

      sizerItems.addGrowableCol(1,1);
      sizerItems.add(wx.StaticText.create(mWindow, null, "TextCtrl"), 0, Sizer.ALIGN_CENTRE_VERTICAL);
      
      var textCtrl = wx.TextCtrl.create(mWindow,null,"Here is some text" );
      sizerItems.add(textCtrl,1,Sizer.EXPAND | Sizer.BORDER_ALL, 10);
     

      sizerItems.add( wx.StaticText.create(mWindow, null, "ComboBox"), 0, Sizer.ALIGN_CENTRE_VERTICAL);
      
      
      var combo = wx.ComboBox.create(mWindow,null,"Some Text", ["Choice 1","Choice 2"]);
      sizerItems.add(combo,1,Sizer.EXPAND | Sizer.BORDER_ALL, 10);

      sizerItems.add(wx.StaticText.create(mWindow,null,"Text 3"),0,Sizer.ALIGN_CENTRE_VERTICAL);
      
      var textCtrl = wx.TextCtrl.create(mWindow,null,"Type something here..." );
      sizerItems.add(textCtrl, 1, Sizer.EXPAND | Sizer.BORDER_ALL, 10);
      
        sizerItems.add(wx.StaticText.create(mWindow, null, "StaticText"), 0, Sizer.ALIGN_CENTRE_VERTICAL);
        
      var staticText = wx.StaticText.create(mWindow, null, "StaticText");
      sizerItems.add(staticText, 1,  Sizer.EXPAND | Sizer.BORDER_ALL, 10);

	textCtrl.onTextUpdated = function(_) {
		staticText.label = textCtrl.value;
	}
      

      mWindow.sizer = sizerVertical;

      mDrawArea.backgroundColour = 0xffffff;
     

      layout();

      mDrawArea.onPaint = paintWindow;
      App.setTopWindow(mFrame);
      mFrame.shown = true;
   }

   function paintWindow(dc:wx.DC)
   {
      dc.clear();
      dc.pen = new wx.Pen( wx.Colour.Pink(), 3 );
      dc.drawLine(0,0,300,250);
      dc.brush = new wx.Brush( wx.Colour.Yellow(), wx.Brush.SOLID );
      dc.drawRectangle(100,100,100,200);
      dc.brush = new wx.Brush( wx.Colour.DarkGreen(), wx.Brush.SOLID );
      dc.pen = new wx.Pen( wx.Colour.Black(), 3, wx.Pen.SHORT_DASH );
      dc.drawCircle(100,10,50);
      dc.drawEllipse(100,200,200,40);
      dc.font = new wx.Font(20);
      dc.drawText("Hello!",20,20);
   }

   function layout()
   {
      mWindow.size = mFrame.clientSize;
   }

   public static function main()
   {
      App.boot(function() { new Main(); });
   }
}
