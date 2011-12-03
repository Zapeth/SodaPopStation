/*
 * File:   my_popstation.cpp
 * Author: derric
 *
 * Created on August 29, 2009, 1:32 PM
 */

#include "wx/wx.h"
#include "popstationGUI.h"
/*
 *
 */
class myPopstation: public wxApp{
    virtual bool OnInit();
};


IMPLEMENT_APP(myPopstation)
bool myPopstation::OnInit(){
    
    popFrame* frame=new popFrame((wxFrame *)NULL, -1, wxS("MyPopstation"), wxPoint(-1,-1),
            wxSize(-1,-1), wxDEFAULT_FRAME_STYLE);
    frame->Center();
    frame->Show(TRUE);
    SetTopWindow(frame);

    return TRUE;
}

