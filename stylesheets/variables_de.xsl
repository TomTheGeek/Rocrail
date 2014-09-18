<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<!-- Tabellenkopf einrichten -->
<xsl:template match="/">

   <html>
   <head><title>Rocrail Objekte Variables</title>
  </head>
  <body>
  <h2>Rocrail Report: Variablen</h2>

  <table border="1" rules="all" cellpadding="2" cellspacing="0" width="95%">
      <THEAD>
	  <TR>
	     <TH width="15%" height="30" bgcolor="lightgrey">Kennung</TH>
			  <th width="20%" height="20" bgcolor="lightgrey">Gruppe</th>
				<th width="20%" height="20" bgcolor="lightgrey">Beschreibung</th>
 	     <th width="20%" height="20" bgcolor="lightgrey">Text</th>
			 <th width="8%" height="20" bgcolor="lightgrey">Wert</th>
		 <th width="8%%" height="20" bgcolor="lightgrey">Min</th>
		 <th width="8%%" height="20" bgcolor="lightgrey">Max</th>
		 
		 </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>	
   </html>
   
</xsl:template>

<!-- Sortierung ID -->
	<xsl:template match="vrlist">

		<xsl:apply-templates>
		  <xsl:sort select="@id" order="ascending" data-type="text" />
		</xsl:apply-templates>

  </xsl:template>


<!-- Vorlage für Kennung -->
	<xsl:template match="vrlist/vr">
		<tr>
		<td align="center">
			<xsl:value-of select="@id" />
		</td>
		
<!-- Gruppe auswerten-->
		   <xsl:variable name="Gruppe" select="@group" />
	   <TD align="center">

	   <xsl:choose>
         <xsl:when test="$Gruppe = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@group" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	
<!-- Beschreibung auswerten-->
		   <xsl:variable name="Beschr" select="@desc" />
	   <TD align="center">

	   <xsl:choose>
         <xsl:when test="$Beschr = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@desc" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	
<!-- Text auswerten-->
		   <xsl:variable name="Text" select="@text" />
	   <TD align="center">

	   <xsl:choose>
         <xsl:when test="$Text = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@text" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
		
<!-- Wert auswerten-->

		<td align="center">
			<xsl:value-of select="@value" />
		</td>
		

		
<!-- Min auswerten-->
	<td align="center">
			<xsl:value-of select="@min" />
		</td>
  
<!-- Max auswerten-->
	<td align="center">
			<xsl:value-of select="@max" />
		</td>
		
  

	</tr>
<!-- Aktionen anzeigen -->
    <xsl:apply-templates/>
	</xsl:template> 
  

  <xsl:template match="vrlist/vr/actionctrl">


    <TR>
      <TD></TD>
      <TD colspan="2" bgcolor="#F8F8F8">
        Aktion :  <xsl:value-of select="@id" />
      </TD>
    </TR>
 </xsl:template>
  

</xsl:stylesheet>



