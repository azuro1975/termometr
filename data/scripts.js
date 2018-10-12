//.pragma library
function httpGetData()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
           // Typical action to be performed when the document is ready:
           var dataJson = JSON.parse(xhttp.responseText);
           document.getElementById("infotext").innerHTML = xhttp.responseText;
           document.getElementById("Czas").innerHTML = "Data godzina: "+dataJson["Time"];
        }
    };
    xhttp.open("GET", "Data.json", true);
    xhttp.send();
}

function parsejson()
{
    httpGetData();
    console.log(document.getElementById("infotext").innerHTML);
}
