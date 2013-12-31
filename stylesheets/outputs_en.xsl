<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="UTF-8"/>

<!-- Tabellenkopf einrichten -->
<xsl:template match="/">

   <html>
   <head><title>Rocrail Objekte Ausgänge</title>
  </head>
  <body>
  <h2>Rocrail Report: Outputs</h2>

  <table border="1" rules="all" cellpadding="2" cellspacing="0" width="95%">
      <THEAD>
	  <TR>
	     <TH width="20%" height="30" bgcolor="lightgrey"> ID</TH>
 	     <th width="10%" height="20" bgcolor="lightgrey">Controller</th>
		 <th width="10%" height="20" bgcolor="lightgrey">Address</th>
		 <th width="10%" height="20" bgcolor="lightgrey">Port</th>
		 <th width="10%" height="20" bgcolor="lightgrey">Gate</th>
		 <th width="10%" height="20" bgcolor="lightgrey">Type</th>
		 <th width="10%" height="20" bgcolor="lightgrey">State</th>
		 <th width="5%" height="20" bgcolor="lightgrey">Accessory</th>
		 <th width="10%" height="20" bgcolor="lightgrey">Options</th>
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
  <xsl:template match="colist">

    <xsl:apply-templates>
      <xsl:sort select="@id" order="ascending" data-type="text" />
    </xsl:apply-templates>

  </xsl:template>


    <!-- Vorlage für Ausgänge -->
	<xsl:template match="colist/co">
	<xsl:variable name="asswitch" select="@asswitch" />
	<tr>
	<td align="center">
	<xsl:value-of select="@id" />
	</td>
	
	<xsl:variable name="iid1" select="@iid" />
	<td align="center">
	   <xsl:choose>
         <xsl:when test="$iid1 = ''">
         <xsl:text>-</xsl:text>
       </xsl:when>
       <xsl:otherwise>
         <xsl:value-of select="@iid" />
       </xsl:otherwise>
       </xsl:choose>
	</td>
	
	<td align="center"><xsl:value-of select="@addr" /></td>
	<td align="center"><xsl:value-of select="@port" /></td>
	
	<!--  Ausgang als Weiche und Gate auswerten -->
	  <xsl:variable name="gate" select="@gate" />
	  <TD align="center">
	  <xsl:choose>
	  <xsl:when test="@asswitch='true'">
	    <xsl:text>switch</xsl:text>
      </xsl:when>
	  <xsl:otherwise>
	       
       <xsl:choose>
          <xsl:when test="$gate='0'">
            <xsl:text>red</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>green</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
		</xsl:otherwise>
		</xsl:choose>
      </TD>
	
	<!-- Typ auswerten -->
      <xsl:variable name="toggleswitch" select="@toggleswitch" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$toggleswitch= 'true'">
            <xsl:text>Toggle switch</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>Push button</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>
	
	<!-- Anzahl Stellungen auswerten -->
      <xsl:variable name="tristate" select="@tristate" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$tristate= 'true'">
            <xsl:text>3-state</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>2-state</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>
		
	<!-- Zubehör Ja/Nein auswerten -->
      <xsl:variable name="accessory" select="@accessory" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$accessory= 'true'">
            <xsl:text>Yes</xsl:text>
          </xsl:when>
          <xsl:otherwise>
		    <xsl:text>Porttype</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>
	
	<!-- Optionen auswerten -->
	  <xsl:variable name="inv" select="@inv" />
	  <TD align="center" STYLE="font-size:9pt">
       <xsl:choose>
          <xsl:when test="$inv= 'true'">
		    <xsl:text>inverted</xsl:text>
		  </xsl:when>
        </xsl:choose>
	    
	  <xsl:variable name="blink" select="@blink" />
       <xsl:choose>
          <xsl:when test="$blink= 'true'">
		  <br></br>
            <xsl:text>blink</xsl:text>
		  </xsl:when>
	   </xsl:choose>
	     
	   <xsl:choose>
          <xsl:when test="$asswitch= 'true'">
		  <br></br>
           <xsl:text>switch</xsl:text>
		   </xsl:when>
		   <xsl:otherwise>
		    <xsl:text>-</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
	  
      </TD>
	  
	</tr>
	
    <!-- Aktionen anzeigen -->
    <xsl:apply-templates/>

  </xsl:template>

  <xsl:template match="colist/co/actionctrl">


    <TR>
      <TD></TD>
      <TD colspan="3" bgcolor="#F8F8F8">
        Action :  <xsl:value-of select="@id" />
      </TD>
    </TR>
 </xsl:template>

</xsl:stylesheet>



