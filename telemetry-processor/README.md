# Telemetry Processor

Dette modul fungerer som data-vaskemaskinen i vores Edge Gateway. Den lytter på rå sensordata, validerer det og forbereder det til den centrale database.

## Funktionalitet
- Subscribes på `sensors/+/raw`.
- Parser indgående JSON og validerer at nødvendige felter eksisterer (temperatur, fugt, tryk).
- Afviser korrupt data og logger fejlen til `journalctl`.
- Tilføjer lokalt `timestamp` i ISO 8601 UTC format.
- Tilføjer `gateway_id` og en `status` indikator.
- Publicerer den berigede payload til `gateway/bp-06/weather`.

## Bygge-information
For at bygge, linker Makefilen kildekoden fra `src/` sammen med den delte MQTT-logik fra `../common/src/`. Alt output gemmes i en `build/` mappe for at holde roden ren.

## Eksempel på output (Processed JSON)
```json
{
  "device_id": "bp-06",
  "gateway_id": "bp-06",
  "temperature_c": 22.4,
  "humidity_pct": 48.1,
  "pressure_hpa": 1012.5,
  "status": "valid",
  "timestamp": "2026-08-19T07:57:53Z"
}