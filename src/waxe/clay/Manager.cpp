#include <clay/Layout.h>
#include <clay/Skin.h>
#include "HintWindow.h"
#include <clay/Serializer.h>
#include "wx/minifram.h"

#include <set>

DEFINE_EVENT_TYPE(wxEVT_WINDOW_LIST_CHANGED)

// --- HiddenContainer ----------------------------------------
// The HiddenContainer holds contains that are "closed" but not destroyed.

namespace clay
{


class HiddenContainer : public Container
{
   typedef std::set<Container *> Containers;

public:
   HiddenContainer(Manager *inManager,wxWindow *inParent) :
       Container(inManager), mParent(inParent)
   {
      mHold = new wxWindow(mParent,-1,wxPoint(-100,-100),wxSize(10,10));
      mHold->Show(false);
   }
   bool Visible() { return false; }

   void ToSerial(Serializer &outSerializer) const
   {
      outSerializer["type"] = "hidden";
      int idx = 0;
      for(Containers::const_iterator i = mContainers.begin();
          i!=mContainers.end(); ++i )
         (*i)->ToSerial(outSerializer[idx++]);
   }




   wxWindow *AsParent() { return mHold; }

   ContainerStyle GetStyle() { return csHold; }

   // Child/parent management
   Container *GetParent() { return 0; }
   void Reparent(Container *inContainer) { *(int *)0=0; }
   void ReplaceChild(Container *inContainer,Container *inNew=0)
   {
      Containers::iterator i = mContainers.find(inContainer);
      if (i!=mContainers.end())
         mContainers.erase(i);
      if (inNew)
      {
         mContainers.insert(inNew);
         inNew->Reparent(this);
      }
   }

   void Raise() { }

   void RaiseChild(Container *inChild)
   {
      if (mContainers.find(inChild)!=mContainers.end())
         mManager->FloatContainer(inChild);
   }


   void DestroyOrClose() { }

   Container *Add(Container *inContainer,AddPosition inWhere)
   {
      if (inContainer)
      {
         mContainers.insert(inContainer);
         inContainer->Reparent(this);
      }
      return inContainer;
   }
   Container *Insert(Container *inContainer,Container *inSibling,
                         AddPosition inWhere)
   {  return Add(inContainer,inWhere); }

   bool CanAddChild(AddPosition inWhere)  { return true; }

   // Positioning
   // Rects are w.r.t parent window, not container...
   void Show(bool inShow) { }
   void SetRect(const wxRect &inRect) { }
   wxRect Rect() { return wxRect(); }

   void Render(wxDC &inDC,Skin *inSkin) { }


