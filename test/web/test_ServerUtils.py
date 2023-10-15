import os
import unittest
import utils.ServerUtils as Utils
import json

scriptFile = (__file__)

class TestServerUtils(unittest.TestCase):
    def test_AllIsWorking(self):
        self.assertEqual(True, True)

    def test_PathsAreKorrect(self):
        sut = Utils.MockDataReader(scriptFile, 1, ['web'], "TestResponses.json")
        serverDir = os.path.dirname(scriptFile)
        abovePath = os.path.dirname(serverDir)
        innerPath = os.path.join(abovePath, "web")
        self.assertTrue(sut.rootDir == abovePath, f"path '{sut.rootDir}' not as expected ({abovePath})")
        self.assertTrue(sut.webSiteDir == innerPath, f"path '{sut.webSiteDir}' not as expected '{(abovePath)}'")

    def test_ResponsesFileIsScanned_pathFound(self):
        sut = Utils.MockDataReader(scriptFile, 2, ['web'], "utils\TestResponses.json")
        result = sut.ScanResponseDefinitionsForRouteTakeFirst("/api/valid/route")
        self.assertTrue(result != None)
        self.assertEqual(json.dumps(result) , '{"health": "OK", "wifi": "IsSet"}', f"response was: {result}")
    
    def test_ResponsesFileIsScanned_pathNotFoundBecauseNotExisting(self):
        sut = Utils.MockDataReader(scriptFile, 2, ['web'], "utils\TestResponses.json")
        result = sut.ScanResponseDefinitionsForRouteTakeFirst("/api/trash/route")
        self.assertTrue(result == None)


if __name__ == '__main__':
    unittest.main()
