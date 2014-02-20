// Code "borrowd" from wxAUI
#include "HintWindow.h"
#include <clay/Layout.h>

namespace clay
{

class RegionBasedHintWindow : public wxFrame,  public HintWindow
{
public:
    RegionBasedHintWindow()
        : wxFrame(0, -1, "", wxDefaultPosition, wxDefaultSize, wxFRAME_SHAPED )
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        m_Amount=0;
        m_MaxWidth=0;
        m_MaxHeight=0;
        m_lastWidth=0;
        m_lastHeight=0;
#ifdef __WXGTK__
        m_CanSetShape = false; // have to wait for window create event on GTK
#else
        m_CanSetShape = true;
#endif
        m_Region = wxRegion(0, 0, 0, 0);
        SetTransparent(0);
    }


    virtual bool SetTransparent(wxByte alpha)
    {
        if (m_CanSetShape)
        {
            int w=100; // some defaults
            int h=100;
            GetClientSize(&w, &h);

            m_MaxWidth = w;
            m_MaxHeight = h;
            m_Amount = alpha;
            m_Region.Clear();
//            m_Region.Union(0, 0, 1, m_MaxWidth);
            if (m_Amount)
            {
                for (int y=0; y<m_MaxHeight; y++)
                {
                    // Reverse the order of the bottom 4 bits
                    int j=((y&8)?1:0)|((y&4)?2:0)|((y&2)?4:0)|((y&1)?8:0);
                    if ((j*16+8)<m_Amount)
                        m_Region.Union(0, y, m_MaxWidth, 1);
                }
            }
            SetShape(m_Region);
            Refresh();
        }
        return true;
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        if (m_Region.IsEmpty())
            return;

#ifdef __WXMAC__
        dc.SetBrush(wxColour(128, 192, 255));
#else
        dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
#endif
        dc.SetPen(*wxTRANSPARENT_PEN);

        wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

        while (upd)
        {
            wxRect rect(upd.GetRect());
            dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

            upd++;
        }
    }

    void OnWindowCreate(wxWindowCreateEvent& WXUNUSED(event))
    {
        if (!m_CanSetShape)
        {
           m_CanSetShape=true;
           SetTransparent(0);
           Raise();
        }
    }

    void OnSize(wxSizeEvent& event)
    {
        // We sometimes get surplus size events
        if ((event.GetSize().GetWidth() == m_lastWidth) &&
            (event.GetSize().GetHeight() == m_lastHeight))
        {
            event.Skip();
            return;
        }
        m_lastWidth = event.GetSize().GetWidth();
        m_lastHeight = event.GetSize().GetHeight();

        SetTransparent(m_Amount);
        m_Region.Intersect(0, 0, event.GetSize().GetWidth(),
                           event.GetSize().GetHeight());
        SetShape(m_Region);
        Refresh();
        event.Skip();
    }

private:
    wxByte m_Amount;
    int m_MaxWidth;
    int m_MaxHeight;
    bool m_CanSetShape;
    int m_lastWidth,m_lastHeight;

    wxRegion m_Region;

    DECLARE_EVENT_TABLE()
};



BEGIN_EVENT_TABLE(RegionBasedHintWindow, wxFrame)
    EVT_PAINT(RegionBasedHintWindow::OnPaint)
    EVT_SIZE(RegionBasedHintWindow::OnSize)
    EVT_WINDOW_CREATE(RegionBasedHintWindow::OnWindowCreate)
END_EVENT_TABLE()


class TransparentHintWindow : public wxFrame
{
public:
    TransparentHintWindow(wxFrame *inParent) :
      wxFrame(inParent, wxID_ANY, wxEmptyString,
                 wxDefaultPosition, wxSize(1,1),
                 wxFRAME_TOOL_WINDOW |
                 wxFRAME_FLOAT_ON_PARENT |
                 wxFRAME_NO_TASKBAR |
                 wxNO_BORDER)
    {
       SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
    }
};


