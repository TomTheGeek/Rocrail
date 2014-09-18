<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<!-- Setting table header -->
<xsl:template match="/">

   <html>
   <head><title>Rocrail Objects Variables</title>
  </head>
  <body>
  <h2>Rocrail Report: Variables</h2>

  <table border="1" rules="all" cellpadding="2" cellspacing="0" width="95%">
      <THEAD>
	  <TR>
	    <th width="15%" height="30" bgcolor="lightgrey">ID</th>
			<th width="20%" height="20" bgcolor="lightgrey">Group</th>
			<th width="20%" height="20" bgcolor="lightgrey">Description</th>
 	    <th width="20%" height="20" bgcolor="lightgrey">Text</th>
			<th width="8%" height="20" bgcolor="lightgrey">Value</th>
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

<!-- Sorting ID -->
	<xsl:template match="vrlist">

		<xsl:apply-templates>
		  <xsl:sort select="@id" order="ascending" data-type="text" />
		</xsl:apply-templates>

  </xsl:template>


<!-- Template ID -->
	<xsl:template match="vrlist/vr">
		<tr>
		<td align="center">
			<xsl:value-of select="@id" />
		</td>
		
<!-- evaluate group-->
		 <xsl:variable name="Group" select="@group" />
	   <TD align="center">

	   <xsl:choose>
         <xsl:when test="$Group = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@group" />
       </xsl:otherwise>
     </xsl:choose>

	   </TD>
	
<!-- evaluate description-->
		 <xsl:variable name="desc1" select="@desc" />
	   <TD align="center">

	   <xsl:choose>
         <xsl:when test="$desc1 = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@desc" />
       </xsl:otherwise>
     </xsl:choose>

	   </TD>
	
<!-- evaluate text-->
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
		
<!-- evaluate value-->

		<td align="center">
			<xsl:value-of select="@value" />
		</td>
		

		
<!-- evaluate min-->
		<td align="center">
			<xsl:value-of select="@min" />
		</td>
  
<!-- evaluate max-->
		<td align="center">
			<xsl:value-of select="@max" />
		</td>
		
  

	</tr>
<!-- show actions -->
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



