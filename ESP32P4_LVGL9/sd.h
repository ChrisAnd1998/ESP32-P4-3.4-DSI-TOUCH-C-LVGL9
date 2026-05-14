#include "FS.h"
#include "SD_MMC.h"

bool drawBmp24(const char *filename, int16_t x, int16_t y)
{
  char path[128];

  // Add leading "/" if missing
  if (filename[0] == '/')
    strncpy(path, filename, sizeof(path));
  else
    snprintf(path, sizeof(path), "/%s", filename);

  path[sizeof(path) - 1] = '\0';

  File bmpFile = SD_MMC.open(path);
  if (!bmpFile) return false;

  uint8_t header[54];
  if (bmpFile.read(header, 54) != 54) {
    bmpFile.close();
    return false;
  }

  int32_t width  = *(int32_t *)&header[18];
  int32_t height = *(int32_t *)&header[22];
  uint16_t depth = *(uint16_t *)&header[28];

  if (depth != 24) {
    bmpFile.close();
    return false;
  }

  uint32_t rowSize = (width * 3 + 3) & ~3;

  uint16_t *lineBuffer = (uint16_t *)ps_malloc(width * sizeof(uint16_t));
  uint8_t  *sdbuffer   = (uint8_t  *)malloc(rowSize);

  if (!lineBuffer || !sdbuffer) {
    if (lineBuffer) free(lineBuffer);
    if (sdbuffer) free(sdbuffer);
    bmpFile.close();
    return false;
  }

  // BMP files are stored bottom-to-top
  int32_t yStart = y + height - 1;

  gfx->startWrite();

  for (int row = 0; row < height; row++)
  {
    if (bmpFile.readBytes((char*)sdbuffer, rowSize) != rowSize)
      break;

    uint8_t *p = sdbuffer;

    for (int col = 0; col < width; col++)
    {
      uint8_t b = *p++;
      uint8_t g = *p++;
      uint8_t r = *p++;

      lineBuffer[col] = ((r & 0xF8) << 8) |
                        ((g & 0xFC) << 3) |
                        (b >> 3);
    }

    gfx->draw16bitRGBBitmap(x, yStart - row,
                            lineBuffer, width, 1);
  }

  gfx->endWrite();

  free(sdbuffer);
  free(lineBuffer);
  bmpFile.close();

  return true;
}

void sdInit() {
  SD_MMC.setPins(43, 44, 39, 40, 41, 42);

  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD_MMC card attached");
    return;
  }

  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

  Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));

  delay(1000);
}