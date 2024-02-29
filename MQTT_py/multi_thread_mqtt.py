import time
import requests
from bs4 import BeautifulSoup
import re
import threading
import logging
import paho.mqtt.client as paho
import yaml


yaml_file = '/home/zero/Desktop/Project_Like_counter/MQTT_py/data.yaml'

def read_and_modify_one_block_of_yaml_data(filename, key, value):
    with open(f'{filename}', 'r') as f:
        data = yaml.safe_load(f)
        data[f'{key}'] = value 
        print(data) 
    with open(f'{filename}', 'w') as file:
        yaml.dump(data,file,sort_keys=False)
    print('done!')

def read_one_block_of_yaml_data(filename, key):
    with open(f'{filename}','r') as f:
        output = yaml.safe_load(f)
    return output[f'{key}'] 

def count_blocks(yaml_data):
    with open(f'{yaml_data}','r') as f:
        try :
            yaml_data = yaml.safe_load(f)
            if isinstance(yaml_data, dict):
                # Count the number of keys in the dictionary
                return len(yaml_data)
            else:
                return 0  # Not a dictionary, so not considered a block
        except yaml.YAMLError as e:
            print(f'Error parsing YAML file: {e}')

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

def on_message(client, userdata, msg):
    print(f"Received message on topic {msg.topic}: {msg.payload.decode()}")
    read_and_modify_one_block_of_yaml_data(yaml_file,key = f'{msg.topic}',value = [f'{msg.payload.decode()}',0])

def thread_function(index, pub_back_topic):
    logging.info(f'running task {index}')
    while(True):
        pageID, like_  = read_one_block_of_yaml_data(yaml_file,key = f'plc_{index}')
        like_count = get_like_v2(pageID)
        print(f'{pub_back_topic} :: {like_count}')
        logging.info("get!!!")
        if like_ != like_count and like_count != 0:
            # publish_to_firestore(id,pageID,like_count)
            client.publish(pub_back_topic,like_count,1)
            logging.info("pub!!!")

# Callback when a message is received
        
client = paho.Client()
client.connect("localhost", port=1883, keepalive=60)
client.on_message = on_message
# Start the MQTT loop to handle incoming messages
# client.loop_start()

def main():
    lengh_device = (int) (count_blocks(yaml_file))

    for i in range(1,lengh_device + 1):
        client.subscribe(f'plc_{i}')

    threads = list()
    for index in range(1 , lengh_device + 1):
        Topic_pub_back = f'plc_{index}/like'
        th = threading.Thread(target=thread_function, args=(index,Topic_pub_back,))
        threads.append(th)
        th.start()

    for thread in threads:
        thread.join()


if __name__ == '__main__':
    try :
        format = "%(asctime)s: %(message)s"
        logging.basicConfig(format=format, level=logging.INFO,
                    datefmt="%H:%M:%S")
        main()
    except KeyboardInterrupt :
        client.disconnect()
        client.loop_stop()
        print("Disconnected from MQTT broker.")
        pass