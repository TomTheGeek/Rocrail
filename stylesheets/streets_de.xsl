<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Routes report</title>
  </head>
  <body>
  <h2>Liste der definierten Fahrstrassen (PB)</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">ID</TH>
	     <TH width="15%" height="30" bgcolor="lightgrey">Von Block</TH>
	     <TH width="15%" height="30" bgcolor="lightgrey">Nach Block</TH>
	     <TH width="10%" height="30" bgcolor="lightgrey">Kreuzender Block</TH>
	     <TH width="20%" height="30" bgcolor="lightgrey">Geschwindigkeit</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="stlist">  
  
  <xsl:apply-templates>
   <xsl:sort select="@id" order="ascending" data-type="text" />
  
  </xsl:apply-templates>

  </xsl:template>


<xsl:template match="st">  

	<TR>
       <th rowspan="3" align="center">
	    <xsl:value-of select="@id" />
	   </th>	
 
       <td align="center">
	   <xsl:choose>
         <xsl:when test="@bka">
         <xsl:value-of select="@bka" />
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </td>	

	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="@bkb">
         <xsl:value-of select="@bkb" />
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <TD align="center">
	   <xsl:variable name="bkc" select="@bkc" />
	   <xsl:choose>
         <xsl:when test="@bkc">
   	       <xsl:choose>
             <xsl:when test="$bkc = ''">
                <xsl:text>-</xsl:text>
             </xsl:when>
           <xsl:otherwise>
                <xsl:value-of select="@bkc" />
           </xsl:otherwise>
           </xsl:choose>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <TD align="center">
       <xsl:variable name="speed" select="@speed" />
	   <xsl:choose>
            <xsl:when test="$speed = 'percent'">
                  <xsl:value-of select="@speedpercent" /> <xsl:text>%</xsl:text>
            </xsl:when>
       <xsl:otherwise>
            <xsl:value-of select="@speed" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
   </TR>
   <TR>
	  <xsl:variable name="nocommuter" select="@nocommuter" />
	  <TD align="center">
         <xsl:choose>
         <xsl:when test="$nocommuter = 'false'">
          <xsl:text>-</xsl:text>
           </xsl:when>
           <xsl:when test="$nocommuter = 'true'">
           <xsl:text>Pendel: nein</xsl:text>
         </xsl:when>
         </xsl:choose>
      </TD>
      
	  <xsl:variable name="commuter" select="@commuter" />
	  <TD align="center">
         <xsl:choose>
         <xsl:when test="$commuter = 'false'">
          <xsl:text>-</xsl:text>
           </xsl:when>
           <xsl:when test="$commuter = 'true'">
           <xsl:text>Pendel: nur</xsl:text>
         </xsl:when>
         </xsl:choose>
      </TD>

      <xsl:variable name="swappost" select="@swappost" />
      <TD align="center">
	   <xsl:choose>
         <xsl:when test="@swappost">
   	       <xsl:choose>
             <xsl:when test="$swappost = 'true'">
                <xsl:text>swap after</xsl:text>
             </xsl:when>
           <xsl:otherwise>
                <xsl:text>-</xsl:text>
           </xsl:otherwise>
           </xsl:choose>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   
	  <xsl:variable name="placing" select="@placing" />
	  <TD align="center">
         <xsl:choose>
         <xsl:when test="$placing = '1'">
          <xsl:text>nur Standard</xsl:text>
           </xsl:when>
           <xsl:when test="$placing = '2'">
           <xsl:text>nur Swapped</xsl:text>
         </xsl:when>
        <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
        </xsl:choose>
      </TD>
   </TR>

    <TR>
	<td colspan="4">
    <table border="0" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR colspan="2">
	     <TH align="left" bgcolor="white">Weichen</TH>
	  </TR>
	  <TR>
	     <TH width="25%" height="30" bgcolor="lightgrey">Weichenkennung</TH>
	     <TH width="25%" height="30" bgcolor="lightgrey">Befehl</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
      </table>

	</td>
	</TR>
 
</xsl:template>

<xsl:template match="swcmd">  

      <TR>
	   <xsl:variable name="swid" select="@id" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$swid = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@id" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="swcmd" select="@cmd" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$swcmd = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@cmd" />
       </xsl:otherwise>
       </xsl:choose>

	   <xsl:variable name="swtrack" select="@track" />
	   <xsl:choose>
         <xsl:when test="$swtrack = ''">
         <xsl:text></xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text> </xsl:text>
         <xsl:value-of select="@track" />
       </xsl:otherwise>
       </xsl:choose>
	   
	   </TD>
	   	   
	</TR>
</xsl:template>


</xsl:stylesheet>

