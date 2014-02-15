package wx;

class EventHandler
{
   public var wxHandle:Dynamic;

   public function new(inHandle:Dynamic)
   {
      wxHandle = inHandle;
      if (wxHandle!=null)
         wx_set_window_handler(wxHandle,this);
   }

   // These two functions are called by the external DLL...
   function _wx_deleted() { wxHandle = null; }
   function _handle_event(event:Dynamic) { HandleEvent(event); }

   /*
	  All events have these fields:
	   "type"
	   "name"
	   "skip"
	   "veto"
	   "id"

     Mouse Events have additional fields:
	   "x"
	   "y"
	   "leftIsDown"
	   "middleIsDown"
	   "rightIsDown"
	   "wheelDelta"
	   "controlDown"
	   "metaDown"
	   "shiftDown"
	   "cmdDown"
	*/
   function HandleEvent(event:Dynamic)
   {

   }

   static var wx_set_window_handler = Loader.load("wx_set_window_handler",2);
   static var wx_get_window_handler = Loader.load("wx_get_window_handler",1);
}

