import urllib.parse
import urllib.request


__unit_lookup = {
    'k':1000,
    'm':1000000
}


def get_facebook_likes_for_page(page):
    url = "https://www.facebook.com/plugins/like.php?locale=en_US&{}%2F&width=450&layout=standard&action=like&size=small&show_faces=false&share=false&height=35&appId"
    args = { 'href': 'https://www.facebook.com/{}'.format(page)}
    encoded_args = urllib.parse.urlencode(args)

    source = urllib.request.urlopen(url.format(encoded_args)).read().decode('ascii',  'ignore')

    # likes = source[:source.index('people like this')].rsplit('span>', 1)[1]
    likes = source[:source.index('people like this')].rsplit('span>', 1)[1]

    # unit = likes[-1]
    # if not unit.isdigit():
    #     likes, unit = likes[:-1], likes[-1:]
    #     multiplier = __unit_lookup[unit]
    #     likes = float(likes) * multiplier
    #     return likes
    
    return likes, source


def get_facebook_likes_from_facebook_url(facebook_url):
    if facebook_url.endswith('/'):
        facebook_url = facebook_url[:-1]
    page = facebook_url.rsplit('/', 1)[1]
    return get_facebook_likes_for_page(page)
    
like ,source = get_facebook_likes_for_page('TubeCoffeeCambodia')
print(like)

