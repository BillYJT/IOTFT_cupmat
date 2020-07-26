
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
 
String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;} 
    #main {display: table; margin: auto;  padding: 10px 10px 10px 10px; } 
    #content { border: 5px solid blue; border-radius: 15px; padding: 10px 0px 10px 0px;}
    h2 {text-align:center; margin: 10px 0px 10px 0px;} 
    p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
    #time_P { margin: 10px 0px 15px 0px;}
  </style>
 
  <script> 
    function updateTime() 
    {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;
    }
 
    function updateTemp() 
    {  
       ajaxLoad('getCount'); 
    }
 
    var ajaxRequest = null;
    if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
    else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
 
    function ajaxLoad(ajaxURL)
    {
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
 
      ajaxRequest.open('GET',ajaxURL,true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
          var tmpArray = ajaxResult.split("|");
          document.getElementById('ncup').innerHTML = tmpArray[0];
        }
      }
      ajaxRequest.send();
    }
 
    var myVar1 = setInterval(updateTemp, 5000);  
    var myVar2 = setInterval(updateTime, 1000);  
 
  </script>
 
 
  <title>Smart Cup Mat</title>
 </head>
 
 <body>
   <div id='main'>
     <h2>Smart Cup Mat</h2>
     <div id='content'> 
       <p id='P_time'>Time</p>
       <h2>You have drunk</h2>
       <p> <span id='ncup'>--</span> </p>
     </div>
   </div> 
 </body>
</html>
)====="; 

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
//#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
//#include <ESP8266mDNS.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
WiFiServer server(80);    // Create a webserver object that listens for HTTP request on port 80

String request = "";
int tmpval = 0;
int state = 0; //0:none 1:empty 2:full
int ncup = 0;
//int led = 2;

//void handleRoot();              // function prototypes for HTTP handlers
//void handleValue();
//void handleNotFound();

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
//    if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
//      Serial.println("mDNS responder started");
//    } else {
//      Serial.println("Error setting up MDNS responder!");
//    }

      Serial.println("");
      Serial.println(F("[CONNECTED]"));
      Serial.print("[IP ");              
      Serial.print(WiFi.localIP()); 
      Serial.println("]");
      
//    server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
//    server.on("/value", HTTP_POST, handleValue);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
//    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

    server.begin();                           // Actually start the server
    Serial.println("HTTP server started");
}

void loop() {

    tmpval = 0;
    for(int i =0; i<=10; i++){
      tmpval = tmpval + analogRead(A0);  
      delay(100);
    }
    tmpval = tmpval/10;
    Serial.println(tmpval);    


    if (abs(tmpval - 75)<15 && state==2){
      state = 1;
      ncup++;
      Serial.print("Emptied a cup!") ;
    } 

    if (abs(tmpval - 180)<15) {
      if (state == 1){
        state = 2;
        Serial.print("Refilled!");  
      }else if(state==0){
        state = 2;
        Serial.print("First cup of the day!");  
      }
    }

    WiFiClient client = server.available();     // Check if a client has connected
    if (!client){
      return;
    }

    request = client.readStringUntil('\r');     // Read the first line of the request
    Serial.println(request);
    Serial.println("");
    
    //server.handleClient();                    // Listen for HTTP requests from clients
    
    if ( request.indexOf("getCount") > 0 )
     { 
          Serial.println("getCount received");
 
          if ( !isnan(tmpval) )
          {
              client.print( header );
              client.print( ncup ); client.print( "|" );
              Serial.println("data sent");
          }
          else
          {
              Serial.println("Error reading the sensor");
          }
     }
 
     else
     {
        client.flush();
        client.print( header );
        client.print( html_1 ); 
        Serial.println("New page served");
     }
}

//void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
//  server.send(200, "text/html", "<form action=\"/value\" method=\"POST\"><input type=\"submit\" value=\"Extract value\"></form>");
//}
//
//void handleValue() {                          // If a POST request is made to URI /LED
//  String mystr;
//  mystr = String(tmpval);
//  //Serial.println(val);
//  server.send(200,"text/html", "<h1>ADC reads, " + mystr + "!</h1><p>Login successful</p>"); 
//
//}
//
//void handleNotFound(){
//  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
//}
