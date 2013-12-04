<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" encoding="UTF-8"/>

  <xsl:template match="/">

    <html>
      <head>
        <title>Rocrail Block report</title>
      </head>
      <body>
        <h2>Block Report</h2>

        <!-- Tabellenüberschrift -->
        <table border="1" rules="all" cellpadding="5" cellspacing="0" width="95%">
          <THEAD>
            <TR>
              <TH width="5%" height="30" bgcolor="lightgrey">ID</TH>
              <TH width="30%" bgcolor="lightgrey">Description</TH>
              <TH width="12%" bgcolor="lightgrey">Wait</TH>
              <TH width="12%" bgcolor="lightgrey">Wait details</TH>
              <TH width="8%" bgcolor="lightgrey">Stop</TH>
              <TH width="5%" bgcolor="lightgrey">Arrive</TH>
              <TH width="10%" bgcolor="lightgrey">Type</TH>
              <TH width="10%" bgcolor="lightgrey">Permissions</TH>
              <TH width="10%" bgcolor="lightgrey">Length</TH>
              <TH width="5%" bgcolor="lightgrey">El.</TH>
              <TH width="8%" bgcolor="lightgrey">Terminal</TH>
              <TH width="8%" bgcolor="lightgrey">cd</TH>
              <TH width="8%" bgcolor="lightgrey">BBT</TH>
              <TH width="8%" bgcolor="lightgrey">Commuter</TH>
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
  <xsl:template match="bklist">

    <xsl:apply-templates>
      <xsl:sort select="@id" order="ascending" data-type="text" />

    </xsl:apply-templates>

  </xsl:template>


  <xsl:template match="bk">

    <TR>

      <!-- Blockkennung -->
      <xsl:variable name="search_bk_id" select="@id" />
      <TD align="center">
        <xsl:value-of select="@id" />
      </TD>

      <!-- Beschreibung -->
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

      <!-- Wait -->
      <TD align="center">
        <xsl:variable name="waitmode" select="@waitmode" />
        <xsl:variable name="wait" select="@wait" />      
        <xsl:choose>
          <xsl:when test="$wait = 'false'">
            <xsl:text>no</xsl:text>
          </xsl:when>    
          <xsl:otherwise> 
          <xsl:choose>
          <xsl:when test="$waitmode = 'random'">
            <xsl:text>Random</xsl:text>
          </xsl:when>
          <xsl:when test="$waitmode = 'fixed'">
            <xsl:text>Fixed</xsl:text>
          </xsl:when>
          <xsl:when test="$waitmode = 'loc'">
            <xsl:text>Loco</xsl:text>
          </xsl:when>
          </xsl:choose>
          </xsl:otherwise>    
        </xsl:choose>

      </TD>

      <!-- Wait details -->
      <xsl:variable name="waitmode" select="@waitmode" />
      <xsl:variable name="wait" select="@wait" />
      <TD align="center">
        <xsl:choose>
        <xsl:when test="$wait = 'false'">
            <xsl:text>-</xsl:text>
        </xsl:when>    
          <xsl:otherwise>     
          <xsl:choose>
          <xsl:when test="$waitmode = 'random'">
            <xsl:value-of select="@minwaittime" /> to <xsl:value-of select="@maxwaittime" />
          </xsl:when>
          <xsl:when test="$waitmode = 'fixed'">
            <xsl:value-of select="@waittime" />
          </xsl:when>
          <xsl:when test="$waitmode = 'loc'">
            <xsl:text>-> Loco!</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>-</xsl:text>
          </xsl:otherwise>
          </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>

      </TD>


      <!-- v-STOP -->
      <TD align="center">
        <xsl:variable name="stopspeed" select="@stopspeed" />
        <xsl:choose>
          <xsl:when test="$stopspeed = 'min'">
            <xsl:text>Min.</xsl:text>
          </xsl:when>
          <xsl:when test="$stopspeed = 'mid'">
            <xsl:text>Mid.</xsl:text>
          </xsl:when>
          <xsl:when test="$stopspeed = 'percent'">
            <xsl:text>%</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="@stopspeed" />
          </xsl:otherwise>
        </xsl:choose>

      </TD>

      <!-- v-Arrive -->
      <xsl:variable name="speed" select="@speed" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$speed = 'min'">
            <xsl:text>Min.</xsl:text>
          </xsl:when>
          <xsl:when test="$speed = 'mid'">
            <xsl:text>Mid.</xsl:text>
          </xsl:when>
          <xsl:when test="$speed = 'cruise'">
            <xsl:text>Cruise</xsl:text>
          </xsl:when>
          <xsl:when test="$speed = 'max'">
            <xsl:text>Max.</xsl:text>
          </xsl:when>
          <xsl:when test="$speed = 'percent'">
            <xsl:text>%</xsl:text>
          </xsl:when>

          <xsl:otherwise>
            <xsl:value-of select="@speed" />
          </xsl:otherwise>
        </xsl:choose>
      </TD>
      <!--  Block type  -->
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

      <!-- Permissions -->
      <xsl:variable name="typeperm" select="@typeperm" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$typeperm = 'none'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'ice'">
            <xsl:text>Intercity</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'mixed'">
            <xsl:text>Mixed</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'person'">
            <xsl:text>Person</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'goods'">
            <xsl:text>Freight</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'regional'">
            <xsl:text>Regional</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'light'">
            <xsl:text>Light</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'lightgoods'">
            <xsl:text>Light freight</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'cleaning'">
            <xsl:text>Cleaning</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'post'">
            <xsl:text>Post</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = ''">
            <xsl:text>-</xsl:text>
          </xsl:when>

          <xsl:otherwise>
            <xsl:value-of select="@typeperm" />
          </xsl:otherwise>
        </xsl:choose>

      </TD>

      <!-- Block length -->
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



      <!-- Electrified -->
      <xsl:variable name="electrified" select="@electrified" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$electrified = 'false'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>yes</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>
      <!-- Terminal station -->
      <xsl:variable name="terminalstation" select="@terminalstation" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$terminalstation = 'false'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>yes</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>

      <!-- allow change direction -->
      <xsl:variable name="allowchgdir" select="@allowchgdir" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$allowchgdir = 'true'">
            <xsl:text>yes</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>no</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>


      <!-- BBT allowed -->
      <xsl:variable name="allowbbt" select="@allowbbt" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$allowbbt = 'false'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>yes</xsl:text>

          </xsl:otherwise>
        </xsl:choose>

      </TD>

      <!-- Commuter train -->
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

    </TR>

    <!-- Lok zulassen -->
    <xsl:apply-templates/>

  </xsl:template>

  <xsl:template match="incl">


    <TR>
      <TD></TD>
      <TD colspan="4">
        Allow Loco: <xsl:value-of select="@id" />
      </TD>
    </TR>

    <!-- denied loco -->
    <xsl:apply-templates/>

  </xsl:template>
  <xsl:template match="excl">

    <TR>
      <TD></TD>
      <TD colspan="4">
        Deny Loco: <xsl:value-of select="@id" />
      </TD>
    </TR>

    <xsl:apply-templates/>

  </xsl:template>


</xsl:stylesheet>
