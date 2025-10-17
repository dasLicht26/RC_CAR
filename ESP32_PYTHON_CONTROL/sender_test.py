import serial
import time
import json
import random

# COM-Port anpassen wenn nötig
ser = serial.Serial('COM3', 115200)
time.sleep(2)  # Warten bis ESP bereit ist

# Beispiel-JSON: Servo auf 45°, ESC auf 1600 µs, LED an
johannas_car = {
    "servo": 45,
    "esc": 1600,
    "led": True,
    "buzzer": False
}

while True:

    # Zufällige Werte für Servo und ESC
    johannas_car["servo"] = random.randint(0, 180)
    johannas_car["esc"] = random.randint(1000, 2000)

    # Zufälliger Wert für LED
    johannas_car["led"] = random.choice([True, False])

    # Zufällige Werte für Servo und ESC
    johannas_car["servo"] = 20
    #command["esc"] = random.randint(1000, 2000)

    # Zufälliger Wert für LED
    johannas_car["led"] = random.choice([True, False])
    johannas_car["led"] = False

    json_str = json.dumps(johannas_car)
    ser.write((json_str + '\n').encode())
    print("Gesendet:", json_str)
    time.sleep(2)  # alle 2 Sekunden senden