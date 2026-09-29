#include <SPI.h>       
#include <MFRC522.h>   

#define SS_PIN 10       
#define RST_PIN 9      
#define LED_GREEN  3   
#define LED_RED 4       
#define LED_YELLOW 2    
#define BUZZER_PIN 5    

const byte NUMBER_OF_CARDS = 2; // عدد البطاقات المسموح لها بالدخول
// جدول يحتوي على أرقام كل البطاقات المسموحة (يمكنك التعديل عليها بسهولة)
byte Allowed_UIDs[NUMBER_OF_CARDS][4] = {
  {0x43, 0x12, 0xD1, 0x26}, // أرقام البطاقة الأولى
  {0x11, 0x22, 0x33, 0x44}  // أرقام البطاقة الثانية
};

MFRC522 rfid(SS_PIN, RST_PIN); // إنشاء كائن للتحكم بالقارئ

void setup() { 
  SPI.begin();          // تشغيل بروتوكول الاتصال SPI
  rfid.PCD_Init();      // تفعيل قطعة قارئ البطاقات
  
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);  
  pinMode(LED_RED, OUTPUT);  
  pinMode(BUZZER_PIN, OUTPUT); 

  digitalWrite(LED_YELLOW, HIGH); // تشغيل الليد الأصفر فوراً ليدل على جاهزية النظام
}
 
void loop() {
  // التحقق مما إذا كانت هناك بطاقة جديدة ونجحت قراءتها، وإذا لم تنجح يتم إنهاء الدورة والبدء من جديد
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ) return;

  bool isMatch = false; // متغير لحفظ النتيجة (نفترض أولاً عدم التطابق)

  // حلقة تمر على جميع البطاقات المسجلة فوق تلقائياً وتقارنها بالبطاقة الممسوحة حالياً
  for (byte c = 0; c < NUMBER_OF_CARDS; c++) {
    // دالة memcmp تقارن الـ 4 بايتات للبطاقة الممسوحة مع السطر [c] في الجدول دفعة واحدة
    if (memcmp(rfid.uid.uidByte, Allowed_UIDs[c], 4) == 0) {
      isMatch = true; // إذا نجحت المقارنة، تصبح النتيجة متطابقة
      break;          // الخروج من الحلقة فوراً لسرعة الأداء
    }
  }

  // [الحالة الأولى]: إذا تطابقت البطاقة الممسوحة مع أي بطاقة في الجدول
  if (isMatch) {
    digitalWrite(LED_YELLOW, LOW); 
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH); 
    delay(500);                    
    digitalWrite(BUZZER_PIN, LOW); 
    delay(1500);                   
    digitalWrite(LED_YELLOW, HIGH); 
    digitalWrite(LED_GREEN, LOW); 
  } 
  // [الحالة الثانية]: إذا لم تتطابق البطاقة مع أي من البطاقات المسجلة فوق
  else {
    digitalWrite(LED_YELLOW, LOW);   digitalWrite(LED_RED, HIGH);  
    digitalWrite(BUZZER_PIN, HIGH);  delay(200);                    
    digitalWrite(BUZZER_PIN, LOW);   delay(100);                    
    digitalWrite(BUZZER_PIN, HIGH);  delay(200);                    
    digitalWrite(BUZZER_PIN, LOW);   delay(1500);                   
    digitalWrite(LED_YELLOW, HIGH);  digitalWrite(LED_RED, LOW);    
  }

  rfid.PICC_HaltA();       // وضع البطاقة الحالية في السكون لتفادي تكرار قراءتها وهي ثابتة
  rfid.PCD_StopCrypto1();  // إيقاف التشفير لتهيئة القارئ لاستقبال عملية مسح جديدة
}
