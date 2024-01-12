import time
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

import urllib.request
import threading
import logging


cred = credentials.Certificate("/home/zero/Desktop/Project_Like_counter/Google_data_base/like-counter-3d752-firebase-adminsdk-5171a-3e9fd0d978.json")
firebase_admin.initialize_app(cred)
db = firestore.client() # connecting to firestore
collection = db.collection('project_like_counter')  # create collection

import requests
from bs4 import BeautifulSoup
import re

def get_telegram_subscriber(page):
    vgm_url = 'https://t.me/' + page
    html_text = requests.get(vgm_url).text
    soup = BeautifulSoup(html_text, 'html.parser')
    like = 0

    for link in soup.findAll("div", {"class":"tgme_page_extra"}):
        print (link)
        content = link.get_text()
        text = content.replace(" ", "")
        num = re.findall(r'[\d]+[.,\d]+', text)
        like = (int)(num[0])
        print(like)
    
    return like

def get_follower(page):
    vgm_url = 'https://www.instagram.com/' + page
    html_text = requests.get(vgm_url).text
    soup = BeautifulSoup(html_text, 'html.parser')
    like = 0

    for link in soup.findAll("script", {"type":"application/ld+json"}):
        source = str(link)
        text = source[:source.index('</script>')].rsplit('userInteractionCount', 1)[1]
        num = re.findall(r'[\d]+[.,\d]+', text)
        like = (int)(num[0])


    # print(like)
    
    return like

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

def get_like(page):
    with urllib.request.urlopen('https://web.facebook.com/' + page) as response :
        body = response.read()
    character_set = response.headers.get_content_charset()
    content = body.decode(character_set)
    workspace = content
    try:
        like = workspace[:workspace.index('&#x1785')].rsplit('.', 1)[1]
        count = ''
        for i in like:
            if i == ',':
                pass
            else :
                count += i
        like_count = (int) (count)
    except :
        print('error')
    return like_count

def publish_to_firestore(id,pageID,like_count):
    collection.document(id).set({ # insert document
        'like': like_count,
        'pageID': pageID,
        'state' : "running"
    })
    

def thread_function(id,pageID,like_):
    
    like_count = get_like_v2(pageID)
    logging.info("get!!!")
    if like_ != like_count and like_count != 0:
        publish_to_firestore(id,pageID,like_count)
        logging.info("pub!!!")


def main():
    docs = collection.stream()  
    threads = list()
    for doc in docs:
        print(f'{doc.id} => {doc.to_dict()}')
        state = str(doc.to_dict()['state'])
        pageID = str(doc.to_dict()['pageID'])
        like = str(doc.to_dict()['like'])
        try :
            like_fl = (int)(like)
        except : 
            like_fl = 0
        th = threading.Thread(target=thread_function, args=(doc.id,pageID,like_fl,))
        threads.append(th)
        th.start()

    for thread in threads:
        thread.join()


if __name__ == '__main__':
    try :
        db = firestore.client() # connecting to firestore
        collection = db.collection('project_like_counter')  # create collection
        while(True):
            format = "%(asctime)s: %(message)s"
            logging.basicConfig(format=format, level=logging.INFO,
                        datefmt="%H:%M:%S")
            main()
    except KeyboardInterrupt :
        pass