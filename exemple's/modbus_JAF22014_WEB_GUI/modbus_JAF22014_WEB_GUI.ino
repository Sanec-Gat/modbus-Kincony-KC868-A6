#include <WiFi.h>
#include <WebServer.h>
#include <ModbusMaster.h>
#include <ArduinoJson.h>
#include <vector>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>JAF22014 Control</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:'Segoe UI',Arial,sans-serif;background:#0a0e17;color:#c8d0e0;padding:20px}
.app{max-width:1400px;margin:auto}
.header{display:flex;justify-content:space-between;align-items:center;padding:15px 25px;background:#111927;border:1px solid #1a2a3a;margin-bottom:25px;flex-wrap:wrap;gap:10px}
.header h1{font-size:20px;font-weight:300;color:#7aadda;letter-spacing:3px}
.status-bar{display:flex;gap:20px;font-size:13px;flex-wrap:wrap}
.status-dot{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:6px}
.status-dot.online{background:#4caf50}
.status-dot.offline{background:#f44336}
.controls{display:flex;gap:15px;margin-bottom:25px;flex-wrap:wrap}
.controls input,.controls button{padding:10px 16px;background:#0d1520;border:1px solid #1a2a3a;color:#c8d0e0;font-size:14px;font-family:inherit;outline:none}
.controls input:focus{border-color:#4a8aad}
.controls input{flex:1;min-width:150px}
.controls button{cursor:pointer;text-transform:uppercase;letter-spacing:1px;font-size:12px;min-width:100px}
.controls button:hover{background:#1a2a3a}
.controls .primary{border-color:#4a8aad;color:#4a8aad}
.controls .primary:hover{background:#4a8aad;color:#0a0e17}
.controls .danger{border-color:#8a3a3a;color:#8a6a6a}
.controls .danger:hover{background:#8a3a3a;color:#0a0e17}
.grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(380px,1fr));gap:20px;margin-bottom:25px}
.card{background:#111927;border:1px solid #1a2a3a;padding:20px}
.card:hover{border-color:#2a4a6a}
.card.offline{opacity:.5}
.head{display:flex;justify-content:space-between;align-items:center;border-bottom:1px solid #1a2a3a;padding-bottom:12px;margin-bottom:15px}
.head .name{font-size:16px;font-weight:500;color:#7aadda}
.head .addr{font-size:12px;color:#4a6a8a;background:#0d1520;padding:3px 10px;border:1px solid #1a2a3a}
.head .del{background:none;border:none;color:#6a3a3a;cursor:pointer;font-size:18px;padding:0 8px}
.head .del:hover{color:#f44336}
.params{display:grid;grid-template-columns:1fr 1fr;gap:8px 20px}
.param{display:flex;justify-content:space-between;padding:5px 0;font-size:13px;border-bottom:1px solid #0d1520}
.param .lbl{color:#4a6a8a}
.param .val{color:#7aadda;font-weight:500}
.param .val.temp{color:#ff9a4a}
.param .val.on{color:#4caf50}
.param .val.off{color:#666}
.btns{margin-top:15px;padding-top:15px;border-top:1px solid #1a2a3a;display:flex;flex-wrap:wrap;gap:8px}
.btns button{padding:6px 14px;background:#0d1520;border:1px solid #1a2a3a;color:#8899bb;cursor:pointer;font-size:12px;font-family:inherit}
.btns button:hover{background:#1a2a3a}
.btns .primary{border-color:#4a8aad;color:#4a8aad}
.btns .primary:hover{background:#4a8aad;color:#0a0e17}
.btns .danger{border-color:#8a3a3a;color:#8a6a6a}
.btns .danger:hover{background:#8a3a3a;color:#0a0e17}
.modal{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.7);justify-content:center;align-items:center;z-index:9999}
.modal.active{display:flex}
.modal-box{background:#111927;border:1px solid #2a4a6a;padding:30px;max-width:500px;width:90%;max-height:80vh;overflow-y:auto}
.modal-box h2{color:#7aadda;font-weight:300;margin-bottom:20px;border-bottom:1px solid #1a2a3a;padding-bottom:12px}
.field{margin-bottom:15px}
.field label{display:block;font-size:13px;color:#4a6a8a;margin-bottom:5px}
.field input,.field select{width:100%;padding:10px;background:#0d1520;border:1px solid #1a2a3a;color:#c8d0e0;font-size:14px;font-family:inherit;outline:none}
.field input:focus{border-color:#4a8aad}
.mbtns{display:flex;gap:10px;margin-top:20px;justify-content:flex-end}
.mbtns button{padding:10px 24px;background:#0d1520;border:1px solid #1a2a3a;color:#8899bb;cursor:pointer;font-size:13px;font-family:inherit}
.mbtns button:hover{background:#1a2a3a}
.mbtns .primary{border-color:#4a8aad;color:#4a8aad}
.mbtns .primary:hover{background:#4a8aad;color:#0a0e17}
@media(max-width:600px){.grid{grid-template-columns:1fr}.params{grid-template-columns:1fr}.controls{flex-direction:column}.controls input{width:100%}.header{flex-direction:column;gap:10px;text-align:center}}
::-webkit-scrollbar{width:8px}
::-webkit-scrollbar-track{background:#0a0e17}
::-webkit-scrollbar-thumb{background:#1a2a3a;border-radius:4px}
::-webkit-scrollbar-thumb:hover{background:#2a4a6a}
</style>
</head>
<body>
<div class="app">
<div class="header"><h1>JAF22014 Control</h1><div class="status-bar"><span><span class="status-dot online"></span> Система: <span id="sysStatus">ONLINE</span></span><span>Устройств: <span id="deviceCount">0</span></span><span id="clock"></span></div></div>
<div class="controls"><input type="number" id="newAddr" placeholder="Адрес (1-254)" min="1" max="254"><button class="primary" onclick="add()">➕ Добавить</button><button onclick="refresh()">🔄 Обновить</button><button class="danger" onclick="clearAll()">✖ Удалить все</button></div>
<div id="grid" class="grid"></div>
<div class="modal" id="modal"><div class="modal-box"><h2 id="modalTitle">Настройка</h2><div id="modalBody"></div><div class="mbtns"><button onclick="closeModal()">Отмена</button><button class="primary" onclick="saveModal()">Сохранить</button></div></div></div>
</div>
<script>
let devices=[], editAddr=null;
function clock(){document.getElementById('clock').textContent=new Date().toLocaleTimeString('ru-RU')}
setInterval(clock,1000);clock();

async function load(){
 try{
  const r=await fetch('/api/devices');
  devices=await r.json();
  document.getElementById('deviceCount').textContent=devices.length;
  render();
 }catch(e){console.error(e)}
}

function render(){
 const g=document.getElementById('grid');
 if(!devices.length){g.innerHTML='<div style="grid-column:1/-1;text-align:center;padding:60px 20px;color:#4a6a8a;border:1px dashed #1a2a3a;"><div style="font-size:48px;margin-bottom:15px;">📡</div><div>Нет устройств</div></div>';return}
 let h='';
 devices.forEach(d=>{
  const on=d.isConnected,t=d.currentTemp!==undefined?d.currentTemp.toFixed(1):'--',l=d.lowLimit!==undefined?d.lowLimit.toFixed(1):'--',hi=d.highLimit!==undefined?d.highLimit.toFixed(1):'--',p=d.powerState===1?'ВКЛ':'ВЫКЛ',pc=d.powerState===1?'on':'off',m=d.mode===0?'Вентилятор':'Нагрев',lg=d.logic===0?'Прямой':'Инверсный';
  h+=`<div class="card ${on?'':'offline'}"><div class="head"><span class="name">Устройство #${d.address}</span><span class="addr">${on?'🟢 ONLINE':'🔴 OFFLINE'}</span><button class="del" onclick="remove(${d.address})">×</button></div>
  <div class="params"><div class="param"><span class="lbl">Температура</span><span class="val temp">${t} °C</span></div><div class="param"><span class="lbl">Нижний предел</span><span class="val">${l} °C</span></div><div class="param"><span class="lbl">Верхний предел</span><span class="val">${hi} °C</span></div><div class="param"><span class="lbl">Питание</span><span class="val ${pc}">${p}</span></div><div class="param"><span class="lbl">Режим</span><span class="val">${m}</span></div><div class="param"><span class="lbl">Логика</span><span class="val">${lg}</span></div><div class="param"><span class="lbl">Modbus адрес</span><span class="val">${d.address}</span></div></div>
  <div class="btns"><button class="primary" onclick="edit(${d.address})">⚙ Настройка</button><button onclick="setPower(${d.address},1)">Включить</button><button onclick="setPower(${d.address},0)">Выключить</button><button class="danger" onclick="resetDev(${d.address})">Сброс</button></div></div>`;
 });
 g.innerHTML=h;
}

async function add(){
 const inp=document.getElementById('newAddr'),addr=parseInt(inp.value);
 if(isNaN(addr)||addr<1||addr>254){alert('Введите адрес 1-254');return}
 if(devices.some(d=>d.address===addr)){alert('Уже есть');return}
 try{
  const r=await fetch('/api/devices',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({address:addr})});
  const res=await r.json();
  if(res.success){inp.value='';load()}else alert('Ошибка: '+res.error)
 }catch(e){alert('Ошибка: '+e.message)}
}

async function remove(addr){
 if(!confirm(`Удалить #${addr}?`))return;
 const r=await fetch('/api/device/remove',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({address:addr})});
 if((await r.json()).success)load();
}

async function clearAll(){
 if(!confirm('Удалить все?'))return;
 const r=await fetch('/api/devices/all',{method:'DELETE'});
 if((await r.json()).success)load();
}

async function refresh(){
 await fetch('/api/refresh',{method:'POST'});
 setTimeout(load,800);
}

async function edit(addr){
 const d=devices.find(x=>x.address===addr);
 if(!d)return;
 editAddr=addr;
 document.getElementById('modalTitle').textContent='Настройка #'+addr;
 document.getElementById('modalBody').innerHTML=`
  <div class="field"><label>Нижний предел (°C) <span style="color:#4a6a8a;font-size:11px;">(-40-100)</span></label><input type="number" id="editLow" step="any" min="-40" max="100" value="${d.lowLimit||0}"></div>
  <div class="field"><label>Верхний предел (°C) <span style="color:#4a6a8a;font-size:11px;">(-40-100)</span></label><input type="number" id="editHigh" step="any" min="-40" max="100" value="${d.highLimit||0}"></div>
  <div class="field"><label>Режим</label><select id="editMode"><option value="0" ${d.mode===0?'selected':''}>Вентилятор</option><option value="1" ${d.mode===1?'selected':''}>Нагрев</option></select></div>
  <div class="field"><label>Логика</label><select id="editLogic"><option value="0" ${d.logic===0?'selected':''}>Прямой</option><option value="1" ${d.logic===1?'selected':''}>Инверсный</option></select></div>
  <div class="field"><label>Питание</label><select id="editPower"><option value="0" ${d.powerState===0?'selected':''}>ВЫКЛ</option><option value="1" ${d.powerState===1?'selected':''}>ВКЛ</option></select></div>`;
 document.getElementById('modal').style.display='flex';
}

function closeModal(){
 document.getElementById('modal').style.display='none';
 editAddr=null;
}

async function saveModal(){
 const low=parseFloat(document.getElementById('editLow').value);
 const high=parseFloat(document.getElementById('editHigh').value);
 const mode=parseInt(document.getElementById('editMode').value);
 const logic=parseInt(document.getElementById('editLogic').value);
 const power=parseInt(document.getElementById('editPower').value);
 
 console.log('💾 Сохранение:', {low, high, mode, logic, power});
 
 if(isNaN(low)||low<-40||low>100){alert('Нижний предел -40-100°C');return}
 if(isNaN(high)||high<-40||high>100){alert('Верхний предел -40-100°C');return}
 if(low>=high){alert('Нижний предел должен быть меньше верхнего!');return}
 
 try {
  const r=await fetch('/api/device/update',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({address:editAddr,lowLimit:low,highLimit:high,mode,logic,powerState:power})});
  const res=await r.json();
  console.log('Ответ:', res);
  if(res.success){closeModal();setTimeout(load,500);}else{alert('Ошибка: '+JSON.stringify(res))}
 } catch(e) {
  console.error('Ошибка:', e);
  alert('Ошибка соединения');
 }
}

async function setPower(addr,val){
 const r=await fetch('/api/device/power',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({address:addr,powerState:val})});
 if((await r.json()).success)setTimeout(load,500);
}

async function resetDev(addr){
 if(!confirm(`Сбросить #${addr}?`))return;
 const r=await fetch('/api/device/reset',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({address:addr})});
 if((await r.json()).success)setTimeout(load,500);
}

document.getElementById('modal').addEventListener('click', function(e) {
 if(e.target===this) closeModal();
});

setInterval(load,5000);load();
</script>
</body>
</html>
)rawliteral";

const char* ssid = "kom56";
const char* password = "12341234";

#define RS485_TX 27
#define RS485_RX 14
#define BAUD_RATE 9600

WebServer server(80);
ModbusMaster node;

struct Device {
  uint16_t addr;
  float temp, low, high;
  uint16_t mode, logic, power;
  bool online;
};

std::vector<Device> devices;

bool readReg(uint16_t reg, uint16_t &val) {
  uint8_t r = node.readHoldingRegisters(reg, 1);
  if (r == node.ku8MBSuccess) { 
    val = node.getResponseBuffer(0); 
    return true; 
  }
  return false;
}

void readDevice(Device &d) {
  uint16_t raw;
  d.online = true;
  d.online &= readReg(0x0000, raw); d.temp = raw - 40;
  d.online &= readReg(0x0001, raw); d.low = raw - 40;
  d.online &= readReg(0x0002, raw); d.high = raw - 40;
  d.online &= readReg(0x0004, raw); d.mode = raw;
  d.online &= readReg(0x0006, raw); d.logic = raw;
  d.online &= readReg(0x0009, raw); d.power = raw;
}

void sendJSON(String s) { server.send(200, "application/json", s); }

void handleRoot() { server.send(200, "text/html", index_html); }
void handleGet() {
  String json = "[";
  for (auto &d : devices) {
    json += "{\"address\":" + String(d.addr) + 
            ",\"currentTemp\":" + String(d.temp,1) +
            ",\"lowLimit\":" + String(d.low,1) +
            ",\"highLimit\":" + String(d.high,1) +
            ",\"mode\":" + String(d.mode) +
            ",\"logic\":" + String(d.logic) +
            ",\"powerState\":" + String(d.power) +
            ",\"isConnected\":" + String(d.online ? "true" : "false") + "},";
  }
  if (json.length() > 1) json.remove(json.length()-1);
  sendJSON(json + "]");
}

void handleAdd() {
  JsonDocument doc; deserializeJson(doc, server.arg("plain"));
  uint16_t addr = doc["address"];
  for (auto &d : devices) if (d.addr == addr) return sendJSON("{\"success\":false,\"error\":\"exists\"}");
  node.begin(addr, Serial1);
  Device d; d.addr = addr; readDevice(d);
  devices.push_back(d);
  sendJSON("{\"success\":true}");
}

void handleRemove() {
  JsonDocument doc; deserializeJson(doc, server.arg("plain"));
  uint16_t addr = doc["address"];
  for (int i = 0; i < devices.size(); i++) {
    if (devices[i].addr == addr) { devices.erase(devices.begin()+i); break; }
  }
  sendJSON("{\"success\":true}");
}

// ============================================================
// ГЛАВНАЯ ФУНКЦИЯ С МАКСИМАЛЬНОЙ ОТЛАДКОЙ
// ============================================================
void handleUpdate() {
  Serial.println("\n╔═══════════════════════════════════════════════════╗");
  Serial.println("║       📩 ПОЛУЧЕН ЗАПРОС /api/device/update     ║");
  Serial.println("╚═══════════════════════════════════════════════════╝");
  
  String body = server.arg("plain");
  Serial.print("📄 ТЕЛО ЗАПРОСА: ");
  Serial.println(body);
  Serial.println("─────────────────────────────────────────────────────");
  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    Serial.printf("❌ ОШИБКА JSON: %s\n", error.c_str());
    sendJSON("{\"success\":false,\"error\":\"Invalid JSON\"}");
    return;
  }
  
  uint16_t addr = doc["address"];
  Serial.printf("📌 АДРЕС: %d\n", addr);
  
  int idx = -1;
  for (int i = 0; i < devices.size(); i++) {
    if (devices[i].addr == addr) { idx = i; break; }
  }
  
  if (idx == -1) {
    Serial.println("❌ УСТРОЙСТВО НЕ НАЙДЕНО");
    sendJSON("{\"success\":false,\"error\":\"not found\"}");
    return;
  }
  
  Serial.printf("✅ Устройство найдено, индекс: %d\n", idx);
  
  bool hasLow = doc.containsKey("lowLimit");
  bool hasHigh = doc.containsKey("highLimit");
  bool hasMode = doc.containsKey("mode");
  bool hasLogic = doc.containsKey("logic");
  bool hasPower = doc.containsKey("powerState");
  
  Serial.printf("📋 ПОЛЯ: low=%d, high=%d, mode=%d, logic=%d, power=%d\n", 
    hasLow, hasHigh, hasMode, hasLogic, hasPower);
  
  if (hasLow) {
    float val = doc["lowLimit"].as<float>();
    Serial.printf("📊 НИЖНИЙ: %.1f°C\n", val);
  }
  if (hasHigh) {
    float val = doc["highLimit"].as<float>();
    Serial.printf("📊 ВЕРХНИЙ: %.1f°C\n", val);
  }
  
  node.begin(addr, Serial1);
  bool ok = true;
  
  // НИЖНИЙ предел
  if (hasLow) {
    float val = doc["lowLimit"].as<float>();
    uint16_t raw = (uint16_t)(val + 40);
    Serial.printf("🔄 ПИШЕМ: %.1f°C → raw=%d (0x%04X)\n", val, raw, raw);
    
    uint8_t result = node.writeSingleRegister(0x0001, raw);
    if (result == node.ku8MBSuccess) {
      devices[idx].low = val;
      Serial.println("✅ НИЖНИЙ ЗАПИСАН УСПЕШНО!");
    } else {
      ok = false;
      Serial.printf("❌ ОШИБКА ЗАПИСИ: код 0x%02X\n", result);
    }
  }
  
  // ВЕРХНИЙ предел
  if (hasHigh) {
    float val = doc["highLimit"].as<float>();
    uint16_t raw = (uint16_t)(val + 40);
    Serial.printf("🔄 ПИШЕМ: %.1f°C → raw=%d (0x%04X)\n", val, raw, raw);
    
    uint8_t result = node.writeSingleRegister(0x0002, raw);
    if (result == node.ku8MBSuccess) {
      devices[idx].high = val;
      Serial.println("✅ ВЕРХНИЙ ЗАПИСАН УСПЕШНО!");
    } else {
      ok = false;
      Serial.printf("❌ ОШИБКА ЗАПИСИ: код 0x%02X\n", result);
    }
  }
  
  if (hasMode) {
    uint16_t val = doc["mode"];
    Serial.printf("🔄 РЕЖИМ: %d\n", val);
    if (node.writeSingleRegister(0x0004, val) == node.ku8MBSuccess) {
      devices[idx].mode = val;
      Serial.println("✅ РЕЖИМ ЗАПИСАН");
    }
  }
  
  if (hasLogic) {
    uint16_t val = doc["logic"];
    Serial.printf("🔄 ЛОГИКА: %d\n", val);
    if (node.writeSingleRegister(0x0006, val) == node.ku8MBSuccess) {
      devices[idx].logic = val;
      Serial.println("✅ ЛОГИКА ЗАПИСАНА");
    }
  }
  
  if (hasPower) {
    uint16_t val = doc["powerState"];
    Serial.printf("🔄 ПИТАНИЕ: %d\n", val);
    if (node.writeSingleRegister(0x0009, val) == node.ku8MBSuccess) {
      devices[idx].power = val;
      Serial.println("✅ ПИТАНИЕ ЗАПИСАНО");
    }
  }
  
  delay(100);
  Serial.println("🔄 ПЕРЕЧИТЫВАЕМ ДАННЫЕ...");
  readDevice(devices[idx]);
  
  Serial.println("═══════════════════════════════════════════════════");
  Serial.printf("📊 РЕЗУЛЬТАТ: success=%s\n", ok ? "true" : "false");
  Serial.println("═══════════════════════════════════════════════════\n");
  
  sendJSON("{\"success\":" + String(ok ? "true" : "false") + "}");
}

void handlePower() {
  JsonDocument doc; deserializeJson(doc, server.arg("plain"));
  uint16_t addr = doc["address"], val = doc["powerState"];
  node.begin(addr, Serial1);
  sendJSON(node.writeSingleRegister(0x0009, val) == node.ku8MBSuccess ? "{\"success\":true}" : "{\"success\":false}");
}

void handleReset() {
  JsonDocument doc; deserializeJson(doc, server.arg("plain"));
  uint16_t addr = doc["address"];
  node.begin(addr, Serial1);
  node.writeSingleRegister(0x0020, 0x000A);
  sendJSON("{\"success\":true}");
}

void handleRefresh() {
  for (auto &d : devices) { node.begin(d.addr, Serial1); readDevice(d); }
  sendJSON("{\"success\":true}");
}

void handleClear() { devices.clear(); sendJSON("{\"success\":true}"); }

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/api/devices", HTTP_GET, handleGet);
  server.on("/api/devices", HTTP_POST, handleAdd);
  server.on("/api/devices/all", HTTP_DELETE, handleClear);
  server.on("/api/refresh", HTTP_POST, handleRefresh);
  server.on("/api/device/remove", HTTP_POST, handleRemove);
  server.on("/api/device/update", HTTP_POST, handleUpdate);
  server.on("/api/device/power", HTTP_POST, handlePower);
  server.on("/api/device/reset", HTTP_POST, handleReset);
  server.begin();
  Serial.println("Server started");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n========================================");
  Serial.println("JAF22014 v2.0 - ОТЛАДОЧНАЯ ВЕРСИЯ");
  Serial.println("========================================");
  Serial1.begin(BAUD_RATE, SERIAL_8N1, RS485_RX, RS485_TX);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  int t=0;
  while(WiFi.status()!=WL_CONNECTED && t<40){delay(500);Serial.print(".");t++;}
  Serial.println();
  if(WiFi.status()==WL_CONNECTED){Serial.print("IP: ");Serial.println(WiFi.localIP());}
  else {Serial.println("AP mode");WiFi.softAP("JAF22014","12345678");Serial.print("AP IP: ");Serial.println(WiFi.softAPIP());}
  
  setupWebServer();
  Serial.println("========================================");
  Serial.println("ГОТОВ! Откройте консоль браузера (F12)");
  Serial.println("========================================\n");
}

void loop() {
  static uint32_t last=0;
  if(millis()-last>=2000){last=millis();for(auto &d:devices){node.begin(d.addr,Serial1);readDevice(d);}}
  server.handleClient();
}