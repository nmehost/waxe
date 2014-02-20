#include <clay/Layout.h>
#include <clay/Skin.h>
#include <clay/ToolBox.h>
#include <clay/Serializer.h>
#include "HintWindow.h"

#include <list>
#include <algorithm>


//DEFINE_EVENT_TYPE(wxEVT_GET_SIZE_FROM_WIDTH)
//DEFINE_EVENT_TYPE(wxEVT_GET_SIZE_FROM_HEIGHT)

//#define DEBUG_COLOUR


namespace clay
{

Serializer s;

static wxRect RectClientToScreen(wxWindow *inParent,const wxRect &inRect)
{
   wxPoint p0 = inParent->ClientToScreen( inRect.GetTopLeft() );
   wxPoint p1 = inParent->ClientToScreen( inRect.GetBottomRight() );
   return wxRect(p0,p1);
}


static int sUID = 1;
std::list<int> sFreeUID;

void Container::CriticalError()
{
   #ifndef __has_builtin
   #define __has_builtin(x) 0
   #endif

    #if __has_builtin(__builtin_trap)
    __builtin_trap();
    #else
    (* (int *) 0) = 0;
    #endif
}

Container::Container(Manager *inManager) : mManager(inManager)
{
    if (sFreeUID.empty())
       mUID = sUID++;
    else
    {
       mUID = *sFreeUID.begin();
       sFreeUID.pop_front();
    }
}

Container::~Container()
{
   sFreeUID.push_back(mUID);
}

void Container::GetDockPos(Container *inDrop, int inDirection, wxRect &outDock)
{
   wxWindow *p = AsParent();
   if (!p)
   {
      outDock = wxRect();
      return;
   }

   Container *parent = GetParent();

   // Check to see if direction matches parents grouping.
   // Or in the case of non-page-mode, any of the ancestors match.
   if (parent && parent->GetSiblingDock(inDrop,this,
            inDirection, outDock) )
      return;

   wxRect r = Rect();
   switch(inDirection)
   {
      case apBelow:
      case apAbove:
         r.height/=2;
         if (inDirection==apBelow)
            r.y += r.height;
         break;
      case apLeft:
      case apRight:
         r.width/=2;
         if (inDirection == apRight)
            r.x += r.width;
         break;
   }

   outDock = RectClientToScreen(p,r);
}


wxSize Container::GetSize(SizeType inType)
{
   return mSizes[inType];
}

   
void Container::RecordSize(SizeType inType,wxRect inRect)
{
   if (inRect.width==0 && inRect.height==0)
      inRect = Rect();

   mSizes[inType] = wxSize(inRect.width,inRect.height);
   if ((inType==stWideDock || inType==stTallDock))
   {
      mSizes[stFloating] == mSizes[inType];
   }
}



bool Container::HintsFromMouse(HintInfo &outHints,Container *inDrop,
                   wxPoint inPos)
{
   // Convert rect to screen co-ordinates...
   wxWindow *p = AsParent();
   if (!p || !Visible())
      return false;

   wxRect screen(RectClientToScreen(p,Rect()));

   if (screen.Contains(inPos))
   {
      outHints.mCentrePoint.x = screen.x+screen.width/2;
      outHints.mCentrePoint.y = screen.y+screen.height/2;

      outHints.mOverContainer = this;

      for(int i=0;i<5;i++)
         GetDockPos(inDrop,i,outHints.mHintRect[i]);

      return true;
   }

   return false;
}

void Container::CopySizeInfo(Container *inSource)
{
   RecordSize(stFloating, inSource->GetSize(stFloating));
   RecordSize(stWideDock, inSource->GetSize(stWideDock));
   RecordSize(stTallDock, inSource->GetSize(stTallDock));
   RecordSize(stFloating, inSource->GetSize(stFloating));
   SetRect( inSource->Rect() );
}

void Container::ToSerial(Serializer &outSerializer) const
{
   outSerializer["float"]["w"] = mSizes[stFloating].x;
   outSerializer["float"]["h"] = mSizes[stFloating].y;
   outSerializer["wide"]["w"] = mSizes[stWideDock].x;
   outSerializer["wide"]["h"] = mSizes[stWideDock].y;
   outSerializer["tall"]["w"] = mSizes[stTallDock].x;
   outSerializer["tall"]["h"] = mSizes[stTallDock].y;
}

void Container::FromSerial(Serializer &inSerializer)
{
   mSizes[stFloating].x = inSerializer["float"]["w"];
   mSizes[stFloating].y = inSerializer["float"]["h"];
   mSizes[stWideDock].x = inSerializer["wide"]["w"];
   mSizes[stWideDock].y = inSerializer["wide"]["h"];
   mSizes[stTallDock].x = inSerializer["tall"]["w"];
   mSizes[stTallDock].y = inSerializer["tall"]["h"];
}



Container *Container::Add(Container *inContainer,AddPosition inWhere)
{
   Container *parent = GetParent();
   if (!parent)
      CriticalError();

   if (parent->CanAddChild(inWhere))
      return parent->Insert(inContainer,this,inWhere);

   Container *new_child = 0;
   bool wide;

   switch(inWhere)
   {
      case apLeft:
      case apRight:
         wide = parent->Rect().width > Rect().height;
         new_child = mManager->Create(csHStack,wide ? stWideDock : stTallDock);
         break;
      case apAbove:
      case apBelow:
         wide = parent->Rect().height < Rect().width;
         new_child = mManager->Create(csVStack,wide ? stWideDock : stTallDock);
         break;
      case apOver:
         {
         Container *client = FindClientContainer();
         if (client)
            return client->Add(inContainer,inWhere);
         new_child = mManager->Create(csNotebook);
         }
   }

   wxRect r = Rect();
   new_child->CopySizeInfo(this);

   parent->ReplaceChild(this,new_child);

   new_child->Add(this,inWhere);
   new_child->Add(inContainer,inWhere);

   if (inWhere==apLeft || inWhere==apRight)
      new_child->SetHeight(r.height,wide?stWideDock:stTallDock);
   else if (inWhere==apAbove || inWhere==apOver)
      new_child->SetWidth(r.width,wide?stWideDock:stTallDock);

   parent->OnChildChange(ccfWindow|ccfChildAdded,new_child);

   return inContainer;
}

Container *Container::Add(wxWindow *inWindow,AddPosition inWhere,
                    wxIconBundle inIcons,
                    unsigned int inFlags,
                    WindowStyle inStyle)
{
   return Add(mManager->Create(inWindow,inStyle,inIcons,inFlags),inWhere);
}

Container *Container::Add(ToolBox *inToolBox,AddPosition inWhere)
{
   return Add(mManager->Create(inToolBox),inWhere);
}




// ---


/*
  The frame container sits inside the top-level frame and passes on events
   to its children.  It snoops the frame for resize and paint events
*/

class FrameContainer;
class CursorWindow : public wxWindow
{
public:
   CursorWindow(wxWindow *inParent,FrameContainer *inC) : wxWindow(inParent,-1), mContainer(inC) { }

   void OnPaint(wxPaintEvent &inEvt);
   void OnErase(wxEraseEvent &inEvt) { }

   FrameContainer *mContainer;

   DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(CursorWindow,wxWindow)
   EVT_PAINT(CursorWindow::OnPaint)
   EVT_ERASE_BACKGROUND(CursorWindow::OnErase)
END_EVENT_TABLE()



class FrameContainer : public wxEvtHandler, public Container
{
public:
   FrameContainer(Manager *inManager,wxFrame *inFrame,bool inFloating=false)
      : Container(inManager)
   {
      mFrame = inFrame;
      mFrame->PushEventHandler(this);
      mChild = 0;
      mIsFloating = inFloating;

      #ifdef HX_WINDOWS
      mCursorWindow = 0;
      #else
      mCursorWindow = new CursorWindow(mFrame,this);
      #endif
   }

   ContainerStyle GetStyle() { return csFrame; } 

   bool Visible() { return true; }

   ~FrameContainer()
   {
      mFrame->RemoveEventHandler(this);
      delete mChild;
   }

   Container *GetOnlyChild()
   {
       return mChild;
   }

   bool ShouldFloatFullFrame() const
      { return mChild && mChild->ShouldFloatFullFrame(); }

   void ToSerial(Serializer &outSerializer) const
   {
      Container::ToSerial(outSerializer);
      clay::ToSerial(mFrame,outSerializer["frame"]);
      outSerializer["type"] = "frame";
      outSerializer["floating"] = mIsFloating;
      outSerializer["fullframe"] = ShouldFloatFullFrame();
      if (mChild)
         mChild->ToSerial(outSerializer["child"]);
   }

   void FromSerial(Serializer &inSerializer)
   {
      Container::FromSerial(inSerializer);
      if (inSerializer.HasKey("child"))
      {
         Container *c = mManager->Create(inSerializer["child"]);
         if (c)
            Add(c,apOver);
      }
   }

   bool GetPreferredSize(wxSize &ioSize,bool inFromWidth)
   {
      if (!mChild)
         return false;
      return mChild->GetPreferredSize(ioSize,inFromWidth);
   }

   wxString GetCaption()
   {
      if (mChild)
         return mChild->GetCaption();
      return "?";
   }

   void Raise()
   {
      if (!mFrame->IsMaximized())
         mFrame->Maximize(false);
      mFrame->Show(true);
      mFrame->Raise();
   }

   void RaiseChild(Container *inContainer)
   {
      Raise();
   }


   // FrameContainer
   void OnChildChange(unsigned int inFlags,Container *inChild) 
   {
      if ( mChild && mIsFloating )
      {
         mFrame->SetTitle(mChild->GetCaption());
         const wxIconBundle *icons = mChild->GetIconSet();
         if (icons)
            mFrame->SetIcons(*icons);
          else
            mFrame->SetIcon(wxIcon());
      }

      if ( mChild && mFrame && (inFlags & ccfMinSize) )
      {
         wxSize min = mChild->GetSize(stMin);
         mFrame->SetMinSize(min);
      }

      if (mChild && mFrame)
      {
         unsigned int flags = mFrame->GetWindowStyle();
         bool closable = mChild->CanClose();
         if (closable != (( flags & wxCLOSE_BOX)!=0) )
         {
            // This does not seem to work ?
            if (closable)
               flags |= wxCLOSE_BOX;
            else
               flags &= ~wxCLOSE_BOX;
            mFrame->SetWindowStyle(flags);
         }
      }
   }


   bool ShowsChildsTitle() { return mIsFloating; }

   wxWindow *AsParent() { return mCursorWindow ? mCursorWindow : mFrame; }

   bool CanAddChild(AddPosition inWhere) { return mChild==0; }

   bool CanClose() { return !mChild || mChild->CanClose(); }

   void DestroyOrClose()
   {
      if (mChild)
         mChild->DestroyOrClose();
      //mFrame->Destroy();
      delete this;
   }

