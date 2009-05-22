#include "uhl633x0dlg.h"

Uhl633x0Dlg::Uhl633x0Dlg( wxWindow* parent )
:
uhl633x0dlggen( parent )
{

}

void Uhl633x0Dlg::event( iONode event ) {
  /* clean up event node */
  NodeOp.base.del(event);

}

