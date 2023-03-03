import urllib.request

## TubeCoffeeCambodia   ## problem ## fixed
## koithecambodia
## AnchaPhnompenh
## milkgreenteacambodia
with urllib.request.urlopen('https://web.facebook.com/TubeCoffeeCambodia') as response :
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
    print(like_count)
except :
    print('error')

