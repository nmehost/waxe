#include <wx/app.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/spinctrl.h>
#include <wx/srchctrl.h>
#include <memory>
#include <map>

#include <clay/Layout.h>
#include <clay/ToolBox.h>
#include <clay/Skin.h>
#include <clay/Serializer.h>

using namespace clay;

enum
{
   FILE_EXIT = wxID_HIGHEST+1,
   ID_COMBO,
   ID_SPIN,
   ID_VIEW_LAYOUT_SAVE,
   ID_VIEW_LAYOUT_LOAD,
   ID_VIEW_WINDOW_BASE,
   ID_VIEW_WINDOW_MAX = ID_VIEW_WINDOW_BASE + 100,
};

typedef wxMDIParentFrame Frame;




class DrawBox : public wxWindow
{
public:
   DrawBox(wxWindow *inParent,int inX,int inY) : wxWindow(inParent,-1)
   {
      mX = inX;
      mY = inY;
   }

   void OnPaint(wxPaintEvent &inEvt)
   {
      wxPaintDC dc(this);
      dc.DrawLine(0,0,mX,mY);
      dc.DrawLine(0,mY,mX,0);
   }

   int mX,mY;
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(DrawBox,wxWindow)
   EVT_PAINT(OnPaint)
END_EVENT_TABLE()

class MyFrame : public Frame
{
public:
   MyFrame() :
     Frame(0, wxID_ANY, "Hello", wxDefaultPosition, wxSize(800,600),
        wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE)
   {
      mWindowsMenu = 0;
      mManager = 0;
      SetMenuBar(create_menubar());

      mManager = new Manager(this);

      mRows = 1;
      mCustomDisabled = false;


      Container *vsplit =
          mManager->GetRootContainer()->Add(mManager->Create(csVStack),apLeft);
 

      // Create the content window
      wxWindow  *box = new DrawBox(this,200,200);
      box->SetMinSize(wxSize(200,200));

      vsplit->Add(box,apBelow);
   
      wxTextCtrl *text =
         new wxTextCtrl(this, -1, "Conent 2", wxPoint(), wxSize(),
         wxTE_MULTILINE | wxHSCROLL | wxVSCROLL | wxCLIP_SIBLINGS);
      text->SetMinSize(wxSize(50,50));
      text->SetName("Unclosable");

      vsplit->Add(text,apBelow, CreateIcons(true,wxColour(255,0,0)),
          cfFloatFullFrame | cfCantClose);


      Container *tabs = vsplit->Add(mManager->Create(csNotebook),apBelow);

      text = new wxTextCtrl(this, -1, "Child Two", wxPoint(), wxSize(),
         wxTE_MULTILINE | wxHSCROLL | wxVSCROLL | wxCLIP_SIBLINGS);
      text->SetMinSize(wxSize(100,50));
      tabs->Add(text,apOver);
   
      for(int i=0;i<6;i++)
      {
         wxString txt;
         txt.Printf("Child %d",i);
         text =
            new wxTextCtrl(this, -1, txt, wxPoint(), wxSize(),
            wxTE_MULTILINE | wxHSCROLL | wxVSCROLL | wxCLIP_SIBLINGS);
         text->SetName(txt);
         text->SetMinSize(wxSize(50,100));
         tabs->Add(text,apOver,
             CreateIcons(false,wxColour(rand()&0xff,rand()&0xff,rand()&0xff)),
                 cfFloatFullFrame|cfDestroyOnClose);
      }

      for(int j=0;j<3;j++)
      {
         mToolbar = ToolBox::Create(this,tblNone,true, Manager::GetSkin(),
                      "Some Toolbar");
         PopulateToolbar(mToolbar);
         mToolbar->Realize();
         Container *tbc =
            mManager->GetRootContainer()->Add(mToolbar,apAbove);
      }

   }

   wxIconBundle CreateIcons(bool inCircle,wxColour inCol)
   {
      wxIconBundle result;

      for(int s=16;s<=64;s++)
      {
         wxMemoryDC dc;
         wxBitmap bmp(s,s);
         dc.SelectObject(bmp);
         dc.SetBrush( wxBrush( *wxWHITE_BRUSH ) );
         dc.SetPen( *wxTRANSPARENT_PEN );
         dc.DrawRectangle(0,0,s,s);
         dc.SetBrush( wxBrush( inCol ) );
         dc.SetPen( wxPen( *wxBLACK_PEN ) );
         if (inCircle)
            dc.DrawCircle(s/2,s/2,s/3);
         else
            dc.DrawRectangle(2,2,s-4,s-4);
         dc.SelectObject( wxBitmap() );
         bmp.SetMask( new wxMask(bmp,wxColour(255,255,255)) );
         wxIcon icon;
         icon.CopyFromBitmap(bmp);
         result.AddIcon(icon);
      }

      return result;
   }

