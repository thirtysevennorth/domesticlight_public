const char index_html[] = R"=====(

<!DOCTYPE html>

<html>

<head>
<style>
header {
    text-align: center;
    text-transform: uppercase;
    background-color: lightslategray;
    margin: 30px;
    margin-top: 30px;
    margin-bottom: 00px;
    padding: 50px;
    padding-top: 30px;
    padding: 10px;
    font-family: Helvetica;
    font-size: 150%;
}   

p {
  color: black;
  padding-left: 50px;
  text-justify: left;
  background-color: whitesmoke;
}

h1 {text-align: center;
  background-color: lightslategray;
  font-family: Helvetica;
  }

.myDiv {
  border: 5px firebrick;
  background-color: whitesmoke;
  font-family: Helvetica;
  margin: 20px;
  margin-top: 0px;
  padding: 50px;
  padding-top: 20px;
  text-align: left;
}

.myContainer {
  background-color: lightslategray;
  font-family: Helvetica;
  padding: 50px;
  padding-top: 20px;
  padding-bottom: 10 px;
  margin: 0px;
  border: 5px firebrick;
}
</style>

</head>

<body>

<section class ="myContainer">
   <title>DOMESTIC LIGHT INIT</title>

   <header> 
   <h1><b>DOMESTIC LIGHT INIT MODE</b></h1>
   <h2>Sensor Configuration and Setup</h2> 
   </header>

      <div class="myDiv">
      <p><h3>Welcome to the Domestic Light project.</h3>Thanks so much for participating. <br>
       If you need more help beyond this configuration page please go to <a href="https://domesticlight.art/technical-information">https://domesticlight.art/technical-information/</a></p>
      <h2><p>Your Sensor's Raw Color Values:<span id="colorvalue"> 0 </span></p></h2>
      <h2><p>Sensor MAC Address:<span id="macaddress">%s</span> </p></h2>
      <h2><p>Your Sensor and Site ID: <span id="uuid">%s</span></p></h2> 
       <hr>

       <p><h3>INSTRUCTIONS</h3></p>
      
       <p> NETWORK: Enter the name or SSID your wireless network the sensor should connect to.<br>
       <p> PASSWORD: Enter the password for the network. Note that at this time the sensor does not connect to unsecured networks. <br>
      
      <form action="/get">
      
       <p>NETWORK:&nbsp&nbsp&nbsp<input type="text" name="network" value="%s"></p>
       <p>PASSWORD:&nbsp<input type="text" name="password" value="%s"></p></h3>
       <hr>
       <h3><p>SEND OSC?  <input type="checkbox" name="oscsend" value="Send OSC"> </p>
       <p>OSC<br>TARGET IP:&nbsp&nbsp<input type="text" name="oscipaddr" value="%s"></p>
       <p>PORT:&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp<input type="number" name="oscport" value="%d" min="1" max="65535"></p></h3>
        <hr>
        <input type="submit" value="submit">
        </form></p><br>
        </div>
  </section>

    <script>

      setInterval(function() {
        getData();
      }, 1000);

      function getData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("colorvalue").innerHTML =
          this.responseText;
        }
      };
      xhttp.open("GET", "color", true);
      xhttp.send();
      }
    </script>
  </body>
</html>

)=====";