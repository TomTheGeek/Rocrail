/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifndef __dipdlg__
#define __dipdlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include <rocs/public/node.h>

class DIPDlg : public wxDialog
{
  private:
  const char* m_Title;
  iONode m_DIP;
  int m_CVNr;
  int m_Value;
  void initDIP();
  void addDIPGroup(iONode group, int idx);
  wxRadioBox* m_RadioBox[32];
  wxCheckBox* m_CheckBox[32][32];
  iONode m_Group[32][32];
  wxWindow* m_Parent;

  protected:
    wxStdDialogButtonSizer* m_sdButtons;
    wxButton* m_sdButtonsOK;
    wxButton* m_sdButtonsApply;
    wxButton* m_sdButtonsCancel;
    wxBoxSizer* m_Sizer;


    void onCancel( wxCommandEvent& event );
    void onOK( wxCommandEvent& event );
    void onApply( wxCommandEvent& event );
    void onClose( wxCloseEvent& event );



  public:

    DIPDlg( wxWindow* parent, iONode dip, int cvnr, int value, const char* title=NULL );
    ~DIPDlg();

    int getValue(int* cvnr);

};

#endif //__fxdlggen__
