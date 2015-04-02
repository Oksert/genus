import glob

def getTopicName (fileName):
	with open (fileName, "r") as myFile:
		for line in myFile:
			curLine = line.split("=")
			left = curLine[0]
			right = curLine[1]
			if left == "DATASET":
				right = right[:right.index("\r")]
				return right
				pass
def getAllTopics (dirName):
	allFiles = glob.glob(dirName)
	allTopic = {}
	i = 1
	for f in allFiles:
		curTopic = getTopicName(f)
		allTopic[curTopic] = i
		i = i + 1
	return allTopic