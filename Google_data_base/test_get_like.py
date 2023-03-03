import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

import urllib.request
import time

cred = credentials.Certificate("/home/zero/Desktop/Project_Like_counter/Google_data_base/like-counter-3d752-firebase-adminsdk-5171a-3e9fd0d978.json")
firebase_admin.initialize_app(cred)

def main():
    main_loop = time.time()  # return time in sec
    with urllib.request.urlopen('https://web.facebook.com/TubeCoffeeCambodia') as response :
        body = response.read()
    character_set = response.headers.get_content_charset()
    content = body.decode(character_set)
    workspace = content
    main_loop_time = time.time()
    print('Total time: ', main_loop_time - main_loop)
    try:
        like = workspace[:workspace.index('&#x1785')].rsplit('.', 1)[1]
        count = ''
        for i in like:
            if i == ',':
                pass
            else :
                count += i
        like_count = (int) (count)

        # print(like_count)
        db = firestore.client() # connecting to firestore

        collection = db.collection('project_like_counter')  # create collection
        res = collection.document('page_001').set({ # insert document
            'name': 'TubeCoffeeCambodia',
            'like': like_count,
            'state': 'Running',
            'Programming_languages': ['Python', 'C++']
        })
    except :
        print('error')

if __name__ == "__main__":
    try :
        while(True):
            main()
    except KeyboardInterrupt :
        pass





