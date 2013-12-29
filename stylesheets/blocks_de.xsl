<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" encoding="UTF-8"/>
    
  <xsl:template match="/">
	
    <html>
      <head>
        <title>Rocrail Block report</title>
      </head>
      <body>
        <h2>Liste aller definierten Bl&#246;cke</h2>

        <!-- Tabellenüberschrift -->
        <table border="1" rules="all" cellpadding="5" cellspacing="0" width="95%">
          <THEAD>
            <TR>
              <TH width="5%" height="30" bgcolor="lightgrey">ID</TH>
              <TH width="30%" bgcolor="lightgrey">Beschreibung</TH>
              <TH width="12%" bgcolor="lightgrey">Warten</TH>
              <TH width="12%" bgcolor="lightgrey">Wartedetails</TH>
              <TH width="8%" bgcolor="lightgrey">Stopp</TH>
              <TH width="5%" bgcolor="lightgrey">Ankunft</TH>
              <TH width="10%" bgcolor="lightgrey">Blocktyp</TH>
              <TH width="10%" bgcolor="lightgrey">Zugtyp</TH>
              <TH width="10%" bgcolor="lightgrey">Länge</TH>
              <TH width="5%" bgcolor="lightgrey">El.</TH>
              <TH width="8%" bgcolor="lightgrey">Kopfbhf</TH>
              <TH width="8%" bgcolor="lightgrey">RW</TH>
              <TH width="8%" bgcolor="lightgrey">BBT</TH>
              <TH width="8%" bgcolor="lightgrey">Pendel</TH>
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

  <!-- Vorlage für Blöcke -->
  <xsl:template match="bk">
	<xsl:variable name="waitmode" select="@waitmode" />
	<xsl:variable name="wait" select="@wait" />
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

      <!-- Warten Zufall Fest Block Nein -->
      <TD align="center">
        <xsl:choose>
		  <xsl:when test="$wait = 'false'">
            <xsl:text>Nein</xsl:text>
          </xsl:when>    
          <xsl:otherwise> 
          <xsl:choose>
          <xsl:when test="$waitmode = 'random'">
            <xsl:text>Zufall</xsl:text>
          </xsl:when>
          <xsl:when test="$waitmode = 'fixed'">
            <xsl:text>Fest</xsl:text>
          </xsl:when>
          <xsl:when test="$waitmode = 'loc'">
            <xsl:text>Lok</xsl:text>
          </xsl:when>
          </xsl:choose>
          </xsl:otherwise>    
        </xsl:choose>

      </TD>

      <!-- Wartedetails -->
      <TD align="center">
        <xsl:choose>
        <xsl:when test="$wait = 'false'">
            <xsl:text>-</xsl:text>
        </xsl:when>    
          <xsl:otherwise>     
          <xsl:choose>
          <xsl:when test="$waitmode = 'random'">
            <xsl:value-of select="@minwaittime" /> bis <xsl:value-of select="@maxwaittime" />
          </xsl:when>
          <xsl:when test="$waitmode = 'fixed'">
            <xsl:value-of select="@waittime" />
          </xsl:when>
          <xsl:when test="$waitmode = 'loc'">
            <xsl:text>-> Lok!</xsl:text>
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

      <!-- v-Ankunft -->
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
            <xsl:text>Reise</xsl:text>
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
      <!-- Einzelheiten Block-Typ  -->
      <xsl:variable name="type" select="@type" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$type = 'none'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'ice'">
            <xsl:text>Fernverkehr</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'shunting'">
            <xsl:text>Rangieren</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'local'">
            <xsl:text>Nahverkehr</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'goods'">
            <xsl:text>G&#252;terverkehr</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'regional'">
            <xsl:text>Regionalzug</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'light'">
            <xsl:text>Nebenbahn</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'lightgoods'">
            <xsl:text>G&#252;ter-Nebenbahn</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'turntable'">
            <xsl:text>Drehscheibe</xsl:text>
          </xsl:when>
          <xsl:when test="$type = 'post'">
            <xsl:text>Post</xsl:text>
          </xsl:when>

          <xsl:otherwise>
            <xsl:value-of select="@type" />
          </xsl:otherwise>
        </xsl:choose>

      </TD>

      <!-- Berechtigungen Zug-Typ -->
      <xsl:variable name="typeperm" select="@typeperm" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$typeperm = 'none'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'ice'">
            <xsl:text>Fernverkehr</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'mixed'">
            <xsl:text>Mischverkehr</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'person'">
            <xsl:text>Nahverkehr</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'goods'">
            <xsl:text>G&#252;terverkehr</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'regional'">
            <xsl:text>Regionalzug</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'light'">
            <xsl:text>Nebenbahn</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'lightgoods'">
            <xsl:text>G&#252;ter-Nebenbahn</xsl:text>
          </xsl:when>
          <xsl:when test="$typeperm = 'cleaning'">
            <xsl:text>Gleisreinigung</xsl:text>
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

      <!-- Blocklänge -->
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



      <!-- Elektrifiziert -->
      <xsl:variable name="electrified" select="@electrified" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$electrified = 'false'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>Ja</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>
      <!-- Kopfbahnhof -->
      <xsl:variable name="terminalstation" select="@terminalstation" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$terminalstation = 'false'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>Ja</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>

      <!-- Richtungswechsel erlaubt -->
      <xsl:variable name="allowchgdir" select="@allowchgdir" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$allowchgdir = 'true'">
            <xsl:text>Ja</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>Nein</xsl:text>
          </xsl:otherwise>
        </xsl:choose>

      </TD>


      <!-- BBT erlaubt -->
      <xsl:variable name="allowbbt" select="@allowbbt" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$allowbbt = 'false'">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>Ja</xsl:text>

          </xsl:otherwise>
        </xsl:choose>

      </TD>

      <!-- Pendelzug -->
      <xsl:variable name="commuter" select="@commuter" />
      <TD align="center">
        <xsl:choose>
          <xsl:when test="$commuter = 'no'">
            <xsl:text>Nein</xsl:text>
          </xsl:when>
          <xsl:when test="$commuter = 'yes'">
            <xsl:text>Ja</xsl:text>
          </xsl:when>
          <xsl:when test="$commuter = 'only'">
            <xsl:text>Nur</xsl:text>
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

  <xsl:template match="bklist/bk/incl">


    <TR>
      <TD></TD>
      <TD colspan="4">
        zugelassene Lok:  <xsl:value-of select="@id" />
      </TD>
    </TR>

    <!-- Lok ausschliessen -->
    <xsl:apply-templates/>

  </xsl:template>
  <xsl:template match="bklist/bk/excl">

    <TR>
      <TD></TD>
      <TD colspan="4">
        ausgeschlossene Lok:  <xsl:value-of select="@id" />
      </TD>
    </TR>

    <xsl:apply-templates/>

  </xsl:template>


</xsl:stylesheet>