   Container *FindClientContainer() { return mChild?mChild->FindClientContainer():0; }


   // FrameContainer
   Container *Add(Container *inContainer, AddPosition inWhere)
   {
      if (!mChild)
      {
         mChild = inContainer;
         inContainer->Reparent(this);
      }
      else
      {
         Container *client = 0;
         if (inWhere==apOver)
             client = FindClientContainer();
         if (client)
         {
            return client->Add(inContainer,inWhere);
         }
         else if (mChild->CanAddChild(inWhere))
         {
            mChild->Add(inContainer,inWhere);
         }
         else
         {
            Container *new_child = 0;
            switch(inWhere)
            {
               case apLeft:
               case apRight:
                  new_child = mManager->Create(csHStack);
                  break;
               case apAbove:
               case apBelow:
                  new_child = mManager->Create(csVStack);
                  break;
               case apOver:
                  new_child = mManager->Create(csNotebook);
            }
         
            new_child->CopySizeInfo(mChild);
         
            new_child->Add(mChild,inWhere);
            new_child->Add(inContainer,inWhere);
            new_child->Reparent(this);
            mChild = new_child;
         }
      }

      OnChildChange(ccfCaption|ccfMinSize|ccfChildAdded,this);
      return inContainer;
   }


   Container *Insert(Container *inContainer,Container *inSibling,
                         AddPosition inWhere)
   {
      if (mChild!=0)
         CriticalError();
      return Add(inContainer,inWhere);
   }


   // Frame Container
   bool HintsFromMouse(HintInfo &outHints,Container *inDrop,
                   wxPoint inPos)
   {
      if ( mFrame->GetRect().Contains(inPos) )
      {
         outHints.mFrameRect = RectClientToScreen(mFrame,Rect());
         outHints.mFrameContainer = mChild;

         if (mChild)
            for(int i=0;i<4;i++)
               mChild->GetDockPos(inDrop,i,outHints.mHintRect[i+5]);
      }

      if (mChild)
         return mChild->HintsFromMouse(outHints,inDrop,inPos);

      return Container::HintsFromMouse(outHints,inDrop,inPos);
   }


   virtual void ReplaceChild(Container *inContainer,Container *inNew)
   {
      if (mChild==inContainer)
      {
         mChild = inNew;
         if (mChild)
         {
            mChild->Reparent(this);
            mChild->SetRect( Rect() );
         }
         else
         {
            if (mIsFloating)
               mManager->CloseFloating(mFrame,this);
            else
               mFrame->Refresh();
         }
      }
   }


   // Frame Container
   virtual bool CanAddChild()  { return true; }


   void Reparent(Container *inContainer) { CriticalError(); }
   Container *GetParent() { return 0; }

   void Show(bool inShow) { CriticalError(); }
   void SetRect(const wxRect &inRect) { mFrame->SetSize(inRect); }
   wxRect Rect() { return wxRect(wxPoint(0,0),mFrame->GetClientSize());}


   void Render(wxDC &inDC,Skin *inSkin)
   {
      if (mChild)
         mChild->Render(inDC,inSkin);
      else
         inSkin->DrawEmptyFrame(inDC, mFrame->GetClientSize());
   }

   void OnParentSize(wxSizeEvent &inEvt)
   {
      if (mCursorWindow)
         mCursorWindow->SetSize( mFrame->GetClientSize() );
      if (mChild)
      {
         mChild->SetRect( Rect() );
         mChild->RecordSize(stFloating);
      }
      // Don't skip so we can avoid msw mdi moving our client window
      //inEvt.Skip();
   }

   // Frame Container
   void DoLayout()
   {
      if (mChild)
      {
         mChild->SetRect( Rect() );
         mChild->RecordSize(stFloating);
      }
   }

   void OnParentPaint(wxPaintEvent &inEvt)
   {
      wxPaintDC dc(mFrame);
      // Take toolbars into account ...
      wxPoint pt = mFrame->GetClientAreaOrigin();
      dc.SetDeviceOrigin(pt.x,pt.y);

      Render(dc,mManager->GetSkin());
   }
   void OnCursorWindowPaint(wxPaintEvent &inEvent)
   {
      wxPaintDC dc(mCursorWindow);
      Render(dc,mManager->GetSkin());
   }
   void OnErase(wxEraseEvent &inEvt) { }


   wxWindow  *mCursorWindow;

   Container *mChild;
   wxFrame *mFrame;
   bool     mIsFloating;

   DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(FrameContainer,wxEvtHandler)
   EVT_SIZE(FrameContainer::OnParentSize)
   EVT_PAINT(FrameContainer::OnParentPaint)
   EVT_ERASE_BACKGROUND(FrameContainer::OnErase)
END_EVENT_TABLE()

void CursorWindow::OnPaint(wxPaintEvent &inEvt) { mContainer->OnCursorWindowPaint(inEvt); }

Container *Manager::Create(wxFrame *inFrame,bool inIsFloating)
{
   return new FrameContainer(this,inFrame,inIsFloating);
}


// --- HostedContainer ------------------------------------
/*
   The HostedContainer lives inside a "real" window and snoops some of
    its events and passes them on to child processes and buttons
*/
class HostedContainer : public wxEvtHandler, public Container
{
public:
   HostedContainer(Manager *inManager) : Container(inManager)
   {
      mSnoopWindow = 0;
      mDragOffing = -1;
      mParent = 0;
      mHoverButton = -1;
      mButtonCapture = -1;
   }


   ~HostedContainer()
   {
      if (mSnoopWindow)
         mSnoopWindow->RemoveEventHandler(this);
   }

   Container *GetParent() { return mParent; }

   bool ContainsVisPoint(wxPoint inPos)
   {
      wxWindow *p = AsParent();
      if (!p || !Visible())
         return false;
      wxRect screen(RectClientToScreen(p,Rect()));
      if (!screen.Contains(inPos))
         return false;

      return true;
   }



   void Reparent(Container *inParent)
   {
      wxWindow *window = inParent ? inParent->AsParent() : 0;

      if (mSnoopWindow!=window)
      {
         if (mSnoopWindow)
            mSnoopWindow->RemoveEventHandler(this);
         mSnoopWindow = window;
         if (mSnoopWindow)
            mSnoopWindow->PushEventHandler(this);
      }


      if (mParent!=inParent)
      {
         Container *old_parent = mParent;
         mParent = inParent;
         if (old_parent)
            old_parent->ReplaceChild(this,0);
      }
   }

   void Raise()
   {
      if (mParent)
         mParent->RaiseChild(this);
   }


   wxWindow *AsParent() { return mParent ? mParent->AsParent() : 0; }

   Manager *GetManager() { return mParent ? mParent->GetManager() : 0; }

   virtual bool CanRemove() const { return true; }
   bool DoCanRemove() const { return CanRemove(); }

   void OnChildChange(unsigned int inFlags,Container *inChild) 
   {
      if (mParent)
         mParent->OnChildChange(inFlags,this);
   }

   virtual void DoMouseMove(wxMouseEvent &inEvent)
   {
      if (IgnoreMouse(inEvent))
         return;

      wxPoint mouse = inEvent.GetPosition();
      if (ButtonTest(inEvent))
      {
         if (mSnoopWindow)
            mSnoopWindow->SetCursor(0);
      }
      else if (mSnoopWindow && (mDragOff[0].Contains(mouse) && CanRemove() ||
                           mDragOff[1].Contains(mouse) ) )
      {
         mSnoopWindow->SetCursor( wxCURSOR_SIZING );
      }
      else if (mSnoopWindow)
         mSnoopWindow->SetCursor(0);

   }

   virtual void ButtonClick(ButtonCommand inCommand)
   {
      if (inCommand==bcClose)
      {
         GetCurrentContainer(1)->DestroyOrClose();
      }
   }


   bool ButtonTest(wxMouseEvent &inEvent)
   {
      wxPoint pos = inEvent.GetPosition();

      int old = mHoverButton;
      bool result = false;

      mHoverButton = -1;
      for(int i=0;i<mButtons.size();i++)
      {
         HostedButton &but = mButtons[i];
         if (but.mRect.Contains(pos))
         {
            mHoverButton = i;
            result = true;
            but.mState = HostedButton::Hover;
         }
         else
            but.mState = HostedButton::Normal;
      }

      if (mHoverButton!=old && mSnoopWindow!=0)
         mSnoopWindow->Refresh();

      return result;
   }


   void ButtonRelease()
   {
      if (mHoverButton>=0 && mSnoopWindow!=0)
         mSnoopWindow->Refresh();
      for(int b=0;b<mButtons.size();b++)
         mButtons[b].mState = HostedButton::Normal;
   }

   virtual void DoLeftDown(wxMouseEvent &inEvent)
   {
      wxPoint mouse = inEvent.GetPosition();

      for(int b=0;b<mButtons.size();b++)
      {
         HostedButton &but = mButtons[b];
         if (but.mRect.Contains(mouse))
         {
            if (but.mFireOnDown)
            {
               ButtonClick(but.mCommand);
            }
            else
            {
               mButtonCapture = b;
               but.mState = HostedButton::Down;
               DoCapture();
               if (mSnoopWindow)
               {
                  mSnoopWindow->SetCursor( 0 );
               }
            }
            return;
         }
      }

      if (mDragOff[0].Contains(mouse) && CanRemove() )
      {
         DoCapture();
         mDragOffing = 0;
         mDragOffStart = inEvent.GetPosition();
         mSnoopWindow->SetCursor( wxCURSOR_SIZING );
      }
      else if (mDragOff[1].Contains(mouse) )
      {
         DoCapture();
         mDragOffing = 1;
         mDragOffStart = inEvent.GetPosition();
         mSnoopWindow->SetCursor( wxCURSOR_SIZING );
      }
      else
         inEvent.Skip();
   }

   virtual void DoLeftUp(wxMouseEvent &inEvent)
   {
      if (mButtonCapture>=0)
      {
         DoRelease();
         if (mButtonCapture<mButtons.size())
         {
            HostedButton &but = mButtons[mButtonCapture];
            mButtonCapture = -1;
            but.mState = HostedButton::Normal;
            if (but.mRect.Contains(inEvent.GetPosition()))
            {
               // Click !
               ButtonClick(but.mCommand);
            }
         }
         else
            mButtonCapture = -1;
      }
      else if (mDragOffing>=0)
      {
         DoRelease();
         mDragOffing = -1;
         mSnoopWindow->SetCursor(0);
      }
      else
         inEvent.Skip();
   }

   virtual Container *GetCurrentContainer(int inIndex) = 0;

   void OnLeftDown(wxMouseEvent &inEvent)
   {
      if (!Visible())
      {
         inEvent.Skip();
         return;
      }
      DoLeftDown(inEvent);
   }

