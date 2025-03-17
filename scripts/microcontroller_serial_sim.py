import serial
from enum import IntEnum, Enum, auto
import queue
import threading


class RecieverState(Enum):
    IDLE = auto()
    ID = auto()
    CMD = auto()
    DATA_LENGTH = auto()
    DATA = auto()
    CHECKSUM = auto()


class Command(IntEnum):
    HELLO_THERE = 0xB0  # initial hello
    HAPPY_LANDING = 0xB1  # end connection "nicely"
    HOMING_X = 0xC0  # homing procedure X-axis
    HOMING_Y = 0xC1  # homing procedure Y-axis
    RECIPE_STEP = 0xC2  # move to position + up/down motion
    STOP = 0xC4  # "emergency" stop
    CHANGE_POSITION = 0xCA  # move to specific position without going up/down
    FILL_GLASS = 0xCB  # only up and down motion TODO: Position check

    ACKNOWLEDGE = 0xA0  # Acknowledge a command that was sent
    DONE = 0xA1  # Command is done


class SerialDataHandler:
    FRAME_ESCAPE_CHAR = 0x7D
    FRAME_XOR_CHAR = 0x20
    FRAME_START = 0x10

    def __init__(self, ser):
        self.ser = ser
        self.xorValue = 0x00
        self.receiverStatus = RecieverState.IDLE
        self.inputBuffer = []
        self.checksum = 0
        self.checksumMsb = False
        self.recievedChecksum = 0
        self.dataLength = 0
        self.inQueue = queue.Queue()
        self.outQueue = queue.Queue()

    def write_serial_data(self, payload):
        self.ser.write(payload)

    def process(self):
        if not self.outQueue.empty():
            msg = self.outQueue.get()
            print('Send out msg', msg)
            self.ser.write(msg)
        if self.ser.in_waiting > 0:
            receivedByte = ord(self.ser.read(1))
            if receivedByte == self.FRAME_ESCAPE_CHAR:
                self.xorValue = self.FRAME_XOR_CHAR
            else:
                receivedByte ^= self.xorValue
                self.xorValue = 0x00
                self._decode_data(receivedByte)

    def _decode_data(self, receivedByte):
        match self.receiverStatus:
            case RecieverState.IDLE:
                if receivedByte == self.FRAME_START:
                    self.inputBuffer = [receivedByte]
                    self.checksum = receivedByte
                    self.receiverStatus = RecieverState.ID

            case RecieverState.ID:
                self.inputBuffer.append(receivedByte)
                self.checksum += receivedByte
                self.receiverStatus = RecieverState.CMD

            case RecieverState.CMD:
                self.inputBuffer.append(receivedByte)
                self.checksum += receivedByte
                self.receiverStatus = RecieverState.DATA_LENGTH

            case RecieverState.DATA_LENGTH:
                self.dataLength = receivedByte
                if self.dataLength > 0:
                    self.inputBuffer.append(receivedByte)
                    self.checksum += receivedByte
                    self.receiverStatus = RecieverState.DATA
                else:
                    self.receiverStatus = RecieverState.IDLE

            case RecieverState.DATA:
                self.inputBuffer.append(receivedByte)
                self.checksum += receivedByte
                if self.dataLength - 1 == 0:
                    self.receiverStatus = RecieverState.CHECKSUM
                self.dataLength -= 1

            case RecieverState.CHECKSUM:
                self.inputBuffer.append(receivedByte)
                if self.checksumMsb:
                    self.recievedChecksum += receivedByte
                    if self.recievedChecksum == self.checksum:
                        print("Message complete:", self.inputBuffer)
                        self.inQueue.put(self.inputBuffer)
                    else:
                        print("Error computing checksum:",
                              self.recievedChecksum, self.checksum)
                    self.receiverStatus = RecieverState.IDLE
                    self.checksumMsb = False
                    self.recievedChecksum = 0
                else:
                    self.checksumMsb = True
                    self.recievedChecksum = (receivedByte << 8) & 0xFFFF


class MessageHandler:
    def __init__(self, input, output):
        self.inQueue = input
        self.outQueue = output

    def process_messages(self):
        if not self.inQueue.empty():
            message = self.inQueue.get()
            id = message[1]
            cmd = message[2]  # Extract command byte
            self.send_ack(id)  # send ack immediatly
            match cmd:
                case Command.HAPPY_LANDING:
                    # TODO: ack
                    threading.Timer(2, self.send_delayed_done, [id]).start()
                    pass
                case Command.HELLO_THERE | Command.HOMING_X | Command.HOMING_Y:
                    # TODO: ack, wait 10s and send done
                    threading.Timer(10, self.send_delayed_done, [id]).start()
                    pass
                case Command.STOP:
                    # TODO ack
                    threading.Timer(1, self.send_delayed_done, [id]).start()
                    pass
                case Command.CHANGE_POSITION | Command.FILL_GLASS:
                    # TODO: ack, wait 5s and send done
                    threading.Timer(5, self.send_delayed_done, [id]).start()
                    pass

    def send_ack(self, id):
        msg = [0x10, Command.ACKNOWLEDGE, id]
        (high, low) = self.calc_checksum(msg)
        msg.append(high)
        msg.append(low)
        self.outQueue.put(msg)

    def send_delayed_done(self, id):
        msg = [0x10, Command.DONE, id]
        (high, low) = self.calc_checksum(msg)
        msg.append(high)
        msg.append(low)
        self.outQueue.put(msg)

    def calc_checksum(self, buffer) -> (int, int):
        frame_sum = sum(buffer) & 0xFFFF  # Ensure 16-bit value
        high_byte = (frame_sum >> 8) & 0xFF
        low_byte = frame_sum & 0xFF
        return (high_byte, low_byte)


if __name__ == "__main__":
    try:
        s = serial.Serial('/tmp/ttyArduino', 115200, timeout=1)
    except Exception as e:
        print(e)
        exit(1)
    serialHandler = SerialDataHandler(s)
    msgHandler = MessageHandler(serialHandler.inQueue,
                                serialHandler.outQueue)

    while True:
        serialHandler.process()
        msgHandler.process_messages()