   wxWindow *mParent;
   wxWindow *mHold;
   Containers mContainers;
};


// --- Manager -----------------------------------------------

Skin *Manager::mSkin = 0;



Manager::Manager(wxFrame *inFrame)
{
   mFrame = inFrame;
   mRootContainer = Create(mFrame,false);
   //mRootContainer->SetRect( mRootContainer->Rect() );
   mTopLevelContainers.push_back(mRootContainer);
   mMDI = 0;
   mMDIClientContainer = 0;
   mHintWindow = 0;
   mHiddenContainer = new HiddenContainer(this,mFrame);

   if (inFrame->IsKindOf(CLASSINFO(wxMDIParentFrame)))
   {
      mMDI = (wxMDIParentFrame *)inFrame;
      mMDIClientContainer = Create(mMDI);
      mRootContainer->Add(mMDIClientContainer,apOver);
   }

}

Manager::~Manager()
{
   delete mRootContainer;
   delete mHintWindow;
}

void Manager::RemoveWindow(wxWindow *inWindow)
{
   int id = inWindow ? inWindow->GetId() : 0;

   ContainerMap::iterator i = mWindows.find(inWindow);
   if (i!=mWindows.end())
      mWindows.erase(i);

   wxCommandEvent evt(wxEVT_WINDOW_LIST_CHANGED,id);
   mFrame->ProcessWindowEvent(evt);
}


void Manager::RecordWindow(wxWindow *inWindow,Container *inContainer)
{
   mWindows[inWindow] = inContainer;
   wxCommandEvent evt(wxEVT_WINDOW_LIST_CHANGED,inWindow->GetId());
   mFrame->ProcessWindowEvent(evt);
}


void Manager::HideHints()
{
   if (mHintWindow)
      mHintWindow->HideHints();
}

void Manager::HintsFromMouse(Container *inDrop,wxPoint inPos)
{
   if (!mHintWindow)
      mHintWindow = HintWindow::Create(mFrame);

   HintInfo hints;
   for(int i=mTopLevelContainers.size()-1;i>=0;i--)
   {
      if (mTopLevelContainers[i]!=inDrop)
      {
         if (mTopLevelContainers[i]->HintsFromMouse(hints,inDrop,inPos))
         {
            mHintWindow->SetHintInfo(hints,inPos);
            return;
         }
      }
   }
   //mHintWindow->SetHintInfo(hints);
   mHintWindow->HideHints();
}




void Manager::CloseFloating(wxFrame *inFrame,Container *inFrameContainer)
{
   for(int i=0;i<mTopLevelContainers.size();i++)
      if (mTopLevelContainers[i]==inFrameContainer)
      {
         delete inFrameContainer;
         mTopLevelContainers.erase(mTopLevelContainers.begin()+i);
         inFrame->Destroy();
         return;
      }

   for(int i=0;i<mOldTopLevelContainers.size();i++)
      if (mOldTopLevelContainers[i]==inFrameContainer)
      {
         delete inFrameContainer;
         mOldTopLevelContainers.erase(mOldTopLevelContainers.begin()+i);
         inFrame->Destroy();
         return;
      }

}

void Manager::Park(Container *inContainer)
{
   mHiddenContainer->Add(inContainer,apOver);
}




void Manager::TryDropFrame(wxFrame *inFrame, Container *inFrameContainer,
           wxPoint inMouse)
{
   if (mHintWindow)
   {
      AddPosition pos;
      Container *target = mHintWindow->GetTarget(inMouse,pos);
      if (target)
      {
         Container *child = inFrameContainer->GetOnlyChild();
         if (!child)
            *(int *)0=0;

         target->Add(child,pos);
      }
      mHintWindow->HideHints();
   }
}





Skin *Manager::GetSkin()
{
   if (!mSkin)
      mSkin = CreateDefaultSkin();
   return mSkin;
}


class FloatingFrameHandler : public wxEvtHandler
{
public:
   FloatingFrameHandler(wxFrame *inFrame,Manager *inManager)
   {
      mFrame = inFrame;
      mFrame->PushEventHandler(this);
      mDragging = false;
      mMoves = 0;
      mManager = inManager;
      mContainer = 0;
      mShown = false;
      mOnTop = inFrame->GetWindowStyle() &
        (wxSTAY_ON_TOP | wxFRAME_FLOAT_ON_PARENT);
   }
   void SetContainer(Container *inContainer,const wxRect &inRect)
   {
      mContainer = inContainer;
      mShowRect = inRect;
      mShown = false;
      mFrame->SetClientSize(inRect);
   }
   void MoveWithMouse()
   {
      mDragging = true;
      if (!mOnTop)
         mFrame->SetWindowStyle( mFrame->GetWindowStyle() | wxSTAY_ON_TOP );
      
      mFrame->CaptureMouse();
      mMouse0 = wxGetMousePosition();
      mFrame->SetPosition( mMouse0 + wxPoint(-20,-5) );
   }

   void OnShow(wxShowEvent &inEvent)
   {
      if (!mShown)
      {
         mBorders = mFrame->GetSize() - mFrame->GetClientSize();
         mShown = true;
         wxSize size = mShowRect.GetSize();
         mContainer->GetPreferredSize(size,true);
         mFrame->SetClientSize(size);
      }
   }

   void UpdateHints(wxPoint inPos)
   {
      mManager->HintsFromMouse(mContainer,inPos);
   }

   void OnMouseMove(wxMouseEvent &inEvent)
   {
      if (mDragging)
      {
         // TODO: Set "OnTop" flag
         wxPoint p = wxGetMousePosition();
         mFrame->SetPosition( mFrame->GetPosition() - mMouse0 + p );
         mMouse0 = p;
         UpdateHints(p);
         if (!inEvent.LeftIsDown())
         {
            mDragging = false;
            if (!mOnTop)
               mFrame->SetWindowStyle(
                   mFrame->GetWindowStyle() & ~wxSTAY_ON_TOP );
            mMoves = 0;
            mFrame->ReleaseMouse();
         }
      }
   }

   void OnLeftUp(wxMouseEvent &inEvent)
   {
      if (mDragging)
      {
         mDragging = false;
         if (!mOnTop)
            mFrame->SetWindowStyle( mFrame->GetWindowStyle() & ~wxSTAY_ON_TOP );
         mMoves = 0;
         mFrame->ReleaseMouse();

         if (true)
            mManager->TryDropFrame(mFrame,mContainer,wxGetMousePosition());
         else
            mManager->HideHints();
      }
   }