   void OnLeftUp(wxMouseEvent &inEvent)
   {
      if (!Visible())
      {
         inEvent.Skip();
         return;
      }
      DoLeftUp(inEvent);
   }


   void OnMouseMove(wxMouseEvent &inEvent)
   {
      if (!Visible())
      {
         inEvent.Skip();
         return;
      }
      if (mButtonCapture>=0)
      {
         if (mButtonCapture>=mButtons.size())
         {
            DoRelease();
            mButtonCapture = -1;
         }
         else
         {
            HostedButton &but = mButtons[mButtonCapture];
            but.mState = but.mRect.Contains(inEvent.GetPosition()) ?
                HostedButton::Down : HostedButton::Normal;
         }
      }
      else if (mDragOffing>=0)
      {
         if (!mDragOff[mDragOffing].Contains(inEvent.GetPosition()))
         {
            DoRelease();
            int do_float = mDragOffing;
            mDragOffing = -1;
            mSnoopWindow->SetCursor(0);
            Container *c = GetCurrentContainer(do_float);
            c->RecordSize(stFloating,c->Rect());
            GetManager()->FloatContainer( GetCurrentContainer(do_float) );
         }
         return;
      }
      DoMouseMove(inEvent);
      inEvent.Skip();
   }

   void DoCapture()
   {
      if (mSnoopWindow)
      {
        mSnoopWindow->CaptureMouse();
        mHostedCapture = this;
      }
   }

   void DoRelease()
   {
      if (mHostedCapture==this)
      {
         if (mSnoopWindow)
            mSnoopWindow->ReleaseMouse();
         mHostedCapture = 0;
      }
   }

   bool IgnoreMouse(wxMouseEvent &inEvt)
   {
      if (!Visible())
      {
         inEvt.Skip();
         return true;
      }

      if ( mHostedCapture == this )
         return false;

      if ( mHostedCapture && mHostedCapture!=this )
      {
         inEvt.Skip();
         return true;
      }

      if (!Rect().Contains(inEvt.GetPosition()))
      {
         inEvt.Skip();
         return true;
      }

      if (ChildContains(inEvt.GetPosition()) )
      {
         inEvt.Skip();
         return true;
      }
      return false;
   }

   virtual void DoLeave()
   {
      ButtonRelease();
      // TODO : this does not seem to work on wxMac
      if (mSnoopWindow)
         mSnoopWindow->SetCursor(0);
   }

   void OnLeave(wxMouseEvent &inEvent)
   {
      DoLeave();
      inEvent.Skip();
   }



   // HostedContainer
   void SetRect(const wxRect &inRect)
   {
       mRect =  inRect;
       DoLayout();
   }

   wxRect Rect() { return mRect; }

   
   wxRect   mRect;
   Container *mParent;
   wxWindow  *mSnoopWindow;
   int       mDragOffing;
   wxPoint   mDragOffStart;
   wxRect    mDragOff[2];

   static HostedContainer *mHostedCapture;

   int            mHoverButton;
   int            mButtonCapture;
   HostedButtons  mButtons;


   DECLARE_EVENT_TABLE();
};

HostedContainer *HostedContainer::mHostedCapture = 0;


BEGIN_EVENT_TABLE(HostedContainer,wxEvtHandler)
   EVT_LEFT_DOWN(HostedContainer::OnLeftDown)
   EVT_LEFT_UP(HostedContainer::OnLeftUp)
   EVT_LEAVE_WINDOW(HostedContainer::OnLeave)
   EVT_MOTION(HostedContainer::OnMouseMove)
END_EVENT_TABLE()


// --- WindowContainer -----------------------------------------
/*
   The window container hosts a window, and will draw a title-bar if
    the parent container declines to do so.
*/



class WindowContainer : public HostedContainer
{
public:
   WindowContainer(Manager *inManager,wxWindow *inWindow,WindowStyle inStyle,
      wxIconBundle inIcons,
      unsigned int inFlags) : HostedContainer(inManager), mIcons(inIcons)
   {
      mToolBox = 0;
      mWindow = inWindow;
      mStyle = inStyle;
      mOnWideDock = true;
      mFlags = inFlags;
      Init();
   }
   ~WindowContainer()
   {
      mManager->RemoveWindow(mWindow);
   }

   WindowContainer(Manager *inManager,ToolBox *inToolBox) :
       HostedContainer(inManager)
   {
      mToolBox = inToolBox;
      mWindow = inToolBox;
      mStyle = wsToolbar;
      mFlags  = 0;
      Init();
   }

   void ToSerial(Serializer &outSerializer) const
   {
      Container::ToSerial(outSerializer);
      outSerializer["type"] = "window";
      outSerializer["onwide"] = (int)mOnWideDock;
      outSerializer["name"] = mWindow->GetName();
      outSerializer["id"] = mWindow->GetId();
   }

   void FromSerial(Serializer &inSerializer)
   {
      Container::FromSerial(inSerializer);
      mOnWideDock = (int)inSerializer["onwide"];
   }


   void DestroyOrClose()
   {
      if ( mFlags & cfDestroyOnClose )
      {
         mManager->RemoveWindow(mWindow);
         delete mWindow;
         mWindow = 0;
         mParent->ReplaceChild(this,0);
         delete this;
      }
      else
      {
         GetManager()->Park(this);
      }
   }

   bool ShouldFloatFullFrame() const { return mFlags & cfFloatFullFrame; }

   bool CanClose() { return !(mFlags & cfCantClose); }

   bool ShouldClose()
   {
      wxCloseEvent close;
      close.SetCanVeto(true);
      mWindow->ProcessWindowEvent(close);
      return !close.GetVeto();
   }


   void Init()
   {
      wxSize s = mWindow->GetBestSize();
      wxSize m = mWindow->GetMinSize();
      if (s.x<m.x) s.x = m.x;
      if (s.y<m.y) s.y = m.y;

      if (s.x<2 || s.y<2)
         s = wxSize(40,40);

      // The "mSizes" array does not include the borders...
      mBorderSize = wxSize();
      for(int i=0;i<stSIZE;i++)
         mSizes[i] = s;
      mSizes[stMin] = m;
      mFixed = false;
      if (mToolBox)
      {
         mSizes[stTallDock] =  wxSize(1,1);
         mToolBox->DoLayout(mSizes[stTallDock],true);
      }
      mManager->RecordWindow(mWindow,this);
   }

   bool IsFixed(bool inHorizontal,wxSize &ioSize)
   {
       if (mFixed)
          ioSize= mFixedSize + mBorderSize;
       return mFixed;
   }

   bool GetPreferredSize(wxSize &ioSize,bool inFromWidth)
   {
      if (!mToolBox)
         return false;
      ioSize -= mBorderSize;
      mToolBox->DoLayout(ioSize,inFromWidth);
      ioSize += mBorderSize;
      return true;
   }

   void SetWidth(int inWidth,SizeType inSizer)
   {
      if (!mToolBox)
         return;
      inWidth -= mBorderSize.x;
      wxSize s(inWidth,1);

      mToolBox->DoLayout(s,true);

      mSizes[inSizer] = s;
      mFixed = true;
      mFixedSize = s;
      DoLayout();
   }

   void SetHeight(int inHeight,SizeType inSizer)
   {
      if (!mToolBox)
         return;
      inHeight -= mBorderSize.y;
      wxSize s(0xffff,inHeight);

      mToolBox->DoLayout(s,false);

      mSizes[inSizer] = s;
      mFixed = true;
      mFixedSize = s;
      DoLayout();
   }




   wxString GetCaption() { return mWindow->GetName(); }

   const wxIconBundle *GetIconSet()
   {
      return &mIcons;
   }

   bool Visible()
   {
      if (!mWindow)
         return false;
      if (! mWindow->IsShown())
         return false;
      return true;
   }

   ContainerStyle GetStyle() { return csWindow; }

   wxSize GetSize(SizeType inType)
   {
      //if (mFixed && inType==stMin)
         //return mFixedSize + mBorderSize;

      return mSizes[inType] + mBorderSize;
   }

   void RecordSize(SizeType inType,wxRect inRect)
   {
      if (inRect.width==0 && inRect.height==0)
         inRect = Rect();
      inRect.width -= mBorderSize.x;
      inRect.height -= mBorderSize.y;

      Container::RecordSize(inType,inRect);
   }


   void ReplaceChild(Container *inContainer,Container *inNew)
   {
      CriticalError();
   }

   Container *Insert(Container *inContainer,Container *inSibling,
                         AddPosition inWhere)
   {
      CriticalError();
      return 0;
   }


   void Reparent(Container *inContainer)
   {
      HostedContainer::Reparent(inContainer);
      wxWindow *parent = AsParent();
      mOnWideDock = !inContainer || inContainer->IsWideDock();
      if (parent)
         mWindow->Reparent(parent);
   }

   void Show(bool inShow) { mWindow->Show(inShow); }

   wxWindow *GetWindow() { return mWindow; }

   void Render(wxDC &inDC,Skin *inSkin)
   {
      if (!mWindow || !mWindow->IsShown())
         return;

      if (mParent && mParent->ShowsChildsTitle())
      {
         // Do nothing
      }
      else if (mStyle==wsNormal)
      {
            inSkin->RenderNormalBorders(inDC,mDragOff[0],*this,mButtons);
      }
      else
      {
         inSkin->RenderToolbar(inDC,Rect(),mDragOff[0],mButtons,mOnWideDock);
      }
   }

   Container *GetCurrentContainer(int inIndex) { return this; }


   // Window Container
   void DoLayout()
   {
      wxRect r = mRect;
      wxRect orect = r;


      mDragOff[0] = wxRect();
      Skin *skin = Manager::GetSkin();

      if (mParent && !mParent->ShowsChildsTitle())
      {
         if (mStyle==wsNormal)
         {
            skin->LayoutNormalBorders(r,mDragOff[0],*this,mButtons);
         }
         else
         {
            skin->LayoutToolbar(r,mDragOff[0],mButtons,mOnWideDock);
         }
      }

      mBorderSize.x = orect.width - r.width;
      mBorderSize.y = orect.height - r.height;

      if (mParent)
      {
         wxRect p = mParent->Rect();
         int over_x =  r.GetRight() - p.GetRight();
         if (over_x>0)
            r.width -= over_x;

         int over_y =  r.GetBottom() - p.GetBottom();
         if (over_y>0)
            r.height -= over_y;
      }

      if (r.width<1 || r.height<1)
      {
         mWindow->Show(false);
      }
      else
      {
         if (!mWindow->IsShown())
            mWindow->Show(true);
         if (mWindow->GetRect()!=r)
         {
            mWindow->SetSize(r);
         }
      }
   }

