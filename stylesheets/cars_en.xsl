<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Cars report</title>
  </head>
  <body>
  <h2>Rocrail Cars Report</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">Image</TH>
	     <TH width="16%" height="30" bgcolor="lightgrey">ID</TH>
 	     <TH width="16%" bgcolor="lightgrey">Identifier</TH>
 	     <TH width="10%" bgcolor="lightgrey">Number</TH>
 	     <TH width="38%" bgcolor="lightgrey">Remark</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="carlist">  

  <xsl:apply-templates>
   <xsl:sort select="@addr" order="ascending" data-type="number" />
  </xsl:apply-templates>

</xsl:template>

<xsl:template match="car">  

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

	   <xsl:variable name="ident" select="@ident" />

	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$ident = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@ident" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	   <TD align="center"><xsl:value-of select="@number" /></TD>

	   <TD>
	   <xsl:variable name="remark" select="../../@remark" />
	   <xsl:choose>
         <xsl:when test="@remark = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@remark" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	</TR>
	
	
    <TR>
	<td colspan="5">
    <table border="0" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="20" bgcolor="white">Roadname</TH>
	     <TH width="25%" height="20" bgcolor="white">Manufacturer ID</TH>
	     <TH width="15%" height="20" bgcolor="white">Era</TH>
		 <TH width="20%" height="20" bgcolor="white">Locality</TH>
 	     <TH width="20%" height="20" bgcolor="white">Owner</TH>
	  </TR>
      </THEAD> 
      <TBODY>
      <TR>
	  
	   <xsl:variable name="roadname" select="@roadname" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$roadname = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@roadname" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   
	   <xsl:variable name="manuid" select="@manuid" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$manuid = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@manuid" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   
	   <xsl:variable name="era" select="@era" />
	   <TD align="center">
	   <xsl:value-of select="@era + 1" /> 
       </TD>
	   
	   <xsl:variable name="location" select="@location" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$location = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@location" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="owner" select="@owner" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$owner = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@owner" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   </TR>
	   </TBODY>
	</table>
	</td>
	</TR>
	
	<TR>	
	<td colspan="5">
    <table border="0" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="20" bgcolor="white">Color</TH>
	     <TH width="20%" height="20" bgcolor="white">Lenght</TH>
	     <TH width="20%" height="20" bgcolor="white">Type</TH>
	     <TH width="20%" height="20" bgcolor="white">Subtype</TH>
	     <TH width="20%" height="20" bgcolor="white">Status</TH>
 	  </TR>
      </THEAD> 
      <TBODY>
      <TR>
	   <xsl:variable name="color" select="@color" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$color = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@color" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="len" select="@len" />
	   <TD align="center">
	   <xsl:value-of select="@len" /> 
       </TD>
	   
	   <xsl:variable name="type" select="@type" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$type = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
	   
	   
         <xsl:value-of select="@type" />
			
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="subtype" select="@subtype" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$subtype = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@subtype" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   
	   <xsl:variable name="status" select="@status" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$status = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@status" />
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

