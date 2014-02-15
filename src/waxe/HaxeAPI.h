#ifndef HAXE_EVENT_HANDLER_H
#define HAXE_EVENT_HANDLER_H

#include <wx/wx.h>
#include <hx/CFFI.h>

typedef unsigned char uint8;

wxString Val2Str(value inVal);
int Val2Int(value inVal, int inDefault=0);
wxPoint Val2Point(value inVal);
wxSize Val2Size(value inVal);
wxSizer *Val2Sizer(value inVal);
bool Val2Bool(value inVal,bool inDefault=false);
wxColour Val2Colour(value inVa);
wxArrayString &Val2ArrayString(value inVal,wxArrayString &outStrings);

struct ByteData
{
   uint8 *data;
   int   length;
};
ByteData Val2ByteData(value inData);


value WXToValue(const wxPoint &inPoint);
value WXToValue(const wxSize &inSize);
value WXToValue(const int &inVal);
value WXToValue(const bool &inVal);
value WXToValue(const wxString &inStr);
value WXToValue(const wxColour &inCol);

class HaxeEventHandler : public wxEvtHandler, public wxClientData
{
public:
   HaxeEventHandler(wxWindow *inWindow,value inHandler);
   ~HaxeEventHandler();
   bool ProcessEvent(wxEvent& event);
   value GetHaxeObject() { return *mObject; }

private:
   value *mObject;
   wxWindow *mWindow;
};


DECLARE_KIND(gObjectKind)

value WXToValue(wxObject *inObj);
value WXToDeletingValue(wxObject *inObj);

template<typename TYPE>
bool ValueToWX(value inValue, TYPE *&outPointer)
{
   if (!val_is_kind(inValue,gObjectKind))
	{
		outPointer = 0;
      return false;
	}
   wxObject *obj = (wxObject *)val_data(inValue);
   outPointer = dynamic_cast<TYPE *>( obj );
   return outPointer!=0;
}

inline bool ValueToWX(value inValue, wxString &outString)
{
   outString = Val2Str(inValue);
   return true;
}


inline bool ValueToWX(value inValue, int &outInt)
{
   outInt = (int)val_number(inValue);
   return true;
}


inline bool ValueToWX(value inValue, wxSize &outSize)
{
   outSize = Val2Size(inValue);
   return true;
}


inline bool ValueToWX(value inValue, wxPoint &outPoint)
{
   outPoint = Val2Point(inValue);
   return true;
}

struct CreationParams
{
	CreationParams(value inParams,int inDefaultFlags=0);

	wxWindow *parent;
	wxWindowID id;
	wxString text;
	wxPoint  position;
	wxSize   size;
	int      flags;
};

class HaxeData : public wxClientData
{
public:
   HaxeData(value inData);
   ~HaxeData();
   value GetHaxeObject() { return *mObject; }

private:
   value *mObject;
};


#define WIN_PROPERTY(prefix,type,name,Get,Set,Transform) \
value prefix##_get_##name(value inWindow) \
{ \
	type *window; \
	if (!ValueToWX(inWindow,window)) \
		val_throw(alloc_string("Invalid Window")); \
	return  WXToValue(window->Get()); \
} \
DEFINE_PRIM(prefix##_get_##name,1) \
value prefix##_set_##name(value inWindow,value inValue) \
{ \
	type *window; \
	if (!ValueToWX(inWindow,window)) \
		val_throw(alloc_string("Invalid Window")); \
	window->Set(Transform(inValue)); \
	return alloc_null(); \
} \
DEFINE_PRIM(prefix##_set_##name,2)


#define WIN_PROPERTY_IDX(prefix,type,name,Get,Set,Transform) \
value prefix##_get_##name(value inWindow,value inIDX) \
{ \
	type *window; \
	if (!ValueToWX(inWindow,window)) \
		val_throw(alloc_string("Invalid Window")); \
	return  WXToValue(window->Get(val_int(inIDX))); \
} \
DEFINE_PRIM(prefix##_get_##name,2) \
value prefix##_set_##name(value inWindow,value inValue,value inIDX) \
{ \
	type *window; \
	if (!ValueToWX(inWindow,window)) \
		val_throw(alloc_string("Invalid Window")); \
	window->Set(val_int(inIDX),Transform(inValue)); \
	return alloc_null(); \
} \
DEFINE_PRIM(prefix##_set_##name,3)





#endif

