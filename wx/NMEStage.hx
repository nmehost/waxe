package wx;

import wx.Window;
import wx.EventID;
import nme.display.ManagedStage;
import nme.display.BitmapData;

typedef StageAttribs =
{
  ?width: Null<Int>,
  ?height:Null<Int>,
  ?fullscreen: Null<Bool>,
  ?stencilBuffer: Null<Bool>,
  ?depthBuffer: Null<Bool>,
  ?antiAliasing: Null<Int>,
  ?resizable: Null<Bool>,
  ?vsync: Null<Bool>,
  ?fps:Null<Float>,
  ?color:Null<Int>,
  ?title:String,
  ?icon:BitmapData,
};



class NMEStage extends GLCanvas
{
   public var stage(default,null) : ManagedStage;
   var mLastValue:Int;
   var mTimer:Timer;
   var seenPaint:Bool;

   function new(inHandle:Dynamic,inAttribs:StageAttribs)
   {
      seenPaint = false;
      super(inHandle);
 
      var w:Int = inAttribs.width==null ? -1 : inAttribs.width;
      var h:Int = inAttribs.width==null ? -1 : inAttribs.height;
         
      var me = this;
      mLastValue = 0;
      stage = nme.Lib.createManagedStage(w,h);
      stage.onQuit = App.quit;
      stage.setNextWake = me.setNextWake;
      stage.renderRequest = requestRender;
      if (inAttribs.fps!=null)
         stage.frameRate = inAttribs.fps;
      if (inAttribs.color!=null)
         stage.opaqueBackground = inAttribs.color;
      else
         stage.opaqueBackground = 0;
      onSize = myOnSize;

      // Must use proper paint handler, or opengl will not work
      setHandler(wx.EventID.PAINT, me.render);
      onPaint = render;
      mTimer = new Timer(this);
      var window = stage.window;
      window.nextWakeHandler = setNextWake;
      window.beginRenderImmediate = makeCurrent;
      window.endRenderImmediate = flip;
      setNextWake(1);
   }

   override function _wx_deleted()
   {
      mTimer.stop();
      super._wx_deleted();
   }

   function requestRender() : Bool
   {
      if (seenPaint)
         return true;
      else
         refresh();

      return false;
   }

   function myOnSize(event:Dynamic)
   {
      var s = clientSize;
      stage.onWindowSize(s.width,s.height);
   }

   function pumpMouseEvent(inID:Int, inEvent:Dynamic)
   {
      var e:Dynamic = {};
      e.type = inID;
      e.x = inEvent.x;
      e.y = inEvent.y;
      e.flags = ((inEvent.leftIsDown) ? (ManagedStage.efLeftDown|ManagedStage.efPrimaryTouch) : 0) |
                ((inEvent.controlDown) ? ManagedStage.efCtrlDown : 0) |
                ((inEvent.metaDown) ? ManagedStage.efAltDown : 0) |
                ((inEvent.shiftDown) ? ManagedStage.efShiftDown : 0);
      stage.pumpEvent(e);
   }

   function pumpKeyEvent(inID:Int, inEvent:Dynamic)
   {
      var e:Dynamic = {};
      e.type = inID;
      e.x = inEvent.x;
      e.y = inEvent.y;
      e.flags = ((inEvent.leftIsDown) ? (ManagedStage.efLeftDown|ManagedStage.efPrimaryTouch) : 0) |
                ((inEvent.controlDown) ? ManagedStage.efCtrlDown : 0) |
                ((inEvent.metaDown) ? ManagedStage.efAltDown : 0) |
                ((inEvent.shiftDown) ? ManagedStage.efShiftDown : 0);
      e.code = inEvent.code;
      e.value = inEvent.flashCode;
      stage.pumpEvent(e);
   }

  function IsModifier(inCode:Int):Bool
  {
     return (inCode>=15 && inCode<=20);
  }

   override function HandleEvent(event:Dynamic)
   {
      super.HandleEvent(event);
      switch(Type.createEnumIndex(EventID,Std.int(event.type)))
      {
         case EventID.LEFT_DCLICK: pumpMouseEvent(ManagedStage.etMouseDown,event); 
         case EventID.LEFT_DOWN: pumpMouseEvent(ManagedStage.etMouseDown,event); 
         case EventID.LEFT_UP: pumpMouseEvent(ManagedStage.etMouseUp,event); 
         case EventID.MOTION: pumpMouseEvent(ManagedStage.etMouseMove,event); 
         case EventID.KEY_DOWN:
            if (IsModifier(event.flashCode))
            {
               // These don't generate CHARevents ...
               pumpKeyEvent(ManagedStage.etKeyDown,event); 
               mLastValue = 0;
            }
         case EventID.CHAR:
            mLastValue = event.code;
            pumpKeyEvent(ManagedStage.etKeyDown,event); 
         case EventID.KEY_UP:
            event.code = mLastValue;
            pumpKeyEvent(ManagedStage.etKeyUp,event); 
         case EventID.IDLE:
            if (stage.isDisplayListDirty())
               stage.window.onInvalidFrame();
            stage.pumpEvent({type:ManagedStage.etPoll});
         case EventID.TIMER:
            stage.pumpEvent({type:ManagedStage.etPoll});

         default:
      }
   }

   function setNextWake(inDelay:Float)
   {
      //trace("setNextWake " + inDelay);
      if (inDelay>2000000)
      {
         return;
         //inDelay = 2000000;
      }
      var start = Std.int(inDelay*1000);
      if (start<=#if windows 10 #else 0 #end)
      {
         postIdleEvent();
         return;
      }
      else
          start = 1;
      mTimer.start(start , true );
   }

   function render(_)
   {
      seenPaint = true;
      makeCurrent();
      stage.window.beginRender();
      stage.onRender(true);
      stage.window.endRender();
      flip();
   }

   public static function create(inParent:Window,?inID:Int,
                   ?inPosition:Position,
                   ?inAttribs: StageAttribs,
                  ?inStyle:Int )
   {
      if (inParent==null)
         throw Error.INVALID_PARENT;
      var handle = GLCanvas.wx_glcanvas_create([inParent.wxHandle,inID,"",inPosition,inAttribs, inStyle] );
      var stage = new NMEStage(handle,inAttribs==null?{}:inAttribs);
      stage.myOnSize(null);
      return stage;
   }


}