   void OnWindowMove(wxMoveEvent &inEvent)
   {
      if (!mDragging)
      {
         wxRect r = mFrame->GetRect();

         if (mMoves == 0 ||r.GetSize()==mPrevRect.GetSize())
            mMoves++;
         else
            mMoves = 0;

         if (mMoves>1)
         {
            UpdateHints( wxGetMousePosition() );
         }

         mPrevRect = r;
      }
      inEvent.Skip();
   }


   void CheckMoveFinished(wxIdleEvent &inEvent)
   {
      if (!wxGetMouseState().LeftIsDown())
      {
         if (mMoves > 1)
         {
            if (true)
               mManager->TryDropFrame(mFrame,mContainer, wxGetMousePosition());
            else
               mManager->HideHints();
         }
         mMoves = 0;
      }
   }

   void OnClose(wxCloseEvent &inEvent)
   {
      inEvent.Veto();
      if (mContainer->CanClose())
         mManager->CloseFloating(mFrame,mContainer);
   }

   void OnSizing(wxSizeEvent &inEvent)
   {
      bool width_diff = inEvent.m_rect.width != mFrame->GetSize().x+1;

      wxSize size = inEvent.m_rect.GetSize()-mBorders;
      if (mContainer->GetPreferredSize(size,width_diff))
      {
         size+=mBorders;
         // +1 = seems the target rect must be bigger?
         inEvent.m_rect.width = size.x + 1;
         inEvent.m_rect.height = size.y + 1;
      }

      //inEvent.Skip();
   }

   // too hard - we don't have focus - may need a timer or something
   /*
   void OnKey(wxKeyEvent &inEvent)
   {
      inEvent.Skip();
      if (mDragging || mMoves>1)
         UpdateHints( wxGetMousePosition() );
   }
   */


   wxFrame *mFrame;
   Manager *mManager;
   Container *mContainer;
   bool       mShown;
   wxRect     mShowRect;
   bool mDragging;
   bool mOnTop;
   wxPoint mMouse0;
   int     mMoves;
   wxRect  mPrevRect;
   wxSize  mBorders;

   DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(FloatingFrameHandler,wxEvtHandler)
   EVT_MOTION(FloatingFrameHandler::OnMouseMove)
   EVT_LEFT_UP(FloatingFrameHandler::OnLeftUp)
   EVT_MOVE(FloatingFrameHandler::OnWindowMove)
   EVT_IDLE(FloatingFrameHandler::CheckMoveFinished)
   EVT_SHOW(FloatingFrameHandler::OnShow)
   EVT_CLOSE(FloatingFrameHandler::OnClose)
   EVT_SIZING(FloatingFrameHandler::OnSizing)
END_EVENT_TABLE()




class ManagedMiniFrame: public wxMiniFrame
{
public:

   static unsigned int GetStyle(Container &inContainer)
   {
      unsigned int result =
             wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION |
                wxFRAME_NO_TASKBAR | wxCLOSE_BOX |
                wxCLIP_CHILDREN;
 
      if (!inContainer.CanClose())
         result &= ~wxCLOSE_BOX;
      return result;
   }


   ManagedMiniFrame(wxFrame *inParent,Container &inContainer) :
         wxMiniFrame(inParent,-1,"",wxDefaultPosition,wxDefaultSize,
            GetStyle(inContainer) )
   {

   /*
   long style = wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION |
                wxFRAME_NO_TASKBAR |
                wxCLIP_CHILDREN |
                (pane_info.HasCloseButton()?wxCLOSE_BOX:0) |
                (pane_info.HasMaximizeButton()?wxMAXIMIZE_BOX:0) |
                (pane_info.HasMinimizeButton()?wxMINIMIZE_BOX:0) |
                (pane_info.IsFixed()?0:wxRESIZE_BORDER);
   // frame->SetExtraStyle(wxWS_EX_PROCESS_IDLE);
   */


      SetExtraStyle(wxWS_EX_PROCESS_IDLE);
   }
};

class ManagedNormalFrame: public wxFrame
{
public:
   ManagedNormalFrame(wxFrame *inParent,Container &inContainer) :
         wxFrame(inParent,-1,"myNormap",wxDefaultPosition, wxDefaultSize,
            GetStyle(inContainer) )
   {
      SetExtraStyle(wxWS_EX_PROCESS_IDLE);
   }

   static unsigned int GetStyle(Container &inContainer)
   {
      unsigned int result = wxDEFAULT_FRAME_STYLE;
      if (!inContainer.CanClose())
         result &= ~wxCLOSE_BOX;
      return result;
   }

