#!/usr/bin/env python3
import struct
import rclpy
from rclpy.node import Node
from std_msgs.msg import UInt16MultiArray
import serial

class LeaderNode(Node):
    def __init__(self):
        super().__init__('leader_node')
        
        # Configure Serial Port (Adjust '/dev/ttyACM0' to your Arduino Uno port)
        self.declare_parameter('serial_port', '/dev/ttyACM0')
        self.declare_parameter('baud_rate', 115200)
        
        port = self.get_parameter('serial_port').get_parameter_value().string_value
        baud = self.get_parameter('baud_rate').get_parameter_value().integer_value
        
        try:
            self.ser = serial.Serial(port, baud, timeout=0.1)
            self.get_logger().info(f'Connected to Arduino Uno on {port}')
        except serial.SerialException as e:
            self.get_logger().error(f'Failed to open serial port {port}: {e}')
            return

        # ROS 2 Publisher: Target positions (3 angles + 2 pots) -> Follower
        self.cmd_pub = self.create_publisher(UInt16MultiArray, '/exo_commands', 10)

        # ROS 2 Subscriber: Tactile feedback -> Leader Arduino Uno
        self.haptic_sub = self.create_subscription(
            UInt16MultiArray, 
            '/haptic_feedback', 
            self.haptic_callback, 
            10
        )

        # Timer to poll incoming serial data from Arduino Uno (100 Hz)
        self.create_timer(0.01, self.read_serial_from_uno)

    def read_serial_from_uno(self):
        if not hasattr(self, 'ser') or not self.ser.is_open:
            return

        # Check for frame header 0xAA and at least 10 payload bytes
        while self.ser.in_waiting >= 11:
            header = self.ser.read(1)
            if header == b'\xaa':
                payload = self.ser.read(10)
                if len(payload) == 10:
                    # Unpack 5 unsigned 16-bit integers (little endian)
                    values = list(struct.unpack('<5H', payload))
                    
                    msg = UInt16MultiArray()
                    msg.data = values
                    self.cmd_pub.publish(msg)

    def haptic_callback(self, msg):
        if len(msg.data) != 5 or not hasattr(self, 'ser') or not self.ser.is_open:
            return

        # Pack header 0xAA + 5 UInt16 values into 11 bytes and send to Uno
        packet = bytearray([0xAA]) + struct.pack('<5H', *msg.data)
        self.ser.write(packet)

def main(args=None):
    rclpy.init(args=args)
    node = LeaderNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()