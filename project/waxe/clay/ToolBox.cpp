#include <wx/wx.h>
#include <wx/dcmemory.h>
#include <memory>
#include <algorithm>

#include <clay/ToolBox.h>
#include <clay/Skin.h>

namespace clay
{

class ToolButton : public wxControl
{
   enum State { sNormal, sHover, sDown, sDisable };
   static const int Borders = 2;
   static const int TextGap = 2;
   static const bool ShowText = true;

public:
   ToolButton(wxWindow *inWindow, int inID, const wxString &inLabel,
          const wxBitmap &inBitmap, const wxString &inTip,
          wxItemKind inKind,bool inIsDown=false,
          ToolButtonLabel inLabelPos=tblNone) :
       wxControl(inWindow,inID,wxPoint(0,0),wxSize(10,10), wxBORDER_NONE)
   {
      mBGTop = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
      mBGBottom = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
      mDownCol = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
      mOverCol = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);

      mKind = inKind;
      mState = sNormal;
      mLabel = inLabel;
      mLabelPos = inLabelPos;
      mToggled = false;

      if ( (mKind==wxITEM_RADIO || mKind==wxITEM_CHECK)  && inIsDown)
      {
         mState = sDown;
         mToggled = true;
      }


      mTip = inTip;
      mCapture = false;

      mBase = inBitmap;
      for(int i=0;i<4;i++)
         mBitmaps[i] = CreateBitmap(mBase,i);

      int w = mBitmaps[0].GetWidth();
      int h = mBitmaps[0].GetHeight();
      if (w>1 && h>1)
      {
         mAllBitmap = true;
         SetSize( w, h );
      }
      else
      {
         mAllBitmap = false;
         SetSize(16,16);
      }

      if (!mTip.empty())
         SetToolTip(mTip);
   }

   wxBitmap CreateBitmap(const wxBitmap &inBase, int inState,
              ToolBoxSkin *inSkin=0, const wxRect &inTBRect=wxRect(),
              const wxPoint &inPos=wxPoint())
   {
      int x = Borders;
      int y = Borders;
      int w = inBase.GetWidth();
      int h = inBase.GetHeight();

      wxMemoryDC dc;
      int tx = Borders;
      int ty = Borders;
      wxSize text_size(0,0);

      if (mLabelPos!=tblNone && !mLabel.empty())
      {
         wxBitmap temp(20,20);
         dc.SelectObject(temp);
         dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
         text_size = dc.GetTextExtent(mLabel);

         int gap = inBase.IsOk() ? TextGap : 0;

         if (mLabelPos == tblRight)
         {
            tx+=w+gap;
            w+=text_size.x+gap;

            if (h > text_size.y)
               ty += (h-text_size.y)/2;
            else
            {
               y+=(text_size.y-h)/2;
               h = text_size.y;
            }
         }
         else
         {
            ty+=h+gap;
            h+=text_size.y+gap;

            if (w > text_size.x)
               tx += (w-text_size.x)/2;
            else
            {
               x+=(text_size.x-w)/2;
               w = text_size.x;
            }
         }

      }

      w+=2*Borders;
      h+=2*Borders;

      wxBitmap result(w,h);
      dc.SelectObject(result);
      dc.SetPen( *wxTRANSPARENT_PEN );

      if (inState==sHover)
      {
         wxColour bg(mBGTop);
         wxColour c0(   (bg.Red() + mOverCol.Red())/2,
                        (bg.Green() + mOverCol.Green())/2,
                        (bg.Blue() + mOverCol.Blue()) );
         bg = wxColour(mBGBottom);
         wxColour c1(   (bg.Red() + mOverCol.Red())/2,
                        (bg.Green() + mOverCol.Green())/2,
                        (bg.Blue() + mOverCol.Blue()) );
         wxRect r(0,0,w,h);
         dc.GradientFillLinear(r,c0,c1,wxSOUTH);
      }
      else if (inState==sDown)
      {
         dc.SetBrush( wxBrush(mDownCol) );
         dc.DrawRectangle(0,0,w,h);
      }
      else if (inSkin)
      {
         dc.SetDeviceOrigin(-inPos.x,-inPos.y);
         inSkin->RenderToolbarSubRect(dc,inTBRect, wxRect(inPos,wxSize(w,h)) );
         dc.SetDeviceOrigin(0,0);
      }
      else
      {
         dc.SetBrush( wxBrush(mBGTop) );
         dc.DrawRectangle(0,0,w,h);
      }

      if (inState==sHover || inState==sDown)
      {
         dc.SetPen( wxPen(wxColour(mOverCol)) );
         dc.SetBrush( *wxTRANSPARENT_BRUSH );
         dc.DrawRectangle(0,0,w,h);
      }

      if (inState == sDown)
      {
         x++;
         y++;
         tx++;
         ty++;
      }

      if (text_size.x>0)
      {
         dc.DrawText(mLabel,tx,ty);
      }


      dc.DrawBitmap(inBase,x,y,true);
      return result;
   }

