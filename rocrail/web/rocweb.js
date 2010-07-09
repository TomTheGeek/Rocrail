
function updateBlock(id, clr, lcid, img, ref) {
  text = "<a href='" + ref + "'>" + lcid + "</a>";
  document.getElementById(id).innerHTML = text;
  document.getElementById(id).style.backgroundColor = clr;
  document.getElementById(id).style.backgroundImage = img;
}

function updateItem(id, img, ref) {
  text = "<a href='" + ref + "'><img src='" + img + "'></a>";
  document.getElementById(id).innerHTML = text;
}

function actionSensor(id) {

  // send an XMLHttpRequest
  try {
    req = new XMLHttpRequest();
    req.onreadystatechange = processReqChange;
    req.open("GET", "?fb_flip=" + id.replace("fb_", ""), true);
    req.send("");
  } 
  catch (e) {
  }
}

function actionBlock(id) {
  // send an XMLHttpRequest
  document.getElementById(id).style.backgroundColor = "yellow";
}

function processReqChange() {
  // only if req shows "loaded"
  if (req.readyState == 4) {
    // only if "OK"
    if (req.status == 200) {
      // ...processing statements go here...
    }
  }
}
