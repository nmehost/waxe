#ifndef SKIN_H
#define SKIN_H

#include <wx/wx.h>
#include <vector>
#include "ToolBox.h"

namespace clay
{


enum SkinColour
{
   scMedium,
   scLight,
   scDark,
   scError,
   scTitle,
   scTitleText,
   scTitleTextShadow,
   scTabOutline,
   sc3DLight,
   sc3DDark,
   scBlack,
   scSIZE,

   scBG = scMedium,
};

class Container;

struct TabInfo
{
   wxString     mTitle;
   bool         mTruncated;
   bool         mClosable;
   const wxIconBundle *mIcons;
   wxRect *mHitRect;
};

enum ButtonCommand { bcNone, bcClose, bcMenu, bcMinimise, bcMaximise, bcMAX };

struct HostedButton
{
   enum State { Normal, Hover, Down };

   HostedButton()
   {
      mFireOnDown = false;
      mState = Normal;
      mCommand = bcNone;
   }
   void Render(wxDC &inDC);


   wxRect        mRect;
   wxBitmap      mBitmap[3];
   bool          mFireOnDown;
   State         mState;
   ButtonCommand mCommand;
};

typedef std::vector<HostedButton>  HostedButtons;


typedef std::vector<TabInfo> TabInfos;


class ToolBoxSkin
{
public:
   virtual void RenderToolbarBackground(wxDC &inDC, const wxRect &inRect)=0;
   virtual void RenderToolbarSubRect(wxDC &inDC,
                   const wxRect &inRect,const wxRect &inSubRect)=0;
};


class Skin : public ToolBoxSkin
{
public:
   virtual ~Skin() { };

   virtual void DrawTitleBar(wxDC &inDC,const wxRect &inRect,wxString &inTitle)=0;
   virtual void DrawEmptyFrame(wxDC &inDC,const wxSize &inSize)=0;

   virtual void RenderBackground(wxDC &inDC,const wxRect &inRect)=0;

   virtual void LayoutNormalBorders(wxRect &ioRect, wxRect &outDragBox,
                   Container &inContainer,
                   HostedButtons &outButtons)=0;
   virtual void GetNormalDragBox(Container &inContainer,wxRect &outDragBox)=0;

   virtual void RenderNormalBorders(wxDC &inDC,const wxRect &inRect,
                   Container &inContainer, HostedButtons &inButtons)=0;

   virtual int  GetSashSize()=0;
   virtual void RenderSash(wxDC &inDC,const wxRect &inParent,int inPos,
                  bool inIsHorizontal)=0;
   virtual wxRect GetNotebookPageSize(const wxRect &inRect)=0;

   virtual void LayoutTabs(const wxRect &inParent,
                      TabInfos &inInfo, int inCurrent, int &inFirst,
                      wxRect &outDragBox,
                      wxRect &outTabDragBox,
                      HostedButtons &outButtons)=0;
   virtual void RenderTab(wxDC &inDC,const wxString &inText,const wxIcon *inIcon,
                   int inX,int inY, int inWidth, bool inSelected,
                   bool inRenderBlock)=0;
   virtual void RenderTabs(wxDC &inDC,const wxRect &inParent,
                       TabInfos &inInfo, int inCurrent, int inFirst,
                       HostedButtons &inButtons,bool inCanDrag=true)=0;

   virtual void LayoutToolbar(wxRect &ioRect,
                      wxRect &outDragBox,
                      HostedButtons &outButtons,
                      bool inOnWide)=0;
   virtual void RenderToolbar(wxDC &inDC,const wxRect &inParent,
                      wxRect &outDragBox,
                      HostedButtons &inButtons,
                      bool inOnWide)=0;

   virtual void RenderEmptyNotebook(wxDC &inDC,const wxRect &inRect)=0;
};


Skin *CreateDefaultSkin();


} // end namespace clay

#endif
