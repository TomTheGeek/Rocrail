<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Block report</title>
  </head>
  <body>
  <h2>Block report</h2>

  <table border="1" rules="all" cellpadding="5" cellspacing="0" width="80%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">Block</TH>
 	     <TH width="40%" bgcolor="lightgrey">Description</TH>
 	     <TH width="10%" bgcolor="lightgrey">Length</TH>
 	     <TH width="10%" bgcolor="lightgrey">Type</TH>
 	     <TH width="10%" bgcolor="lightgrey">out of operation</TH>
 	     <TH width="10%" bgcolor="lightgrey">electrified</TH>
 	     <TH width="10%" bgcolor="lightgrey">Terminal Station</TH>
 	     <TH width="10%" bgcolor="lightgrey">change direction</TH>
 	     <TH width="10%" bgcolor="lightgrey">BBT</TH>
 	     <TH width="10%" bgcolor="lightgrey">Commuter train</TH>
 	     <TH width="10%" bgcolor="lightgrey">Permissions</TH>
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
	   
	   <xsl:variable name="type" select="@type" />
		  <TD align="center"> 
		  <xsl:choose>
       <xsl:when test="$type = 'none'">
         <xsl:text>None</xsl:text>
       </xsl:when>  
       <xsl:when test="$type = 'ice'">
         <xsl:text>Intercity</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'shunting'">
         <xsl:text>Shunting</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'local'">
         <xsl:text>Local</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'goods'">
         <xsl:text>Freight</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'regional'">
         <xsl:text>Regional</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'light'">
         <xsl:text>Light</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'lightgoods'">
         <xsl:text>Light freight</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'turntable'">
         <xsl:text>Turntable</xsl:text>
       </xsl:when>
       <xsl:when test="$type = 'post'">
         <xsl:text>Post</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@type" />
       </xsl:otherwise>
       </xsl:choose>
		  </TD>
		  
			<xsl:variable name="state" select="@state" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$state = 'open'">
         <xsl:text></xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>Yes</xsl:text>
       </xsl:otherwise>
       </xsl:choose>

	   </TD>		  
		  
		  
	   <xsl:variable name="electrified" select="@electrified" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$electrified = 'false'">
         <xsl:text></xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text>Yes</xsl:text>
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
         <xsl:text>Yes</xsl:text>
       </xsl:otherwise>
       </xsl:choose>

	   </TD>

			<xsl:variable name="allowchgdir" select="@allowchgdir" />
	   <TD align="center">
	   <xsl:choose>
         <xsl:when test="$allowchgdir = 'true'">
         <xsl:text>Yes</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:text></xsl:text>
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
         <xsl:text>Yes</xsl:text>
       </xsl:otherwise>
       </xsl:choose>

	   </TD>
	   
	   <xsl:variable name="commuter" select="@commuter" />
	   <TD align="center">
	   <xsl:choose>
       <xsl:when test="$commuter = 'no'">
         <xsl:text>no</xsl:text>
       </xsl:when>  
       <xsl:when test="$commuter = 'yes'">
         <xsl:text>yes</xsl:text>
       </xsl:when>
       <xsl:when test="$commuter = 'only'">
         <xsl:text>only</xsl:text>
       </xsl:when>     
       <xsl:otherwise>
         <xsl:value-of select="@commuter" />
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
	   <TD colspan="4">
			Allow loco: <xsl:value-of select="@id" />
		</TD>	
	</TR>	   
    
    <xsl:apply-templates/>

</xsl:template>
<xsl:template match="excl">  

	<TR>	
	   <TD></TD>
	   <TD colspan="4">
			Deny loco: <xsl:value-of select="@id" />
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

