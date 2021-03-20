const char MAIN_page2[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
    <title>System configuration</title>
</head>
<body>
   
</body>
</html>
<script language="javascript">
   var link = "http://" + window.location.hostname + ":80/reset";
    var a = document.createElement('a');
    var linkText = document.createTextNode("Reset system");
    a.appendChild(linkText);
    a.title = "Reset system";
    a.href = link;
    document.body.appendChild(a);
</script>
)=====";
