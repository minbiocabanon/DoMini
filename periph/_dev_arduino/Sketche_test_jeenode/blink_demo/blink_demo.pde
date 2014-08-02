#define LED_PIN 9
void setup () {
    pinMode(LED_PIN, OUTPUT);
}
void loop () {
    digitalWrite(LED_PIN, 1);
    delay(500);
    digitalWrite(LED_PIN, 0);
    delay(500);
} 	