   wxIconBundle mIcons;
   wxSize      mBorderSize;
   WindowStyle mStyle;
   wxWindow    *mWindow;
   ToolBox     *mToolBox;
   bool        mFixed;
   bool        mOnWideDock;
   unsigned int mFlags;
   wxSize      mFixedSize;
};


Container *Manager::Create(wxWindow *inWindow,WindowStyle inStyle,
               wxIconBundle inIcons,
               unsigned int inFlags)
   { return new WindowContainer(this,inWindow,inStyle,inIcons,inFlags); }

Container *Manager::Create(ToolBox *inToolBox)
   { return new WindowContainer(this,inToolBox); }



// --- MultiChildParent ------------------------------------------------
// Some helpers for dealing with many children

template<typename CHILD_>
class MultiChildParent : public HostedContainer
{
public:
   typedef std::vector<CHILD_ *> ChildList;

   MultiChildParent(Manager *inManager) : HostedContainer(inManager)
   {
      mVisible = true;
   }
   ~MultiChildParent()
   {
      for(int c=0;c<mChildren.size();c++)
         delete mChildren[c];
   }

   bool Visible() { return mVisible; }

   bool HintsFromMouse(HintInfo &outHints, Container *inDrop, wxPoint inPos)
   {
      if (!ContainsVisPoint(inPos))
         return false;

      if (mChildren.empty())
         return HostedContainer::HintsFromMouse(outHints,inDrop,inPos);

      for(int c=0;c<mChildren.size();c++)
         if (mChildren[c]->Visible() && 
             mChildren[c]->HintsFromMouse(outHints,inDrop,inPos))
               return true;

      return false;
      //return Container::HintsFromMouse(outHints,inPos,inPageMode);
   }

   wxSize GetSize(SizeType inType)
   {
      if (inType==stMin)
         return mMinSize;
      return HostedContainer::GetSize(inType);
   }

   bool CanClose()
   {
      for(int i=0;i<mChildren.size();i++)
         if (!mChildren[i]->CanClose())
            return false;
      return true;
   }

   bool ShouldClose()
   {
      for(int i=0;i<mChildren.size();i++)
         if (!mChildren[i]->ShouldClose())
            return false;
      return true;
   }


   void Reparent(Container *inParent)
   {
      HostedContainer::Reparent(inParent);
      for(int i=0;i<mChildren.size();i++)
      {
         Container *child = mChildren[i];
         child->Reparent(this);
      }
   }


   Container *FindClientContainer()
   {
      for(int i=0;i<mChildren.size();i++)
      {
         Container *c = mChildren[i]->FindClientContainer();
         if (c) return c;
      }
      return 0;
   }


   void DestroyOrClose()
   {
      int n = mChildren.size();
      for(int i=n-1;i>1;i--)
         mChildren[i]->DestroyOrClose();
   }


   void Show(bool inShow)
   {
      mVisible = inShow;
      for(int i=0;i<mChildren.size();i++)
      {
         Container *child = mChildren[i];
         child->Show(inShow);
      }
      if (inShow)
         DoLayout();
   }

   void Render(wxDC &inDC,Skin *inSkin)
   {
      if (!mVisible || !mParent)
         return;

      RenderDecos(inDC,inSkin);

      for(int c=0;c<mChildren.size();c++)
         if (mChildren[c]->Visible())
            mChildren[c]->Render(inDC,inSkin);
   }

   bool ChildContains(wxPoint inPos)
   {
       for(int i=0;i<mChildren.size();i++)
          if (mChildren[i]->Rect().Contains(inPos))
             return true;
       return false;
   }




   virtual void OnSingleChild()
   {
      if (mChildren.size()==1)
      {
         Container *child = mChildren[0];
         child->Show(true);
         mChildren.resize(0);
         child->CopySizeInfo(this);

         mParent->ReplaceChild(this,child);
         child->GetParent()->OnChildChange(ccfWindow,child);
      }
      else
      {
         mParent->ReplaceChild(this,0);
      }
      delete this;
   }

   virtual void DoOverwriteChild(int inI, Container *inNew) =0;

   virtual void UpdateMinSize() = 0;

   virtual void SetChildDockedSizes() { }

   // MultiChildParent
   void ReplaceChild(Container *inContainer,Container *inNew)
   {
      SetChildDockedSizes();
      for(int i=0;i<mChildren.size();i++)
      {
         if (mChildren[i]==inContainer)
         {
            if (inNew==0)
            {
               mChildren.erase( mChildren.begin() + i );
               if (mChildren.size()<2)
               {
                  OnSingleChild();
                  return;
               }
            }
            else
            {
               inNew->Reparent(this);
               Container *detach = mChildren[i];
               DoOverwriteChild(i,inNew);
               detach->Reparent(0);
               break;
            }
         }
      }

      DoLayout();
      UpdateMinSize();
      if (mSnoopWindow)
         mSnoopWindow->Refresh();
   }


   virtual void RenderDecos(wxDC &inDC,Skin *inSkin) { }

   bool      mVisible;
   ChildList mChildren;
   wxSize    mMinSize;
};

// --- RowContainer ------------------------------------------------
// This will be either horizontal OR vertical ...





 /*

  Given a list of containers, and a direction (horizontal/vertical), and
   a target rectangle, what size should then be made?

  Each item starts with a nominal rectangle - they will be resized in
   proportion to this size.

  First, any fixed sizes are taken care of, using the "GetPreferredSize" call.
  This may also fix the transverse direction.

  The remainder is then tallied against the available size and divided up
   proportionally.  If any control hits its minimum size, then it is set
   at its minimum size, and removed from the available list, and the
   process starts again.

*/


int GetDimension(const wxSize &inS,bool inHorizontal)
 { return inHorizontal ? inS.x : inS.y; }
int GetDimension(const wxRect &inR,bool inHorizontal)
 { return inHorizontal ? inR.width : inR.height; }

#define DIM(s) GetDimension(s,inHorizontal)

#define SDIM(s,val) wxSize(inHorizontal?val:s.x, inHorizontal?s.y:val)

struct Item
{
   Item(int i,int r,int s) : mIndex(i), mRatio(r), mSize(s)
   {
      mTargetSize = mSize;
      if (mRatio<1) mRatio = 1;
   }

   int mIndex;
   int mSize;
   int mTargetSize;
   int mMin;
   int mRatio;
};

typedef std::list<Item> ItemList;

void LayoutContainerSizes(ContainerList &inList,
       std::vector<wxSize> &outSizes,
       wxSize inSize,
       SizeType inShape,
       bool inHorizontal)
{
   int n = inList.size();
   outSizes.resize(n);
   if (!n) return;

   if (n==1)
   {
      outSizes[0] = inList[0]->GetSize(inShape);
      return;
   }


   int gaps = n-1;
   int sash_size = Manager::GetSkin()->GetSashSize();
   int size = DIM(inSize);
   int available = size - gaps*sash_size;

   // Set list of fixed-size ones, and find ratios for other ones ...
   int total_ratio = 0;
   int total_size = 0;

   ItemList stretchable;
   for(int i=0;i<n;i++)
   {
      // No change, fixed...
      wxSize fixed(inSize);
      if (inList[i]->IsFixed(inHorizontal,fixed))
      {
         outSizes[i] = fixed;
         available -= DIM(fixed);
      }
      else
      {
         Item item(i, DIM( inList[i]->GetSize(inShape) ),
                      DIM( inList[i]->GetSize(inShape) ));
         item.mMin =  DIM( inList[i]->GetSize(stMin) );
         total_ratio += item.mRatio;
         total_size += item.mSize;
         stretchable.push_back(item);
      }
   }

   // May need to loop around if something "mins-out"
   bool try_again = true;
   while(try_again && !stretchable.empty())
   {
      int ratios = total_ratio;
      int distribute = available - total_size;
      try_again = false;

      for(ItemList::iterator it = stretchable.begin();
          it!=stretchable.end(); ++it)
      {
         Item &item = *it;

         int extra = item.mRatio * distribute / ratios;
         item.mTargetSize = item.mSize + extra;
         if (item.mTargetSize<item.mMin)
         {
             outSizes[item.mIndex] = SDIM(inSize,item.mMin);

             item.mTargetSize = item.mMin;

             total_ratio -= item.mRatio;
             available -= item.mMin;
             total_size -= item.mSize;
             try_again = true;
             stretchable.erase(it);
             break;
         }
         distribute -= extra;
         ratios -= item.mRatio;
      }
   }

   // Ok, now copy the mTargetSize to the result.
   for(ItemList::iterator it = stretchable.begin();
       it!=stretchable.end(); ++it)
   {
      Item &item = *it;
      outSizes[item.mIndex] = SDIM(inSize,item.mTargetSize);
   }
}








class RowContainer : public MultiChildParent<Container>
{
   typedef MultiChildParent<Container> super;
   struct CRect
   {
      CRect() {}
      CRect(Container *inC) : mContainer(inC)
      {
         mOrigRect = mContainer->Rect();
         mNewRect = mOrigRect;
      }
      void Apply(bool inHoriz,SizeType inType)
      {
         if (mNewRect!=mOrigRect)
         {
            if (inHoriz)
            {
               if (mNewRect.width!=mOrigRect.width)
                  mContainer->SetWidth(mNewRect.width,inType);
            }
            else
            {
               if (mNewRect.height!=mOrigRect.height)
                  mContainer->SetHeight(mNewRect.height,inType);
            }
            mContainer->SetRect(mNewRect);
            mNewRect = mOrigRect;
         }
      }
      wxRect    mOrigRect;
      wxRect    mNewRect;
      Container *mContainer;
   };

   typedef std::vector<CRect> CRectList;

public:
   RowContainer(Manager *inManager,bool inHoriz,SizeType inShape) :
      MultiChildParent<Container>(inManager)
   {
      mHorizontal = inHoriz;
      mSizer = inShape;
      mDragging = false;
      mVisible = true;
      mDebugColour = wxColour(rand()&0xff,rand()&0xff,rand()&0xff);

      Skin *skin = Manager::GetSkin();
      mSashSize = skin->GetSashSize();
   }

   ContainerStyle GetStyle() { return mHorizontal ? csHStack : csVStack; }

   bool ShowsChildsTitle() { return false; }

   bool IsWideDock() { return mSizer==stWideDock; }


