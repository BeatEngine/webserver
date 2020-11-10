function add()
    {
        var a = document.getElementById("a").value;
        var b = document.getElementById("b").value;
        var xhttp =  new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
              document.getElementById("result").innerHTML = this.responseText;
            }
          };
          xhttp.open("POST", "/add", true);
          xhttp.send("a="+a+"&b="+b); 

    }
