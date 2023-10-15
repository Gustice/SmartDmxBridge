from http import server
from io import BytesIO
import re
import os
import json
import this
from urllib import response
import math
import random

def load_binary(file):
    with open(file, 'rb') as file:
        return file.read()
def search_item (url, items):
    for keyval in items:
        if url.lower() == (keyval['url'].lower()):
            return keyval['responses']

class MockDataReader:
    serverDir = ""
    rootDir = ""
    webSiteDir = ""
    responsesPath = ""
    def __init__(self, serverPath : str, breadCrumbsToWebSite : str, responsesFileName):
        self.serverDir = (os.path.dirname(serverPath))
        self.rootDir = self.serverDir

        rPath = breadCrumbsToWebSite
        while str.startswith(rPath, "../"):
            rPath = rPath[3:]
            self.rootDir = os.path.dirname(self.rootDir)
        self.webSiteDir = self.rootDir
        self.webSiteDir = os.path.join(self.webSiteDir, rPath)
        self.responsesPath = os.path.join(self.serverDir, responsesFileName) 

        print("serving data form", self.webSiteDir)

    def ScanResponseDefinitionsForRouteTakeFirst(self, route):
        with open(self.responsesPath, 'r') as response_file:
            responsePool = json.loads(response_file.read())
        found = search_item(route, responsePool['GetResponses'])
        if (found != None):
            found = found[0]
        return found