   void SetBackground(ToolBoxSkin *inSkin,const wxRect &inTBRect,
             const wxPoint &inPos)
   {
      for(int i=0;i<4;i++)
         mBitmaps[i] = CreateBitmap(mBase,i,inSkin,inTBRect,inPos);

   }

   void OnErase(wxEraseEvent &inEvt) { if (!mAllBitmap) inEvt.Skip(); }

   void OnPaint(wxPaintEvent &inEvent)
   {
      wxPaintDC dc(this);
      if (mAllBitmap)
         dc.DrawBitmap(mBitmaps[mState],0,0);
   }

   void OnMotion(wxMouseEvent &inEvent)
   {
      State old = mState;

      if (inEvent.LeftIsDown())
      {
         if (mCapture)
         {
            wxSize s = GetSize();
            mState = ( wxRect(0,0,s.x,s.y).Contains( inEvent.GetPosition() )) ?
                        sDown : (mToggled ? sDown:sNormal);
         }
      }
      else
      {
         if (mCapture) // hmm ...
         {
            mCapture = false;
            ReleaseMouse();
            mState = mToggled ? sDown : (mToggled ? sDown : sNormal);
         }

         if (mState!=sDown && mState!=sHover)
         {
            mState = sHover;
         }

      }

      if (old!=mState)
         Refresh();

      inEvent.Skip();
   }

   void OnEnter(wxMouseEvent &inEvent)
   {
      wxCommandEvent event(wxEVT_COMMAND_TOOL_ENTER,GetId());
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
   }

   void OnLeftDown(wxMouseEvent &inEvent)
   {
      mState = sDown;
      mCapture = true;
      CaptureMouse();
      Refresh();
   }

   void OnLeftUp(wxMouseEvent &inEvent)
   {
      ReleaseMouse();
      mCapture = false;
      wxSize s = GetSize();
      wxPoint pos = inEvent.GetPosition();
      if ( wxRect(0,0,s.x,s.y).Contains(pos) )
      {
         if (mKind==wxITEM_CHECK)
         {
            mToggled = !mToggled;
         }

         // Click !
         wxCommandEvent event(wxEVT_COMMAND_TOOL_CLICKED,GetId());
         event.SetEventObject(this);
         GetEventHandler()->ProcessEvent(event);
      }

      mState = mToggled ? sDown : sNormal;
      Refresh();
   }

   void OnLeave(wxMouseEvent &inEvent)
   {
      wxCommandEvent event(wxEVT_COMMAND_TOOL_ENTER,-1);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);

      if (mState==sHover)
      {
         mState = mToggled ? sDown : sNormal;

         Refresh();
      }
   }

   void SetToggled(bool inToggle)
   {
      if (mKind==wxITEM_RADIO || mKind==wxITEM_CHECK)
      {
         if (mToggled!=inToggle)
         {
            mToggled = inToggle;
            mState = mToggled ? sDown : sNormal;
            Refresh();
         }
      }
   }

   bool GetToggled() const { return mToggled; }

   bool     mAllBitmap;
   bool     mCapture;
   wxBitmap mBase;
   wxBitmap mBitmaps[4];
   wxString mLabel;

   wxItemKind mKind;
   bool       mToggled;

   State    mState;
   ToolButtonLabel mLabelPos;
   wxColour mBGTop;
   wxColour mBGBottom;
   wxColour mDownCol;
   wxColour mOverCol;
   wxString mTip;

   DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(ToolButton,wxControl)
   EVT_ERASE_BACKGROUND(ToolButton::OnErase)
   EVT_PAINT(ToolButton::OnPaint)

   EVT_MOTION(ToolButton::OnMotion)
   EVT_LEFT_DOWN(ToolButton::OnLeftDown)
   EVT_LEFT_UP(ToolButton::OnLeftUp)
   EVT_LEAVE_WINDOW(ToolButton::OnLeave)
   EVT_ENTER_WINDOW(ToolButton::OnEnter)
END_EVENT_TABLE()



class ToolBoxTool
{
public:
   // Generic control
   ToolBoxTool(ToolBox *inParent,wxWindow *inControl) :
        mParent(inParent), mControl(inControl)
   {
      mTool = 0;
      mGroupID = -1;
      mSize = inControl->GetSize();
      mID = inControl->GetId();
   }

