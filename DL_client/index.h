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
  text-align: left;
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
  padding-bottom: 10px;
  margin: 0px;
  border: 5px firebrick;
}
</style>
<title>DOMESTIC LIGHT WIFI CONFIG</title>
</head>

<body>

<section class ="myContainer">
   <header> 
   <h1><b>DOMESTIC LIGHT</b></h1>
   <h2>Sensor Configuration and Setup</h2> 
   </header>

    <div class="myDiv">
      <h3>Welcome to the Domestic Light project. Thanks so much for participating. </h3>
      <p>For more information and support visit <a href="https://domesticlight.art/technical-information">https://domesticlight.art/technical-information/</a>. Rev 12 Aug 2023</p>
      <h2>Your Sensor's Raw Color Values:<span id="colorvalue"> 0 </span></h2>
      <h2>Your Sensor's MAC Address:<span id="macaddress">%s</span></h2>
      <h2>Your Sensor's Site ID: <span id="uuid">%s</span></h2> 
       <hr>
       
       <h3>INSTRUCTIONS</h3>
       <p> NETWORK:  Enter the name or SSID of your wireless network.</p>
       <p> PASSWORD: Enter your wifi network password. This is stored only on your sensor and not shared.</p>
       <p> TO SAVE:  Click "SUBMIT AND RESTART" to save this info. <br>
           The board will flash green for 1 second if saved successfullly, then shutdown its wifi and restart. You can then close this webpage.</p>
       <hr>
      <form action="/get">
      
        <h3>NETWORK:&nbsp;&nbsp;&nbsp;<input type="text" name="network" value="%s"></h3>
        <h3>PASSWORD:&nbsp;<input type="text" name="password" value="%s"></h3>
        <hr><br>
        <h3>SEND OSC?<input type="checkbox" name="oscsend" value="Send OSC"> </h3> 
        <h3>OSC<br>TARGET IP:&nbsp;&nbsp;<input type="text" name="oscipaddr" value="%s"></h3>
        <h3>PORT:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="number" name="oscport" value="%d" min="1" max="65535"></h3>
        <hr>
        <h3><input type="submit" value="SUBMIT AND RESTART"></h3>
        </form><br>
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