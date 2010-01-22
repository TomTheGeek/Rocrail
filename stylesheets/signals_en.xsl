<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Signals report</title>
  </head>
  <body>
  <h2>Signals report</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="80%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">ID</TH>
 	     <TH width="40%" bgcolor="lightgrey">Description</TH>
 	     <TH bgcolor="lightgrey">IID</TH>
 	     <TH bgcolor="lightgrey">Address</TH>
 	     <TH bgcolor="lightgrey">Port</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="sglist">  

  <xsl:apply-templates>
   <xsl:sort select="@addr" order="ascending" data-type="number" />
  
  </xsl:apply-templates>

</xsl:template>


<xsl:template match="sg">  

	<TR>	
	   <TD align="left"><xsl:value-of select="@id" /></TD>
	   <TD align="left"><xsl:value-of select="@desc" /></TD>
	   <TD align="center"><xsl:value-of select="@iid" /></TD>
	   <TD align="center"><xsl:value-of select="@addr" />,<xsl:value-of select="@addr2" />,<xsl:value-of select="@addr3" />,<xsl:value-of select="@addr4" /></TD>
	   <TD align="center"><xsl:value-of select="@port1" />,<xsl:value-of select="@port2" />,<xsl:value-of select="@port3" />,<xsl:value-of select="@port4" /></TD>
	</TR>


<xsl:apply-templates/>
 
</xsl:template>


</xsl:stylesheet>