class BitmapFrame : public wxFrame
{
public:
   BitmapFrame(wxFrame *inParent,const wxBitmap &inBitmap) :
      wxFrame(inParent, wxID_ANY, wxEmptyString,
                 wxDefaultPosition,
                     wxSize(inBitmap.GetWidth(), inBitmap.GetHeight() ),
                 wxFRAME_TOOL_WINDOW |
                 wxFRAME_FLOAT_ON_PARENT |
                 wxFRAME_NO_TASKBAR |
                 wxNO_BORDER), mBitmap(inBitmap)
   {
   }

   void OnBitmapPaint(wxPaintEvent &inEvt)
   {
      wxPaintDC dc(this);
      dc.DrawBitmap(mBitmap,0,0);
   }

   void SetBitmap(const wxBitmap &inBitmap)
   {
      mBitmap = inBitmap;
      Refresh();
   }

   wxBitmap mBitmap;
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(BitmapFrame,wxFrame)
   EVT_PAINT(BitmapFrame::OnBitmapPaint)
END_EVENT_TABLE()


class HintWindowManager : public HintWindow
{
public:
   HintWindowManager(wxFrame *inParent)
   {
      mParent = inParent;
      mHints = new TransparentHintWindow(inParent);

      wxColour show(0,0,255);

      for(int i=0;i<4;i++)
         CreateBitmap(mBitmap[i],show,i);

      CreateTabBitmap(mBitmap[4],show);

      for(int i=0;i<5;i++)
        mWindowDocks[i] = new BitmapFrame(mHints,mBitmap[i]);

      for(int i=5;i<9;i++)
        mWindowDocks[i] = new BitmapFrame(mHints,mBitmap[i-5]);

      SetWindowDockPos(500,500);
   }
   ~HintWindowManager()
   {
      //if (mHints) mHints->Destroy();
   }

   void SetWindowDockPos(int inX,int inY)
   {
      inX -= 16;
      inY -= 16;
      mWindowDocks[apOver]->Move(inX,inY);
      mWindowDocks[apLeft]->Move(inX-32,inY);
      mWindowDocks[apRight]->Move(inX+32,inY);
      mWindowDocks[apAbove]->Move(inX,inY-32);
      mWindowDocks[apBelow]->Move(inX,inY+32);

      for(int i=0;i<5;i++)
      {
         if (!mWindowDocks[i]->IsVisible())
             mWindowDocks[i]->Show(true);
         //mWindowDocks[i]->Raise();
      }
   }

   void SetFrameDocks(const HintInfo &inInfo,wxRect inRect)
   {
      mWindowDocks[5+apLeft]->Move(inRect.x,
                                    inRect.y+inRect.height/2-16);
      mWindowDocks[5+apRight]->Move(inRect.x+inRect.width-32,
                                    inRect.y+inRect.height/2-16);
      mWindowDocks[5+apAbove]->Move(inRect.x+inRect.width/2-16,
                                    inRect.y);
      mWindowDocks[5+apBelow]->Move(inRect.x+inRect.width/2-16,
                                    inRect.y+inRect.height-32);

      for(int i=5;i<9;i++)
      {
         if (!mWindowDocks[i]->IsVisible())
             mWindowDocks[i]->Show(true);
         //mWindowDocks[i]->Raise();
      }
   }

   void HideFrameDocks()
   {
      for(int i=5;i<9;i++)
         mWindowDocks[i]->Show(false);
   }

   void HideContainerDocks()
   {
      for(int i=0;i<5;i++)
         mWindowDocks[i]->Show(false);
   }



