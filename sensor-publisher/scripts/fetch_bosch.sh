#!/usr/bin/env bash
set -euo pipefail

# Bosch' release hedder "BME280 v3.5.1", men Git-tagget/ref'et
# i repositoryet hedder "bme280_v3.5.1".
VERSION="v3.5.1"
REF="bme280_v3.5.1"

BASE="https://raw.githubusercontent.com/boschsensortec/BME280_SensorAPI/${REF}"
DEST="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/vendor/bosch"

mkdir -p "${DEST}"

TMPDIR="$(mktemp -d)"
trap 'rm -rf "${TMPDIR}"' EXIT

for file in bme280.c bme280.h bme280_defs.h LICENSE; do
    echo "Fetching Bosch ${file} (${VERSION}, ref ${REF})..."
    curl --fail --location --silent --show-error \
        "${BASE}/${file}" \
        --output "${TMPDIR}/${file}"
done

# Flyt først filerne på plads, når alle downloads er lykkedes.
for file in bme280.c bme280.h bme280_defs.h LICENSE; do
    mv "${TMPDIR}/${file}" "${DEST}/${file}"
done

cat > "${DEST}/UPSTREAM.txt" <<EOF
Bosch Sensortec BME280 SensorAPI
Driver version: ${VERSION}
Git ref: ${REF}
Repository: https://github.com/boschsensortec/BME280_SensorAPI

The files bme280.c, bme280.h, bme280_defs.h and LICENSE are
unmodified upstream files from Bosch Sensortec.
EOF

echo
echo "Bosch SensorAPI fetched into:"
echo "  ${DEST}"
