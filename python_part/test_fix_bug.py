import time
import urllib.request
req = urllib.request.Request(url='https://web.facebook.com/TubeCoffeeCambodia')
for i in range(2):
    with urllib.request.urlopen(req,timeout=3) as f:
        print(f.read().decode('utf-8'))
    time.sleep(2)