   // ToolButton control
   ToolBoxTool(ToolBox *inParent,ToolButton *inTool,int inGroupID) :
        mParent(inParent)
   {
      mControl = inTool;
      mTool = inTool;
      mSize = mTool->GetSize();
      mID = inTool->GetId();
      mGroupID = inGroupID;
   }

   // Separator
   ToolBoxTool(ToolBox *inParent) : mParent(inParent)
   {
      mTool = 0;
      mControl = 0;
      mGroupID = -1;
      mSize = wxSize(1,1);
      mID = -1;
   }

   bool IsSeparator() const { return mControl == 0; }

   void ApplyLayout()
   {
      if (mControl)
         mControl->SetSize(mLayoutPos);
   }

   void SetToggled(int inToggle)
   {
      if (mTool)
         mTool->SetToggled(inToggle);
   }

   void SetHorizontalSeparatorSize(int inSize)
   {
      if (IsSeparator() && mLayoutPos.width > mLayoutPos.height )
         mLayoutPos.width = inSize;
   }

   bool GetToggled() const
   {
      return mTool!=0 && mTool->GetToggled();
   }

   wxSize GetSize() { return mSize; }


   wxSize      mSize;
   wxRect      mLayoutPos;
   int         mID;
   int         mGroupID;
   ToolBox     *mParent;
   ToolButton  *mTool;
   wxWindow    *mControl;
};


typedef std::vector<ToolBoxTool *> ToolBoxTools;


ToolBox::ToolBox(wxWindow *inParent,const wxString &inName) :
       wxPanel(inParent,-1,wxDefaultPosition,wxDefaultSize, wxCLIP_CHILDREN, inName)
{
}


class ToolBoxImpl : public ToolBox
{
   static const int HGap = 2;
   static const int VGap = 2;
   static const int Borders = 2;
   static const int Separator = 1;

public:
   ToolBoxImpl(wxWindow *inParent,ToolButtonLabel inPos, bool inRadioPreview,
          ToolBoxSkin *inSkin,const wxString &inName) :
       ToolBox(inParent,inName),
            mLabelPos(inPos), mRadioPreview(inRadioPreview),
            mSkin(inSkin)
   {
      mGroupID = -1;
      mMaxGroupID = 0;

      mHGap = HGap;
      mVGap = VGap;
      mBorders = Borders;

      mRestoreGroup = -1;
      mRestoreTool = -1;
      mSeparatorSize = 1;

      mPrevHeight = -1;
   }

   operator wxWindow *() { return this; }


   ToolBoxTool *AddControl(wxWindow *inControl)
   {
      ToolBoxTool * result = new ToolBoxTool(this,inControl);
      mTools.push_back( result);
      mGroupID = -1;
      return result;
   }

   ToolBoxTool* AddTool(int inToolID, const wxString& label,
           const wxBitmap& bitmap1,
           const wxString& shortHelpString = "",
           wxItemKind kind = wxITEM_NORMAL)
   {
      bool down = false;
      if (kind==wxITEM_RADIO)
      {
         if (mGroupID<0)
         {
            mGroupID = mMaxGroupID++;
            down = true;
         }
      }
      else
         mGroupID = -1;

      ToolButton *tool = new ToolButton(this,inToolID,label,bitmap1,
                          shortHelpString, kind, down, mLabelPos);

      ToolBoxTool * result = new ToolBoxTool(this,tool,mGroupID);

      mTools.push_back(result);

      return result;
   }

   void AddSeparator()
   {
      if (mTools.size()>0 &&  mTools[mTools.size()-1]->IsSeparator())
         return;

      mTools.push_back( new ToolBoxTool(this) );
   }



   void CentreOnRow(int inT0,int inTN,int inY, int inHeight)
   {
      for(int t=0; t<inTN; t++)
      {
         ToolBoxTool &tool = *mTools[inT0 + t];
         if (tool.IsSeparator())
         {
            tool.mLayoutPos.y = inY;
            tool.mLayoutPos.height = inHeight;
         }
         else
            tool.mLayoutPos.y += (inHeight - tool.mLayoutPos.height)/2;
      }
   }