   // RowContainer
   bool HintsFromMouse(HintInfo &outHints, Container *inDrop, wxPoint inPos)
   {
      if (!ContainsVisPoint(inPos))
         return false;

      if (MultiChildParent<Container>::HintsFromMouse(outHints,
               inDrop,inPos))
        return true;

      wxPoint p = AsParent()->ScreenToClient(inPos);

      // mouse has landed on the row somewhere - work out where ...
      int pos = mHorizontal ? p.x : p.y;

      int n = mChildren.size();
      for(int i=0;i<n;i++)
      {
         int stop = i<mStops.size() ? mStops[i] : mStops[i-1] +
                            (mHorizontal ? mChildren[i]->Rect().width : 
                                           mChildren[i]->Rect().height );

         // Check for on-sash ...
         if (pos>=stop && pos<=stop+mSashSize)
         {
            // It is between two items ...
            outHints.mDropContainer = mChildren[i];
            outHints.mDropWhere = mHorizontal ? apRight : apBelow;
            // todo - do this properly
            wxRect r =  mChildren[i]->Rect();
            if (mHorizontal)
               r.x += r.width/2;
            else
               r.y += r.height/2;
            outHints.mDropRect = RectClientToScreen(AsParent(),r);
            return true;
         }
         // Check for on the end of a toolbar running across us ...
         else if (pos<stop)
         {
            outHints.mDropContainer = mChildren[i];
            outHints.mDropWhere = mHorizontal ? apBelow : apRight;
            // todo - do this properly
            wxRect r =  mChildren[i]->Rect();
            if (!mHorizontal)
            {
               r.x += r.width;
               r.width = Rect().GetRight() - r.x;
            }
            else
            {
               r.y += r.height;
               r.height = Rect().GetBottom() - r.y;
            }
            outHints.mDropRect = RectClientToScreen(AsParent(),r);
            return true;
         }
      }


      // Must be pass the end of all children ...
      outHints.mDropContainer = mChildren[n-1];
      outHints.mDropWhere = mHorizontal ? apRight : apBelow;
      // todo - do this properly
      wxRect r =  mChildren[n-1]->Rect();
      if (mHorizontal)
      {
          r.x += r.width;
          r.width = Rect().GetRight() - r.x;
      }
      else
      {
          r.y += r.height;
          r.height = Rect().GetBottom() - r.y;
      }
      outHints.mDropRect = RectClientToScreen(AsParent(),r);
      return true;
   }

   void ToSerial(Serializer &outSerializer) const
   {
      Container::ToSerial(outSerializer);
      outSerializer["type"] = mHorizontal ? "hrow" : "vrow";
      outSerializer["iswide"] = mSizer==stTallDock ? 0 : 1;
      for(int i=0;i<mChildren.size();i++)
      {
         outSerializer[i]["size"] =
             GetDimension(mChildren[i]->Rect(),!mHorizontal);
         mChildren[i]->ToSerial(outSerializer[i]["child"]);
      }
   }

   void FromSerial(Serializer &inSerializer)
   {
      Container::FromSerial(inSerializer);
      for(int i=0; inSerializer.HasKey(i); i++)
      {
         Container *c = mManager->Create(inSerializer[i]["child"]);
         if (c)
         {
            Add(c, mHorizontal ? apRight : apBelow);
            int size = inSerializer[i]["size"];
            if (mHorizontal)
               c->SetHeight(size,mSizer);
            else
               c->SetWidth(size,mSizer);
         }
      }
   }




   bool CanAddChild(AddPosition inWhere)
   {
      return mChildren.empty() || 
        ( mHorizontal  && (inWhere==apLeft || inWhere==apRight) ) ||
        ( !mHorizontal && (inWhere==apAbove || inWhere==apBelow) );
   }

   Container *Add(Container *inContainer,AddPosition inWhere)
   {
      bool compatible = mHorizontal ?
                (inWhere==apLeft  || inWhere==apRight) :
                (inWhere==apAbove  || inWhere==apBelow) ;

      if (!compatible)
         return Container::Add(inContainer,inWhere);


      if (!mChildren.size())
         mChildren.push_back(inContainer);
      else if (inWhere==apLeft || inWhere==apAbove)
         mChildren.insert(mChildren.begin(),inContainer);
      else if (inWhere==apRight || inWhere==apBelow)
         mChildren.push_back(inContainer);
      else
         CriticalError();

      inContainer->Reparent(this);

      wxRect r = Rect();

      if (mHorizontal)
         inContainer->SetHeight(r.height==0?0xffff:r.height,mSizer);
      else
         inContainer->SetWidth(r.width==0?0xffff:r.width,mSizer);


      if (mParent)
         mParent->OnChildChange(ccfChildAdded,this);
      UpdateMinSize();
      DoLayout();
      return inContainer;
   }

   //bool TightFit() { return false; }


   // Row container ...
   void UpdateMinSize()
   {
      wxSize osize = mMinSize;

      mMinSize = wxSize();
      for(int i=0;i<mChildren.size();i++)
      {
         wxSize s = mChildren[i]->GetSize(stMin);
         if (mHorizontal)
         {
            mMinSize.x += s.x;
            if (s.y>mMinSize.y) mMinSize.y = s.y;
         }
         else
         {
            mMinSize.y += s.y;
            if (s.x>mMinSize.x) mMinSize.x = s.x;
         }
      }
      int gaps = mChildren.size()-1;
      if (gaps>0)
      {
         if (mHorizontal)
            mMinSize.x+=gaps*mSashSize;
         else
            mMinSize.y+=gaps*mSashSize;
      }

      if (osize!=mMinSize && mParent)
      {
         mParent->OnChildChange(ccfMinSize,this);
      }
   }




   Container *Insert(Container *inContainer,Container *inSibling,
                         AddPosition inDirection)
   {
      bool compatible = mHorizontal ?
                (inDirection==apLeft  || inDirection==apRight) :
                (inDirection==apAbove  || inDirection==apBelow) ;

      if (!compatible)
         CriticalError();

      for(int i=0;i<mChildren.size();i++)
      {
         if (mChildren[i]==inSibling)
         {
            SetChildDockedSizes();
            int pos = inDirection==apLeft || inDirection==apAbove? i : i+1; 
            mChildren.insert(mChildren.begin()+ pos, inContainer );
            inContainer->Reparent(this);

            wxRect r = Rect();
            if (mHorizontal)
               inContainer->SetHeight(r.height,mSizer);
            else
               inContainer->SetWidth(r.width,mSizer);

            UpdateMinSize();

            DoLayout();

            return inContainer;
         }
      }

      CriticalError();
      return 0;
   }


   bool GetSiblingDock(Container *inDrop,Container *inSibling,
                   int inDirection, wxRect &outDock)
   {
      bool compatible = mHorizontal ?
                (inDirection==apLeft  || inDirection==apRight) :
                (inDirection==apAbove  || inDirection==apBelow) ;

      if (!compatible)
         return false;


      return false;

/*
      ChildList list = mChildren;
      int pos = -1;
      if (inSibling==0)
      {
         if (inDirection==apLeft || inDirection==apAbove)
         {
            pos = 0;
            list.insert( list.begin(), inDrop );
         }
         else
         {
            pos = list.size();
            list.push_back( inDrop );
         }
      }
      else
      {
         for(int i=0;i<list.size();i++)
         {
            if (list[i]==inSibling)
            {
               pos = inDirection==apLeft || inDirection==apAbove? i : i+1; 
               list.insert(list.begin()+ pos, inDrop );
               break;
            }
         }
      }

      std::vector<wxRect> rects;
      LayoutContainerList(list,0,&rects);

      outDock.mRect = RectClientToScreen(AsParent(),rects[pos]);
      outDock.mContainer = inSibling;
      return true;
      */
   }



   void RenderDecos(wxDC &inDC,Skin *inSkin)
   {
      if (!Visible())
         return;

      wxRect r = Rect();
      int last_pos=0;
      int size = GetDimension(r,!mHorizontal);

      for(int c=0;c<mChildren.size();c++)
      {
         Container *child = mChildren[c];
         wxRect crect = child->Rect();
         // for "bottom" bit ...
         last_pos = mHorizontal ?
            (crect.x + crect.width) : (crect.y + crect.height);

         // Render children that do not take up whole "width" ...
         int cs = size-GetDimension(crect,!mHorizontal);
         if (cs>0)
         {
            if (!mHorizontal)
            {
               crect.x += size-cs;
               crect.width = cs;
            }
            else
            {
               crect.y += size-cs;
               crect.height = cs;
            }
            inSkin->RenderBackground(inDC,crect);
         }

      }

      for(int j=0;j<mStops.size();j++)
         inSkin->RenderSash(inDC,mRect,mStops[j],mHorizontal);

      if (!mHorizontal)
      {
         last_pos -= r.y;
         if (last_pos< r.height)
         {
            r.y += last_pos;
            r.height-= last_pos;
            inSkin->RenderBackground(inDC,r);
         }

      }
      else
      {
         last_pos -= r.x;
         if (last_pos< r.width)
         {
            r.x += last_pos;
            r.width -= last_pos;
            inSkin->RenderBackground(inDC,r);
         }
      }

#ifdef DEBUG_COLOUR
      inDC.SetPen( *wxTRANSPARENT_PEN );
      inDC.SetBrush( wxBrush(mDebugColour) );
      inDC.DrawRectangle( r );
#endif
   }

   int GetDir(const wxSize &inSize)
   {
      return mHorizontal ? inSize.x : inSize.y;
   }

   int GetDir(const wxRect &inRect)
   {
      return mHorizontal ? inRect.width : inRect.height;
   }

   virtual void DoOverwriteChild(int inI, Container *inNew)
   {
      mChildren[inI] = inNew;
   }

   void LayoutContainerList(ContainerList &inList,
            std::vector<int> *outStops,
            std::vector<wxRect> *outRects)
   {
      wxRect r = mRect;

      wxSize rsize(r.width,r.height);
      std::vector<wxSize> sizes;
      LayoutContainerSizes(inList,sizes,rsize,mSizer,mHorizontal);

      int n = inList.size();
      int gaps = n-1;

      if (gaps<0) gaps = 0;
      if (outStops) outStops->resize(gaps);
      if (outRects) outRects->resize(n);


      int dir = mHorizontal ? r.x : r.y;
      for(int i=0;i<n;i++)
      {
         wxRect pos = r;
         Container &c = *inList[i];

         pos.width = sizes[i].x;
         pos.height = sizes[i].y;

         if (mHorizontal)
            { pos.x = dir; dir+=pos.width; }
         else
            { pos.y = dir; dir+=pos.height; }


         if (outRects)
            (*outRects)[i] = pos;
         else
            c.SetRect( pos );

         if (i+1<n && outStops)
            (*outStops)[i] = dir;

         dir += mSashSize;
      }

      if (!outRects && mSnoopWindow)
         mSnoopWindow->Refresh();
   }


   // RowContainer
   void SetRect(const wxRect &inRect)
   {
       mRect =  inRect;

       DoLayout();
   }

   // RowContainer
   void OnChildChange(unsigned int inFlags,Container *inChild) 
   {
      if (inFlags & (ccfWindow | ccfChildClosed ))
      {
         if (mHorizontal)
         {
             wxSize s = GetSize(mSizer);
             inChild->SetHeight(s.y,mSizer);
         }
         else
         {
             wxSize s = GetSize(mSizer);
             inChild->SetWidth(s.y,mSizer);
         }
         UpdateMinSize();
         DoLayout();
      }
      else if (inFlags & ccfMinSize)
      {
         UpdateMinSize();
         DoLayout();
      }

      MultiChildParent<Container>::OnChildChange(inFlags,inChild);
   }


