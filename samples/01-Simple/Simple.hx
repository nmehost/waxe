import wx.EventID;
import wx.Sizer;
import wx.App;
import wx.MenuBar;
import wx.Menu;

class Simple
{
   var mFrame : wx.Frame;
   var mWindow : wx.Window;
   var mDrawArea : wx.Window;

   function new()
   {
      mFrame = wx.Frame.create(null, null, "Simple01", null, { width: 800, height: 600 });

      mFrame.onSize = function(evt) { layout(); evt.skip=true; }

      mWindow = wx.Panel.create(mFrame);
      mDrawArea = wx.Panel.create(mWindow);


      var vertical_sizer = wx.FlexGridSizer.create(null,1);
      vertical_sizer.addGrowableCol(0);
      var items_sizer = wx.FlexGridSizer.create(null,2);
      var button_sizer = wx.BoxSizer.create(false);
      vertical_sizer.add(items_sizer,0,Sizer.EXPAND);
      vertical_sizer.add(mDrawArea,1,Sizer.EXPAND);
      //var scintilla = wx.Scintilla.create(mWindow,null);
      //vertical_sizer.add(scintilla,1,Sizer.EXPAND);
      vertical_sizer.add(button_sizer,0,
         Sizer.ALIGN_CENTRE | Sizer.BORDER_TOP | Sizer.BORDER_BOTTOM, 10);
      vertical_sizer.addGrowableRow(1);
      var close = wx.Button.create(mWindow,null,"Close");
      button_sizer.add(close);


      items_sizer.addGrowableCol(1,1);
      items_sizer.add(wx.StaticText.create(mWindow,null,"TextCtrl"),0,Sizer.ALIGN_CENTRE_VERTICAL);
      var text = wx.TextCtrl.create(mWindow,null,"Here is some text" );
      items_sizer.add(text,1,Sizer.EXPAND | Sizer.BORDER_ALL, 10);

      items_sizer.add(wx.StaticText.create(mWindow,null,"ComboBox"),0,Sizer.ALIGN_CENTRE_VERTICAL);
      var combo = wx.ComboBox.create(mWindow,null,"Some Text", ["Choice 1","Choice 2"]);
      items_sizer.add(combo,1,Sizer.EXPAND | Sizer.BORDER_ALL, 10);

      items_sizer.add(wx.StaticText.create(mWindow,null,"Text 3"),0,Sizer.ALIGN_CENTRE_VERTICAL);
      var text = wx.TextCtrl.create(mWindow,null,"Hello !" );
      items_sizer.add(text,1,Sizer.EXPAND | Sizer.BORDER_ALL, 10);


      mWindow.sizer = vertical_sizer;

      /*
       * adding menubar for test purposes
       */
      var menu_bar = new wx.MenuBar();
      var file_menu = new wx.Menu();
      file_menu.append(1, "Open File");

      // make sure the About, Preferences and Exit menus are correctly handled on OSX.
      // they shoudl remain under the File menu on other platforms.
      file_menu.append(App.s_macAboutMenuItemId, "About Simple.hx");
      file_menu.append(App.s_macPreferencesMenuItemId, "Preferences of Simple.hx");
      // on OSX, the label of the Exit menu will always be the localized system one.
      // "Quit" in english; you cannot override that label
      file_menu.append(App.s_macExitMenuItemId, "Exit");

      // on OSX, never append or set the menubar before it is completed
      menu_bar.append(file_menu, "File");

      // attach the menubar to the application
      mFrame.menuBar = menu_bar;

      mDrawArea.backgroundColour = 0xffffff;
      close.onClick = function(_) App.quit();

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
      App.boot(function() { new Simple(); });
   }
}
