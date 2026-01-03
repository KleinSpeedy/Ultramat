#!/usr/bin/env python3

import commands_pb2
import serial
import time


def PromptForResp():
    resp = commands_pb2.Response()
    resp.id = int(input("Input Response id: "))
    print("(1) Done (2) Error (3) Emergency Stop")
    resp.type = int(input("Input Type (1) : "))
    hasVers = bool(input("Version yes (1) / no (0): ")) or False
    if hasVers:
        resp.version.major = 1
        resp.version.minor = 2
        resp.version.bugfix = 3
    return resp


def AssignPayloadMsg(msg):
    # HELLO_THERE = 1;
    if msg.type == 1:
        return
    # STOP = 2;
    if msg.type == 2:
        msg.stop.takeDefaultPos = bool(input("Input Stop: "))
    # RECIPE_STEP = 3;
    if msg.type == 3:
        msg.step.xPos = int(input("Input x: "))
        msg.step.yCount = int((input("Input y: ")))
    # MOVE_X = 4;
    if msg.type == 4:
        msg.xPos = int(input("Input x pos: "))
    # MOVE_Y = 5;
    if msg.type == 5:
        msg.yCount = int(input("Input y count: "))


def PromptForMsg():
    msg = commands_pb2.Message()
    msg.id = int(input("Input Message ID: "))
    print("(1) Hello There (2) Stop (3) Step (4) X (5) Y")
    msg.type = int(input("Input Message Type: "))
    AssignPayloadMsg(msg)
    print(msg)
    return msg


def main():
    port = input("Input serial port (/tmp/ttyArduino): ").strip() or '/tmp/ttyArduino'
    s = serial.Serial(port, 115200, timeout=1)
    if not s.is_open:
        print(f'Could not open port {port}')
        return

    msgCounter = 0
    while True:
        if not s.in_waiting and msgCounter == 0:
            print('Nothing yet')
            time.sleep(1)
            continue
        while s.in_waiting:
            msg = commands_pb2.Message()
            msg.ParseFromString(s.readline())
            msgCounter += 1
            print(msg)

        choice = int(input("Msg (1) or Response (2): "))
        out = None
        if choice == 1:
            out = PromptForMsg()
        elif choice == 2:
            out = PromptForResp()
        else:
            print("Invalid input")
            continue
        if out is None:
            print("Error building msg/resp")
            continue
        buffer = out.SerializeToString()
        s.write(buffer)
        s.flush()
        msgCounter -= 1


if __name__ == "__main__":
    main()
