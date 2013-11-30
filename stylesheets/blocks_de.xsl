<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Block report</title>
  </head>
  <body>
  <h2>Liste aller definierten Bl&#246;cke</h2>

  <table border="1" rules="all" cellpadding="5" cellspacing="0" width="80%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">ID</TH>
 	     <TH width="40%" bgcolor="lightgrey">Beschreibung</TH>
 	     <TH width="10%" bgcolor="lightgrey">L&#227;nge</TH>
 	     <TH width="10%" bgcolor="lightgrey">Typ</TH>
 	     <TH width="10%" bgcolor="lightgrey">elektrifiziert</TH>
 	     <TH width="10%" bgcolor="lightgrey">Kopfbahnhof</TH>
 	     <TH width="10%" bgcolor="lightgrey">BBT</TH>
 	     <TH width="10%" bgcolor="lightgrey">Berechtigung</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="bklist">  

  <xsl:apply-templates>
   <xsl:sort select="@id" order="ascending" data-type="text" />
  
  </xsl:apply-templates>

</xsl:template>


<xsl:template match="bk">  

	<TR>	
       <xsl:variable name="search_bk_id" select="@id" />
	   <TD><xsl:value-of select="@id" /></TD>
	   <xsl:variable name="desc1" select="@desc" />
       <TD>

	   <xsl:choose>
         <xsl:when test="$desc1 = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@desc" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	   <xsl:variable name="len" select="@len" />
	   <TD align="center">

	   <xsl:choose>
         <xsl:when test="$len = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@len" />
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
		  <TD align="center"><xsl:value-of select="@type" /></TD> 
		  
	   <xsl:variable name="electrified" select="@electrified" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$electrified = 'false'">
         <xsl:text></xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>Ja</xsl:text>
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
		  
	   <xsl:variable name="terminalstation" select="@terminalstation" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$terminalstation = 'false'">
         <xsl:text></xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>Ja</xsl:text>
       </xsl:otherwise>
       </xsl:choose>

	   </TD>


	   <xsl:variable name="allowbbt" select="@allowbbt" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$allowbbt = 'false'">
         <xsl:text></xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>Ja</xsl:text>
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	   
	   
	   
	   <TD align="center"><xsl:value-of select="@typeperm" /></TD>
	</TR>

    <xsl:variable name="id" select="@id"/>

    <xsl:for-each select="../../bklist/bk">
          <xsl:variable name="bkid" select="@id"/>
            <xsl:for-each select="fbevent">
              <xsl:if test="@id=$id">

			  <tr>
			    <td></td>
                <td colspan="3"><strong>Block: </strong> <xsl:value-of select="$bkid"/> -&gt; <xsl:value-of select="@action"/>
				von Block <xsl:value-of select="@from"/>
				</td>
			  </tr>	
              </xsl:if>
            </xsl:for-each>
    </xsl:for-each>	

    <xsl:apply-templates/>
 
	</xsl:template>
<xsl:template match="incl">  

	<TR>	
	   <TD></TD>
	   <TD colspan="3">
			Einschluss: <xsl:value-of select="@id" />
		</TD>	
	</TR>	   
    
    <xsl:apply-templates/>

</xsl:template>
<xsl:template match="excl">  

	<TR>	
	   <TD></TD>
	   <TD colspan="4">
			Ausschluss: <xsl:value-of select="@id" />
		</TD>	
	</TR>	   
    
    <xsl:apply-templates/>

</xsl:template>

<xsl:template match="bkaction">  

	<TR>	
	   <TD></TD>
	   <TD colspan="3">
			<strong>Aktion: </strong><xsl:value-of select="@type" /> -&gt; <xsl:value-of select="@id" /> -&gt; <xsl:value-of select="@cmd" />	
		</TD>	
	</TR>	   
    
    <xsl:apply-templates/>

</xsl:template>

</xsl:stylesheet>

