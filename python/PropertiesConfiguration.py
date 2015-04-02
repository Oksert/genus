import logging
import Properties
import PropertiesConfiguration

properties = None

def parseProperties():
    logging.debug('Parsing genus.xml')
    PropertiesConfiguration.properties = Properties.Properties()
    PropertiesConfiguration.properties.load(open('./genus.properties'))
    logging.debug("Kernel url = " + getKernelUrl())
    logging.debug("Classifier host: " + getServerHost() + "; port: ")
    logging.debug(getServerPort())

def getServerHost():
    if PropertiesConfiguration.properties == None: parseProperties()
    clasHost = PropertiesConfiguration.properties["genus.xmlrpc.classifier.host"]
    return clasHost
    #return ""

def getServerPort():
    if PropertiesConfiguration.properties == None: parseProperties()
    clasPort = PropertiesConfiguration.properties["genus.xmlrpc.classifier.port"]
    return int(clasPort)

def getServerXmlrpc():
	if PropertiesConfiguration.properties == None: parseProperties()
	clasPath = PropertiesConfiguration.properties["genus.xmlrpc.classifier.path.xmlrpc"]
	return clasPath
	

def getKernelUrl():
    if PropertiesConfiguration.properties == None: parseProperties()
    
    kernelPort = PropertiesConfiguration.properties["genus.xmlrpc.kernel.port"]
    kernelHost = PropertiesConfiguration.properties["genus.xmlrpc.kernel.host"]
    kernelPath = PropertiesConfiguration.properties["genus.xmlrpc.kernel.path"] 
    kernelXmlrpc = PropertiesConfiguration.properties["genus.xmlrpc.kernel.path.xmlrpc"]
    
    base = 'http://'
    host = kernelHost
    port = ':' + kernelPort
    path = '/' + kernelPath
    xmlrpc = '/' + kernelXmlrpc
    result = base + host + port + path + xmlrpc
    return result
