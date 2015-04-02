import logging

from xml.dom.minidom import parse, parseString

import XMLConfiguration

configDom = None

def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

def parseDom():
    logging.debug('Parsing genus.xml')
    XMLConfiguration.configDom = parse('../../../genus/src/conf/genus.xml')
    logging.debug("Kernel url = " + getKernelUrl())
    logging.debug("Classifier host: " + getServerHost() + "; port: ")
    logging.debug(getServerPort())

def getServerHost():
    if XMLConfiguration.configDom == None:
        parseDom()
    clasConf = XMLConfiguration.configDom.getElementsByTagName("host")[0].getElementsByTagName("classifier")[0]
    clasHost = clasConf.getElementsByTagName("host")[0]
    return getText(clasHost.childNodes)
    #return ""

def getServerPort():
    if XMLConfiguration.configDom == None:
        parseDom()
    clasConf = XMLConfiguration.configDom.getElementsByTagName("host")[0].getElementsByTagName("classifier")[0]
    clasPort = clasConf.getElementsByTagName("port")[0]
    return int(getText(clasPort.childNodes))

def getKernelUrl():
    if XMLConfiguration.configDom == None:
        parseDom()
    
    kernelConf = XMLConfiguration.configDom.getElementsByTagName("host")[0].getElementsByTagName("kernel")[0]
    kernelPort = kernelConf.getElementsByTagName("port")[0]
    kernelHost = kernelConf.getElementsByTagName("host")[0]
    kernelPath = kernelConf.getElementsByTagName("path")[0] 
    kernelXmlrpc = kernelConf.getElementsByTagName("xmlrpc")[0]
    
    base = 'http://'
    host = getText(kernelHost.childNodes)
    port = ':' + getText(kernelPort.childNodes)
    path = '/' + getText(kernelPath.childNodes)
    xmlrpc = '/' + getText(kernelXmlrpc.childNodes)
    result = base + host + port + path + xmlrpc
    return result
