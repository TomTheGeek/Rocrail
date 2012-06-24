/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#include "fxdlg.h"

#include "rocs/public/trace.h"


FxDlg::FxDlg( wxWindow* parent, int p_FX, wxSpinCtrl* p_CVnr )
  :fxdlggen( parent )
{
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "FxDlg" );
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

int FxDlg::getConfig() {
  int gen = m_Generation->GetSelection();
  int act = m_Work->GetSelection();

  if( act > 7 ) act += 2;

  m_FX  = act << 4;
  m_FX |= gen & 0x0F;

  return m_FX;
}

void FxDlg::onFunction( wxCommandEvent& event ) {
  int fcv[] = {49,50,51,52,113,114,115,116};
  int f = m_Function->GetSelection();

  m_CVnr->SetValue(fcv[f]);
}


void FxDlg::initLabels() {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
}

void FxDlg::initValues() {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues" );
  int act = m_FX >> 4;
  int gen = m_FX & 0x0F;
  m_Generation->SetSelection(gen);
  if( act > 7 ) act -= 2;
  m_Work->SetSelection(act);

  int cv = m_CVnr->GetValue();
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
