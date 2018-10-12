//.pragma libraryfunction objectToTable(myTabela, myobject)
function dataToTable(myTabela, obiekt, poziom)
{
	for (var item in obiekt)
	{
		
		var row = myTabela.insertRow();
		for ( var i = 0; i<poziom;++i)
		{
			row.insertCell();
		}
		row.insertCell().innerHTML = item;
		if (Array.isArray(obiekt[item]) && obiekt[item].length>0)
		{
			for ( var i = 0 ; i<obiekt[item].length;++i)
			{
				if (typeof obiekt[item][i] == "object")
					dataToTable(myTabela, obiekt[item][i], poziom+1);
				else
				{
					if (item == "TimeOn" || item == "TimeOff")
					{
						var d = new Date();
						d.setTime(obiekt[item][i]);
						row.insertCell().innerHTML = d;
					}
					else
						row.insertCell().innerHTML = obiekt[item][i];
				}
			}
		}
		else
		{			
			row.insertCell().innerHTML = obiekt[item];
		}
	}
}
function httpGetData()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
           // Typical action to be performed when the document is ready:
           var dataJson = JSON.parse(xhttp.responseText, function(key, value){
				if (key=="Time") {
					document.getElementById("Czas").innerHTML = value;
				}
				return value;
		   });
		   var myTabela = document.getElementById("Tabela");
		   dataToTable(myTabela, dataJson, 0);
		   
           document.getElementById("infotext").innerHTML = xhttp.responseText;
           //document.getElementById("Czas").innerHTML = "Data godzina: "+dataJson["Time"];
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
