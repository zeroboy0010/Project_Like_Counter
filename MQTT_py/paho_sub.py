import paho.mqtt.client as mqtt
import requests
from bs4 import BeautifulSoup
import logging
import re
pageid = []
topic_list = []
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

# Callback when a message is received
def on_message(client, userdata, msg):
    print(f"Received message on topic {msg.topic}: {msg.payload.decode()}")
    if msg.topic not in topic_list :
        pageid.append(msg.payload.decode())
        topic_list.append(msg.topic)

# Create an MQTT client instance
client = mqtt.Client()

# Set the callback function for when a message is received
client.on_message = on_message

# Connect to the MQTT broker (replace 'broker_address' and 'port' with your broker's address and port)
client.connect("localhost", port=1883, keepalive=60)

# Subscribe to all topics using '#' wildcard
client.subscribe("#",0)

# Start the MQTT loop to handle incoming messages
client.loop_start()

try:
    # Keep the script running
    while True:
        for topic in topic_list:
            page_like = get_like_v2(pageid[topic_list.index(topic)])
            client.publish(f"{topic}/like", page_like, 1)


        

except KeyboardInterrupt:
    # Disconnect the client when the script is interrupted
    client.disconnect()
    client.loop_stop()
    print("Disconnected from MQTT broker.")
