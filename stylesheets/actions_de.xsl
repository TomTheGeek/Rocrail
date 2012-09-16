<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>


<xsl:template match="/">

   <html>
   <head><title>Rocrail Objekt Aktionen</title>
  </head>
  <body>
  <h2>Rocrail Report: Aktionen von Objekten</h2>

  <table border="1" rules="all" cellpadding="2" cellspacing="0" width="90%">
      <THEAD>
	  <TR>
	     <TH width="30%" height="30" bgcolor="lightgrey">AKTION</TH>
 	     <TH width="60%" height="30" bgcolor="lightgrey">OBJEKT</TH>
	     </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>	
   </html>
   
</xsl:template>

<xsl:template match="//actionctrl">  

	<tr>	
	<TH rowspan="2" align="center" STYLE="font-size:18pt">
	<xsl:value-of select="@id" />
	</TH>
	<TD align="center" STYLE="font-size:14pt">
	<xsl:value-of select="../@id" />
	</TD>
	</tr> 
	


<tr>
<td colspan="3">
	<table  rules="all" cellpadding="2" cellspacing="0" width="100%">
	<thead>
	
	<th width="40%" height="20" bgcolor="lightgrey">Bedingung</th>
	<th width="40%" height="20" bgcolor="lightgrey">Status</th>
	<th width="20%" height="20" bgcolor="lightgrey">Typ</th>
	
	</thead>
	<tbody>
	<xsl:apply-templates/>
	</tbody>
	</table>
</td>
</tr>
</xsl:template>

<xsl:template match="actioncond">
	<tr>
	<td align="center">
	<xsl:value-of select="@id" />
	</td>
	<td align="center"><xsl:value-of select="@state" /></td>
	<td align="center"><xsl:value-of select="@type" /></td>
	</tr>

 </xsl:template>

</xsl:stylesheet>



