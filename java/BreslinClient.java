import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;
import java.nio.ByteBuffer;

public class BreslinClient {
  public static void main(String[] argv) throws Exception {

	DatagramChannel datagramChannel = DatagramChannel.open();

	String newData = "New String to write to file..."
                    + System.currentTimeMillis();

	ByteBuffer buf = ByteBuffer.allocate(48);
	buf.clear();
	buf.put(newData.getBytes());
	buf.flip();

	int bytesSent = datagramChannel.send(buf, new InetSocketAddress("192.168.1.103", 9999));


  }
}