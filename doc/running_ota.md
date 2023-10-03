# Run an OverTheAir (OTA) Update

In terminal navigate into `.pio\build\esp32-poe-iso` directoy 
start http server with by `python -m http.server 8070`

start telnet session with **TeraTerm** 
run 
`update http://192.168.178.10:8070/firmware.bin`
