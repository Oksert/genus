import ConfigParser

class CSConfigParser(ConfigParser.SafeConfigParser):
	def optionxform(self,option):
		return str(option)
