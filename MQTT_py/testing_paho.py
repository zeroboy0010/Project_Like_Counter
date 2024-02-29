import sys
import requests
from bs4 import BeautifulSoup
import logging
import re
import paho.mqtt.client as paho

client = paho.Client()

def get_like_v2(page):
    vgm_url = 'https://web.facebook.com/' + page
    html_text = requests.get(vgm_url).text
    soup = BeautifulSoup(html_text, 'html.parser')
    for link in soup.findAll("meta", {"name":"description"}): ## 
        content = link.get('content')
        num = re.findall(r'[\d]+[.,\d]+', content)
        try :
            like = (int)(num[0].replace(",", ""))
        except :
            logging.error("error getting like")
            logging.error("full text : ")
            # print(html_text)

            logging.error("content : ")
            print(content)
            logging.error("num : ")
            print(num)
            like = 0
        return like


if client.connect("localhost", 1883, 60) != 0:
    print("Couldn't connect to the mqtt broker")
    sys.exit(1)

try:
    # Keep the script running
    while True:
        like_count = get_like_v2("tubecoffeecambodia")
        client.publish("plc0001/like", like_count, 1)

except KeyboardInterrupt:
    # Disconnect the client when the script is interrupted
    client.disconnect()
    client.loop_stop()
    print("Disconnected from MQTT broker.")