
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
cred = credentials.Certificate("/home/zero/Desktop/Project_Like_counter/Google_data_base/like-counter-3d752-firebase-adminsdk-5171a-3e9fd0d978.json")
firebase_admin.initialize_app(cred)

db = firestore.client() # connecting to firestore

collection = db.collection('project_like_counter')  # create collection
res = collection.document('page_001').set({ # insert document
    'name': 'TubeCoffeeCambodia',
    'like': 10000000,
    'state': 'Running',
    'Programming_languages': ['Python', 'C++']
})  
print(res)