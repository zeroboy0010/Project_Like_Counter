import urllib.request
import urllib.robotparser

## TubeCoffeeCambodia   ## problem ## fixed
## koithecambodia
## AnchaPhnompenh
## milkgreenteacambodia
with urllib.request.urlopen('https://web.facebook.com/TubeCoffeeCambodia') as response :
    body = response.read()
character_set = response.headers.get_content_charset()
print(character_set)
# content = body.decode(character_set)

# print(content[:1000])