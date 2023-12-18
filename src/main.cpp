#include <WiFi.h>
#include <ESPAsyncWebServer.h>


const char* ssid = "RobotCar";
const char* password = "abc123";

String sliderValue = "0";

const int freq = 5000;
const int resolution = 8;

const char* PARAM_INPUT = "value";

const int IN1 = 5;
const int IN2 = 18;
const int IN3 = 19;
const int IN4 = 21;

const int EN1= 22;
const int EN2= 23;


bool state = 1;

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Robot Car</title>
  <style>
    body {display:flex; flex-direction: row; flex-wrap: wrap; justify-content: space-around;font-family: Arial; margin:0px auto; padding-top: 30px;}
    td { padding: 8px; }
    .button {
      background-color: #2f4468;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 18px;
      margin: 3px 3px;
      cursor: pointer;
    }
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 

    </style>
</head>
<body>
  
  <div>
  <h2 id="etatRobot"></h2>
  <table>
    <tr><td></td><td><button class="button" onclick="appelServeur('/forward',traiteReponse)" >forward</button></td><td></td></tr>
    <tr><td><button class="button" onclick="appelServeur('/left',traiteReponse)">left</button></td>
    <td><button class="button" onclick="appelServeur('/stop',traiteReponse)">stop</button></td>
    <td><button class="button" onclick="appelServeur('/right',traiteReponse)">right</button></td></tr>
    <tr><td></td><td><button class="button" onclick="appelServeur('/backward',traiteReponse)">backward</button></td><td></td></tr>
  </table>
  </div>
  <div></div>
  <div>
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSlider(this)" id="speed" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
  </div>
<script>
function updateSlider(element) {
  var speedValue = document.getElementById("speed").value;
  document.getElementById("textSliderValue").innerHTML = speedValue;


  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/slider?value="+speedValue+"", true);
  xhttp.send();
}
function appelServeur(url,cFunction){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            cFunction(this);
        }
    };
    xhttp.open("GET",url,true);
    xhttp.send();
}
function traiteReponse(xhttp){
  document.getElementById("etatRobot").innerHTML = "Robot " + xhttp.responseText;
  }
</script>
</body>
</html>
)rawliteral";

void setup(){
  Serial.begin(9600);

  ledcSetup(2, freq, resolution);
  ledcAttachPin(EN1, 2);
  ledcAttachPin(EN2, 2);

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request){
    state = 1;
    digitalWrite(IN1,1);
    digitalWrite(IN2,0);
    digitalWrite(IN3,1);
    digitalWrite(IN4,0);
    request->send_P(200, "text/plain","forward");
  });
  server.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request){
    state = 0;
    digitalWrite(IN1,0);
    digitalWrite(IN2,1);
    digitalWrite(IN3,0);
    digitalWrite(IN4,1);
    request->send_P(200, "text/plain","backward");
  });
    server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(IN1,0);
    digitalWrite(IN2,0);
    digitalWrite(IN3,0);
    digitalWrite(IN4,0);
    request->send_P(200, "text/plain","stop");
  });
  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    if(state){
    digitalWrite(IN1,0);
    digitalWrite(IN2,0);
    digitalWrite(IN3,1);
    digitalWrite(IN4,0);
    request->send_P(200, "text/plain","forward right"); 
    }
    else{
    digitalWrite(IN1,0);
    digitalWrite(IN2,0);
    digitalWrite(IN3,0);
    digitalWrite(IN4,1);
    request->send_P(200, "text/plain","backward right");
    }
    
  });
  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    if(state){
    digitalWrite(IN1,1);
    digitalWrite(IN2,0);
    digitalWrite(IN3,0);
    digitalWrite(IN4,0);
    request->send_P(200, "text/plain","forward left");
    }
    else{
    digitalWrite(IN1,0);
    digitalWrite(IN2,1);
    digitalWrite(IN3,0);
    digitalWrite(IN4,0);
    request->send_P(200, "text/plain","backward left");
    }
    
  });
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;

    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      ledcWrite(2, inputMessage.toInt());
      
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.print("value =");
    Serial.print(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  server.begin();
}
  
void loop() {
}