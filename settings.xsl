<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">


<xsl:template match="/">
 <html>
 <head>
 <style type="text/css">
 
 #config
 {
   width:25%;
   float:left;
 }
 #default-folder
 {
   width:25%;
   float:left;
 }
 #image-sound-files
 {
   width:25%;
   float:left;
 }
 </style>
 <script type="text/javascript"><![CDATA[

function update(thisform)
 {
     xml=loadXMLDoc("settings.xml");
    
     for(var i=0;i<thisform.length;i++)
     {
        xmltag=thisform.elements[i].getAttribute("name");
        tagfound=xml.getElementsByTagName(xmltag)[0];
        if(tagfound){
            //document.write(tagfound.childNodes[0].nodeValue+"="+thisform.elements[i].value+" ");
            tagfound.childNodes[0].nodevalue=thisform.elements[i].value;
        }
     }
     documnet.write(xml.)
     
     return true;
 }
function loadXMLDoc(dname)
{
if (window.XMLHttpRequest)
  {
  xhttp=new XMLHttpRequest();
  }
else
  {
  xhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
xhttp.open("GET",dname,false);
xhttp.send("");
return xhttp.responseXML;
}
]]>
 </script>
 </head>
 <body>
 <h2> MyPopstation Config</h2>
 
 <xsl:apply-templates /> 
 </body>
 </html>
</xsl:template>

<xsl:template match="config-settings">
  
  <xsl:apply-templates select="common"/>
  <xsl:apply-templates select="default-folders"/>
  <xsl:apply-templates select="default-image_sound-files"/>
  
</xsl:template>

<xsl:template match="common">
  <form name="common" method="post" onsubmit="update(this);">
  <div id="config">
  <span style="color:#0099CC" display="inline">
   Common Config options </span>
  <table border="1">
  <tr bgcolor="#D147FF">
    <th>Option</th>
    <th>Value</th>
  </tr>
  <xsl:for-each select="./*">
  <tr>
    <td><xsl:value-of select="name(.)"/></td>
    <td><input type="text">
     <xsl:attribute name="name">
      <xsl:value-of select="name(.)"/>
     </xsl:attribute>
     <xsl:attribute name="value">
     <xsl:value-of select="."/>
     </xsl:attribute>
    </input></td>
  </tr>
  </xsl:for-each>
  </table>
  <input type="submit" id="btn_sub"  value="update"/>
  <input type="reset" name="reset"/>
  </div>
  </form>
</xsl:template>

<xsl:template match="default-folders">
  <form name="default_folders" method="post" onsubmit="update(this);" >
  <div id="default-folder">
  <span style="color:#0099CC" display="inline">
   Default Folders </span>
  <table border="1">
  <tr bgcolor="#D147FF">
    <th>Option</th>
    <th>Value</th>
  </tr>
  <xsl:for-each select="./*">
  <tr>
    <td><xsl:value-of select="name(.)"/></td>
    <td><input type="text">
     <xsl:attribute name="name">
      <xsl:value-of select="name(.)"/>
     </xsl:attribute>
     <xsl:attribute name="value">
     <xsl:value-of select="."/>
     </xsl:attribute>
    </input></td>
  </tr>
  </xsl:for-each>
  </table>
  <input type="submit" id="btn_sub" name="btn_sub" value="update" />
  <input type="reset" name="reset"/>
  </div>
  </form>
</xsl:template>

<xsl:template match="default-image_sound-files">
  <form name="default-img/snd" method="post" onsubmit="update(this);" >
  <div id="image-sound-files">
  <span style="color:#0099CC">
   Default Image Sound Files </span>
  <table border="1">
  <tr bgcolor="#D147FF">
    <th>Option</th>
    <th>Value</th>
  </tr>
  <xsl:for-each select="./*">
  <tr>
    <td><xsl:value-of select="name(.)"/></td>
    <td><input type="text">
     <xsl:attribute name="name">
      <xsl:value-of select="name(.)"/>
     </xsl:attribute>
     <xsl:attribute name="value">
     <xsl:value-of select="."/>
     </xsl:attribute>
    </input></td>
  </tr>
  </xsl:for-each>
  </table>
  <input type="submit" id="btn_sub" name="btn_sub" value="update" />
  <input type="reset" name="reset"/>
  </div>
  </form>
</xsl:template>

</xsl:stylesheet>
