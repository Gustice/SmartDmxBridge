# Run an OverTheAir (OTA) Update

* In terminal navigate into `.pio\build\esp32-poe-iso` directoy
    (or a directory that contains the latest `firmware.bin`)
* start http server with by `python -m http.server 8070`
* start telnet session with **TeraTerm**
* run `update http://<yourIP>:8070/firmware.bin`