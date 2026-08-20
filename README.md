# Embedded Linux Edge Gateway

## Project Overview
Dette projekt demonstrerer en komplet IoT Edge Gateway bygget på BeaglePlay. Systemet læser fysisk miljødata (temperatur, fugtighed, tryk) via en BME280-sensor, publicerer dette lokalt via MQTT, hvorefter en processor validerer, beriger (med timestamps) og videresender data via en Mosquitto Bridge til en central server med PostgreSQL og Grafana.

## Architecture

```mermaid
flowchart LR
    A[BME280] -->|I2C| B[Sensor Publisher]
    B -->|sensors/bp-06/raw| C[(BP Mosquitto)]
    C --> D[Telemetry Processor]
    D -->|gateway/bp-06/weather| C
    C -->|Mosquitto Bridge| E[(Server Mosquitto)]
    E --> F[Telegraf]
    F --> G[(PostgreSQL)]
    G --> H[Grafana]
Build
Projektet består af to applikationer, der bygger på fælles MQTT-kode.
For at bygge begge, gå ind i deres respektive mapper og kør Make:

cd sensor-publisher
make clean && make

cd ../telemetry-processor
make clean && make

Configuration
Konfiguration sker via JSON-filer. Skabeloner (config.example.json) findes i kildekodemapperne.
I produktion er konfigurationsfilerne placeret her:

/etc/eudedge/sensor-config.json

/etc/eudedge/processor-config.json

Værdier der kan ændres inkluderer I2C-enhed, I2C-adresse, MQTT topics og broker-adresser.

Run / Service
Programmerne kører som systemd-tjenester under den dedikerede bruger eudedge (med gpio grupperettigheder).

Vigtigste kommandoer:

Start system: sudo systemctl start sensor-publisher telemetry-processor

Stop system: sudo systemctl stop sensor-publisher telemetry-processor

Status: systemctl status sensor-publisher

Læs logs: sudo journalctl -u telemetry-processor -f

MQTT Topics
Den faktiske løsning anvender følgende topics:

Raw data: sensors/bp-06/raw (Produceret af Sensor Publisher)

Processed data: gateway/bp-06/weather (Produceret af Telemetry Processor, brobygges til serveren)
