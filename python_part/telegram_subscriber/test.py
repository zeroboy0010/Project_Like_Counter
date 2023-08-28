
from time import time
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


main_loop = time()  # return time in sec
likec = get_telegram_subscriber('CSBSPORTS24')
main_loop_time = time()
print('Total time: ', main_loop_time - main_loop)
