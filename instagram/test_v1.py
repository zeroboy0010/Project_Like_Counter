
from time import time
import requests
from bs4 import BeautifulSoup

import re

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


main_loop = time()  # return time in sec
likec = get_follower('cbum')
main_loop_time = time()
print('Total time: ', main_loop_time - main_loop)
