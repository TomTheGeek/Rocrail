<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Locomotives report</title>
  </head>
  <body>
  <h2>Tabulato con tutte le loco</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">Fotografia</TH>
	     <TH width="10%" height="30" bgcolor="lightgrey">ID</TH>
 	     <TH width="10%" bgcolor="lightgrey">IID</TH>
 	     <TH width="10%" bgcolor="lightgrey">Indirizzo</TH>
 	     <TH width="50%" bgcolor="lightgrey">Descrizione</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="lclist">  

  <xsl:apply-templates>
   <xsl:sort select="@addr" order="ascending" data-type="number" />
  
  </xsl:apply-templates>

</xsl:template>


<xsl:template match="lc">  

	<TR>
       <th rowspan="2" align="center">
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

	   <xsl:variable name="iid1" select="@iid" />

	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$iid1 = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@iid" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	   <TD align="center"><xsl:value-of select="@addr" /></TD>

	   <TD>
	   <xsl:variable name="desc1" select="@desc" />
	   <xsl:choose>
         <xsl:when test="$desc1 = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@desc" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	</TR>
    <TR>
	<td colspan="4">
    <table border="0" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="25%" height="30" bgcolor="white">Acquistata</TH>
	     <TH width="25%" height="30" bgcolor="white">Tempo in esercizio</TH>
	     <TH width="25%" height="30" bgcolor="white">v_min</TH>
 	     <TH width="25%" bgcolor="white">v_max</TH>
	  </TR>
      </THEAD> 
      <TBODY>
      <TR>
	  
	   <xsl:variable name="purchased" select="@purchased" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$purchased = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@purchased" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="runtime" select="@runtime" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$runtime = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@runtime" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   
	   <xsl:variable name="v_min" select="@V_min" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$v_min = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@V_min" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="v_max" select="@V_max" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$v_max = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@V_max" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   
	</TR>
    </TBODY>
    </table>

	</td>
	</TR>
    <xsl:apply-templates/>
 
</xsl:template>


</xsl:stylesheet>

