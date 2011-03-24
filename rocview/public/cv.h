/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#ifndef __ROCRAIL_rocgui_CV_H
#define __ROCRAIL_rocgui_CV_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "cv.h"
#endif

/*!
 * Includes
 */

#include "wx/spinctrl.h"
#include "wx/tglbtn.h"
#include "wx/grid.h"
#include "wx/timer.h"
#include "wx/progdlg.h"

#include "rocs/public/node.h"
#include "rocs/public/mutex.h"

enum {
  GET_CV = 1000,
  GET_ADDRESS,
  GET_VSTART,
  GET_VACCEL,
  GET_VDECEL,
  GET_VHIGH,
  GET_VMID,
  GET_VERSION,
  GET_MANUFACTUREDID,
  GET_LADDRESS,
};

enum {
  SET_CV = 2000,
  SET_ADDRESS,
  SET_VSTART,
  SET_VACCEL,
  SET_VDECEL,
  SET_VHIGH,
  SET_VMID,
  SET_VERSION,
  SET_MANUFACTUREDID,
  SET_LADDRESS,
};

enum {
  VAL_CV = 3000,
  VAL_ADDRESS,
  VAL_VSTART,
  VAL_VACCEL,
  VAL_VDECEL,
  VAL_VHIGH,
  VAL_VMID,
  VAL_VERSION,
  VAL_MANUFACTUREDID,
  VAL_GENERIC,
  VAL_LADDRESS,
};

enum {
  CVGET = 1,
  CVSET,
  CVGETALL,
  CVSETALL,
};

#define ID_COMBOBOX_LOCLIST 10001
#define ME_CVTimer 10002


class CV
{
public:
    /// Constructors
    CV( wxScrolledWindow* parent, iONode cvconf, wxWindow* frame );
    void OnButton(wxCommandEvent& event);
    void OnGrid(wxGridEvent& event);
    void setLocProps( iONode props );
    void event( iONode event );
    void init();
    void OnTimer(wxTimerEvent& event);

private:
    /// Creation
    bool Create();
    void initValues();
    void initPresets( int nr, const char* val );
    void readAll();
    void writeAll();

    void onDecConfig();
    void onDecFX();
    void onSpeedCurve();

    void loadCVfromFile();
    void saveCVtoFile();

    void startProgress();
    void stopProgress();

    /// Creates the controls and sizers
    void initLabels();
    void CreateControls();
    void doCV( int command, int index, int value );
    void doCV( int id );
    void updateCVval();
    void updateCVbits();
    void CVconf();
    void update4POM();
    void reset();

    void getLongAddress();
    void setLongAddress();

    int m_CV17;
    int m_CV18;
    int m_TimerCount;
    int m_CVidx;
    int m_CVall[1024];
    int m_CVcountAll;
    int m_CVidxAll;
    Boolean m_bPOM;
    Boolean m_bDirect;
    bool m_bSpeedCurve;
    int m_CVoperation;
    bool m_bCleanUpProgress;
    int m_Curve[28];
    int m_ConfigVal;
    int m_FxVal;
    iOMutex m_TimerMutex;
    bool m_bConfig;
    bool m_bFX;
    bool m_bLongAddress;

    wxWindow* m_Frame;

    wxTimer* m_Timer;
    wxProgressDialog* m_Progress;

    wxScrolledWindow* m_Parent;
    iONode m_LocProps;
    iONode m_CVconf;

    wxBoxSizer* m_MainBox;
    wxScrolledWindow* m_ScrollWindow;

    wxPanel* m_ItemPanel;
    wxBoxSizer* m_PanelMainBox;
    wxBoxSizer* m_LocBox;
    wxBoxSizer* m_TableBox;
    wxFlexGridSizer* m_FlexGrid;

    wxStaticText* m_labelCVaddress;
    wxTextCtrl* m_CVaddress;
    wxButton* m_getAddress;
    wxButton* m_setAddress;
    wxStaticText* m_labelCVlongaddress;
    wxTextCtrl* m_CVlongaddress;
    wxButton* m_getlongAddress;
    wxButton* m_setlongAddress;
    wxStaticText* m_labelCVVstart;
    wxTextCtrl* m_CVVstart;
    wxButton* m_getVstart;
    wxButton* m_setVstart;
    wxStaticText* m_labelCVaccel;
    wxTextCtrl* m_CVaccel;
    wxButton* m_getAccel;
    wxButton* m_setAccel;
    wxStaticText* m_labelCVdecel;
    wxTextCtrl* m_CVdecel;
    wxButton* m_getDecel;
    wxButton* m_setDecel;
    wxStaticText* m_labelCVVhigh;
    wxTextCtrl* m_CVVhigh;
    wxButton* m_getVhigh;
    wxButton* m_setVhigh;
    wxStaticText* m_labelCVVmid;
    wxTextCtrl* m_CVVmid;
    wxButton* m_getVmid;
    wxButton* m_setVmid;
    wxStaticText* m_labelCVversion;
    wxTextCtrl* m_CVversion;
    wxButton* m_getVersion;
    wxButton* m_setVersion;
    wxStaticText* m_labelCVmanufacturedID;
    wxTextCtrl* m_CVmanufacturedID;
    wxButton* m_getManu;
    wxButton* m_setManu;
    wxStaticBoxSizer* m_CVbox;
    wxSpinCtrl* m_CVnr;
    wxTextCtrl* m_CVvalue;
    wxButton* m_Get;
    wxButton* m_Set;
    wxToggleButton* m_PTonoff;
    wxStaticText* m_labIID;
    wxComboBox* m_IID;

    wxButton* m_saveCVs;
    wxButton* m_saveAllCVs;
    wxButton* m_SpeedCurve;
    wxButton* m_Config;
    wxButton* m_FX;
    wxButton* m_loadCVs;
    wxButton* m_loadFile;
    wxButton* m_saveFile;

    wxBoxSizer* m_CVMainBox;
    wxBoxSizer* m_CVSubBox1;
    wxBoxSizer* m_CVSubBox2;
    wxBoxSizer* m_CVSubBox3;
    wxCheckBox* m_bit0;
    wxCheckBox* m_bit1;
    wxCheckBox* m_bit2;
    wxCheckBox* m_bit3;
    wxCheckBox* m_bit4;
    wxCheckBox* m_bit5;
    wxCheckBox* m_bit6;
    wxCheckBox* m_bit7;

    wxButton* m_CopyFrom;
    wxButton* m_WriteAll;
    wxButton* m_ReadAll;

    wxStaticText* m_labTable;
    wxGrid*       m_CVTable;
    wxCheckBox*   m_POM;
    wxCheckBox*   m_Direct;
    wxStaticText* m_labLoc;
    wxComboBox*   m_LcList;
};

#endif
