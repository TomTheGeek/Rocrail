var req;


function actionSensor(id)
{
  //alert("flip sensor " + id);
 
  // send an XMLHttpRequest
  try {
    req = new XMLHttpRequest();
    req.onreadystatechange = processReqChange;
    req.open("GET", "rocweb.xml?fb_flip="+id.replace("fb_",""), true);
    req.send("");
  } 
    catch(e) {
      alert("exception: " + e);
    }
  }


function actionBlock(id)
  {
  // send an XMLHttpRequest
  document.getElementById(id).style.backgroundColor="yellow";
  }


function processReqChange() {
// only if req shows "loaded"
if (req.readyState == 4) {
  //alert("response status = "+req.status);
  if (req.status == 0) {
    try {
      xmlDoc = req.responseXML;
      fblist = xmlDoc.getElementsByTagName("fb");
      for (var i = 0; i < fblist.length; i++) {
        var div = document.getElementById("fb_"+fblist[i].getAttribute('id'));
        if( "true" == fblist[i].getAttribute('state') )
          div.style.backgroundImage = "url(sensor_on_1.png)";
        else
          div.style.backgroundImage = "url(sensor_off_1.png)";
      }
    }
    catch(e) {
      alert("exception: " + e);
    }
 
  }
  if (req.status == 200) {
    // ...processing statements go here...
    alert("response received");
    }
  }
}

function loadPlan() {
  // send an XMLHttpRequest
  try {
    req = new XMLHttpRequest();
    req.onreadystatechange = processReqPlan;
    req.open("GET", "plan.xml", true);
    req.send("");
  } 
    catch(e) {
      alert("exception: " + e);
    }
}

function processReqPlan() {
// only if req shows "loaded"
if (req.readyState == 4) {
  // only if "OK"
  //alert("response status = "+req.status);
  if (req.status == 0) {
    try {
      xmlDoc = req.responseXML;
      
      fblist = xmlDoc.getElementsByTagName("fb");
      for (var i = 0; i < fblist.length; i++) {
        var newdiv = document.createElement('div');
        newdiv.setAttribute('id', "fb_"+fblist[i].getAttribute('id'));
        newdiv.setAttribute('onClick', "actionSensor(this.id)");
        newdiv.style.position = "absolute";
        newdiv.style.width    = 32;
        newdiv.style.height   = 32;
        newdiv.style.left     = parseInt(fblist[i].getAttribute('x')) * 32;
        newdiv.style.top      = parseInt(fblist[i].getAttribute('y')) * 32;
        newdiv.innerHTML      = "";
        
        if( "true" == fblist[i].getAttribute('state') )
          newdiv.style.backgroundImage = "url('sensor_on_1.png')";
        else
          newdiv.style.backgroundImage = "url('sensor_off_1.png')";

        document.body.appendChild(newdiv);
      }
      
      bklist = xmlDoc.getElementsByTagName("bk");
      for (var i = 0; i < bklist.length; i++) {
        var newdiv = document.createElement('div');
        newdiv.setAttribute('id', "bk_"+bklist[i].getAttribute('id'));
        newdiv.setAttribute('onClick', "actionBlock(this.id)");
        newdiv.style.position = "absolute";
        newdiv.style.width    = 128;
        newdiv.style.height   = 32;
        newdiv.style.left     = parseInt(bklist[i].getAttribute('x')) * 32;
        newdiv.style.top      = parseInt(bklist[i].getAttribute('y')) * 32;
        newdiv.innerHTML      = "<p>"+bklist[i].getAttribute('locid')+"</p>";
        newdiv.style.textAlign= "center";
        
        if( "true" == bklist[i].getAttribute('entering') )
          newdiv.style.background = "#CCCCFF";
        else if( "closed" == bklist[i].getAttribute('state') )
          newdiv.style.background = "#CCCCCC";

        newdiv.style.backgroundImage = "url('block_1.png')";

        document.body.appendChild(newdiv);
      }
    }
    catch(e) {
      alert("exception: " + e);
    }
 
  }
  if (req.status == 200) {
    // ...processing statements go here...
    alert("response received");
    }
  }
}


