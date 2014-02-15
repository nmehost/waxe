#include "HaxeAPI.h"
#include <wx/mstream.h>

value wx_bitmap_from_bytes(value inBytes)
{
	ByteData data = Val2ByteData(inBytes);
	if (data.length)
	{
		wxMemoryInputStream stream(data.data,data.length);
		wxBitmap *bitmap = new wxBitmap(wxImage(stream));
		if (bitmap)
			return WXToDeletingValue(bitmap);
	}
	return alloc_null();
}
DEFINE_PRIM(wx_bitmap_from_bytes,1)



