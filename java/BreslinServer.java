import java.net.InetSocketAddress;
import java.nio.channels.DatagramChannel;
import java.nio.ByteBuffer;

public class BreslinServer
{
	public static void main(String[] argv) throws Exception
	{

		DatagramChannel datagramChannel = DatagramChannel.open();
		datagramChannel.configureBlocking(false);
		datagramChannel.socket().bind(new InetSocketAddress(9999));

		while (true)
		{



			ByteBuffer byteBuffer = ByteBuffer.allocate(48);
			byteBuffer.clear();

			if(datagramChannel.receive(byteBuffer) != null)
			{
				System.out.println("got this>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>:");
			}
			else
			{
				System.out.println("got nothing");
			}

		}
	}
}