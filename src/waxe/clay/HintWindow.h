#ifndef HINT_WINDOW_H
#define HINT_WINDOW_H

#include <wx/wx.h>

#include <clay/Layout.h>

namespace clay
{
class Container;


struct HintInfo
{
   HintInfo() : mFrameContainer(0), mOverContainer(0), mDropContainer(0) { }

   // See AddPosition enum
   wxPoint     mCentrePoint;
   wxRect      mFrameRect;

   Container   *mFrameContainer;

   Container   *mOverContainer;
   wxRect      mHintRect[9];

   Container   *mDropContainer;
   AddPosition mDropWhere;
   wxRect      mDropRect;
};


class HintWindow
{
public:
   static HintWindow *Create(wxFrame *inParent);
   virtual ~HintWindow() {} 

   virtual void HideHints()=0;
   virtual void SetHintInfo(const HintInfo &inInfo,wxPoint inMouse)=0;
   virtual Container *GetTarget(wxPoint inPoint,AddPosition &outPosition)=0;
};

}


#endif