   void DoLayout()
   {
      if (!Visible())
         return;
      LayoutContainerList(mChildren,&mStops,0);
   }

   int StopHit(wxMouseEvent &inEvt)
   {
      if (mRect.Contains(inEvt.GetPosition()))
      {
         int val = mHorizontal ? inEvt.GetPosition().x : 
                                 inEvt.GetPosition().y;

         for(int s=0;s<mStops.size();s++)
         {
             if (val>mStops[s] && val<mStops[s]+mSashSize)
                return s;
         }
      }
      return -1;
   }

   // Bake in current dock sizes as "last changed"
   void SetChildDockedSizes()
   {
      for(int i=0;i<mChildren.size();i++)
         mChildren[i]->RecordSize(mSizer, mChildren[i]->Rect() );
   }


   int PushRects(CRectList &inList,int inDelta,int inFirst,bool inMoveDown)
   {
      CRect &cr    = inList[inFirst];
      Container &c = *cr.mContainer;

      int min = GetDir(c.GetSize(stMin));
      int len = GetDir(cr.mOrigRect);

      int new_size = len+inDelta;

      // Squished to its limit
      if (new_size < min)
      {
         new_size = min;
      }
      else
      {
         wxSize s(new_size,new_size);
         if (c.GetPreferredSize(s,mHorizontal))
         {
            new_size = GetDir(s);
            // set transverse size ...
            if (mHorizontal)
               cr.mNewRect.height = s.y;
            else
               cr.mNewRect.width = s.x;
         }
      }

      int moved = new_size - len;

      if (mHorizontal)
      {
         if (inMoveDown)
         {
            cr.mNewRect.x -= moved;
            cr.mNewRect.width += moved;
         }
         else
            cr.mNewRect.width += moved;
      }
      else
      {
         if (inMoveDown)
         {
            cr.mNewRect.y -= moved;
            cr.mNewRect.height += moved;
         }
         else
            cr.mNewRect.height += moved;
      }

      int missing = inDelta - moved;
      if (missing!=0 && inFirst+1<inList.size())
      {
         int extra = PushRects(inList,missing,inFirst+1,inMoveDown);

         if (mHorizontal)
            cr.mNewRect.x += extra;
         else
            cr.mNewRect.y += extra;

         moved+=extra;
      }

      return moved;
   }
            

   void DoDrag(wxPoint inPoint,bool inLeftIsDown)
   {
      ContainerList &l = mChildren;
      int n = l.size();
      // Hmm ...
      if (!inLeftIsDown || mDragStop+1>=n || mDragStop<0)
      {
          DoRelease();
          return;
      }

      int delta = mHorizontal ? (inPoint.x - mDragStart.x) :
                                (inPoint.y - mDragStart.y);

      if (delta<-50)
         delta++;

      if (delta<0)
      {
         // Squash top first, then stretch bottom
         int actual_delta = PushRects(mDragTop,delta,0,false);
         PushRects(mDragBottom,-actual_delta,0,true);
      }
      else
      {
         // Squash bottom first, then stretch top
         int actual_delta = PushRects(mDragBottom,-delta,0,true);
         PushRects(mDragTop,-actual_delta,0,false);
      }

      AsParent()->Freeze();

      // Now apply
      for(int t=0;t<mDragTop.size();t++)
         mDragTop[t].Apply(mHorizontal,mSizer);

      for(int b=0;b<mDragBottom.size();b++)
         mDragBottom[b].Apply(mHorizontal,mSizer);

      SetChildDockedSizes();
      DoLayout();
      UpdateMinSize();
      AsParent()->Thaw();

      AsParent()->Refresh();
   }


   Container *GetCurrentContainer(int inIndex)
   {
      return mChildren[mDragStop];
   }

   bool GetPreferredSize(wxSize &ioSize,bool inFromWidth)
   {
      if (inFromWidth == mHorizontal)
         return false;

      int max_height = 0;
      int max_width = 0;

      // TODO: Accumulate child size to see if it overflows other dimension.
      // actually don't worry - just clip toolbars.

      for(int c=0;c<mChildren.size();c++)
      {
         Container *child = mChildren[c];
         wxSize s = child->GetSize(mSizer);

         if (!inFromWidth)
         {
            // setting height
            s.y = ioSize.y;
            if (child->GetPreferredSize(s,false))
            { 
               if (s.y > max_height) max_height = s.y;
            }
            else
               return false;
         }
         else
         {
            // setting width
            s.x = ioSize.x;
            if (child->GetPreferredSize(s,true))
            { 
               if (s.x > max_width) max_width = s.x;
            }
            else
               return false;
         }
      }

      if (!inFromWidth)
         ioSize.y = max_height;
      else
         ioSize.x = max_width;

      return true;
   }

   void SetWidth(int inWidth,SizeType inSizer)
   {
      if (!mHorizontal)
      {
         for(int i=0;i<mChildren.size();i++)
            mChildren[i]->SetWidth(inWidth,mSizer);
         UpdateMinSize();
         DoLayout();
      }
   }

   void SetHeight(int inHeight,SizeType inSizer)
   {
      if (mHorizontal)
      {
         for(int i=0;i<mChildren.size();i++)
            mChildren[i]->SetHeight(inHeight,mSizer);
         UpdateMinSize();
         DoLayout();
      }
   }

   bool IsFixed(bool inHorizontal,wxSize &outSize)
   {
      if (inHorizontal==mHorizontal)
         return false;

      int n = mChildren.size();
      if (n==0)
         return false;

      int max = 0;
      for(int i=0;i<n;i++)
      {
         wxSize m(outSize);
         if (!mChildren[i]->IsFixed(inHorizontal,m))
            return false;
         max = std::max(max, inHorizontal ? m.x : m.y);
      }

      if (inHorizontal)
         outSize.x = max;
      else
         outSize.y = max;
      return true;
   }




   void DoMouseMove(wxMouseEvent &inEvt)
   {
      if (IgnoreMouse(inEvt)) return;

      if (mDragging)
      {
         DoDrag(inEvt.GetPosition(), inEvt.LeftIsDown());
      }
      else
      {
         if (StopHit(inEvt)>=0 && mSnoopWindow)
            mSnoopWindow->SetCursor( mHorizontal ?  wxCURSOR_SIZEWE  : wxCURSOR_SIZENS );
         else
         {
            super::DoMouseMove(inEvt);
            inEvt.Skip();
         }
      }
   }

   void DoLeftUp(wxMouseEvent &inEvent)
   {
      if (mDragging)
      {
         mSnoopWindow->SetCursor(0);
         mDragging = false;
         DoRelease();
      }
      else
         inEvent.Skip();
   }

   void DoLeftDown(wxMouseEvent &inEvent)
   {
      if (IgnoreMouse(inEvent)) return;

      int stop = StopHit(inEvent);
      if (stop>=0)
      {
         mSnoopWindow->SetCursor( mHorizontal ?  wxCURSOR_SIZEWE  : wxCURSOR_SIZENS );
         mDragging = true;
         mDragStop = stop;

         // Bake in sizes before dragging ...
         SetChildDockedSizes();

         int n = mChildren.size();
         mDragTop.resize(0);
         mDragBottom.resize(0);

         // Build list of children berfore the stop - reverse order so
         //  the top one is moved first....
         for(int i=stop;i>=0;i--)
            mDragTop.push_back(mChildren[i]);

         // And ones below ...
         for(int i=stop+1;i<n;i++)
            mDragBottom.push_back(mChildren[i]);

         mDragStart = inEvent.GetPosition();

         DoCapture();
      }
      else
         inEvent.Skip();
   }


   bool     mHorizontal;
   int      mSashSize;
   SizeType mSizer;
   bool     mDragging;
   int      mDragStop;
   wxPoint  mDragStart;
   wxColour mDebugColour;

   CRectList mDragTop; /* Also left - these are reversed */
   CRectList mDragBottom; /* Also left */

   std::vector<int> mStops;

};



// --- NotebookBase ------------------------------------------------


template<typename CHILD_ = Container>
class NotebookBase : public MultiChildParent<CHILD_>
{
public:
   enum { ChildMenu = 14000 };
   typedef MultiChildParent<CHILD_> super;

   NotebookBase(Manager *inManager) : MultiChildParent<CHILD_>(inManager)
   {
      mCurrentPage = -1;
      mFirst = 0;
      mQuickRestore = -1;
      mMenu = 0;
   }
   ~NotebookBase()
   {
      delete mMenu;
   }


   ContainerStyle GetStyle() { return csNotebook; }

   bool CanAddChild(AddPosition inWhere)
   {
      return super::mChildren.empty() || inWhere==apOver;
   }


   wxString GetCaption()
   {
      if (super::mChildren.empty())
         return "Notebook";
      if (mCurrentPage<0) SetCurrentPage(0);
      if (mCurrentPage>=super::mChildren.size())
         SetCurrentPage(super::mChildren.size()-1);

      return super::mChildren[mCurrentPage]->GetCaption();
   }


   virtual void ButtonClick(ButtonCommand inCommand)
   {
      if (inCommand==bcMenu)
      {
         delete mMenu;
         mMenu = new wxMenu;
         for(int c=0;c<super::mChildren.size();c++)
         {
            Container *child = super::mChildren[c];
            mMenu->Append(ChildMenu + child->GetUID(), child->GetCaption());
         }
         HostedContainer::AsParent()->PopupMenu(mMenu);
      }
      else
         MultiChildParent<CHILD_>::ButtonClick(inCommand);
   }


   void OnCommand(wxCommandEvent &inEvent)
   {
      int id = inEvent.GetId() - ChildMenu;
      for(int c=0;c<super::mChildren.size();c++)
      {
         if (super::mChildren[c]->GetUID()==id)
         {
             SetCurrentPage(c);
             return;
         }
      }
      inEvent.Skip();
   }

   void FromSerial(Serializer &inSerializer)
   {
      Container::FromSerial(inSerializer);

      int current = inSerializer["current"];
      for(int i=0; inSerializer.HasKey(i); i++)
      {
         Container *c = super::mManager->Create(inSerializer[i]);
         if (c)
            super::Add(c,apOver);
         else if (i<current)
            current--;
      }

      if (current>=0 && current<super::mChildren.size())
         SetCurrentPage(current);
   }




   void Render(wxDC &inDC,Skin *inSkin)
   {
      if (!super::mVisible) return;

      inSkin->RenderTabs(inDC,super::mRect,mInfo,mCurrentPage,mFirst,super::mButtons,super::DoCanRemove());

      if (mCurrentPage>=0 && mCurrentPage<super::mChildren.size())
         super::mChildren[mCurrentPage]->Render(inDC,inSkin);

      if (super::mChildren.empty())
      {
         inSkin->RenderEmptyNotebook(inDC,super::mRect);
      }
   }

