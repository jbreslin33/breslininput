import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;
import java.nio.ByteBuffer;

public class BreslinServer {
  public static void main(String[] argv) throws Exception {



	DatagramChannel datagramChannel = DatagramChannel.open();
	datagramChannel.configureBlocking(true);
	datagramChannel.socket().bind(new InetSocketAddress(9999));

	ByteBuffer byteBuffer = ByteBuffer.allocate(48);
	byteBuffer.clear();

	datagramChannel.receive(byteBuffer);


  }
}