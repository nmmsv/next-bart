# used for inspiration: https://github.com/ericdwang/pybart
import sys

sys.path.append('/home/nmmsv/repos/pybart')
from pybart.api import BART

api_key = 'MW9S-E7SL-26DU-VV8V'
BART_URL = 'https://api.bart.gov/api/{api}.aspx'


bart = BART()

root = bart.stn.stninfo('dbrk')
station = root.find('stations').find('station')


# https://api.bart.gov/api/etd.aspx?cmd=etd&orig=ROCK&key=MW9S-E7SL-26DU-VV8V