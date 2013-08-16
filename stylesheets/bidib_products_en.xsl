<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<xsl:template match="/">

   <html>
   <head><title>BiDiB Products report</title>
  </head>
  <body>
  <h2>BiDiB Products</h2>
  <table border="0" rules="none" cellpadding="4" cellspacing="0" width="90%">
      <THEAD>
    <TR>
       <TH bgcolor="lightgrey">VID</TH>
       <TH bgcolor="lightgrey">PID</TH>
       <TH bgcolor="lightgrey">CID</TH>
       <TH bgcolor="lightgrey">Description</TH>
       <TH bgcolor="lightgrey">Documentation</TH>
    </TR>
      </THEAD> 
      <TBODY>
        <xsl:apply-templates/>
      </TBODY>
   </table>
   </body>
   </html>
   
</xsl:template>

<xsl:template match="bidib">  
  <h3>Version: <xsl:value-of select="@version"/></h3>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="productlist">  

  <xsl:apply-templates>
   <xsl:sort select="@pid" order="ascending" data-type="number" />
  
  </xsl:apply-templates>

</xsl:template>

<xsl:template match="product">  

  <TR>  
     <TD align="left"><xsl:value-of select="@vid" /></TD>
     <TD align="left"><xsl:value-of select="@pid" /></TD>
     <TD align="left"><xsl:value-of select="@cid" /></TD>
     <TD align="left"><xsl:value-of select="@desc" /></TD>
     <TD align="left">
       <xsl:element name="a">
       <xsl:attribute name="href">
       <xsl:value-of select="@url"/>
       </xsl:attribute>
       <xsl:value-of select="@url"/>
       </xsl:element>
     </TD>
  </TR>


<xsl:apply-templates/>
 
</xsl:template>


</xsl:stylesheet>
