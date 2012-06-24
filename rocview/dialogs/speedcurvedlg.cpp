/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#include "speedcurvedlg.h"

#include "rocs/public/trace.h"

SpeedCurveDlg::SpeedCurveDlg( wxWindow* parent, int* curve )
  :speedcurvedlggen( parent )
{
  m_Step[ 0] = m_Step1;
  m_Step[ 1] = m_Step2;
  m_Step[ 2] = m_Step3;
  m_Step[ 3] = m_Step4;
  m_Step[ 4] = m_Step5;
  m_Step[ 5] = m_Step6;
  m_Step[ 6] = m_Step7;
  m_Step[ 7] = m_Step8;
  m_Step[ 8] = m_Step9;
  m_Step[ 9] = m_Step10;
  m_Step[10] = m_Step11;
  m_Step[11] = m_Step12;
  m_Step[12] = m_Step13;
  m_Step[13] = m_Step14;
  m_Step[14] = m_Step15;
  m_Step[15] = m_Step16;
  m_Step[16] = m_Step17;
  m_Step[17] = m_Step18;
  m_Step[18] = m_Step19;
  m_Step[19] = m_Step20;
  m_Step[20] = m_Step21;
  m_Step[21] = m_Step22;
  m_Step[22] = m_Step23;
  m_Step[23] = m_Step24;
  m_Step[24] = m_Step25;
  m_Step[25] = m_Step26;
  m_Step[26] = m_Step27;
  m_Step[27] = m_Step28;

  m_SliderStep[ 0] = m_SliderStep1;
  m_SliderStep[ 1] = m_SliderStep2;
  m_SliderStep[ 2] = m_SliderStep3;
  m_SliderStep[ 3] = m_SliderStep4;
  m_SliderStep[ 4] = m_SliderStep5;
  m_SliderStep[ 5] = m_SliderStep6;
  m_SliderStep[ 6] = m_SliderStep7;
  m_SliderStep[ 7] = m_SliderStep8;
  m_SliderStep[ 8] = m_SliderStep9;
  m_SliderStep[ 9] = m_SliderStep10;
  m_SliderStep[10] = m_SliderStep11;
  m_SliderStep[11] = m_SliderStep12;
  m_SliderStep[12] = m_SliderStep13;
  m_SliderStep[13] = m_SliderStep14;
  m_SliderStep[14] = m_SliderStep15;
  m_SliderStep[15] = m_SliderStep16;
  m_SliderStep[16] = m_SliderStep17;
  m_SliderStep[17] = m_SliderStep18;
  m_SliderStep[18] = m_SliderStep19;
  m_SliderStep[19] = m_SliderStep20;
  m_SliderStep[20] = m_SliderStep21;
  m_SliderStep[21] = m_SliderStep22;
  m_SliderStep[22] = m_SliderStep23;
  m_SliderStep[23] = m_SliderStep24;
  m_SliderStep[24] = m_SliderStep25;
  m_SliderStep[25] = m_SliderStep26;
  m_SliderStep[26] = m_SliderStep27;
  m_SliderStep[27] = m_SliderStep28;

  MemOp.copy( m_Curve, curve, 28 * sizeof(int));

  for( int i = 0; i < 28; i++ ) {
    m_SliderStep[i]->SetValue(m_Curve[i]);
    m_Step[i]->SetValue(wxString::Format( _T("%d"), m_Curve[i]));
  }

}

int* SpeedCurveDlg::getCurve(){
  return m_Curve;
}


void SpeedCurveDlg::onSlider( wxScrollEvent& event ){
  for( int i = 0; i < 28; i++ ) {
    if( event.GetEventObject() == m_SliderStep[i] ) {
      m_Step[i]->SetValue(wxString::Format( _T("%d"), m_SliderStep[i]->GetValue()));
      m_Curve[i] = m_SliderStep[i]->GetValue();
      break;
    }
  }
}


void SpeedCurveDlg::onStep( wxCommandEvent& event ) {
  for( int i = 0; i < 28; i++ ) {
    if( event.GetEventObject() == m_Step[i] ) {
      long step = 0;
      m_Step[i]->GetValue().ToLong(&step);
      if( step > 255 ) step = 255;
      if( step < 0   ) step = 0;
      m_SliderStep[i]->SetValue((int)step);
      m_Curve[i] = step;
      break;
    }
  }
}


void SpeedCurveDlg::onLinearize( wxCommandEvent& event ) {
  int Vmin = m_SliderStep[ 0]->GetValue();
  int Vmax = m_SliderStep[27]->GetValue();
  float step = (Vmax -Vmin) / 27.0;
  for( int i = 1; i < 27; i++ ) {
    float Vf = step * i;
    int V = (int)(step * i);
    if( Vf - (float)V >= 0.5 )
      V++;
    m_SliderStep[i]->SetValue( V + Vmin );
    m_Step[i]->SetValue(wxString::Format( _T("%d"), V + Vmin ));
  }
}


void SpeedCurveDlg::onLogarithmize( wxCommandEvent& event ){
  int Vmin = m_SliderStep[ 0]->GetValue();
  int Vmax = m_SliderStep[27]->GetValue();
  float step = (Vmax -Vmin) / 27.0;
  float fact = 1.1;
  for( int i = 1; i < 27; i++ ) {
    float Vf = step * i * log(fact);
    int V = (int)(Vf);
    if( Vf - (float)V >= 0.5 )
      V++;
    TraceOp.trc( "vcurve", TRCLEVEL_INFO, __LINE__, 9999,
        "i=%d fact=%.2f log=%.2f V=%d", i, fact, log(fact), V  );

    fact += 1.6 / 26; // hack
    m_SliderStep[i]->SetValue( V + Vmin );
    m_Step[i]->SetValue(wxString::Format( _T("%d"), V + Vmin ));
  }
}


void SpeedCurveDlg::onCancel( wxCommandEvent& event ) {
  EndModal( 0 );

}
void SpeedCurveDlg::onOK( wxCommandEvent& event ) {
  EndModal( wxID_OK );
}

