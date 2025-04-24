"""
Possible interesting packages for obis:
no analog stuff:
https://pypi.org/project/obis-laser-controller/
could be used as dependency:
https://pypi.org/project/easy-scpi/
has digital external control:
https://github.com/AlistairBoettiger/storm-control/blob/master/sc_hardware/coherent/obis.py
not python:    
https://github.com/hc5/CoherentOBIS

Note that we need to set the input impedence of the analog modulation port.
And also set the laser in "analog modulation mode".
This might not be implemented in existing packages.
    


@author: Aron Opheij
"""
import serial  # pip install pyserial
from serial.tools import list_ports
import logging
import time

def list_comports():
    """This is a helper function for the user, to identify serial ports."""
    print('port     vid    pid  description')
    for dev in list_ports.comports():
        try:
            print(f'{dev.device:5}: {dev.vid:5}  {dev.pid:5}  {dev.description}')
        except:
            pass
        

class ObisArduino:
    def __init__(self, port=None, vid=6790, pid=29987):
        self.logger = logging.getLogger(__name__)
        self.port = port
        if port is None:
            found = []
            for d in list_ports.comports():
                try:
                    if d.vid == vid and d.pid == pid:
                        found.append(d.device)
                except:
                    pass

            if not found:
                self.logger.error('No device found with vid={vid} pid={pid}')
                print('All possible devices on comports are:')
                list_comports()
                return
            self.port = found[0]
            print('Found arduino on', self.port)
        # self.dev = serial.Serial(self.port, baudrate=115200)
        self.dev = serial.Serial()
        self.dev.port = port  #, , **kwargs)
        self.dev.baudrate = 115200

        for k in range(5):
            try:
                self.dev.open()
            except:
                pass
            if self.dev.is_open:
                break
            print('Arduino: Connection attempt {} failed. Trying again'.format(k))
            self.dev.close()
            time.sleep(1)

        time.sleep(1.5)
    
    # def __new__(self, port=None, vid=6790, pid=29987):
        
    def close(self):
        self.dev.close()
    
    def trigger(self):
        self.dev.write(b't')
        
    def __del__(self):
        self.close()
        
    def __exit__(self):
        self.close()
        
    def setup(self, n_pulses: int = 16, us_on: int = 1, ms_off: int = 60, mode: str = 'single'):
        if not all(type(a) is int for a in (us_on, ms_off, n_pulses)):
            self.logger.warning('us_on, ms_off and n_pulses converted to integer')
            us_on, ms_on, n_pulses = int(us_on), int(ms_off), int(n_pulses)
        if type(mode) is not str:
            self.logger.error('mode need to be of type str')
            return
        if us_on > 65535:
            self.logger.error('Maximum us_on is 65535')
        print(f'SENDING: {mode[0]}{n_pulses},{us_on},{ms_off}')
        self.dev.write(f'{mode[0]}{n_pulses},{us_on},{ms_off}'.encode('ascii'))



# if __name__ == '__main__':

    # Number_cycle=1000
    # off_time=1000
    #
    # record_duration=100 #us
    # # Delete existing object when re-running the code in interactive console:
    # try:
    #     get_ipython().del_var('obis')
    # except:
    #     pass
    #
    # obis = ObisArduino('COM6')
    #
    #
    # obis.setup(Number_cycle, 1, off_time, 'multiple')
    # time.sleep(1)
    # NikonRFA.
    # obis.trigger()
    # Cobra.record(record_duration)
    
    
    