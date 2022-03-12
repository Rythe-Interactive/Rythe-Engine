import sys
import xml.etree.ElementTree as ET
from pprint import pprint

output = ''

try:
    for line in sys.stdin:
        output += line
except KeyboardInterrupt:
    sys.stdout.flush()
    pass


class github_log:
    def __init__(self):
        self.type = ''
        self.file = ''
        self.line = ''
        self.col = ''
        self.message = ''

    def generate(self):
        if self.type != '':
            print(f'::{self.type} file={self.file},line={self.line},col={self.col}::{self.message}')
            return 1
        else:
            return 0

return_code = 0

report = ET.fromstring(output)
for error in report[1]:
    if error.tag == 'error':

        #generate new github log and populate with default values
        log = github_log();
        log.type = error.attrib.get('severity','warning')
        log.message = error.attrib.get('msg','unknown')

        if len(list(error)) > 0:
            if error[0].tag == 'location':
                log.line = error[0].attrib.get('line','0')
                log.col  = error[0].attrib.get('column','0')
                log.file = error[0].attrib.get('file','[internal]')

        else:
            log.col = '0'
            log.line = '0'
            log.file = '[internal]'

        code = log.generate()
        if return_code == 0:
            return_code = code

sys.exit(return_code)
