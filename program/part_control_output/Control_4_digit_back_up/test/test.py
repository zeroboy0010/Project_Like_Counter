import time
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import logging

cred = credentials.Certificate("/home/zero/Desktop/Project_Like_counter/Google_data_base/like-counter-3d752-firebase-adminsdk-5171a-3e9fd0d978.json")
firebase_admin.initialize_app(cred)
db = firestore.client() # connecting to firestore
collection = db.collection('project_like_counter')  # create collection

def publish_to_firestore(id,pageID,like_count):
    collection.document(id).set({ # insert document
        'like': like_count,
        'pageID': pageID,
        'state' : "running"
    })

def main():
    docs = collection.stream()  
    i = 0
    while(True):
        time.sleep(5)
        for doc in docs:
            print(f'{doc.id} => {doc.to_dict()}')
            state = str(doc.to_dict()['state'])
            pageID = str(doc.to_dict()['pageID'])
            like = str(doc.to_dict()['like'])
        if i == 0:
            like_count = 0
        elif i == 1:
            like_count = 1111111
        elif i == 2:
            like_count = 2222222
        elif i == 3:
            like_count = 3333333
        elif i == 4:
            like_count = 4444444
        elif i == 5:
            like_count = 5555555
        elif i == 6:
            like_count = 6666666
        elif i == 7:
            like_count = 7777777
        elif i == 8:
            like_count = 8888888
        elif i == 9:
            like_count = 9999999
        i = i + 1
        if (i >= 10) : i = 0
        publish_to_firestore(doc.id,pageID,like_count)


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