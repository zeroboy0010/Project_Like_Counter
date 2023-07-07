import time
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

import urllib.request

cred = credentials.Certificate("/home/zero/Desktop/Project_Like_counter/Google_data_base/like-counter-3d752-firebase-adminsdk-5171a-3e9fd0d978.json")
firebase_admin.initialize_app(cred)

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

def main():
    main_loop = time.time()  # return time in sec
    db = firestore.client() # connecting to firestore
    collection = db.collection('project_like_counter')  # create collection
    docs = collection.stream()  
    for doc in docs:
        # print(f'{doc.id} => {doc.to_dict()}')
        ##########
        state = str(doc.to_dict()['state'])
        pageID = str(doc.to_dict()['pageID'])
        like = str(doc.to_dict()['like'])
        ##########
        print("-----------------------------------")
        main_loop_time = time.time()
        print('Total time catch ID from firestore : ', main_loop_time - main_loop)
        ####
        while(True):
            main_loop = time.time()  # return time in sec
            like_count = get_like(pageID)
            main_loop_time = time.time()
            print('Total time get like from url : ', main_loop_time - main_loop)

            main_loop = time.time()  # return time in sec

            res = collection.document(doc.id).set({ # insert document
                'like': like_count,
                'pageID': pageID,
                'state' : state
            })
            main_loop_time = time.time()
            print('Total time pub like to firestore : ', main_loop_time - main_loop)

if __name__ == '__main__' :
    try :
        main()
    except KeyboardInterrupt :
        pass