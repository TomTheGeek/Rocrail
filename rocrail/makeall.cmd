md ..\bin
echo Making the rocs library...
cd ..\rocs
call rocs.cmd
echo Making the rocint library...
cd ..\rocint
call rocint.cmd
echo Making the rocrail library...
cd ..\rocrail
call rocrail.cmd
echo Making rocdigs...
cd ..\rocdigs
call rocdigs.cmd
echo Making roclcdr...
cd ..\roclcdr
call roclcdr.cmd
echo Making rocgui...
cd ..\rocgui
call rocgui.cmd
cd ..\rocrail
echo Ready.