   void CreateBitmap(wxBitmap &outBitmap,wxColour &inColour,int inDir)
   {
      wxMemoryDC dc;
      outBitmap = wxBitmap(32,32);
      dc.SelectObject(outBitmap);
      dc.SetBrush( *wxWHITE_BRUSH );
      dc.SetPen( wxPen(
        wxColour(inColour.Red()/3, inColour.Green()/3, inColour.Blue()/3) ) );

      dc.DrawRectangle(-1,-1,34,34);
      dc.DrawRectangle(1,1,30,30);
      dc.SetBrush( wxBrush(inColour) );

      switch(inDir)
      {
         case apLeft:
            dc.DrawRectangle(1,1,15,30);
            break;
         case apRight:
            dc.DrawRectangle(16,1,15,30);
            break;
         case apAbove:
            dc.DrawRectangle(1,1,30,15);
            break;
         case apBelow:
            dc.DrawRectangle(1,16,30,15);
            break;
      }
      wxBitmap empty;
      dc.SelectObject(empty);
   }

   void CreateTabBitmap(wxBitmap &outBitmap,wxColour inColour)
   {
      wxMemoryDC dc;
      outBitmap = wxBitmap(32,32);
      dc.SelectObject(outBitmap);
      dc.SetBrush( *wxWHITE_BRUSH );
      dc.SetPen( wxPen(
        wxColour(inColour.Red()/3, inColour.Green()/3, inColour.Blue()/3) ) );

      dc.DrawRectangle(-1,-1,34,34);
      dc.SetBrush( wxBrush(inColour) );

      wxPoint points[] = 
       { wxPoint(2,2), wxPoint(5,2), wxPoint( 7,4), wxPoint(30,4),
         wxPoint(30,28), wxPoint(2,28) };
      dc.DrawPolygon(6,points);

      wxBitmap bitmap;
      dc.SelectObject(bitmap);
   }

   void HideHints()
   {
       for(int i=0;i<9;i++)
          mWindowDocks[i]->Show(false);
       mHints->Show(false);
   }

   void SetHintInfo(const HintInfo &inInfo,wxPoint inMouse)
   {
      mInfo = inInfo;

      if (mInfo.mFrameContainer)
         SetFrameDocks(mInfo,mInfo.mFrameRect);
      else
         HideFrameDocks();

      wxPoint pos = mInfo.mCentrePoint;
      if (mInfo.mOverContainer)
         SetWindowDockPos( pos.x, pos.y );
      else
         HideContainerDocks();

      if (mInfo.mFrameContainer || mInfo.mOverContainer || mInfo.mDropContainer)
      {
          mHints->SetTransparent(100);
          if (!mHints->IsVisible())
             mHints->Show();
          mHints->Raise();
      }
      else
         return;

       bool set = false;

       for(int i=8;i>=0;i--)
       {
          if (mWindowDocks[i] && mWindowDocks[i]->IsVisible() &&
              mWindowDocks[i]->GetRect().Contains(inMouse))
          {
             SetGlobalRect(mInfo.mHintRect[i]);
             set = true;
             break;
          }
       }

       if (!set && mInfo.mDropContainer)
          SetGlobalRect(mInfo.mDropRect);
       else if (! mInfo.mDropContainer && !set)
          SetGlobalRect( wxRect(0,0,1,1) );

   }



   void SetGlobalRect(wxRect inRect)
   {
      mHints->Move(inRect.GetTopLeft());
      mHints->SetSize(inRect.width,inRect.height);
   }


   Container *GetTarget(wxPoint inPoint,AddPosition &outPosition)
   {
       if (!mHints || !mHints->IsVisible())
          return 0;

       for(int i=8;i>=0;i--)
       {
          if (mWindowDocks[i] && mWindowDocks[i]->IsVisible() &&
              mWindowDocks[i]->GetRect().Contains(inPoint))
          {
             outPosition = (AddPosition)(i % 5);
             return i<5 ? mInfo.mOverContainer : mInfo.mFrameContainer;
          }
       }

       if (mInfo.mDropContainer)
       {
          outPosition = mInfo.mDropWhere;
          return mInfo.mDropContainer;
       }

       return 0;
   }

   wxBitmap mBitmap[5];
   wxFrame *mParent;
   wxFrame *mHints;
   BitmapFrame *mWindowDocks[9];
   HintInfo   mInfo;
};



HintWindow *HintWindow::Create(wxFrame *inParent)
{
   return new HintWindowManager(inParent);
}

} // end namespacec clay
