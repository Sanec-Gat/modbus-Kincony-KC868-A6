#include <ModbusMaster.h>

#define RS485_TX 27
#define RS485_RX 14

// Пин для управления DE/RE трансивера (если требуется)
#define RS485_DE_RE 4


// НАСТРОЙКА MODBUS
#define SLAVE_ADDRESS 1  // Адрес JAF22014 
#define BAUD_RATE 9600   // Скорость обмена 

// Создаем объект Modbus
ModbusMaster jafNode;

// КОЛБЭКИ ДЛЯ УПРАВЛЕНИЯ НАПРАВЛЕНИЕМ ПЕРЕДАЧИ (если используется DE/RE)
void preTransmission() {
  digitalWrite(RS485_DE_RE, HIGH);  // Включаем режим передачи
}

void postTransmission() {
  digitalWrite(RS485_DE_RE, LOW);  // Включаем режим приема
}

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ КОНВЕРТАЦИИ ДАННЫХ

// Функция для преобразования значения температуры со смещением +40
float convertTemperature(uint16_t rawValue) {
  return (rawValue - 40.0);
}

// Функция для преобразования значения адреса
// Диапазон: 0x0001 ~ 0x00FE
uint16_t convertAddress(uint16_t rawValue) {
  return rawValue;
}

// Функция для преобразования режима работы
String convertMode(uint16_t rawValue) {
  switch (rawValue) {
    case 0x0000: return "Режим отключения вентилятора";
    case 0x0001: return "Режим отключения нагрева";
    default: return "Неизвестный режим (0x" + String(rawValue, HEX) + ")";
  }
}

// Функция для преобразования логики регулирования
String convertLogic(uint16_t rawValue) {
  switch (rawValue) {
    case 0x0000: return "Пропорциональный выходной сигнал";
    case 0x0001: return "Инверсный выход";
    default: return "Неизвестная логика (0x" + String(rawValue, HEX) + ")";
  }
}

// Функция для преобразования состояния включения/выключения
String convertPowerState(uint16_t rawValue) {
  switch (rawValue) {
    case 0x0000: return "Устройство ВЫКЛЮЧЕНО";
    case 0x0001: return "Устройство ВКЛЮЧЕНО (работает)";
    default: return "Неизвестное состояние (0x" + String(rawValue, HEX) + ")";
  }
}



// ФУНКЦИЯ ДЛЯ ЧТЕНИЯ ОДНОГО РЕГИСТРА С ОБРАБОТКОЙ ОШИБОК
bool readRegister(uint16_t regAddress, uint16_t& value, const char* regName) {
  uint8_t result = jafNode.readHoldingRegisters(regAddress, 1);

  if (result == jafNode.ku8MBSuccess) {
    value = jafNode.getResponseBuffer(0);
    return true;
  } else {
    Serial.print("  ❌ Ошибка чтения регистра ");
    Serial.print(regName);
    Serial.print(" (0x");
    Serial.print(regAddress, HEX);
    Serial.print("). Код ошибки: 0x");
    Serial.println(result, HEX);
    return false;
  }
}


