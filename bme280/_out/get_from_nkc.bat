@rem Empfangen einer Datei vom NKC:
@rem Starten des Slaves am NKC (mit 115200 Baud)
@rem rs232d 
@rem Empfangen der Datei am PC

python dl.py -p com3 -b 115200 -d 1:%1

@rem sollen keine weiteren dateien mehr übertragen werden kann der Slave am NKC mit der taste 'x' beenden werden.
@rem Die empfangenen files sollten dann im aktuellen Laufwerk liegen
