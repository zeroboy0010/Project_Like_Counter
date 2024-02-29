import time

# selenium 3
from selenium import webdriver
from webdriver_manager.firefox import GeckoDriverManager

driver = webdriver.Firefox(executable_path=GeckoDriverManager().install())



driver.get('https://www.facebook.com/koithecambodia/friends_likes/')

time.sleep(20) # Let the user actually see something!

input_email = driver.find_element_by_name('email')

input_email.send_keys('nakimhoir.km@ai.thoth.pro')

input_password = driver.find_element_by_name('pass')
input_password.send_keys('Hoir1009')

input_password.submit()

time.sleep(20) # Let the user actually see something!
html = driver.page_source
with open('html.html', 'w') as f:
    f.write(html)
driver.quit()