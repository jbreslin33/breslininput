package breslin.game;

//game
import com.jme3.app.SimpleApplication;

//graphics
import com.jme3.material.Material;
import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.shape.Box;
import com.jme3.math.ColorRGBA;

//input
import com.jme3.input.controls.AnalogListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.KeyInput;

//networking
import java.net.*;
import java.io.*;


public class Game extends SimpleApplication
{

	boolean isRunning     = false;
	boolean mSentJoinGame = false;
	String  mServerIP;

	public static void main(String[] args)
	{
        Game app = new Game(args[0]);
        app.start();
    }

	//constructor
	Game(String serverIP)
	{
		mServerIP = serverIP;
	}

    public void simpleInitApp()
    {
		initializeKeys();
		isRunning = true;
    }

	/** Custom Keybinding: Map named actions to inputs. */
  	private void initializeKeys()
  	{
	   	// You can map one or several inputs to one named action
	   	inputManager.addMapping("Join",   new KeyTrigger(KeyInput.KEY_J));
	   	inputManager.addMapping("Quit",   new KeyTrigger(KeyInput.KEY_Q));

    	// Add the names to the action listener.
    	inputManager.addListener(analogListener, new String[]{"Join", "Quit"});
	}


	void joinGame()
	{

		mSentJoinGame = true;
		int messageCount = 0;

    	//networking
    	try
    	{

    		// get a datagram socket
			DatagramSocket socket = new DatagramSocket();

			// send request
    		byte[] buf = new byte[1400];

			//write to message
    		buf[0] = -101;
    		buf[1] = 'n';

			messageCount = 5;

			//now make new buffer so that it's only as big as needed.
			byte[] realByteArray = new byte[messageCount];

			for (int i = 0; i < messageCount; i++)
			{
				realByteArray[i] = buf[i];
			}

			//create DataGramPacket
    		InetAddress address = InetAddress.getByName(mServerIP);
			DatagramPacket packet = new DatagramPacket(realByteArray, realByteArray.length, address, 30004);

			//send packet to server
    		socket.send(packet);

			//print out what you sent for debugging
			String s = new String(packet.getData());

    		System.out.println("Data Sent:" + s);

		}
		catch (IOException e)
		{
			e.printStackTrace();
    	}
	}


	private AnalogListener analogListener = new AnalogListener()
  	{
  		public void onAnalog(String name, float value, float tpf)
  		{
      		if (isRunning)
      		{
        		if (name.equals("Join"))
        		{
					if (!mSentJoinGame)
					{
						System.out.println("hetttt");
        				joinGame();
					}
        		}

      		}
		}

	};
}