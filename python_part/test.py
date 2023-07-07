
from time import time
import requests
from bs4 import BeautifulSoup
import re

def get_like(page):
    vgm_url = 'https://web.facebook.com/' + page
    html_text = requests.get(vgm_url).text
    soup = BeautifulSoup(html_text, 'html.parser')
    like = 0
    for link in soup.findAll("meta", {"name":"description"}): ## 
        content = link.get('content')
        num = re.findall(r'[\d]+[.,\d]+', content)
        like = (int)(num[0].replace(",", ""))
    
    print(like)
    
    return like
main_loop = time()  # return time in sec
likec = get_like('koithecambodia')
main_loop_time = time()
print('Total time: ', main_loop_time - main_loop)
