import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

import urllib.request
import time

cred = credentials.Certificate("/home/zero/Desktop/Project_Like_counter/Google_data_base/like-counter-3d752-firebase-adminsdk-5171a-3e9fd0d978.json")
firebase_admin.initialize_app(cred)

def get_like(page):
    with urllib.request.urlopen('https://web.facebook.com/' + page) as response :
        body = response.read()
        print(len(body))
    character_set = response.headers.get_content_charset()
    content = body.decode(character_set)
    workspace = content
    print(workspace[2000:5000])
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
        like_count = 0
        print('error')
    return like_count





def main():
    main_loop = time.time()  # return time in sec

    like_count = get_like('TubeCoffeeCambodia')
    main_loop_time = time.time()
    print('Total time: ', main_loop_time - main_loop)
    print(like_count)
    db = firestore.client() # connecting to firestore

    collection = db.collection('project_like_counter')  # create collection
    res = collection.document('page_001').set({ # insert document
        'name': 'TubeCoffeeCambodia',
        'like': like_count,
        'state': 'Running',
        'Programming_languages': ['Python', 'C++']
    })


if __name__ == "__main__":
    try :
        while(True):
            main()
    except KeyboardInterrupt :
        pass





