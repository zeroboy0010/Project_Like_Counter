from __future__ import division
from django.template import Library, Node, Variable
import urllib.request
import json

class FbLikeNode(Node):

    def __init__(self, page_name, type):
        self.page_name = page_name
        self.type = type

    def render(self, context):
        self.page_name = Variable(str(self.page_name))
        self.type = Variable(str(self.type))

        response = urllib.request.urlopen(
            'http://api.facebook.com/method/fql.query?query=' +
            'select%20like_count%20from%20link_stat%20where%20url=' +
            '%27https://www.facebook.com/' +
            str(self.page_name) + '%27&format=json')

        data = json.loads(response.read())
        response.close()

        if(str(self.type) == 'not_formatted'):
            like_count = data[0]['like_count']
        elif(str(self.type) == 'formatted'):
            like_count = str(round(int(data[0]['like_count'])/1000, 1))+"K"
        else:
            raise AssertionError("invalid second parameter")

        return like_count

inttt = FbLikeNode('tubecoffeecambodia','formatted')
like = inttt.render('')
print(like)