   void PopulateToolbar(ToolBox* toolBar)
   {
       // Set up toolbar
       enum
       {
           Tool_new,
           Tool_open,
           Tool_save,
           Tool_copy,
           Tool_cut,
           Tool_paste,
           Tool_print,
           Tool_help,
           Tool_Max
       };
   
       wxBitmap toolBarBitmaps[Tool_Max];
   
       #define INIT_TOOL_BMP(bmp) \
           toolBarBitmaps[Tool_##bmp] = wxBITMAP(bmp)
   
       INIT_TOOL_BMP(new);
       INIT_TOOL_BMP(open);
       INIT_TOOL_BMP(save);
       INIT_TOOL_BMP(copy);
       INIT_TOOL_BMP(cut);
       INIT_TOOL_BMP(paste);
       INIT_TOOL_BMP(print);
       INIT_TOOL_BMP(help);
   
       int w = toolBarBitmaps[Tool_new].GetWidth(),
           h = toolBarBitmaps[Tool_new].GetHeight();
   
       // w *= 2; h *= 2;
   
       for ( size_t n = Tool_new; n < WXSIZEOF(toolBarBitmaps); n++ )
       {
          toolBarBitmaps[n] =
          wxBitmap(toolBarBitmaps[n].ConvertToImage().Scale(w, h));
       }
   
       // toolBar->SetToolBitmapSize(wxSize(w, h));
   
       toolBar->AddTool(wxID_NEW, _T("New"),
                        toolBarBitmaps[Tool_new], wxNullBitmap, wxITEM_NORMAL,
                        _T("New file"), _T("This is help for new file tool"));
       toolBar->AddTool(wxID_OPEN, _T("Open"),
                        toolBarBitmaps[Tool_open], wxNullBitmap, wxITEM_NORMAL,
                        _T("Open file"), _T("This is help for open file tool"));

       // adding a combo to a vertical toolbar is not very smart
       //if ( !( toolBar->IsVertical() ) )
       {
           wxComboBox *combo = new wxComboBox(*toolBar, ID_COMBO, wxEmptyString, wxDefaultPosition, wxSize(100,-1) );
           combo->Append(_T("This"));
           combo->Append(_T("is a"));
           combo->Append(_T("combobox"));
           combo->Append(_T("in a"));
           combo->Append(_T("toolbar"));
           toolBar->AddControl(combo);
   
           wxSpinCtrl *spin = new wxSpinCtrl( *toolBar, ID_SPIN, wxT("0"), wxDefaultPosition, wxSize(80,wxDefaultCoord), 0, 0, 100 );
           toolBar->AddControl( spin );
           
           wxTextCtrl *text = new wxTextCtrl( *toolBar, -1, wxT("text"), wxDefaultPosition, wxSize(80,wxDefaultCoord) );
           toolBar->AddControl( text );
           
           wxSearchCtrl *srch = new wxSearchCtrl( *toolBar, -1, wxT("xx"), wxDefaultPosition, wxSize(80,wxDefaultCoord), wxSUNKEN_BORDER );
           toolBar->AddControl( srch );
       }
   
       toolBar->AddTool(wxID_SAVE, _T("Save"), toolBarBitmaps[Tool_save], _T("Toggle button 1"), wxITEM_CHECK);
       toolBar->AddTool(wxID_COPY, _T("Copy"), toolBarBitmaps[Tool_copy], _T("Toggle button 2"), wxITEM_CHECK);
       toolBar->AddTool(wxID_CUT, _T("Cut"), toolBarBitmaps[Tool_cut], _T("Toggle/Untoggle help button"));
       toolBar->AddTool(wxID_PASTE, _T("Paste"), toolBarBitmaps[Tool_paste], _T("Paste"));
   
       if ( mCustomDisabled )
       {
           /*
           wxBitmap bmpDisabled(w, h);
           {
               wxMemoryDC dc;
               dc.SelectObject(bmpDisabled);
               dc.DrawBitmap(toolBarBitmaps[Tool_print], 0, 0);
   
               wxPen pen(*wxRED, 5);
               dc.SetPen(pen);
               dc.DrawLine(0, 0, w, h);
           }
   
           toolBar->AddTool(wxID_PRINT, _T("Print"), toolBarBitmaps[Tool_print],
                            bmpDisabled);
           */
       }
       else
       {
           toolBar->AddTool(wxID_PRINT, _T("Print"), toolBarBitmaps[Tool_print],
                            _T("Delete this tool. This is a very long tooltip to test whether it does the right thing when the tooltip is more than Windows can cope with."));
       }
   
       toolBar->AddSeparator();
       toolBar->AddTool(wxID_HELP, _T("Help"), toolBarBitmaps[Tool_help], _T("Help button"), wxITEM_CHECK);
   
       if ( !m_pathBmp.empty() )
       {
           // create a tool with a custom bitmap for testing
           wxImage img(m_pathBmp);
           if ( img.Ok() )
           {
               if ( img.GetWidth() > w && img.GetHeight() > h )
                   img = img.GetSubImage(wxRect(0, 0, w, h));
   
               toolBar->AddSeparator();
               toolBar->AddTool(wxID_ANY, _T("Custom"), img);
           }
       }
   
       // after adding the buttons to the toolbar, must call Realize() to reflect
       // the changes
       toolBar->Realize();
   
       //toolBar->SetRows(!(toolBar->IsVertical()) ? mRows : 10 / mRows);
   }




   wxMenuBar *create_menubar()
   {
      wxMenuBar *mb = new wxMenuBar;
   
      // File menu
      wxMenu *file = new wxMenu;
      file->Append(FILE_EXIT, wxT("E&xit"));
      mb->Append(file, wxT("&File"));

      wxMenu *view = new wxMenu;
      mWindowsMenu = new wxMenu;
      view->AppendSubMenu(mWindowsMenu,"&Windows");
      view->AppendSeparator();
      view->Append(ID_VIEW_LAYOUT_SAVE,"&Save Layout");
      view->Append(ID_VIEW_LAYOUT_LOAD,"&Load Layout");
      mb->Append(view,"&View");
      CreateViewWindowMenu();
   
      return mb;
   }

   void CreateViewWindowMenu()
   {
      if (mWindowsMenu && mManager)
      {
         int items = mWindowsMenu->GetMenuItemCount();
         for(int i=0;i<items;i++)
            mWindowsMenu->Destroy(ID_VIEW_WINDOW_BASE + i);
         mWindowIDs.clear();

         const ContainerMap c = mManager->GetWindowMap();
         int id = ID_VIEW_WINDOW_BASE;
         for(ContainerMap::const_iterator i = c.begin();i!=c.end();++i)
         {
            mWindowIDs[id] = i->first;
            mWindowsMenu->Append(id++,i->second->GetCaption());
         }
      }

   }

   void OnWindowListChanged(wxCommandEvent &inEvt)
   {
      CreateViewWindowMenu();
   }

   void OnViewWindow(wxCommandEvent &inEvent)
   {
      int id = inEvent.GetId();

      const ContainerMap c = mManager->GetWindowMap();
      ContainerMap::const_iterator i = c.find(mWindowIDs[id]);
      if (i!=c.end())
      {
        Container *container = i->second;
        container->Raise();
      }
   }
   

   void OnLayoutSave(wxCommandEvent &event)
   {
      mSavedLayout = mManager->ToSerial();
      //wxMessageBox(mSavedLayout,"Layout");
   }

   void OnLayoutLoad(wxCommandEvent &event)
   {
      /*
      Serializer s;
      int p=0;
      s.FromSerial(mSavedLayout,p);
      wxString back = s.ToSerial();
      if (mSavedLayout == back)
         wxMessageBox("Checks out!");
      else
         wxMessageBox(back + " != " + mSavedLayout, "Not equal ?");
      */

      if (!mManager->FromSerial(mSavedLayout))
         wxMessageBox("There was a problem loading the layout");
      else
         wxMessageBox("Ok!");
   }


   void OnExit(wxCommandEvent &event) { Close(); }

   void OnClose(wxCloseEvent &inEvent)
   {
      delete mManager;
      Destroy();
   }

   bool mCustomDisabled;
   wxString m_pathBmp;
   int  mRows;

   Manager *mManager;
   ToolBox  *mToolbar;

   wxString mSavedLayout;

   std::map<int,wxWindow *> mWindowIDs;

   wxMenu   *mWindowsMenu;

   DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(MyFrame, Frame)
   EVT_CLOSE(MyFrame::OnClose)
   EVT_MENU(FILE_EXIT, MyFrame::OnExit)
   EVT_MENU_RANGE(ID_VIEW_WINDOW_BASE,ID_VIEW_WINDOW_MAX, MyFrame::OnViewWindow)
   EVT_MENU(ID_VIEW_LAYOUT_SAVE, MyFrame::OnLayoutSave)
   EVT_MENU(ID_VIEW_LAYOUT_LOAD, MyFrame::OnLayoutLoad)
   EVT_COMMAND(-1,wxEVT_WINDOW_LIST_CHANGED,MyFrame::OnWindowListChanged)
END_EVENT_TABLE()








// --- Boot -------------------------------------

class MyApp : public wxApp
{
public:
   bool MyApp::OnInit()
   {
      m_window = new MyFrame;
      m_window->Show();
      return true;
   }

private:
   Frame *m_window;
};

DECLARE_APP(MyApp);
IMPLEMENT_APP(MyApp);

