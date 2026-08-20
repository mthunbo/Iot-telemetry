# Sensor Publisher

Dette modul er ansvarligt for at snakke med den fysiske hardware (BME280) via I2C på BeaglePlay.

## Funktionalitet
- Forbinder til `/dev/play/grove/i2c` via I2C-adressen (standard 0x76).
- Anvender Boschs officielle C-driver API via en udleveret C++ wrapper.
- Læser temperatur, fugtighed og tryk.
- Pakker data ind i et `raw` JSON-format.
- Publicerer til den lokale Mosquitto broker via MQTT.

## Bygge-information
Dette modul kræver, at kildekoden fra `vendor/bosch/` er til stede. `Makefile` sørger automatisk for at bygge BME280 C-driveren sammen med vores egne C++ filer og de delte filer fra `../common/`.

## Eksempel på output (Raw JSON)
```json
{
  "device_id": "bp-06",
  "temperature_c": 22.4,
  "humidity_pct": 48.1,
  "pressure_hpa": 1012.5
}

Connector:           Grove
Stabilt Linux alias: /dev/play/grove/i2c
Underliggende bus:   /dev/i2c-1
Sensoradresse:        0x76
Chip-ID:              0x60
Identificeret sensor: BME280