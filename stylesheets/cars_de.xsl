<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Wagons report</title>
  </head>
  <body>
  <h2>Liste aller definierten Wagen</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">Bild</TH>
	     <TH width="16%" height="30" bgcolor="lightgrey">1. Kennung</TH>
 	     <TH width="16%" bgcolor="lightgrey">2. Kennung</TH>
 	     <TH width="10%" bgcolor="lightgrey">Nummer</TH>
 	     <TH width="48%" bgcolor="lightgrey">Bemerkung</TH>
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
	   <xsl:variable name="gremark" select="../../@remark" />
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
    <table border="0" rules="all" cellpadding="2" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
		 <TH width="20%" height="20" bgcolor="white">Gesellschaft</TH>
		 <TH width="20%" height="20" bgcolor="white">Hersteller</TH>
		 <TH width="20%" height="20" bgcolor="white">Epoche</TH>
		 <TH width="20%" height="20" bgcolor="white">Ortschaft</TH>
 	     <TH width="20%" height="20" bgcolor="white">Besitzer</TH>
	     
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
    <table border="0" rules="all" cellpadding="2" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="20" bgcolor="white">Farbe</TH>
	     <TH width="20%" height="20" bgcolor="white">L&#228;nge</TH>
		 <TH width="20%" height="20" bgcolor="white">Typ</TH>
	     <TH width="20%" height="20" bgcolor="white">Untergruppe</TH>
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
		 <xsl:when test="$type = 'freight'">
			<xsl:text>G&#252;terverkehr</xsl:text>
         </xsl:when>
		 <xsl:when test="$type = 'passenger'">
			<xsl:text>Personenverkehr</xsl:text>
         </xsl:when>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="subtype" select="@subtype" />
	   <TD align="center">
	   <xsl:choose>
			<xsl:when test="$subtype = ''">
				<xsl:text>-</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'coach'">
				<xsl:text>Schlafwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'lounge'">
				<xsl:text>Salonwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'dome'">
				<xsl:text>Aussichtswagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'express'">
				<xsl:text>Express</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'dinner'">
				<xsl:text>Speisewagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'baggage'">
				<xsl:text>Gep&#228;ckwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'postoffice'">
				<xsl:text>Postamt</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'boxcar'">
				<xsl:text>G&#252;terwagon</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'gondola'">
				<xsl:text>offener G&#252;terwagon</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'flatcar'">
				<xsl:text>Flachwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'reefer'">
				<xsl:text>K&#252;hlwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'stockcar'">
				<xsl:text>Viehwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'tankcar'">
				<xsl:text>Kesselwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'wellcar'">
				<xsl:text>Containerwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'hopper'">
				<xsl:text>Beh&#228;lterwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'caboose'">
				<xsl:text>Dienstwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'autorack'">
				<xsl:text>Autotransportwagen</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'autocarrier'">
				<xsl:text>Autotransporter</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'logdumpcar'">
				<xsl:text>Langholzwaggon</xsl:text>
			</xsl:when>
			<xsl:when test="$subtype = 'coilcar'">
				<xsl:text>Bandstahltransportwagen</xsl:text>
			</xsl:when>
       </xsl:choose>
	   </TD>
	   <xsl:variable name="status" select="@status" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$status = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
	   <xsl:when test="$status = 'empty'">
			<xsl:text>Leer</xsl:text>
		</xsl:when>
		<xsl:when test="$status = 'loaded'">
			<xsl:text>Beladen</xsl:text>
		</xsl:when>
		<xsl:when test="$status = 'maintenance'">
			<xsl:text>Wartung</xsl:text>
		</xsl:when>	
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

