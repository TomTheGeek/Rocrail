<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>Rocrail Feedback report</title>
  </head>
  <body>
  <h2>Feedback report</h2>

  <table border="1" rules="all" cellpadding="4" cellspacing="0" width="80%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey">ID</TH>
 	     <TH width="40%" bgcolor="lightgrey">Description</TH>
 	     <TH width="20%" bgcolor="lightgrey">IID</TH>
 	     <TH width="20%" bgcolor="lightgrey">Address</TH>
	  </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="fblist">  

  <xsl:apply-templates>
   <xsl:sort select="@addr" order="ascending" data-type="number" />
  
  </xsl:apply-templates>

</xsl:template>


<xsl:template match="fb">  

	<TR>	
       <xsl:variable name="search_fb_id" select="@id" />
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
	</TR>

    <xsl:variable name="id" select="@id"/>

    <xsl:for-each select="../../bklist/bk">
          <xsl:variable name="bkid" select="@id"/>
            <xsl:for-each select="fbevent">
              <xsl:if test="@id=$id">
			  <tr>
			    <td></td>
                <td colspan="3"><strong>Block: </strong> <xsl:value-of select="$bkid"/> -&gt; <xsl:value-of select="@action"/>
				from Block <xsl:value-of select="@from"/>
				</td>
			  </tr>	
              </xsl:if>
            </xsl:for-each>
    </xsl:for-each>	

    <xsl:apply-templates/>
 
	</xsl:template>

<xsl:template match="fbaction">  

	<TR>	
	   <TD></TD>
	   <TD colspan="3">
			<strong>Action: </strong><xsl:value-of select="@type" /> -&gt; <xsl:value-of select="@id" /> -&gt; <xsl:value-of select="@cmd" />	
		</TD>	
	</TR>	   
    
    <xsl:apply-templates/>

</xsl:template>

</xsl:stylesheet>

