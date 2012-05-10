
#include "wx/wx.h"


#include "basedlg.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocview/public/guiapp.h"

#include "rocs/public/node.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"


bool BaseDialog::existID( wxWindow* dlg, iONode list, iONode props, wxString  id ) {
  if( StrOp.equals( wItem.getid(props), id.mb_str(wxConvUTF8) ) ) {
    return false;
  }

  if( id.Len() == 0 ) {
    wxMessageDialog( dlg, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    return true;
  }
  if( list != NULL ) {
    int cnt = NodeOp.getChildCnt( list );
    for( int i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( list, i );
      if( StrOp.equals( wItem.getid(child), id.mb_str(wxConvUTF8) )) {
        wxMessageDialog( dlg,
            wxString::Format(wxGetApp().getMsg("existingid"), wItem.getx(child), wItem.gety(child)) + _T("\n") + wxString(wItem.getdesc(child),wxConvUTF8),
            _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
        return true;
      }
    }
  }
  return false;
}


void BaseDialog::doDoc( wxCommandEvent& event, const char* xslName ) {
  
  // get the model:
  iONode model = wxGetApp().getModel();
  // serialize the model into a string:
  NodeOp.setStr(model, "guiimagepath", wGui.getimagepath(wxGetApp().getIni()));
  char* xml = model->base.toString( model );
  
  // create a path and filename:
  char* fileName = StrOp.fmt( "%s%c%s-doc.xml", FileOp.pwd(), SystemOp.getFileSeparator(), xslName );
  
  // open the file for writing:
  iOFile planFile = FileOp.inst( fileName, False );
  if( planFile != NULL ) {
    char* styleSheet = NULL;
    
    // Check if a XSL exists for the GUI language:
    const char* lang = wGui.getlang( wxGetApp().getIni() );
    
    char* xslPath = NULL;
    if( FileOp.isAbsolute( wGui.getxslpath( wxGetApp().getIni() ) ) ) {
      xslPath = StrOp.dup( wGui.getxslpath( wxGetApp().getIni() ) );
    }
    else {
      xslPath = StrOp.fmt( "%s%c%s",FileOp.pwd(), SystemOp.getFileSeparator(), wGui.getxslpath( wxGetApp().getIni() ));
    }
    
    char* xslFileName = StrOp.fmt( "%s%c%s_%s.xsl", xslPath, SystemOp.getFileSeparator(), xslName, lang );
    if( !FileOp.exist(xslFileName) )
      lang = "en"; // use the default
    StrOp.free(xslFileName);

    // create the stylesheet line:
    styleSheet = StrOp.fmt("<?xml-stylesheet type=\"text/xsl\" href=\"file:///%s%c%s_%s.xsl\"?>\n", 
        xslPath, SystemOp.getFileSeparator(), xslName, lang );
    StrOp.free(xslPath);

    // write the stylsheet line at the beginning of the file:
    FileOp.write( planFile, styleSheet, StrOp.len( styleSheet ) );
    // write the rest:
    FileOp.write( planFile, xml, StrOp.len( xml ) );
    FileOp.close( planFile );
    
    TraceOp.trc( "fbdlg", TRCLEVEL_INFO, __LINE__, 9999, "%ld bytes saved in %s.", 
        FileOp.getWritten( planFile ), fileName );

    /* Cleanup. */
    planFile->base.del(planFile);
    StrOp.free(styleSheet);
  }
  /* Cleanup. */
  StrOp.free(xml);

  // call the browser:
  wxLaunchDefaultBrowser(wxString("file:///",wxConvUTF8 )+wxString(fileName,wxConvUTF8 ), wxBROWSER_NEW_WINDOW );
  
  /* Cleanup. */
  StrOp.free(fileName);
  
}
