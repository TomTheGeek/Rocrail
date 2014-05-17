/*
 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 

 */
#include "fxdlg.h"

#include "rocview/public/guiapp.h"
#include "rocs/public/trace.h"


FxDlg::FxDlg( wxWindow* parent, int p_FX, wxSpinCtrl* p_CVnr )
  :fxdlggen( parent )
{
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "FxDlg fx=%d cv=%d", p_FX, p_CVnr->GetValue() );
  m_FX = p_FX;
  m_CVnr = p_CVnr;
  initLabels();
  initValues();
}

void FxDlg::onCancel( wxCommandEvent& event ) {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "onCancel" );
  EndModal( 0 );
}


void FxDlg::onOK( wxCommandEvent& event ) {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "onOK" );
  EndModal( wxID_OK );
}

int FxDlg::getConfig( int* cvnr ) {
  int gen = m_Generation->GetSelection();
  int act = m_Work->GetSelection();
  m_FX  = (act << 4);
  m_FX |= (gen & 0x0F);

  int cv = m_Function->GetSelection();
  switch( cv ) {
    case  0: *cvnr=49; break;
    case  1: *cvnr=50; break;
    case  2: *cvnr=51; break;
    case  3: *cvnr=52; break;
    case  4: *cvnr=113; break;
    case  5: *cvnr=114; break;
    case  6: *cvnr=115; break;
    case  7: *cvnr=116; break;
  }

  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "getConfig FX=0x%02X act=0x%02X gen=0x%02X cvnr=%d", m_FX, act, gen, *cvnr );

  return m_FX;
}

void FxDlg::onFunction( wxCommandEvent& event ) {
  int fcv[] = {49,50,51,52,113,114,115,116};
  int f = m_Function->GetSelection();

  m_CVNumber->SetValue(fcv[f]);
  m_CVnr->SetValue(fcv[f]);
}


void FxDlg::initLabels() {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
}

void FxDlg::initValues() {
  int act = ((m_FX & 0xF0) >> 4);
  int gen = (m_FX & 0x0F);
  m_Generation->SetSelection(gen);
  m_Work->SetSelection(act);

  m_CVValue->SetValue(m_FX);
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues FX=0x%02X act=0x%02X gen=0x%02X", m_FX, act, gen );

  int cv = m_CVnr->GetValue();
  m_CVNumber->SetValue(m_CVnr->GetValue());

  switch( cv ) {
    case  49: m_Function->SetSelection(0); break;
    case  50: m_Function->SetSelection(1); break;
    case  51: m_Function->SetSelection(2); break;
    case  52: m_Function->SetSelection(3); break;
    case 113: m_Function->SetSelection(4); break;
    case 114: m_Function->SetSelection(5); break;
    case 115: m_Function->SetSelection(6); break;
    case 116: m_Function->SetSelection(7); break;
  }
}

void FxDlg::onGeneration( wxCommandEvent& event ) {
  int cvnr = 0;
  int val = getConfig(&cvnr);
  m_CVValue->SetValue(val);
}

void FxDlg::onActivation( wxCommandEvent& event ) {
  onGeneration(event);
}

void FxDlg::onHelp( wxCommandEvent& event ) {
  wxGetApp().openLink( "pt" );
}