// ФУНКЦИЯ ДЛЯ ВЫВОДА ВСЕХ РЕГИСТРОВ
void readAllRegisters() {
  Serial.println("================================================================================");
  Serial.println("📊 ЧТЕНИЕ ВСЕХ РЕГИСТРОВ JAF22014");
  Serial.println("================================================================================");
  Serial.println();

  uint16_t value;

  // --------------------------------------------------------------------------
  // 1. Регистр 0x0000: Текущая температура (ТОЛЬКО ЧТЕНИЕ)
  // --------------------------------------------------------------------------
  Serial.println("📌 1. ТЕКУЩАЯ ТЕМПЕРАТУРА");
  Serial.println("   Регистр: 0x0000");
  Serial.println("   Операция: Только чтение (FC 0x03)");
  Serial.println("   Описание: Текущая температура с смещением +40");

  if (readRegister(0x0000, value, "температуры")) {
    float temp = convertTemperature(value);
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → Температура: ");
    Serial.print(temp, 1);
    Serial.println(" °C");
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 2. Регистр 0x0001: Нижний предел температуры (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 2. НИЖНИЙ ПРЕДЕЛ ТЕМПЕРАТУРЫ");
  Serial.println("   Регистр: 0x0001");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Минимальная температура, при которой включается нагрев");
  Serial.println("   Диапазон: 0x002A ~ 0x008A (42 ~ 138 в DEC)");

  if (readRegister(0x0001, value, "нижнего предела")) {
    float temp = convertTemperature(value);
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → Нижний предел: ");
    Serial.print(temp, 1);
    Serial.println(" °C");
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 3. Регистр 0x0002: Верхний предел температуры (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 3. ВЕРХНИЙ ПРЕДЕЛ ТЕМПЕРАТУРЫ");
  Serial.println("   Регистр: 0x0002");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Максимальная температура, при которой выключается нагрев");
  Serial.println("   Диапазон: 0x002B ~ 0x008B (43 ~ 139 в DEC)");

  if (readRegister(0x0002, value, "верхнего предела")) {
    float temp = convertTemperature(value);
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → Верхний предел: ");
    Serial.print(temp, 1);
    Serial.println(" °C");
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 4. Регистр 0x0003: Modbus-адрес (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 4. MODBUS-АДРЕС УСТРОЙСТВА");
  Serial.println("   Регистр: 0x0003");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Текущий адрес устройства в сети Modbus");
  Serial.println("   Диапазон: 0x0001 ~ 0x00FE (1 ~ 254 в DEC)");
  Serial.println("   Заводской адрес по умолчанию: 01");

  if (readRegister(0x0003, value, "адреса")) {
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → Адрес устройства: ");
    Serial.println(value);
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 5. Регистр 0x0004: Режим работы (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 5. РЕЖИМ РАБОТЫ");
  Serial.println("   Регистр: 0x0004");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Режим управления нагрузкой");
  Serial.println("   =0x0000: Режим отключения вентилятора");
  Serial.println("   =0x0001: Режим отключения нагрева");

  if (readRegister(0x0004, value, "режима работы")) {
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → ");
    Serial.println(convertMode(value));
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 6. Регистр 0x0005: Нижний предел выходного шага (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 6. НИЖНИЙ ПРЕДЕЛ ВЫХОДНОГО ШАГА");
  Serial.println("   Регистр: 0x0005");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Минимальный эффективный выходной шаг (процент напряжения)");
  Serial.println("   Диапазон: 0x0014 ~ 0x0050 (20 ~ 80 в DEC)");

  if (readRegister(0x0005, value, "выходного шага")) {
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → Минимальный шаг: ");
    Serial.print(value);
    Serial.println(" (20-80% от максимального)");
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 7. Регистр 0x0006: Логика регулирования температуры (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 7. ЛОГИКА РЕГУЛИРОВАНИЯ ТЕМПЕРАТУРЫ");
  Serial.println("   Регистр: 0x0006");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Направление регулирования температуры");
  Serial.println("   =0x0000: Пропорциональный выходной сигнал");
  Serial.println("   =0x0001: Инверсный выход");

  if (readRegister(0x0006, value, "логики регулирования")) {
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → ");
    Serial.println(convertLogic(value));
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 8. Регистр 0x0007: Калибровка выходного сигнала (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 8. КАЛИБРОВКА ВЫХОДНОГО СИГНАЛА");
  Serial.println("   Регистр: 0x0007");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Параметры калибровки выходного сигнала");
  Serial.println("   Допустимые значения: 0x0000, 0x000A, 0x000F, 0x0014");

  if (readRegister(0x0007, value, "калибровки")) {
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → Калибровочный параметр: ");
    Serial.println(value);
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 9. Регистр 0x0008: Выходной шаг (управление) (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 9. ВЫХОДНОЙ ШАГ (УПРАВЛЕНИЕ)");
  Serial.println("   Регистр: 0x0008");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Принудительное управление выходным напряжением");
  Serial.println("   Значения: 0x0000, 0x0014 ~ 0x0050, 0xFFFF");
  Serial.println("   При записи 0xFFFF: возврат к управлению температурой");

  if (readRegister(0x0008, value, "выходного шага")) {
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → ");
    if (value == 0xFFFF) {
      Serial.println("Режим управления температурой (АВТО)");
    } else if (value == 0x0000) {
      Serial.println("Выход ВЫКЛЮЧЕН (0%)");
    } else if (value >= 0x0014 && value <= 0x0050) {
      Serial.print("Выходное напряжение: ");
      Serial.print(map(value, 0x0014, 0x0050, 20, 80));
      Serial.println("% от максимума");
    } else {
      Serial.println("Нестандартное значение");
    }
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 10. Регистр 0x0009: Управление включением/выключением (ЧТЕНИЕ/ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 10. УПРАВЛЕНИЕ ВКЛ/ВЫКЛ");
  Serial.println("   Регистр: 0x0009");
  Serial.println("   Операция: Чтение/Запись (FC 0x03 / 0x06)");
  Serial.println("   Описание: Состояние устройства");
  Serial.println("   =0x0000: Устройство выключено");
  Serial.println("   =0x0001: Устройство работает");

  if (readRegister(0x0009, value, "включения")) {
    Serial.print("   ✅ Значение (HEX): 0x");
    Serial.print(value, HEX);
    Serial.print(" (DEC: ");
    Serial.print(value);
    Serial.print(") → ");
    Serial.println(convertPowerState(value));
  }
  Serial.println();

  // --------------------------------------------------------------------------
  // 11. Регистр 0x0020: Сброс устройства (ТОЛЬКО ЗАПИСЬ)
  // --------------------------------------------------------------------------
  Serial.println("📌 11. СБРОС УСТРОЙСТВА");
  Serial.println("   Регистр: 0x0020");
  Serial.println("   Операция: Только запись (FC 0x06)");
  Serial.println("   Описание: Запись 0x000A вызывает полный сброс устройства");
  Serial.println("   ⚠️  ВНИМАНИЕ: Этот регистр только для записи!");
  Serial.println("   ⚠️  Чтение не поддерживается, поэтому пропускаем");
  Serial.println();

  Serial.println("================================================================================");
  Serial.println("✅ ЧТЕНИЕ ЗАВЕРШЕНО");
  Serial.println("================================================================================");
  Serial.println();
}


void setup() {
  // Инициализация последовательного порта для отладки
  Serial.begin(115200);

  // Небольшая задержка для стабилизации
  delay(1000);

  Serial.println("================================================================================");
  Serial.println("🚀 ЗАПУСК MODBUS MASTER ДЛЯ JAF22014");
  Serial.println("================================================================================");
  Serial.println();

// --- ИНИЦИАЛИЗАЦИЯ RS485 ---

// Если используется пин DE/RE
#ifdef RS485_DE_RE
  pinMode(RS485_DE_RE, OUTPUT);
  digitalWrite(RS485_DE_RE, LOW);  // По умолчанию в режиме приема
  Serial.println("🔧 Пин DE/RE инициализирован в режиме приема");
#endif

  // Инициализация аппаратного UART для RS485
  // Параметры: скорость 9600, 8 бит данных, без четности, 1 стоп-бит
  Serial1.begin(BAUD_RATE, SERIAL_8N1, RS485_RX, RS485_TX);
  Serial.print("🔧 UART инициализирован: скорость ");
  Serial.print(BAUD_RATE);
  Serial.println(" bps, 8N1");

  // Инициализация Modbus. Адрес ведомого устройства (JAF22014)
  jafNode.begin(SLAVE_ADDRESS, Serial1);
  Serial.print("🔧 Modbus инициализирован: адрес ведомого = ");
  Serial.println(SLAVE_ADDRESS);

// Передаем колбэки библиотеке (если используется DE/RE)
#ifdef RS485_DE_RE
  jafNode.preTransmission(preTransmission);
  jafNode.postTransmission(postTransmission);
  Serial.println("🔧 Колбэки DE/RE зарегистрированы");
#endif

  Serial.println();
  Serial.println("✅ Инициализация завершена. Начинаем опрос...");
  Serial.println();

  // Делаем первую паузу перед чтением
  delay(1000);
}

void loop() {
  // Читаем все регистры
  readAllRegisters();

  // Пауза между опросами (5 секунд)
  Serial.println("⏳ Ожидание 5 секунд до следующего опроса...");
  Serial.println();
  delay(5000);
}