   bool  ShowsChildsTitle() { return true; }

   virtual void SetPageRect(const wxRect &inRect) { }
   virtual void SetEmpty() { }

   virtual void DoLayout()
   {
      if (!super::Visible())
         return;

      wxRect r = super::mRect;
      typename super::ChildList &l = super::mChildren;
      int n = l.size();

      mInfo.resize(n);
      mHitRects.resize(n);

      for(int i=0;i<n;i++)
      {
         mInfo[i].mTitle = l[i]->GetCaption();
         mInfo[i].mIcons = l[i]->GetIconSet();
         mInfo[i].mHitRect = &mHitRects[i];
         mInfo[i].mClosable = l[i]->CanClose();
         mHitRects[i] = wxRect(0,0,0,0);
      }

      if (n)
      {
         if (mCurrentPage<0)
            SetCurrentPage(0);
         else if (mCurrentPage>=n)
            SetCurrentPage( n-1 );

         Skin *skin = Manager::GetSkin();
         wxRect b = skin->GetNotebookPageSize(r);
         skin->LayoutTabs(r,mInfo,mCurrentPage,mFirst,super::mDragOff[0],super::mDragOff[1],super::mButtons);

         SetPageRect(b);
      }
      else
         SetEmpty();

      if (super::mSnoopWindow)
         super::mSnoopWindow->Refresh();
   }

   int HitRect(wxMouseEvent &inEvent)
   {
      for(int i=0;i<mHitRects.size();i++)
         if (mHitRects[i].Contains(inEvent.GetPosition()))
            return i;
      return -1;
   }

   virtual void SetCurrentPage(int inPage)
   {
      mCurrentPage = inPage;

      int n = super::mChildren.size();
      bool vis = super::Visible();
      for(int i=0;i<n;i++)
         ((Container *)super::mChildren[i])->Show(i==mCurrentPage && vis);

      DoLayout();
      if (super::mParent)
         super::mParent->OnChildChange(ccfCurrent|ccfCaption,this);
   }

   void RaiseChild(Container *inChild)
   {
      int n = super::mChildren.size();
      bool vis = super::Visible();
      for(int i=0;i<n;i++)
         if (super::mChildren[i]==inChild)
            SetCurrentPage(i);
      super::Raise();
   }


   // Notebook
   void OnChildChange(unsigned int inFlags,Container *inContainer)
   {
      DoLayout();
      if (super::mParent)
         super::mParent->OnChildChange(inFlags,this);
   }

   void DoLeftDown(wxMouseEvent &inEvent)
   {
      if (super::IgnoreMouse(inEvent)) return;

      int h = HitRect(inEvent);
      // This stops the dragging of the "preview" tab - don't think it's
      //  what we want though
      //if (h>=0 && ((h!=mCurrentPage || mQuickRestore!=-1)&&
        // mCurrentPage!=mQuickRestore))
      if (h>=0 )
      {
         mQuickRestore = -1;
         SetCurrentPage(h);

         // We will miss the drag if we do this...
         //return;
      }

      super::DoLeftDown(inEvent);
   }

   void QuickRelease()
   {
      if (mQuickRestore>=0)
      {
         SetCurrentPage( mQuickRestore );
         mQuickRestore = -1;
         DoLayout();
      }
   }

   Container *GetCurrentContainer(int inIndex)
   {
      if (inIndex==0)
         return this;
      return super::mChildren[mCurrentPage];
   }


   wxSize BorderSize()
   {
      Skin *skin = Manager::GetSkin();
      wxRect b = skin->GetNotebookPageSize(wxSize(1000,1000));
      return wxSize(1000-b.width,1000-b.height);
   }


   void DoMouseMove(wxMouseEvent &inEvent)
   {
      if (super::IgnoreMouse(inEvent))
      {
          QuickRelease();
          return;
      }

      if (super::ButtonTest(inEvent))
         return;

      if (super::mSnoopWindow)
      {
         wxRect r = super::Rect();
         super::mSnoopWindow->SetCursor(0);
      }

      int h = HitRect(inEvent);
      if (h>=0 && h!=mCurrentPage)
      {
         if (mQuickRestore<0)
            mQuickRestore = mCurrentPage;
         SetCurrentPage(h);
         DoLayout();
         return;
      }
      if (h<0)
         QuickRelease();

      inEvent.Skip();
   }

   void DoLeave()
   {
      QuickRelease();
      super::DoLeave();
   }


   int                 mFirst;
   int                 mQuickRestore;
   TabInfos            mInfo;
   wxRect              mTabDragOff;
   std::vector<wxRect> mHitRects;
   int                 mCurrentPage;
   wxMenu              *mMenu;

};



class NotebookContainer : public NotebookBase<Container>
{
   typedef NotebookBase<Container> super;
public:
   NotebookContainer(Manager *inManager) :
       NotebookBase<Container>(inManager)
   {
   }

   Container *Add(Container *inContainer,AddPosition inWhere)
   {
      if (inWhere!=apOver)
         return Container::Add(inContainer,inWhere);

      if (!super::mChildren.size())
         super::mChildren.push_back(inContainer);
      else if (inWhere==apOver)
         super::mChildren.push_back(inContainer);
      else
         CriticalError();

      inContainer->Reparent(this);
      UpdateMinSize();
      SetCurrentPage( super::mChildren.size()-1 );
      if (mParent)
         mParent->OnChildChange(ccfChildAdded,this);
      DoLayout();
      return inContainer;
   }

   void ToSerial(Serializer &outSerializer) const
   {
      Container::ToSerial(outSerializer);
      outSerializer["type"] = "notebook";
      outSerializer["current"] = mCurrentPage;
      for(int i=0;i<super::mChildren.size();i++)
         super::mChildren[i]->ToSerial(outSerializer[i]);
   }


   void UpdateMinSize()
   {
      wxSize osize = mMinSize;
      mMinSize = wxSize();
      for(int i=0;i<super::mChildren.size();i++)
      {
         wxSize s = super::mChildren[i]->GetSize(stMin);
         if (s.x>mMinSize.x) mMinSize.x = s.x;
         if (s.y>mMinSize.y) mMinSize.y = s.y;
      }
      mMinSize+=BorderSize();
      if (mParent && mMinSize!=osize)
         mParent->OnChildChange(ccfMinSize,this);
         
   }


   void SetPageRect(const wxRect &inRect)
   {
      bool vis = Visible();

      if (inRect.width>0 && inRect.height>0 && vis)
      {
         super::mChildren[mCurrentPage]->SetRect(inRect);
         super::mChildren[mCurrentPage]->Show(true);
      }
      else
         super::mChildren[mCurrentPage]->Show(false);


      int n = super::mChildren.size();
      for(int i=0;i<n;i++)
         if (i!=mCurrentPage)
            super::mChildren[i]->Show(false);
   }

   Container *Insert(Container *inContainer,Container *inSibling,
                         AddPosition inWhere)
   {
      for(int i=0;i<super::mChildren.size();i++)
         if (super::mChildren[i]==inSibling)
         {
            super::mChildren.insert(super::mChildren.begin()+i+1,inContainer);
            inContainer->Reparent(this);
            wxRect s = Rect();
            int n = super::mChildren.size()-1;
            if (n<1) n = 1;
            wxRect init_size(0,0, s.width/n,s.height/n);
            inContainer->SetRect(init_size);

            mParent->OnChildChange(ccfWindow|ccfChildAdded,inContainer);

            SetCurrentPage( i+1 );
            DoLayout();
            return inContainer;
         }
      CriticalError();
      return 0;
   }

   void DoOverwriteChild(int inI, Container *inNew)
   {
      super::mChildren[inI] = inNew;
   }



   void Show(bool inShow)
   {
      if (inShow!=mVisible)
      {
         mVisible = inShow;
         for(int i=0;i<super::mChildren.size();i++)
            super::mChildren[i]->Show(inShow && i==mCurrentPage);
         if (inShow)
            DoLayout();
      }
   }

   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(NotebookContainer, NotebookBase<Container> )
   EVT_MENU_RANGE( ChildMenu, ChildMenu+1000, NotebookContainer::OnCommand)
END_EVENT_TABLE()


// -- MDI Client Container ---


typedef void (wxMDIChildFrame::*wxActivateEventFunction2)(wxActivateEvent&);


#define wxActivateEventHandler2(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxActivateEventFunction2, &func)


class MyMDIChildFrame : public wxMDIChildFrame, public FrameContainer
{
   typedef FrameContainer super;
public:
   MyMDIChildFrame(wxMDIParentFrame *inParent,Manager *inManager,
              Container *inClientContainer) :
         wxMDIChildFrame(inParent,-1,"MDI Child"),
         FrameContainer(inManager,this),
         mClientContainer(inClientContainer)
   {
      wxWindow *obj = this;
      obj->Connect( wxEVT_ACTIVATE,
          wxActivateEventHandler2(MyMDIChildFrame::ChildActivate), 0, obj  );
   }
   ~MyMDIChildFrame()
   {
      wxWindow *obj = this;
      obj->Disconnect( wxEVT_ACTIVATE );
   }

   void Raise()
   {
      mClientContainer->RaiseChild(this);
   }

   void ToSerial(Serializer &outSerializer) const
   {
      FrameContainer::ToSerial(outSerializer);
      outSerializer["type"] = "mdichild";
   }

   void FromSerial(Serializer &inSerializer)
   {
      FrameContainer::FromSerial(inSerializer);
   }


   // MyMDIChildFrame
   void OnChildChange(unsigned int inFlags,Container *inContainer)
   {
      if ( mChild && (inFlags & ccfCaption))
      {
         SetTitle(mChild->GetCaption());
      }


      if (mChild)
      {
         unsigned int flags = GetWindowStyle();
         bool closable = mChild->CanClose();
         if (closable != (( flags & wxCLOSE_BOX)!=0) )
         {
            if (closable)
               flags |= wxCLOSE_BOX;
            else
               flags &= ~wxCLOSE_BOX;
            SetWindowStyle(flags);
         }
      }


      mClientContainer->OnChildChange(inFlags,this);
   }

   void ReplaceChild(Container *inContainer,Container *inNew)
   {
      FrameContainer::ReplaceChild(inContainer,inNew);

      if (mChild==0)
      {
          if (mClientContainer)
             mClientContainer->OnChildChange(ccfChildClosed,this);
          Destroy();
      }
   }


   void ChildActivate(wxActivateEvent &inEvt)
   {
      mClientContainer->OnChildChange(ccfActivate,this);
   }


   bool ShowsChildsTitle() { return true; }

