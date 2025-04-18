/////////////////////////// Glitch animation \\\\\\\\\\\\\\\\\\\\\\\\\\\

void glitch() {
  if (state == 2) {
    if (millis() - glitchDelay > 7000) {
      if (millis() - glitchSpeed > 100) {
        counter++;
        if (counter == 1) {
          matrix.fillRect(MawLeft, 0, 32, 8, 0);
          matrix.fillRect(MawRight, 0, 32, 8, 0);
          matrix.drawBitmap(MawLeft, 0, Glitch1L, 32, 8, ANGRYCOLOR);
          matrix.drawBitmap(MawRight, 0, Glitch1, 32, 8, ANGRYCOLOR);
          matrix.show();
        }
        if (counter == 2) {
          matrix.fillRect(MawLeft, 0, 32, 8, 0);
          matrix.fillRect(MawRight, 0, 32, 8, 0);
          matrix.drawBitmap(MawLeft, 0, Glitch2L, 32, 8, ANGRYCOLOR);
          matrix.drawBitmap(MawRight, 0, Glitch2, 32, 8, ANGRYCOLOR);
          matrix.show();
        }
        glitchSpeed = millis();
      }
      if (counter == 3) {
        matrix.fillRect(MawLeft, 0, 32, 8, 0);
        matrix.fillRect(MawRight, 0, 32, 8, 0);
        matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, ANGRYCOLOR);
        matrix.drawBitmap(MawRight, 0, maw, 32, 8, ANGRYCOLOR);
        matrix.show();
        counter = 0;
        glitchDelay = millis();
      }
    }
  }
}
