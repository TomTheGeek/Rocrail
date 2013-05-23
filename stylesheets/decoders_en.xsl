<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Decoders report</title>
  </head>
  <body>
  <h2>Rocrail Decoders Report</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">Image</TH>
	     <TH width="16%" height="30" bgcolor="lightgrey">ID</TH>
 	     <TH width="16%" bgcolor="lightgrey">Address</TH>
 	     <TH width="10%" bgcolor="lightgrey">Manufacturer</TH>
 	     <TH width="38%" bgcolor="lightgrey">CatNr</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="declist">  

  <xsl:apply-templates>
   <xsl:sort select="@addr" order="ascending" data-type="number" />
  </xsl:apply-templates>

</xsl:template>

<xsl:template match="dec">  

	<TR>
       <th rowspan="3" align="center">
	  <xsl:choose>
	   <xsl:when test="contains(@image, '.xpm')">
              <xsl:text>-</xsl:text>
          </xsl:when>
 	      <xsl:when test="contains(@image, '.XPM')">
              <xsl:text>-</xsl:text>
          </xsl:when>
		  <xsl:otherwise>
        <img><xsl:attribute name="src"><xsl:value-of select="concat(../../@guiimagepath, '/', @image)" /></xsl:attribute></img>
       </xsl:otherwise>
	   </xsl:choose>
	   </th>

	   <TD><xsl:value-of select="@id" /></TD>

	   <xsl:variable name="addr" select="@addr" />

	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$addr = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@addr" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	   <TD align="center"><xsl:value-of select="@manu" /></TD>

	   <TD>
	   <xsl:variable name="remark" select="../../@desc" />
	   <xsl:choose>
         <xsl:when test="@remark = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@desc" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	</TR>
 
</xsl:template>

</xsl:stylesheet>