   void LayoutRects(wxSize &ioSize)
   {
      int allow_width =  ioSize.x - mBorders;
      int x = mBorders;
      int y = mBorders;
      int max_w = 0;
      int max_h = 0;
      int on_row = 0;
      int prev_separator_size = 0;
      int rows = 1;

      for(int t=0;t<mTools.size();t++)
      {
         ToolBoxTool &tool = *mTools[t];
         if (tool.IsSeparator())
         {
            prev_separator_size = mHGap + mSeparatorSize;
            on_row++;
            continue;
         }


         wxSize s= tool.GetSize();

         // New row ?
         int right = x + prev_separator_size + s.x;
         if (on_row && (right > allow_width) )
         {
            // finish old row ...
            x+= mBorders-mHGap;
            max_w = std::max(max_w,x);
            if (on_row>1)
               CentreOnRow(t-on_row,on_row,y,max_h);
            y+=max_h + mVGap;

            // start new row
            x = mBorders;

            // Convert vertical separator, which wouldn't fit, into horizontal
            //  separator ...
            if (prev_separator_size)
            {
               mTools[t-1]->mLayoutPos = wxRect(x,y,0,0);
               //mTools[t-1]->mLayoutPos = wxRect(x,y,max_w,1);
               //y+=1+mVGap;
               prev_separator_size = 0;
            }

            on_row = 0;
            max_h = 0;
            rows++;
         }

         // Add previous separator we delayed until here ...
         if (prev_separator_size)
         {
            mTools[t-1]->mLayoutPos = wxRect(x,y,1,10);
            x+=prev_separator_size;
            prev_separator_size = 0;
         }

         tool.mLayoutPos = wxRect(x,y,s.x,s.y);
         if (max_h<s.y)  max_h = s.y;
         x+=s.x + mHGap;
         on_row++;
      }

      if (prev_separator_size)
      {
         // TODO:
      }

      if (on_row>1)
         CentreOnRow(mTools.size()-on_row,on_row,y,max_h);

      x+= mBorders - mHGap;
      max_w = std::max(max_w,x);
      if (rows>1)
         y-=VGap;

      y+=max_h + mBorders;

      for(int i=0;i<mTools.size();i++)
         mTools[i]->SetHorizontalSeparatorSize(max_w);

      ioSize = wxSize( max_w, y );
   }

   void DoLayout(wxSize &ioSize,bool inFitWidth,bool inRecurse)
   {
      if (inFitWidth)
      {
         LayoutRects(ioSize);
         if (inRecurse)
            DoLayout(ioSize,false,false);
      }
      else  /* Fit height */
      {
         // Get max width ...
         wxSize s(0x7fffffff,1);
         LayoutRects(s);

         wxSize result = s;
         // Find the thinest one that fits ...
         while(s.y <= ioSize.y)
         {
            result = s;
            s.x--;
            LayoutRects(s);
            if (s.x==result.x)
               break;
         }
         ioSize= result;

         if (inRecurse)
            DoLayout(ioSize,true,false);
         else if (s.y>ioSize.y)
         // Gone too far go back one
         {
            DoLayout(ioSize,true,false);
         }


      }
   }

   void DoLayout(wxSize &ioSize,bool inFitWidth)
   {
      DoLayout(ioSize,inFitWidth,true);
   }

   void ApplyLayout()
   {
      for(int i=0;i<mTools.size();i++)
         mTools[i]->ApplyLayout();
      SetBackgrounds();
      Refresh();
   }

   void Realize()
   {
      wxSize min_size;
      wxSize size(1,1);
      // layout vertically
      DoLayout(size,true);
      min_size.x = size.x;

      // layout horizontally
      size = wxSize(1,1);
      DoLayout(size,false);
      min_size.y = size.y;

      ApplyLayout();
      SetSize(size);
      SetMinSize(min_size);
   }

   void SetBackgrounds()
   {
      if (!mSkin)
         return;

      wxSize size = GetClientSize();
      if (size!=mLastSize)
      {
         wxRect r( wxPoint(0,0), size );
         mLastSize = size;
         for(int t=0;t<mTools.size();t++)
         {
            ToolButton *tool = mTools[t]->mTool;
            if (tool)
               tool->SetBackground(mSkin,r,tool->GetPosition());
         }
      }
   }

   void OnSize(wxSizeEvent &inEvent)
   {
      inEvent.Skip();
      wxSize size = GetClientSize();
      DoLayout(size,size.y!=mPrevHeight);
      mPrevHeight = size.y;
      ApplyLayout();
   }

   int FindTool(int inID) const
   {
      for(int t=0;t<mTools.size();t++)
         if (mTools[t]->mID == inID)
            return t;
      return -1;
   }

   void SetButtonGroup(int inGroupID,int inToolID)
   {
      for(int t=0;t<mTools.size();t++)
      {
         ToolBoxTool &tool = *mTools[t];
         if (tool.mGroupID==inGroupID)
            tool.SetToggled(tool.mID == inToolID);
      }
   }

