///////////////////////////Blink animation\\\\\\\\\\\\\\\\\\\\\\\\\\\

void Blink() {
  if ((millis() - blinkTime > blinkDelay) && (state != 3))  {                  //The blinking animation begins here
    if (millis() - blinkSpeed > 20) {
      if (Step < 8) {
        Step++;
        matrix.fillRect(EyeRight, -8 + Step, 16, 8, 0);
        matrix.fillRect(EyeLeft, -8 + Step, 16, 8, 0);
        matrix.show();
        microphone();
      }
      else if (Step >= 8) {
        Step++;
        writeExpres();
        matrix.fillRect(EyeRight, (Step - 8) * -1, 16, 8, 0);
        matrix.fillRect(EyeLeft, (Step - 8) * -1, 16, 8, 0);
        matrix.show();
        microphone();
      }
      if (Step == 16) {
        Step = 0;
        blinkTime = millis();
      }
      blinkSpeed = millis();
    }
    blinkState = 1;
  }
}
