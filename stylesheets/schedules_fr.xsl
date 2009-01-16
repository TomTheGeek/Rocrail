<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Schedules report</title>
  </head>
  <body>
  <h2>Compte-rendu des horaires</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">ID</TH>
	     <TH width="15%" height="30" bgcolor="lightgrey">Temps relatif</TH>
	     <TH width="30%" height="30" bgcolor="lightgrey"> D&#233;marrage de l'horaire</TH>
	     <TH width="35%" height="30" bgcolor="lightgrey">Sortie</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="sclist">  
  
  <xsl:apply-templates/>

</xsl:template>


<xsl:template match="sc">  

	<TR>
       <th rowspan="2" align="center">
	    <xsl:value-of select="@id" />
	   </th>	
 
       <td align="center">
	   <xsl:choose>
         <xsl:when test="@relativetime">
         <xsl:value-of select="@relativetime" />
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>false</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </td>	

	   <xsl:variable name="scaction" select="@scaction" />

	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$scaction = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@scaction" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="coaction" select="@coaction" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="@coaction">
   	       <xsl:choose>
             <xsl:when test="$coaction = ''">
                <xsl:text>-</xsl:text>
             </xsl:when>
           <xsl:otherwise>
                <xsl:value-of select="@coaction" /> -&gt; <xsl:value-of select="@coactioncmd" />
           </xsl:otherwise>
           </xsl:choose>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
   </TR>

    <TR>
	<td colspan="3">
    <table border="0" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="25%" height="30" bgcolor="white">Bloc</TH>
	     <TH width="25%" height="30" bgcolor="white">Heure</TH>
	     <TH width="25%" height="30" bgcolor="white">Minute</TH>
 	     <TH width="25%" bgcolor="white">Swap</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
      </table>

	</td>
	</TR>
 
</xsl:template>

<xsl:template match="scentry">  

      <TR>
	  
	   <xsl:variable name="block" select="@block" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$block = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@block" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="hour" select="@hour" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$hour = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@hour" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>
	   
	   <xsl:variable name="minute" select="@minute" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$minute = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@minute" />
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   <xsl:variable name="swap" select="@swap" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$swap = 'true'">
         <xsl:value-of select="@swap" />
       </xsl:when>
       </xsl:choose>
	   </TD>
	   
	</TR>
</xsl:template>


</xsl:stylesheet>