   wxWindow *AsParent() { return this; }
   Container *mClientContainer;
};



#ifdef PSEUDO_MDI

class MDIClientContainer : public NotebookContainer
{
   typedef NotebookContainer super;
   wxMDIParentFrame *mFrame;
public:
   MDIClientContainer(Manager *inManager, wxMDIParentFrame *inFrame) : super(inManager)
   {
      mFrame = inFrame;
      mClient = (wxMDIClientWindow *)inFrame->GetClientWindow();
      wxSize s = mFrame->GetClientSize();
      for(int i=0;i<stSIZE;i++)
         mSizes[i] = s;
      mSizes[stMin] = wxSize(20,20);
      mSizes[stMax] = wxSize(0xffff,0xffff);
   }

   wxWindow *AsParent() { return mFrame; }

   virtual bool CanAddChild(AddPosition inWhere) 
   {
      return inWhere==apOver;
      //return true;
   }

   void OnSingleChild()
   {
      DoLayout();
   }

   /*
   Container *Add(Container *inContainer,AddPosition inWhere)
   {
      return super::Add(inContainer,inWhere);
   }
   */

   Container *FindClientContainer() { return this; }

   ContainerStyle GetStyle() { return csMDIClient; }

   bool CanRemove() const { return false; }


   bool Visible() { return true; }

   wxMDIClientWindow *mClient;
};

#else

class MDIClientContainer : public NotebookBase<MyMDIChildFrame>
{
   typedef NotebookBase<MyMDIChildFrame> super;
public:
   MDIClientContainer(Manager *inManager,wxMDIParentFrame *inMDI) :
       NotebookBase<MyMDIChildFrame>(inManager)
   {
      mMDI = inMDI;
      mClient = (wxMDIClientWindow *)inMDI->GetClientWindow();
      mParent = 0;

      for(int i=0;i<stSIZE;i++)
         mSizes[i] = wxSize(200,200);
      mSizes[stMin] = wxSize(40,20);
      mSizes[stMax] = wxSize(0xffff,0xffff);
   }

   void ToSerial(Serializer &outSerializer) const
   {
      Container::ToSerial(outSerializer);
      outSerializer["type"] = "mdiclient";
      outSerializer["current"] = mCurrentPage;
      for(int i=0;i<super::mChildren.size();i++)
         super::mChildren[i]->ToSerial(outSerializer[i]);
   }

   void FromSerial(Serializer &inSerializer)
   {
      Container::FromSerial(inSerializer);

      int current = inSerializer["current"];
      for(int i=0; inSerializer.HasKey(i); i++)
      {
         MyMDIChildFrame *child =
              new MyMDIChildFrame(mMDI,GetManager(),this);
         child->FromSerial(inSerializer[i]);
         super::mChildren.push_back(child);
      }

      if (current>=0 && current<super::mChildren.size())
         SetCurrentPage(current);
   }





   ContainerStyle GetStyle() { return csMDIClient; }

   // Child/parent management
   Container *GetParent() { return mParent; }

   void Reparent(Container *inParent)
   {
      HostedContainer::Reparent(inParent);

      if (inParent != mParent) 
      {
         Container *old_parent = mParent;
         mParent = inParent;
         if (old_parent)
            old_parent->ReplaceChild(this,0);
      }
      else
         mParent = inParent;
   }

   void ReplaceChild(Container *inContainer,Container *inNew=0)
   {
      CriticalError();
   }

   int HitRect(wxMouseEvent &inEvent)
   {
      for(int i=0;i<mHitRects.size();i++)
         if (mHitRects[i].Contains(inEvent.GetPosition()))
         {
            return i;
         }
      return -1;
   }

   void DoOverwriteChild(int inI, Container *inNew)
   {
      CriticalError();
      // super::mChildren[i] = inNew;
   }

   void SetCurrentPage(int inPage)
   {
      super::mChildren[inPage]->Activate();
      if (mParent)
         mParent->OnChildChange(ccfCurrent|ccfCaption,this);
      //NotebookBase<MyMDIChildFrame>::SetCurrentPage(inPage);
      //DoLayout();
      //mMDI->Refresh();
   }

   
   Container *GetCurrentContainer(int inIndex)
   {
      return super::mChildren[mCurrentPage]->GetOnlyChild();
   }


   Container *Add(Container *inContainer,AddPosition inWhere)
   {
      if (inWhere!=apOver)
      {
         return Container::Add(inContainer,inWhere);
      }

      MyMDIChildFrame *child =
          new MyMDIChildFrame(mMDI,GetManager(),this);

      child->SetClientSize( inContainer->GetSize(stFloating) );

      child->Add(inContainer,inWhere);
      super::mChildren.push_back(child);
      DoLayout();
      return inContainer;
   }

   Container *Insert(Container *inContainer,Container *inSibling,
                         AddPosition inWhere)
   {
      CriticalError();
      // TODO
      return 0;
   }

   bool Visible() { return true; }


   Manager *GetManager() { return mParent ? mParent->GetManager() : 0; }

   // Window info ...
   wxString     GetCaption() { return "MDI Child"; }
   const wxIconBundle *GetIconSet() { return 0; }
   wxWindow *AsParent() { return mClient; }


   // Container details
   virtual bool IsEmpty() { return super::mChildren.empty(); }
   virtual bool IsWindow() { return false; }
   virtual bool CanAddChild(AddPosition inWhere) 
   {
      return inWhere==apOver;
   }

   // Positioning
   // Rects are w.r.t parent window, not container...
   void Show(bool inShow)
   {
      mClient->Show(inShow);
   }

   void SetPageRect(const wxRect &inRect)
   {
      mClient->SetSize(inRect);
      mMDI->Refresh();
   }
   void SetEmpty()
   {
      mClient->SetSize(mRect);
      mMDI->Refresh();
   }

   void SetRect(const wxRect &inRect)
   {
      bool from_width = mRect.height == inRect.height;
      mRect = inRect;
      DoLayout();
   }

   wxRect Rect() { return mRect; }


   // mdi client
   bool HintsFromMouse(HintInfo &outHints, Container *inDrop, wxPoint inPos)
   {
      wxRect screen(RectClientToScreen(mMDI,Rect()));

      if (screen.Contains(inPos))
      {
         bool result = Container::HintsFromMouse(outHints,inDrop,inPos);
         outHints.mDropContainer = 0;
         return result;
      }

      return false;
   }

   Container *FindClientContainer() { return this; }
   
   // MDI Client container
   void GetDockPos(Container *inDrop, int inDirection, wxRect &outDock)
   {
      // Windows always dock "over" ontop of themselves ...
      if (inDirection==apOver)
      {
          wxSize s = inDrop->GetSize(stFloating);
          if (s.x<10 || s.y<10)
             s = wxSize(200,200);

          outDock =  RectClientToScreen(mClient,wxRect(10,10,s.x,s.y));
          return;
      }

      Container::GetDockPos(inDrop,inDirection, outDock);
   }

   // TODO
   void UpdateMinSize()
   {
      mMinSize=BorderSize();
   }


   // For rendering and layout-out
   void Render(wxDC &inDC,Skin *inSkin)
   {
      if (!super::mChildren.empty())
      {
         inSkin->RenderTabs(inDC,mRect,mInfo,mCurrentPage,mFirst,mButtons,DoCanRemove());
      }
   }

   // MDI Client Container
   void OnChildChange(unsigned int inFlags,Container *inChild)
   {
      if (inFlags & ccfChildClosed)
      {
         for(int i = 0;i<super::mChildren.size();i++)
            if (super::mChildren[i]==inChild)
            {
               super::mChildren.erase(mChildren.begin()+i);
               break;
            }
      }

      if (inFlags & ccfActivate)
      {
         wxMDIChildFrame *active = mMDI->GetActiveChild();
   
         for(int i = 0;i<super::mChildren.size();i++)
            if (super::mChildren[i]==active)
            {
               if (i!=mCurrentPage)
                  mCurrentPage = i;
               break;
            }
      }


      DoLayout();
   }



   wxMDIParentFrame  *mMDI;
   wxMDIClientWindow *mClient;

   DECLARE_EVENT_TABLE();
};




BEGIN_EVENT_TABLE(MDIClientContainer,NotebookBase<MyMDIChildFrame> )
   EVT_LEFT_DOWN(MDIClientContainer::OnLeftDown)
   EVT_LEAVE_WINDOW(MDIClientContainer::OnLeave)
   EVT_MENU_RANGE( ChildMenu, ChildMenu+1000, NotebookBase<MyMDIChildFrame>::OnCommand)
END_EVENT_TABLE()

#endif



Container *Manager::Create(wxMDIParentFrame *inMDI)
  {  return new MDIClientContainer(this,inMDI); }


Container *Manager::Create(ContainerStyle inStyle,SizeType inType)
{
   switch(inStyle)
   {
      case csVStack:
         return new RowContainer(this,false,inType==stBest ? stWideDock : inType);
      case csHStack:
         return new RowContainer(this,true,inType==stBest ? stTallDock : inType);
      case csNotebook:
         return new NotebookContainer(this);
      default: ;
   }
   return 0;
}


Container *Manager::Create(Serializer &inSerial)
{
   wxString type = inSerial["type"];

   Container *result = 0;

   if (type=="window")
   {
      wxString name = inSerial["name"];
      int id = inSerial["id"];
      // See if we have the window already...
      std::vector<ContainerMap::iterator> found;
      for(ContainerMap::iterator i=mTransfer.begin();i!=mTransfer.end();++i)
         if ( i->second->GetCaption()==name )
            found.push_back(i);

      if (!found.empty())
      {
         int idx = 0;
         for(int i=0;i<found.size();i++)
            if (found[i]->second->GetWindow()->GetId() == id)
               { idx = i; break; }
        result = found[idx]->second;
        mTransfer.erase(found[idx]);
        mWindows[result->GetWindow()] = result;
      }
      else
      {
         // Hmmm ...
      }
   }
   else if (type=="hrow" || type=="vrow")
   {
      result = new RowContainer(this,type=="hrow",
                   (int)inSerial["iswide"] ? stWideDock : stTallDock );
   }
   else if (type=="notebook")
   {
      result = new NotebookContainer(this);
   }
   else if (type=="mdiclient")
   {
      if (!mMDIClientUsed && mMDIClientContainer)
      {
         result = mMDIClientContainer;
         mMDIClientUsed = true;
      }
   }
   else if (type=="mdichild")
   {
      if (mMDIClientUsed && mMDIClientContainer && mMDI)
         result = new MyMDIChildFrame(mMDI,this,mMDIClientContainer);
   }
   else if (type=="frame")
   {
      result = new FrameContainer(this,mFrame,(int)(inSerial["floating"]));
   }

   if (result)
      result->FromSerial(inSerial);

   return result;
}

} // end namespace clay
