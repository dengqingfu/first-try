
\<p>亮度: <span id="v">0</span></p >
</body>
</html>
)";

void setup() {
    Serial.begin(115200);
    
    // 新版API：直接绑定引脚
    ledcAttach(ledPin, 5000, 8);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println(WiFi.localIP());
    
    server.on("/", [](){ server.send(200, "text/html", htmlPage); });
    server.on("/set", [](){
        ledcWrite(ledPin, server.arg("val").toInt());  // 直接写引脚
        server.send(200, "text/plain", "OK");
    });
    
    server.begin();
}

void loop() {
    server.handleClient();
}