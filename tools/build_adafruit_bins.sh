#!/usr/bin/env bash

rm -rf ports/atmel-samd/build*
rm -rf ports/esp8266/build*
rm -rf ports/nrf/build*

# Alphabetical.
HW_BOARDS="\
arduino_mkr1300 \
arduino_zero \
catwan_usbstick \
circuitplayground_express \
circuitplayground_express_crickit \
feather_huzzah \
feather_m0_adalogger \
feather_m0_basic \
feather_m0_express \
feather_m0_express_crickit \
feather_m0_rfm69 \
feather_m0_rfm9x \
feather_m4_express \
feather_nrf52832 \
feather_nrf52840_express \
feather_radiofruit_zigbee \
gemma_m0 \
grandcentral_m4_express \
hallowing_m0_express \
itsybitsy_m0_express \
itsybitsy_m4_express \
makerdiary_nrf52840_mdk \
metro_m0_express \
metro_m4_express \
meowmeow \
pca10056 \
pca10059 \
pirkey_m0 \
trellis_m4_express \
trinket_m0 \
"
ROSIE_SETUPS="rosie-ci"

PARALLEL="-j 5"
if [[ "$TRAVIS" == "true" ]]; then
    PARALLEL="-j 2"
fi

if [[ -z "$TRAVIS_BOARD" ]]; then
    boards=$HW_BOARDS
else
    boards=$TRAVIS_BOARD
fi

version=`git describe --tags --exact-match`
if [[ $? -ne 0 ]]; then
    version=`date +%Y%m%d`-`git rev-parse --short HEAD`
fi


if [[ "$TRAVIS" == "true" ]]; then
    sha=$TRAVIS_COMMIT
    if [[ "$TRAVIS_PULL_REQUEST" != "false" ]]; then
       version=`date +%Y%m%d`-`echo $TRAVIS_PULL_REQUEST_SHA | cut -c1-7`
       sha=$TRAVIS_PULL_REQUEST_SHA
    fi
fi

for board in $boards; do
    mkdir -p bin/$board/
    extension=uf2
    for language_file in $(ls locale/*.po); do
        language=$(basename -s .po $language_file)
        echo "Building $board for $language"
        # There is a bug in the Huzzah Makefile that causes it to fail occasionally with -j > 1.
        if [[ $board == "feather_huzzah" ]]; then
            make -C ports/esp8266 TRANSLATION=$language BOARD=$board
            (( exit_status = exit_status || $? ))
            temp_filename=ports/esp8266/build/firmware-combined.bin
            extension=bin
        elif [[ $board == "feather_nrf52832" ]]; then
            make $PARALLEL -C ports/nrf TRANSLATION=$language BOARD=$board
            (( exit_status = exit_status || $? ))
            temp_filename=ports/nrf/build-$board-s132/firmware.bin
            extension=bin
        elif [[ $board == "feather_nrf52840_express" || $board == "pca10056" || $board == "pca10059" ]]; then
            make $PARALLEL -C ports/nrf TRANSLATION=$language BOARD=$board SD=s140
            (( exit_status = exit_status || $? ))
            temp_filename=ports/nrf/build-$board-s140/firmware.uf2
            extension=uf2
        elif [[ $board == "makerdiary_nrf52840_mdk" ]]; then
            make $PARALLEL -C ports/nrf TRANSLATION=$language BOARD=$board SD=s140
            (( exit_status = exit_status || $? ))
            temp_filename=ports/nrf/build-$board-s140/firmware.hex
            extension=hex
        else
            time make $PARALLEL -C ports/atmel-samd TRANSLATION=$language BOARD=$board
            (( exit_status = exit_status || $? ))
            cp ports/atmel-samd/build-$board/firmware.bin bin/$board/adafruit-circuitpython-$board-$language-$version.bin
            (( exit_status = exit_status || $? ))
            temp_filename=ports/atmel-samd/build-$board/firmware.uf2
            extension=uf2
        fi
        final_filename=bin/$board/adafruit-circuitpython-$board-$language-$version.$extension
        cp $temp_filename $final_filename
        (( exit_status = exit_status || $? ))
        # Only upload to Rosie if its a pull request.
        if [[ "$TRAVIS" == "true" ]]; then
            for rosie in $ROSIE_SETUPS; do
                echo "Uploading to https://$rosie.ngrok.io/upload/$sha"
                curl -F "file=@$final_filename" https://$rosie.ngrok.io/upload/$sha
            done
        fi
        echo
    done
done

exit $exit_status