   /*
   long style = wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION |
                wxFRAME_NO_TASKBAR |
                wxCLIP_CHILDREN |
                (pane_info.HasCloseButton()?wxCLOSE_BOX:0) |
                (pane_info.HasMaximizeButton()?wxMAXIMIZE_BOX:0) |
                (pane_info.HasMinimizeButton()?wxMINIMIZE_BOX:0) |
                (pane_info.IsFixed()?0:wxRESIZE_BORDER);
   // frame->SetExtraStyle(wxWS_EX_PROCESS_IDLE);
   */


};



void Manager::FloatContainer(Container *inContainer)
{
   HideHints();

   wxFrame *frame = 0;
   
   if (inContainer->ShouldFloatFullFrame())
      frame = new ManagedNormalFrame(mFrame,*inContainer);
   else
      frame = new ManagedMiniFrame(mFrame,*inContainer);

   FloatingFrameHandler *handler = new FloatingFrameHandler(frame,this);

   Container *inside = Create(frame,true);
   handler->SetContainer(inside,inContainer->GetSize(stFloating) );

   mTopLevelContainers.push_back(inside);

   inContainer->Reparent(inside);

   inside->Add(inContainer,apOver);
   inside->SetRect( inside->Rect() );
   inContainer->Show(true);

   wxPoint pos = wxGetMousePosition();
   frame->SetPosition(pos-wxPoint(10,5));
   frame->Show(true);

   handler->MoveWithMouse();

}

// --- Serialization --------------------------------------------

void ToSerial(const wxFrame *inFrame, Serializer &outSerializer)
{
   wxRect r = inFrame->GetRect();
   outSerializer["x"] = r.x;
   outSerializer["y"] = r.y;
   outSerializer["w"] = r.width;
   outSerializer["h"] = r.height;
   outSerializer["min"] = inFrame->IsIconized();
   outSerializer["max"] = inFrame->IsMaximized();
}

void FromSerial(wxFrame *inFrame, Serializer &inSerializer)
{
   wxRect r;
   r.x = inSerializer["x"];
   r.y = inSerializer["y"];
   r.width = inSerializer["w"];
   r.height = inSerializer["h"];
   int min = inSerializer["min"];
   int max = inSerializer["max"];
   if (min)
      inFrame->Iconize();
   else if (max)
      inFrame->Maximize();
   else
   {
      inFrame->Maximize(false);
      inFrame->SetSize(r);
   }
}



wxString Manager::ToSerial()
{
   Serializer serializer;

   clay::ToSerial(mFrame,serializer);

   mTopLevelContainers[0]->ToSerial(serializer["root"]);
   for(int i=1;i<mTopLevelContainers.size();i++)
   {
      mTopLevelContainers[i]->ToSerial(serializer[i]);
   }

   return serializer.ToSerial();
}


bool Manager::FromSerial(const wxString &inString)
{
   Serializer serializer;
   int pos = 0;
   if (!serializer.FromSerial(inString.c_str(),pos))
      return false;

   mMDIClientUsed = false;
   mWindows.swap(mTransfer);

   Container *new_root = Create(serializer["root"]);

   if (new_root)
   {
      mOldTopLevelContainers.swap(mTopLevelContainers);

      mRootContainer = new_root;
      mTopLevelContainers.push_back(mRootContainer);

      for(int i=1; serializer.HasKey(i); i++)
      {
         Container *child = Create(serializer[i]["child"]);

         wxFrame *frame =  serializer[i]["fullframe"] ?
             (wxFrame *)new ManagedNormalFrame(mFrame,*child) :
             (wxFrame *)new ManagedMiniFrame(mFrame,*child);

         FloatingFrameHandler *handler = new FloatingFrameHandler(frame,this);

         Container *inside = Create(frame,true);

         handler->SetContainer(inside,child->GetSize(stFloating) );

         child->Reparent(inside);

         inside->Add(child,apOver);

         mTopLevelContainers.push_back(inside);

         child->Show(true);

         clay::FromSerial(frame,serializer[i]["frame"]);
         frame->Show();
      }

      for(int i=0;i<mOldTopLevelContainers.size();i++)
         mOldTopLevelContainers[i]->DestroyOrClose();
      mOldTopLevelContainers.resize(0);

      // TODO: Destroy transfers too
   }
   else
   {
      mWindows.swap(mTransfer);
      return false;
   }

   clay::FromSerial(mFrame,serializer);

   mRootContainer->DoLayout();

   return true;
}


} // end namespace clay

