#include "HaxeAPI.h"

value wx_timer_create(value inOwner,value inID)
{
	wxEvtHandler *handler=0;
   ValueToWX(inOwner,handler);

	if (handler)
	{
      wxClientData *data = handler->GetClientObject();
      if (data)
      {
         HaxeEventHandler *haxe = (HaxeEventHandler *)data;
			handler = haxe;
		}
	}

	wxTimer *timer = new wxTimer( handler, val_int(inID) );

	return WXToDeletingValue(timer);
}
DEFINE_PRIM(wx_timer_create,2)

value wx_timer_stop(value inTimer)
{
	wxTimer *timer;
   if (ValueToWX(inTimer,timer))
	{
		timer->Stop();
	}
	return alloc_null();
}
DEFINE_PRIM(wx_timer_stop,1)

value wx_timer_start(value inTimer, value inMilliSeconds, value inOneShot)
{
	wxTimer *timer;
   if (ValueToWX(inTimer,timer))
	{
		timer->Start(val_int(inMilliSeconds), val_bool(inOneShot) );
	}
	return alloc_null();
}


DEFINE_PRIM(wx_timer_start,3)


