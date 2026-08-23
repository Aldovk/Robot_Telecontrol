import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import serial

class SensorPublisher(Node):
    def __init__(self):
        super().__init__('sensor_publisher')
        self.publisher_ = self.test_publisher = self.create_publisher(String, 'sensor_array', 10)
        
        # Adjust serial port if necessary (e.g., /dev/ttyACM0)
        try:
            self.serial_port = serial.Serial('/dev/ttyACM0', 115200, timeout=0.1)
        except Exception as e:
            self.get_logger().error(f"Failed to open serial port: {e}")
            self.serial_port = None

        self.timer = self.create_timer(0.01, self.read_serial_callback)

    def read_serial_callback(self):
        if self.serial_port and self.serial_port.in_waiting > 0:
            line = self.serial_port.readline().decode('utf-8', errors='ignore').strip()
            if line:
                msg = String()
                msg.data = line
                self.publisher_.publish(msg)
                self.get_logger().info(f'Published: "{line}"')

def main(args=None):
    rclpy.init(args=args)
    node = SensorPublisher()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        # Safely clean up without duplicate shutdown exceptions
        if rclpy.ok():
            node.destroy_node()
            rclpy.shutdown()

if __name__ == '__main__':
    main()