   bool GetToolState(int toolId) const
   {
      int t = FindTool(toolId);
      if (t<0)
         return false;
      return mTools[t]->GetToggled();
   }

   void Restore()
   {
      if (mRestoreGroup>=0 && FindTool(mRestoreTool))
      {
         SetButtonGroup(mRestoreGroup,mRestoreTool);

         wxCommandEvent event(wxEVT_COMMAND_TOOL_CLICKED,mRestoreTool);
         mRestoreGroup = mRestoreTool = -1;

         GetParent()->GetEventHandler()->ProcessEvent(event);
      }
   }


   void OnTool(wxCommandEvent &inEvent)
   {
      inEvent.Skip();


      int tid = FindTool(inEvent.GetId());
      if (tid>=0)
      {
         ToolBoxTool &tool = *mTools[tid];
         if (tool.mGroupID>=0)
         {
            if (tool.mGroupID==mRestoreGroup)
               mRestoreGroup = -1;

            SetButtonGroup(tool.mGroupID,inEvent.GetId());
         }
      }

      Restore();
   }

   int GetGroupTool(int inGroupID)
   {
      for(int t=0;t<mTools.size();t++)
      {
         ToolBoxTool &tool = *mTools[t];
         if (tool.mGroupID==inGroupID && tool.GetToggled())
            return tool.mID;
      }
      return -1;
   }

   void OnToolEnter(wxCommandEvent &inEvent)
   {
      // Hold the button down, but switch back if they move their mouse away
      if (mRadioPreview)
      {
         int id = inEvent.GetId();
         if (id>0)
         {
            int pos = FindTool(id);
            if (pos>=0)
            {
               ToolBoxTool &tool = *mTools[pos];
               if (tool.mGroupID>=0)
               {
                  if (mRestoreGroup!=tool.mGroupID)
                     Restore();

                  int current = GetGroupTool(tool.mGroupID);
                  if (current!=tool.mID)
                  {
                     if (mRestoreGroup<0)
                     {
                        mRestoreTool = current;
                        mRestoreGroup = tool.mGroupID;
                     }

                     SetButtonGroup(mRestoreGroup,tool.mID);
                     wxCommandEvent event(wxEVT_COMMAND_TOOL_CLICKED,tool.mID);
                     GetParent()->GetEventHandler()->ProcessEvent(event);
                  }
               }
            }
         }
         else
            Restore();
      }
   }

   void OnErase(wxEraseEvent &inEvt) { }

   void OnPaint(wxPaintEvent &inEvent)
   {
      wxPaintDC dc(this);
      wxSize s = GetClientSize();

      if (mSkin)
         mSkin->RenderToolbarBackground(dc,wxRect(0,0,s.x,s.y));
      else
      {
         dc.SetPen( *wxTRANSPARENT_PEN );
         dc.SetBrush(wxBrush(
              wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)));
         dc.DrawRectangle( 0,0, s.x, s.y );
      }


      dc.SetPen( wxPen( wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW) ));

      for(int t=0;t<mTools.size();t++)
      {
         ToolBoxTool &tool = *mTools[t];
         if (tool.IsSeparator())
         {
            wxRect r = tool.mLayoutPos;
            if (r.width>r.height)
               dc.DrawLine(r.x,r.y,r.x+r.width,r.y);
            else
               dc.DrawLine(r.x,r.y,r.x,r.y+r.height);
         }
      }
   }



   ToolBoxTools mTools;
   int          mGroupID;
   int          mMaxGroupID;
   ToolBoxSkin  *mSkin;

   int          mHGap;
   int          mVGap;
   int          mBorders;
   int          mSeparatorSize;

   bool         mRadioPreview;
   int          mRestoreGroup;
   int          mRestoreTool;

   int          mPrevHeight;
   wxSize       mLastSize;

   ToolButtonLabel mLabelPos;

   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ToolBoxImpl,wxPanel)
   EVT_SIZE(ToolBoxImpl::OnSize)
   EVT_TOOL_ENTER(-1,ToolBoxImpl::OnToolEnter)
   EVT_TOOL(-1,ToolBoxImpl::OnTool)
   EVT_ERASE_BACKGROUND(ToolBoxImpl::OnErase)
   EVT_PAINT(ToolBoxImpl::OnPaint)
END_EVENT_TABLE()

ToolBox *ToolBox::Create(wxWindow *inParent,
                  ToolButtonLabel inLabelPos,
                  bool inRadioPreview,
                  ToolBoxSkin *inSkin,
                  const wxString &inName)
{
   return new ToolBoxImpl(inParent,inLabelPos,inRadioPreview,inSkin,inName);
}

} // end namespace clay
