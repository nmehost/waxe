package wx;

class Timer
{
	var wxHandle:Dynamic;

   public function new(inHandler:EventHandler,inID:Int = -1)
	{
	  wxHandle = wx_timer_create(inHandler.wxHandle,inID);
	}
	public function start(inMilliSeconds:Int=-1, inOneShot:Bool=false)
	{
	   wx_timer_start(wxHandle,inMilliSeconds,inOneShot);
	}
	public function stop()
	{
	   wx_timer_stop(wxHandle);
	}

   static var wx_timer_create = Loader.load("wx_timer_create",2);
   static var wx_timer_start = Loader.load("wx_timer_start",3);
   static var wx_timer_stop = Loader.load("wx_timer_stop",1);
}

