/**
 * All

Here's a non-blocking IO skeleton, which follows the ideas I presented in class last night.  Be aware that it requires at least java 1.4.
Note that it creates only 1 DatagramChannel and receives and sends from it.  The DatagramChannel has a built-in socket, but we never use the socket directly.

Just for fun, you can insert the code
      try
      {
         Thread.sleep( (long) (datagramInterval*Math.random ()) );
      } catch (InterruptedException e)
      {}
just before
               millisecsUntilSendNextDatagram -= System.currentTimeMillis() - start;
and see that a message is still sent every datagramInterval.

Java™ NIO,By Ron Hitchens is a book on NON-BLOCKING IO by O'Reilly.  if you're willing to give your credit card # you can read it on-line for free for 2 weeks.

One handy thing about ByteBuffers is that they have putInt( index ) and getInt( index ) methods that could be used to handle sequence numbers.

YOU MAY USE THIS CODE IN YOUR RUC PROGRAM.  BE AWARE THAT THE EXCEPTION HANDLING IS COMPLETELY INADEQUATE.

/**
 * @(#)Nonblocking_IO_example.java 1.0 03/03/25
 *
 * @author Arthur Goldberg
 */


import java.io.*;
import java.net.*;
import java.nio.*;
import java.nio.channels.*;  // bug? redundant with previous one??
import java.util.*;

/**
  The key ideas, for a 'Non-blocking' implementation of RUC:
  RUC does 2 network activities:
  1. Send datagrams, but no faster than Maximum data rate (MAX_RATE)
  2. Read datagrams, and process them

  To do this in 1 thread, use select() which blocks until one of many
things happen, such as
  ·  time elapses
  ·  a DatagramChannel has data to read
  ·  a DatagramChannel can be written without waiting
  ·  etc.

  Use this technique.  Only block in 'select'; let the select return
whenever either
  ·  a datagram can be read and processed immediately,
  ·  its time to send a datagram.
*/

class Server {

   // set input parameters
   String host = "localhost";   // "slinky.cs.nyu.edu";
   int port = 10000;
   int datagramSize = 512;
   int datagramInterval = 1000;
   int numMsgs = 24;

   // array of bytes for  a datagram to send
   byte[] sendData = new byte[ datagramSize ];
   ByteBuffer theSendDataByteBuffer = ByteBuffer.wrap( sendData );

   // array of bytes for receiving datagrams
   byte[] receiveData = new byte[ datagramSize ];
   ByteBuffer theReceiveDataByteBuffer = ByteBuffer.wrap( receiveData );

   Server()
   {
      try
      {
         Random theRandom = new Random();
         InetSocketAddress theInetSocketAddress = new InetSocketAddress( host, port);

         // make a DatagramChannel
         DatagramChannel theDatagramChannel = DatagramChannel.open();

         // A channel must first be placed in nonblocking mode
         // before it can be registered with a selector
         theDatagramChannel.configureBlocking( false );

         // instantiate a selector
         Selector theSelector = Selector.open();

         // register the selector on the channel to monitor reading
         // datagrams on the DatagramChannel
         theDatagramChannel.register( theSelector, SelectionKey.OP_READ );

         long millisecsUntilSendNextDatagram = 0;
         int i = 1;  int j = 1;

         // send and read concurrently, but do not block on read:
         while (j < 50)
         {
            long start = System.currentTimeMillis();
			System.out.println("j:" + j);
			if ( j == 10 || j == 20 || j == 30 || j == 40)
			{
   		          theDatagramChannel.send( theSendDataByteBuffer, theInetSocketAddress );
                  System.out.println("sent Datagram " + j);
			}

               // read the datagram from the DatagramChannel,
               theDatagramChannel.receive( theReceiveDataByteBuffer );
               System.out.println("theDatagramChannel.receive " + i++);
               // datagram would be processed here

               // Get an iterator over the set of selected keys
               Iterator it = theSelector.selectedKeys().iterator( );

               j++;

         }
         theSelector.close();
      }
      catch (Exception e)
      {
         e.printStackTrace();
         System.out.println("Exception " + e);
         return;
      }
   }

   public static void main( String args[] )
   {
      new Server();
   }
}
