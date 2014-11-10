package wx;

import wx.Window;
import wx.EventID;
import nme.display.ManagedStage;


class NMEStage extends GLCanvas
{
   public var stage(default,null) : ManagedStage;
   var mLastValue:Int;
   var mTimer:Timer;

   function new(inHandle:Dynamic,inWidth:Int, inHeight:Int)
   {
      super(inHandle);
      var me = this;
      mLastValue = 0;
      stage = nme.Lib.createManagedStage(inWidth,inHeight);
      stage.onQuit = App.quit;
      stage.setNextWake = me.setNextWake;
      stage.renderRequest = me.refresh;
      onSize = myOnSize;

      // Must use proper paint handler, or opengl will not work
      //setHandler(wx.EventID.PAINT, me.render);
      onPaint = render;
      mTimer = new Timer(this);
      stage.window.nextWakeHandler = setNextWake;
      setNextWake(1);
   }

   override function _wx_deleted()
   {
      mTimer.stop();
      super._wx_deleted();
   }

   function myOnSize(event:Dynamic)
   {
      var s = clientSize;
      stage.resize(s.width,s.height);
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
      if (inDelay>2000000)
         inDelay = 2000000;
      var start = Std.int(inDelay*1000);
      if (start<=1) start = 1;
      mTimer.start(start , true );
   }

   function render(_)
   {
      makeCurrent();
      stage.window.beginRender();
      stage.onRender(true);
      stage.window.endRender();
      flip();
   }

   public static function create(inParent:Window,?inID:Int,?inPosition:Position,
                   ?inSize:Size, ?inStyle:Int )
   {
      if (inParent==null)
         throw Error.INVALID_PARENT;
      var handle = GLCanvas.wx_glcanvas_create([inParent.wxHandle,inID,"",inPosition,inSize, inStyle] );
      var w:Int = inSize==null ? -1 : inSize.width;
      var h:Int = inSize==null ? -1 : inSize.height;
      var stage = new NMEStage(handle,w,h);
      //stage.myOnSize(null);
      return stage;
   }


}
