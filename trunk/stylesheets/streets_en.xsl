<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Routes report</title>
  </head>
  <body>
  <h2>Routes report</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">ID</TH>
	     <TH width="15%" height="30" bgcolor="lightgrey">From block</TH>
	     <TH width="15%" height="30" bgcolor="lightgrey">To block</TH>
	     <TH width="10%" height="30" bgcolor="lightgrey">Crossing block</TH>
	     <TH width="20%" height="30" bgcolor="lightgrey"><xsl:text> </xsl:text></TH>
	  </TR>
	  <TR>
	     <TH height="30" bgcolor="white"><xsl:text> </xsl:text></TH>
	     <TH height="30" bgcolor="lightgrey">Usage</TH>
	     <TH height="30" bgcolor="lightgrey">Run direction</TH>
	     <TH height="30" bgcolor="lightgrey">Swap placing</TH>
	     <TH height="30" bgcolor="lightgrey">Speed</TH>
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
  
  <xsl:apply-templates/>

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

	   <xsl:variable name="bkc" select="@bkc" />
	   <TD align="center">
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
   </TR>
   <TR>
      <xsl:variable name="dir" select="@dir" />

      <TD align="center">
	   <xsl:choose>
         <xsl:when test="@dir">
   	       <xsl:choose>
             <xsl:when test="$dir = 'true'">
                <xsl:text>From - To</xsl:text>
             </xsl:when>
           <xsl:otherwise>
                <xsl:text>Both directions</xsl:text>
           </xsl:otherwise>
           </xsl:choose>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

      <xsl:variable name="lcdir" select="@lcdir" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="@lcdir">
   	       <xsl:choose>
             <xsl:when test="$lcdir = 'true'">
                <xsl:text>forward</xsl:text>
             </xsl:when>
           <xsl:otherwise>
                <xsl:text>reverse</xsl:text>
           </xsl:otherwise>
           </xsl:choose>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>-</xsl:text>
       </xsl:otherwise>
       </xsl:choose>
	   </TD>

	   
       <xsl:variable name="swap" select="@swap" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="@sawp">
   	       <xsl:choose>
             <xsl:when test="$swap = 'true'">
                  <xsl:value-of select="@swap" />
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

       <xsl:variable name="speed" select="@speed" />
	   <TD align="center">
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
	<td colspan="4">
    <table border="0" rules="all" cellpadding="4" cellspacing="0" width="100%">
      <THEAD>
	  <TR colspan="2">
	     <TH align="left" bgcolor="white">Switches</TH>
	  </TR>
	  <TR>
	     <TH width="25%" height="30" bgcolor="lightgrey">ID</TH>
	     <TH width="25%" height="30" bgcolor="lightgrey">Command</TH>
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

